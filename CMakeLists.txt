cmake_minimum_required(VERSION 2.8)

project(Grenier)

add_executable(Grenier grenier.c main.c)
find_path(MAGIC_INCLUDE_DIR magic.h)
find_library(MAGIC_LIBRARY magic)
include_directories(${MAGIC_INCLUDE_DIR})
target_link_libraries(Grenier ${MAGIC_LIBRARY})
message("MAGIC_INCLUDE_DIR: ${MAGIC_INCLUDE_DIR}")
message("MAGIC_LIBRARY    : ${MAGIC_LIBRARY}")
