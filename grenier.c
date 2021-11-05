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


/// Find picture files in a folder.
///
/// @param foldername The name of the folder where to find pictures.
/// @return A list of file names that are pictures.

char ** findPictures (const char * foldername)
{
  if ( isPicture (foldername) )
  {
    char ** pictures = malloc(2*sizeof(char *));
    pictures[0] = strdup(foldername);
    pictures[1] = NULL;
    return pictures;
  }

  DIR * folder = opendir(foldername);
  if ( !folder )
  {
    return NULL;
  }
  
  char ** pictures = NULL;

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
      pictures = StringListAdd ( pictures, path );
    }
    if ( isFolder( path ) )
    {
      char ** picturesinfolder = findPictures(path);
      pictures = StringListAppend(pictures, picturesinfolder);
      StringListFree(picturesinfolder);
    }
    free(path);
  }

  closedir(folder);

  return pictures;
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


/// Free memory held by a list of strings.
///
/// @param list The list of strings to be freed

void StringListFree (char ** list)
{
  if ( !list ) return;

  for ( int i=0 ; list[i] ; i++ )
  {
    free(list[i]);
    list[i] = NULL;
  }
  
  free(list);
}


/// Count the number of strings in a string list.
///
/// @param list The list to count.
/// @return The number of strings in the list. If the list is _NULL_, the count
///         is 0;

int StringListCount(char ** list)
{
  int count = 0;

  if (!list)
  {
    return count;
  }

  while (list[count])
  {
    count++;
  }

  return count;
}


/// Add a string to a string list.
///
/// @param list The string list to which the string is added.
/// @param string The string to add to the string list.
/// @return The modified string list.

char ** StringListAdd(char ** list, const char * string)
{
  if (!string)
  {
    return list;
  }

  char **newlist;
  int nitems;
  if (list)
  {
    nitems = StringListCount(list)+1;
    newlist = realloc(list, sizeof(char*)*(nitems+1));
  }
  else
  {
    nitems = 1;
    newlist = malloc(sizeof(char*)*(nitems+1));
  }
  newlist[nitems-1] = strdup(string);
  newlist[nitems] = NULL;

  return newlist;
}


/// Append a string list to another one.
///
/// @param body The string list at the end of which the other string
///             list is appended.
/// @param tail The string list to append to the other one.
/// @return The modified string list. Strings from @b tail string list are
///         duplicated.

char ** StringListAppend(char ** body, char ** tail)
{
  if (!tail)
  {
    return body;
  }

  if (!body)
  {
    return StringListDup(tail);
  }

  int nbodyitems = StringListCount(body);
  int ntailitems = StringListCount(tail);
  int nnewbodyitems = nbodyitems + ntailitems;
  char ** newbody = realloc(body, sizeof(char*)*(nnewbodyitems+1));
  newbody[nnewbodyitems] = NULL;
  for ( int i = 0 ; i < ntailitems ; i++ )
  {
    newbody[nbodyitems+i] = strdup(tail[i]);
  }

  return newbody;
}


/// Duplicate a string list.
///
/// @param list The string list to duplicate.
/// @return A new string list identical to the @b list string list. All strings
///         in @b list are duplicated as well.

char ** StringListDup(char ** list)
{
  if (!list)
  {
    return NULL;
  }

  char ** newlist = NULL;
  int nitems = StringListCount(list);
  for ( int i = 0 ; i < nitems ; i++ )
  {
    newlist = StringListAdd ( newlist, strdup(list[i]) );
  }

  return newlist;
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
