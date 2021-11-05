#include "grenier.h"
#include <magic.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>


/// Check if a file is a picture.
///
/// @param filename The name of the file to check.
/// @return _true_ if the file is a picture, _false_ otherwise.

bool isPicture (const char * filename)
{
  return MimeTypeMatches(filename, "image/");
}


/// Check if a path is a folder.
///
/// @param pathname The name of the path to check.
/// @return _true_ if the path is a folder, _false_ otherwise.

bool isFolder (const char * pathname)
{
  return MimeTypeMatches(pathname, "inode/directory");
}


/// Iterate over pictures in a folder and run a function on each picture found.
///
/// @param foldername The name of the folder where to find pictures.
/// @param function The name of the function to run.
/// @param context A pointer to some context needed by @b function.

void foreachPicture (
    const char * foldername,
    void ( * function ) ( void *, void * ),
    void * context )
{
  if ( isPicture (foldername) )
  {
    char * path = strdup(foldername);
    function(path,context);
    free(path);
    return;
  }

  DIR * folder = opendir(foldername);
  if ( !folder )
  {
    return;
  }
  
  struct dirent * entry;
  while ( entry = readdir(folder) )
  {
    if ( !strcmp(entry->d_name, "." ) ) continue;
    if ( !strcmp(entry->d_name, ".." ) ) continue;
    char* path = NULL;
    path = StringAppend(path,foldername);
    path = StringAppend(path,"/");
    path = StringAppend(path,entry->d_name);
    if ( isPicture( path ) )
    {
      function(path,context);
    }
    if ( isFolder( path ) )
    {
      foreachPicture(path, function, context);
    }
    free(path);
  }

  closedir(folder);

  return;
}


/// Check if a file matches a given MIME type.
///
/// @param filename The file name to check.
/// @param expectedmime The expected MIME type.
/// @return _true_ if file name MIME type matches expected MIME type, _false_
///         otherwise.

bool MimeTypeMatches ( const char * filename, const char * expectedmime )
{
  if (!filename || !expectedmime)
  {
    return false;
  }

  /// The function uses the **libmagic(3)** library to get the MIME type of
  /// the file.
  magic_t query = magic_open (MAGIC_MIME_TYPE);
  magic_load (query, NULL);
  const char * actualmime = magic_file (query, filename);

  /// The file matches if its MIME type starts with expected MIME type.
  bool result = ( strncmp (actualmime, expectedmime, strlen(expectedmime)) == 0 );
  magic_close (query);

  return result;
}


/// Append a string to another one.
///
/// @param body The string at the end of which the other string is
///             appended.
/// @param tail The string to append to the other one.
/// @return The modified string.

char * StringAppend(char * body, const char * tail)
{
  if (!tail)
  {
    return body;
  }

  if (!body)
  {
    return strdup(tail);
  }

  size_t bodylen = strlen(body);
  size_t taillen = strlen(tail);
  size_t newlen = bodylen + taillen;
  char * newbody = realloc(body, sizeof(char)*(newlen+1));
  newbody[newlen] = '\0';
  for ( size_t i = 0 ; i < taillen ; i++ )
  {
    newbody[i+bodylen] = tail[i];
  }

  return newbody;
}
