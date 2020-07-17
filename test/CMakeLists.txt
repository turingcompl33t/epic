# test/CMakeLists.txt

# pull in catch_discover_tests()
include(${CMAKE_SOURCE_DIR}/deps/catch2/contrib/Catch.cmake)

add_library(catch-main STATIC "catch_main.cpp")
target_link_libraries(catch-main PUBLIC Catch2::Catch2)

set(TEST_SUITE_SRC
    "atomic.cpp"
    "base.cpp"
    "epoch.cpp"
    "internal.cpp"
    "ordering.cpp"
    "owned.cpp"
    "pointer.cpp"
    "shared.cpp")

add_executable(epic-test ${TEST_SUITE_SRC})
target_link_libraries(epic-test PRIVATE epic catch-main)

catch_discover_tests(epic-test)