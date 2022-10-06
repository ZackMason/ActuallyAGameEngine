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

add_library(Engine ${lib_files})

target_include_directories(Engine PUBLIC include)
target_include_directories(Engine PUBLIC include/imgui)

# This will break the build for some fucking insane reason if it is higher in this file, do not fucking touch, I have wasted so much fucking time because of this stupid fucking flag
add_compile_options(/bigobj) 

# This asset dir is used for internal tests, you should use your own asset directory
target_compile_definitions(Engine PUBLIC CMAKE_ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")

file(GLOB_RECURSE test_files 
    ${PROJECT_SOURCE_DIR}/tests/*.c*
)

add_executable(EngineTests ${test_files})

set_target_properties(EngineTests 
                        PROPERTIES 
                            CXX_STANDARD 20
                            CXX_STANDARD_REQUIRED YES
                            CXX_EXTENSIONS ON
)

# This asset dir is used for internal tests, you should use your own asset directory
target_compile_definitions(EngineTests PUBLIC CMAKE_ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")
target_link_libraries(EngineTests PUBLIC ${CONAN_LIBS} Engine)

target_link_libraries(Engine PUBLIC ${CONAN_LIBS})

target_link_libraries(Engine PUBLIC d3d12.lib)
target_link_libraries(Engine PUBLIC dxgi.lib)