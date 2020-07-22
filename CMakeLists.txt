# CMakeLists.txt

cmake_minimum_required(VERSION 3.17)

project(epic CXX)

option(BUILD_TESTS "Build test suite" ON)
option(BUILD_EXAMPLES "Build example programs" ON)

set(GCC_FLAGS "-ggdb")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_FLAGS}")

add_subdirectory(deps/lowlock)
add_subdirectory(deps/expected)

set(${PROJECT_NAME}_SRC
    "src/bag.cpp"
    "src/collector.cpp"
    "src/global.cpp"
    "src/guard.cpp"
    "src/local.cpp"
    "src/local_handle.cpp"
    "src/ordering.cpp")

add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SRC})
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>)
target_link_libraries(${PROJECT_NAME} PUBLIC lowlock expected)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

if(${BUILD_TESTS})
    message("Configuring tests...")
    enable_testing()
    add_subdirectory(deps/catch2)
    add_subdirectory(test)
endif()

if(${BUILD_EXAMPLES})
    message("Configuring examples...")
    add_subdirectory(example)
endif()