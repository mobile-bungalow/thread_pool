cmake_minimum_required(VERSION 3.14)

project(
  BunglePool
  VERSION 0.1.0
  LANGUAGES C
  )

add_compile_options(-g -Wshadow -Wall -Werror -Wno-error=maybe-uninitialized)
find_package(Threads REQUIRED)
add_library(BunglePool STATIC src/bungle_pool.c include/bungle_pool.h)
set_property(TARGET BunglePool PROPERTY C_STANDARD 99)
target_link_libraries(BunglePool Threads::Threads)


add_subdirectory(tests)
