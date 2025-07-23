cmake_minimum_required(VERSION 3.24.1)
project(FTFS VERSION 2.0.5 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if (MSVC)
    # For deciding whether to statically or dynamically link MSVC runtime libs
    option(BUILD_SHARED_MSVC "Specifies whether or not to build the MSVC runtime libraries as shared or static." OFF)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

option(FTFS_BUILD_TESTS "Specifies whether or not to build the unit tests." ON)

include(src/SourceFilesList.cmake) # Explicitly define all source code files

add_executable(FTFS ${FTFS_ROOT_SOURCE} ${FTFS_EXTERNAL_SOURCE} ${FTFS_CORE_SOURCE} ${FTFS_GRAPHICS_SOURCE} ${FTFS_INTERFACE_SOURCE} ${FTFS_SERIALIZATION_SOURCE}
    ${FTFS_STATES_SOURCE} ${FTFS_UTIL_SOURCE})

add_subdirectory(libs/freetype)
add_subdirectory(libs/glfw)
add_subdirectory(libs/glm)
add_subdirectory(libs/json)

target_include_directories(FTFS PRIVATE "${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/libs/glad/include" "${CMAKE_SOURCE_DIR}/libs/irrKlang/include" "${CMAKE_SOURCE_DIR}/libs/stb")
target_link_libraries(FTFS PRIVATE freetype glfw glm nlohmann_json::nlohmann_json "${PROJECT_SOURCE_DIR}/libs/irrKlang/bin/irrKlang.lib")

if(WIN32 AND MSVC)
    target_link_options(FTFS PRIVATE "$<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>" "$<$<CONFIG:Release>:/ENTRY:mainCRTStartup>")
endif()

add_custom_command(TARGET FTFS POST_BUILD 
    COMMAND "${CMAKE_COMMAND}" ARGS -E copy_if_different "${CMAKE_SOURCE_DIR}/libs/irrklang/bin/irrKlang.dll" "${CMAKE_BINARY_DIR}"
    COMMAND "${CMAKE_COMMAND}" ARGS -E copy_if_different "${CMAKE_SOURCE_DIR}/resources.json" "${CMAKE_BINARY_DIR}"
    COMMAND "${CMAKE_COMMAND}" ARGS -E copy_directory_if_different "${CMAKE_SOURCE_DIR}/audio" "${CMAKE_BINARY_DIR}/audio"
    COMMAND "${CMAKE_COMMAND}" ARGS -E copy_directory_if_different "${CMAKE_SOURCE_DIR}/data" "${CMAKE_BINARY_DIR}/data"
    COMMAND "${CMAKE_COMMAND}" ARGS -E copy_directory_if_different "${CMAKE_SOURCE_DIR}/fonts" "${CMAKE_BINARY_DIR}/fonts"
    COMMAND "${CMAKE_COMMAND}" ARGS -E copy_directory_if_different "${CMAKE_SOURCE_DIR}/misc" "${CMAKE_BINARY_DIR}/misc"
    COMMAND "${CMAKE_COMMAND}" ARGS -E copy_directory_if_different "${CMAKE_SOURCE_DIR}/shaders" "${CMAKE_BINARY_DIR}/shaders"
    COMMAND "${CMAKE_COMMAND}" ARGS -E copy_directory_if_different "${CMAKE_SOURCE_DIR}/textures" "${CMAKE_BINARY_DIR}/textures"
)

#if(FTFS_BUILD_TESTS)
#    include(CTest)
#    enable_testing()
#    add_subdirectory(tests)
#endif()