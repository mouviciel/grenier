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
  /// The application returns _yes_ or _no_ depending on whether the file
  /// passed as first argument  is a picture or not.
  printf ("%s \n", isPicture (argv[1]) ? "yes" : "no");
  return 0;
}
