#include "grenier.h"
#include <stdio.h>


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
    char ** pictures = findPictures(argv[i]);
    for ( int j=0 ; pictures && pictures[j] ; j++ )
    {
      printf ("%s\n", pictures[j]);
    }
    StringListFree(pictures);
  }

  return 0;
}
