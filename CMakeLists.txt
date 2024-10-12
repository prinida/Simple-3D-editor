cmake_minimum_required(VERSION 3.23.1 FATAL_ERROR)

set(PROJECT_NAME Simple-3D-editor)
project(${PROJECT_NAME})

add_executable(${PROJECT_NAME} 
	src/stb_image.h
	src/ResourcesManager.h
	src/ShaderProgram.h
	src/Texture.h
	src/OpenGLManager.h
	src/GLFWManagement.h
	src/ReplicatedCutObject.h
	src/Camera.h
	src/LightTypes.h
	src/MaterialTypes.h
	src/LightManager.h
	src/Enums.h
	src/Sphere.h

	src/main.cpp
	src/ResourcesManager.cpp
	src/ShaderProgram.cpp
	src/Texture.cpp
	src/OpenGLManager.cpp
	src/ReplicatedCutObject.cpp
	src/Camera.cpp
	src/GLFWManagement.cpp
	src/LightManager.cpp
	src/Sphere.cpp
	
	src/ImGui/imconfig.h
	src/ImGui/imgui.cpp
	src/ImGui/imgui.h
	src/ImGui/imgui_demo.cpp
	src/ImGui/imgui_draw.cpp
	src/ImGui/imgui_internal.h
	src/ImGui/imgui_tables.cpp
	src/ImGui/imgui_widgets.cpp
	src/ImGui/imstb_rectpack.h
	src/ImGui/imstb_textedit.h
	src/ImGui/imstb_truetype.h
	src/ImGui/imgui_impl_glfw.h
	src/ImGui/imgui_impl_glfw.cpp
	src/ImGui/imgui_impl_opengl3.h
	src/ImGui/imgui_impl_opengl3.cpp
	src/ImGui/imgui_impl_opengl3_loader.h
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) 
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE) 

add_subdirectory(external/glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

add_subdirectory(external/glad)
target_link_libraries(${PROJECT_NAME} PRIVATE glad)

include_directories(external/glm)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy_directory
					${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res)

if (MSVC)
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()