cmake_minimum_required(VERSION 3.1)

project(Grenier)

add_executable(Grenier grenier.c cexiftool.c main.c)
set_property(TARGET Grenier PROPERTY C_STANDARD 99)

include(FindPackageHandleStandardArgs)

find_path(MAGIC_INCLUDE_DIR magic.h REQUIRED)
find_library(MAGIC_LIBRARY magic REQUIRED)
include_directories(${MAGIC_INCLUDE_DIR})
target_link_libraries(Grenier ${MAGIC_LIBRARY})
find_package_handle_standard_args(Magic DEFAULT_MSG MAGIC_LIBRARY MAGIC_INCLUDE_DIR)

find_program(EXIFTOOL_EXECUTABLE exiftool REQUIRED)
find_package_handle_standard_args(ExifTool DEFAULT_MSG EXIFTOOL_EXECUTABLE)

find_package(Doxygen)
if (DOXYGEN_FOUND)
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in"
    "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
    @ONLY)
  add_custom_target(Documentation ALL
    COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
    COMMENT "Building documentation...")
endif()
