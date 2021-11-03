#include "grenier.h"
#include <magic.h>
#include <string.h>

bool isPicture (char * filename)
{
  if ( !filename )
  {
    return false;
  }

  /// The function uses the **libmagic(3)** library to get the MIME type of
  /// the file.
  magic_t query = magic_open (MAGIC_MIME_TYPE);
  magic_load (query, NULL);
  const char * mime = magic_file (query, filename);

  /// The file is identified as a picture if its MIME type starts with
  /// _image/_.
  bool result = ( strncmp (mime, "image/", 6) == 0 );
  magic_close (query);

  return result;
}
