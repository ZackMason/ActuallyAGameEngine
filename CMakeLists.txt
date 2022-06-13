cmake_minimum_required(VERSION 2.8.12)
project(TheGame C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 11)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

file(GLOB_RECURSE src_files 
    ${PROJECT_SOURCE_DIR}/src/main.c*
    ${PROJECT_SOURCE_DIR}/src/Game/*.c*
)

file(GLOB_RECURSE lib_files 
    ${PROJECT_SOURCE_DIR}/src/*.c*
)
list(FILTER lib_files EXCLUDE REGEX ${PROJECT_SOURCE_DIR}/src/main.c*)
list(FILTER lib_files EXCLUDE REGEX ${PROJECT_SOURCE_DIR}/src/Game/*.c*)

include_directories(include)
include_directories(include/imgui)
add_compile_options(/bigobj)

add_library(Engine ${lib_files})
add_executable(${PROJECT_NAME} ${src_files})
target_compile_definitions(${PROJECT_NAME} PUBLIC CMAKE_ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")

file(GLOB_RECURSE test_files 
    ${PROJECT_SOURCE_DIR}/tests/*.c*
)

add_executable(tests ${test_files})
target_compile_definitions(tests PUBLIC CMAKE_ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")
target_compile_definitions(Engine PUBLIC CMAKE_ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")
target_link_libraries(tests PUBLIC ${CONAN_LIBS} Engine)

target_link_libraries(Engine PUBLIC ${CONAN_LIBS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${CONAN_LIBS} Engine)

