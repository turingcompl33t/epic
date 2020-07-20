# example/CMakeLists.txt

add_executable(owned "owned.cpp")
target_link_libraries(owned PRIVATE epic)

add_executable(shared "shared.cpp")
target_link_libraries(shared PRIVATE epic)