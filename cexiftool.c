#include "cexiftool.h"
#include "grenier.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


char * exiftool ( char * argv[] )
{
  char * command = NULL;

  command = StringAppend(command, "exiftool -m -q -q");

  for ( int i = 0 ; argv[i] ; i++ )
  {
    command = StringAppend(command, " ");
    command = StringAppend(command, argv[i]);
  }

  FILE * CommandOutput = popen(command, "r");
  char * result = NULL;
  if ( CommandOutput )
  {
    char buffer[1000];
    while ( fgets(buffer, sizeof(buffer), CommandOutput) )
    {
      char * newline = strrchr(buffer, '\n');
      newline[0] = '\0';
      result = StringAppend(result, buffer);
    }
  }
  free(command);
  return result;
}
