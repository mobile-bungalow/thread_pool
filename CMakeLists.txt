cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(
  BunglePool
  VERSION 0.1.0
  LANGUAGES C
  )
find_package(Threads REQUIRED)
add_library(BunglePool STATIC src/bungle_pool.c include/bungle_pool.h)
target_link_libraries(BunglePool Threads::Threads)
add_subdirectory(tests)
