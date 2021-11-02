#include "grenier.h"
#include <magic.h>
#include <string.h>

bool isPicture (char * filename)
{
  if ( !filename )
  {
    return false;
  }

  magic_t query = magic_open (MAGIC_MIME_TYPE);
  magic_load (query, NULL);
  const char * mime = magic_file (query, filename);
  bool result = ( strncmp (mime, "image/", 6) == 0 );
  magic_close (query);

  return result;
}
