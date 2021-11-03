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
  /// a picture.
  for (int i=1 ; argv[i] ; i++ )
  {
    if (isPicture(argv[i]))
    {
      printf ("%s\n", argv[i]);
    }
  }

  return 0;
}
