#include "grenier.h"
#include <stdio.h>
#include <magic.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>


/// Print a string to standard output.
///
/// @param string The string to print.
/// @param format The format to use for printing.

void StringPrint(void * string, void * format)
{
  printf(format, string);
}


/// Print information on picture file.
///
/// @param picture The filename of the picture.
/// @param notused A dummy parameter to conform to foreachPicture() prototype.

void printPictureInformation(void * picture, void * notused)
{
  char * mime = MimeTypeGet ( picture );
  char * tags[] = { "MimeType", "ImageWidth", "ImageHeight", NULL };
  char ** exif = ExifGet ( picture, tags );
  off_t size = FileSizeGet ( picture );
  char * md5hash = FileMd5Get ( picture );

  printf("- (%s, :%s:%s:%s:, %zd, %s) %s\n",
      mime,
      exif[0], exif[1], exif[2],
      (intmax_t)size,
      md5hash,
      picture);

  free(md5hash);
  StringListFree(exif);
  free(mime);
}


/// Entry point of the application.
///
/// @param argc The number of command line arguments.
/// @param argv The list of command line arguments, starting with command name
///             at index #0.
/// @return The execution status of the application. 0 means _success_.
int main (int argc, char * argv[])
{
  /// The application scans arguments and prints them only if the file is
  /// a picture. If an argument is a directory, the application scans its
  /// contents and prints directory entries that are pictures.
  for (int i=1 ; argv[i] ; i++ )
  {
    foreachPicture(argv[i], printPictureInformation, NULL);
  }

  return 0;
}

// vim: set tw=79 ts=2 sw=2 sts=2 et ai si syn=c fo+=ro dip+=iwhite ff=unix:
