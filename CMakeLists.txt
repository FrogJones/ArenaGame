cmake_minimum_required(VERSION 3.15)

# Force vcpkg toolchain if not already set
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
set(CMAKE_TOOLCHAIN_FILE "C:/Users/panfr/Documents/vcpkg/scripts/buildsystems/vcpkg.cmake"
CACHE STRING "Vcpkg toolchain file")
endif()

project(PS1Graphics LANGUAGES C CXX)

# Use C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages from vcpkg
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(OpenAL CONFIG REQUIRED)
find_package(SndFile CONFIG REQUIRED)

# Your source files - now including all the new .cpp files
set(SOURCES
src/main/main.cpp
src/main/gameEngine.cpp
src/main/gameState.cpp
src/main/inputHandler.cpp
src/main/renderer.cpp
src/main/config.cpp
src/main/model.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Hide console and use static runtime
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
    )
endif()

# Link libraries
target_link_libraries(${PROJECT_NAME}
PRIVATE
glad::glad 
glfw
glm::glm
assimp::assimp
OpenAL::OpenAL
SndFile::sndfile
)

# Include directories
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/include
)

# ==================================================
# Copy asset folders into build directory
# ==================================================
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/src/shaders"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders"
    COMMENT "Copying shaders folder to build output"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/src/models"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/models"
    COMMENT "Copying models folder to build output"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/src/sfx"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/sfx"
    COMMENT "Copying sfx folder to build output"
)

# ==================================================
# Distribution Target (optional packaging step)
# ==================================================
add_custom_target(dist
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/dist"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/dist"
    # Copy executable
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${PROJECT_NAME}>" "${CMAKE_BINARY_DIR}/dist/"
    # Copy DLL if it exists
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_BINARY_DIR}/glfw3.dll" "${CMAKE_BINARY_DIR}/dist/" || echo "glfw3.dll not found"
    # Copy asset folders
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/src/shaders" "${CMAKE_BINARY_DIR}/dist/shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/src/models" "${CMAKE_BINARY_DIR}/dist/models"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/src/sfx" "${CMAKE_BINARY_DIR}/dist/sfx"
    COMMENT "Creating distribution folder with all assets"
    DEPENDS ${PROJECT_NAME}
)