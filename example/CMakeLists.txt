# example/CMakeLists.txt

add_executable(owned "owned.cpp")
target_link_libraries(owned PRIVATE epic)

add_executable(shared "shared.cpp")
target_link_libraries(shared PRIVATE epic)

add_executable(atomic "atomic.cpp")
target_link_libraries(atomic PRIVATE epic)

add_executable(deferred "deferred.cpp")
target_link_libraries(deferred PRIVATE epic)