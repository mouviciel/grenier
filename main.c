#include "grenier.h"
#include "cexiftool.h"
#include <stdio.h>

#include <magic.h>
#include <string.h>
#include <stdlib.h>


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
  magic_t query = magic_open (MAGIC_MIME_TYPE);
  magic_load (query, NULL);
  char * mimemagic = strdup (magic_file (query, picture));
  magic_close (query);

  char *argv[] = { "-p", "'$MimeType'", picture, NULL };
  char * mimeexif = exiftool( argv );
  printf("- (%s, %s) %s\n", mimemagic, mimeexif, picture);
  free(mimemagic);
  free(mimeexif);
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
