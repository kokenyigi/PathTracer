cmake_minimum_required(VERSION 3.16)
project(MyProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Saját include mappa


# GLFW opciók (clean build)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

# GLFW hozzáadása
add_subdirectory(dependencies/GLFW)

# GLAD hozzáadása
add_library(glad dependencies/GLAD/src/glad.c)
target_include_directories(glad PUBLIC dependencies/GLAD/include)

#GLM hozzáadása


# Összes cpp fájl automatikusan
file(GLOB_RECURSE SOURCES 
	src/*.cpp
	dependencies/stb_image/src/*.cpp
	dependencies/ImGui/src/*.cpp
	dependencies/NeonOpenGL/src/*.cpp
	dependencies/NeonUtils/src/*.cpp
	dependencies/NeonGui/src/*.cpp
	dependencies/Neon2DRenderer/src/*.cpp
	)

# Executable
add_executable(PathTracer ${SOURCES})

target_include_directories(PathTracer PRIVATE
	./include
	dependencies/GLM/include
	dependencies/stb_image/include
	dependencies/ImGui/include
	dependencies/ImGui/include/backends
	dependencies/NeonOpenGL/include
	dependencies/NeonUtils/include
	dependencies/NeonGui/include
	dependencies/Neon2DRenderer/include)

# Linkelés GLFW-vel és GLAD-al
find_package(OpenGL REQUIRED)
target_link_libraries(PathTracer 
    glfw 
    OpenGL::GL
    glad
)