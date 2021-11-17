cmake_minimum_required(VERSION 3.1)


project(Grenier)
add_executable(Grenier grenier.c main.c)
set_property(TARGET Grenier PROPERTY C_STANDARD 99)


include(FindPackageHandleStandardArgs)


# libmagic(3) is the library used by file(1). It is used to find MIME type of
# files and detect pictures.
find_path(MAGIC_INCLUDE_DIR magic.h REQUIRED)
find_library(MAGIC_LIBRARY magic REQUIRED)
include_directories(${MAGIC_INCLUDE_DIR})
target_link_libraries(Grenier ${MAGIC_LIBRARY})
find_package_handle_standard_args(Magic DEFAULT_MSG MAGIC_LIBRARY MAGIC_INCLUDE_DIR)


# exiftool(1) extracts EXIF metadata from picture files.
find_program(EXIFTOOL_EXECUTABLE exiftool REQUIRED)
find_package_handle_standard_args(ExifTool DEFAULT_MSG EXIFTOOL_EXECUTABLE)


# openssl(3) is used to compute MD5 hash of picture files.
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
target_link_libraries(Grenier ${OPENSSL_LIBRARIES})


# doxygen(1) is used to build source code documentation.
find_package(Doxygen)
if (DOXYGEN_FOUND)
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in"
    "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
    @ONLY)
  add_custom_target(Documentation ALL
    COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
    COMMENT "Building documentation...")
endif()
