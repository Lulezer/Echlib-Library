#https://github.com/meemknight/GLFWCMakeSetup
#Version 1.0.0

cmake_minimum_required(VERSION 3.16)
# Bundled GLFW still declares CMake 3.1; CMake 4.x needs this floor.
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

# 1. ALWAYS DEFINE THE PROJECT FIRST!
project(echlib)

set(CMAKE_CXX_STANDARD 17)

option(PRODUCTION_BUILD "Make this a production build" OFF)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE) # link time optimization

if(MSVC)
    add_compile_options(/arch:AVX2) # Now this actually works!
endif()

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(thirdparty/glfw-3.3.2)         # window opener
add_subdirectory(thirdparty/glad)                # opengl loader
add_subdirectory(thirdparty/stb_image)           # loading images
add_subdirectory(thirdparty/stb_truetype)        # loading ttf files
add_subdirectory(thirdparty/raudio)              # audio
add_subdirectory(thirdparty/glm)                 # math
add_subdirectory(thirdparty/imgui-docking)       # ui
add_subdirectory(thirdparty/gl2d)                # rendering

# 2. Gather the source files
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
list(FILTER MY_SOURCES EXCLUDE REGEX "/main\\.cpp$")

# 3. Define the library target and pass the sources directly
add_library(echlib STATIC ${MY_SOURCES})

target_compile_definitions(echlib PUBLIC GLFW_INCLUDE_NONE=1)

if(PRODUCTION_BUILD)
    target_compile_definitions(echlib PUBLIC RESOURCES_PATH="./resources/")
    target_compile_definitions(echlib PUBLIC PRODUCTION_BUILD=1)
else()
    target_compile_definitions(echlib PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/")
    target_compile_definitions(echlib PUBLIC PRODUCTION_BUILD=0)
endif()

if(MSVC)
    target_compile_definitions(echlib PUBLIC _CRT_SECURE_NO_WARNINGS)
endif()

target_include_directories(echlib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

target_link_libraries(echlib PUBLIC glm glfw glad stb_image stb_truetype gl2d raudio imgui)

set_target_properties(echlib PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib"
)