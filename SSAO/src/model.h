#pragma once
#include "gl_env.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader.h"

#include "mesh.h"

#include <string>

#include <fstream>
#include <sstream>
#include <iostream>

#include <map>

#include <vector>

using namespace std; //

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);
unsigned int TextureFrom_FBX_EmbeddedTexture(const aiTexture *aiTex, bool gama = false);

unsigned int GenerateTex(unsigned char *data, int width, int height, int nrComponents, bool gama = false);

class Model
{
public:
	vector<Texture> textures_loaded;
	vector<Mesh> meshes;
	string directory;
	bool gammaCorrection;

	Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
	{
		loadModel(path);
	}

	void Draw(Shader &shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}

private:
	// use assimp to load model
	void loadModel(string const &path)
	{
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return;
		}

		directory = path.substr(0, path.find_last_of('/'));

		// all node
		processNode(scene->mRootNode, scene);
	}

	void processNode(aiNode *node, const aiScene *scene)
	{
		// all mesh
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}

		// children node
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh processMesh(aiMesh *mesh, const aiScene *scene)
	{
		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<Texture> textures;

		// all vertices / every vertex
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector;

			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;

			vertex.Position = vector;

			// check normals
			if (mesh->HasNormals())
			{
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.Normal = vector;
			}

			// texture coords
			if (mesh->mTextureCoords[0])
			{
				glm::vec2 vec;

				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;

				// tangent
				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;
				vertex.Tangent = vector;

				// bitangent
				vector.x = mesh->mBitangents[i].x;
				vector.y = mesh->mBitangents[i].y;
				vector.z = mesh->mBitangents[i].z;
				vertex.Bitangent = vector;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);

			vertices.push_back(vertex);
		}
		// all surface
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];

			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		// process materials
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

		// 1. diffuse maps
		vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		// 2. specular maps
		vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", scene);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		// 3. normal maps
		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", scene);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		// 4. height maps
		std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height", scene);
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		return Mesh(vertices, indices, textures);
	}

	// 1

	// FBX load
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName, const aiScene *scene)
	{
		vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString imagePath;
			mat->GetTexture(type, i, &imagePath);
			// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j].path.data(), imagePath.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
					break;
				}
			}
			if (!skip)
			{
				// if texture hasn't been loaded already, load it
				Texture texture;

				// auto filePath = directory + str.C_Str() ;

				// if there's embedded texture
				auto tex = scene->GetEmbeddedTexture(imagePath.C_Str());

				if (tex != nullptr)
				{

					texture.id = TextureFrom_FBX_EmbeddedTexture(tex);
				}
				else
				{

					texture.id = TextureFromFile(imagePath.C_Str(), this->directory);
				}

				texture.type = typeName;
				texture.path = imagePath.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture); // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			}
		}
		return textures;
	}
};

// 2.

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma)
{
	string filename = string(path);
	filename = directory + '/' + filename;

	int width, height, nrComponents;

	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

	return GenerateTex(data, width, height, nrComponents);
}

unsigned int TextureFrom_FBX_EmbeddedTexture(const aiTexture *aiTex, bool gama)
{
	int width, height, channels;

	unsigned char *data = nullptr;

	if (aiTex->mHeight == 0)
	{
		data = stbi_load_from_memory(reinterpret_cast<unsigned char *>(aiTex->pcData), aiTex->mWidth, &width, &height, &channels, 0);
	}
	else
	{
		data = stbi_load_from_memory(reinterpret_cast<unsigned char *>(aiTex->pcData), aiTex->mWidth * aiTex->mHeight, &width, &height, &channels, 0);
	}

	return GenerateTex(data, width, height, channels);
}

unsigned int GenerateTex(unsigned char *data, int width, int height, int nrComponents, bool gama)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	if (data)
	{
		GLenum format;

		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);

		// gamma
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "failed to load texture: " << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// alt.
// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
// vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
//{
//	vector<Texture> textures;
//	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
//	{
//		aiString str;
//		mat->GetTexture(type, i, &str);
//		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
//		bool skip = false;
//		for (unsigned int j = 0; j < textures_loaded.size(); j++)
//		{
//			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
//			{
//				textures.push_back(textures_loaded[j]);
//				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
//				break;
//			}
//		}
//		if (!skip)
//		{   // if texture hasn't been loaded already, load it
//			Texture texture;
//			texture.id = TextureFromFile(str.C_Str(), this->directory);
//			texture.type = typeName;
//			texture.path = str.C_Str();
//			textures.push_back(texture);
//			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
//		}
//	}
//	return textures;
//}

// alt..

//
// unsigned int TextureFromFile(const char* path, const string& directory, bool gamma)
//{
//	string filename = string(path);
//	filename = directory + '/' + filename;
//
//	unsigned int textureID;
//	glGenTextures(1, &textureID);
//
//	int width, height, nrComponents;
//	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
//	if (data)
//	{
//		GLenum format;
//		if (nrComponents == 1)
//			format = GL_RED;
//		else if (nrComponents == 3)
//			format = GL_RGB;
//		else if (nrComponents == 4)
//			format = GL_RGBA;
//
//		glBindTexture(GL_TEXTURE_2D, textureID);
//		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//		glGenerateMipmap(GL_TEXTURE_2D);
//
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//		stbi_image_free(data);
//	}
//	else
//	{
//		std::cout << "Texture failed to load at path: " << path << std::endl;
//		stbi_image_free(data);
//	}
//
//	return textureID;
//}