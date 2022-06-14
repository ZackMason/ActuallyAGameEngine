cmake_minimum_required(VERSION 2.8.12)
project(Engine C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 11)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

file(GLOB_RECURSE lib_files 
    ${PROJECT_SOURCE_DIR}/src/*.c*
)

add_library(${PROJECT_NAME} ${lib_files})

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_include_directories(${PROJECT_NAME} PUBLIC include/imgui)


add_compile_options(/bigobj)

target_compile_definitions(${PROJECT_NAME} PUBLIC CMAKE_ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")

file(GLOB_RECURSE test_files 
    ${PROJECT_SOURCE_DIR}/tests/*.c*
)

add_executable(EngineTests ${test_files})
target_compile_definitions(EngineTests PUBLIC CMAKE_ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")
target_link_libraries(EngineTests PUBLIC ${CONAN_LIBS} Engine)

target_link_libraries(${PROJECT_NAME} PUBLIC ${CONAN_LIBS})

