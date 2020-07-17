# example/CMakeLists.txt

add_executable(ex-owned "owned.cpp")
target_link_libraries(ex-owned PRIVATE epic)

add_executable(ex-shared "shared.cpp")
target_link_libraries(ex-shared PRIVATE epic)