add_executable(cgfin24ssao
        gl_env.h
        main.cpp
        shader.h
        mesh.h
        model.h)

target_link_libraries(cgfin24ssao PRIVATE assimp::assimp glew_s glm stb glfw imgui)
target_include_directories(cgfin24ssao PRIVATE
        ../third_party/glew/include
        ${CMAKE_CURRENT_BINARY_DIR})

target_compile_features(cgfin24ssao PRIVATE cxx_std_11)

configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)