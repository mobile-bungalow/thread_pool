include_directories(../include)
add_executable(test test.c)
target_link_libraries(test BunglePool)

enable_testing()
add_test(thread_pool_tests test)

