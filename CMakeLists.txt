cmake_minimum_required(VERSION 3.1)

project(Grenier)

add_executable(Grenier grenier.c main.c)
set_property(TARGET Grenier PROPERTY C_STANDARD 99)

find_path(MAGIC_INCLUDE_DIR magic.h)
find_library(MAGIC_LIBRARY magic)
include_directories(${MAGIC_INCLUDE_DIR})
target_link_libraries(Grenier ${MAGIC_LIBRARY})

find_package(Doxygen)
if (DOXYGEN_FOUND)
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in"
    "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
    @ONLY)
  add_custom_target(Documentation ALL
    COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
    COMMENT "Building documentation...")
endif()
