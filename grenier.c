#include "grenier.h"
#include <magic.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>


bool isPicture (const char * filename)
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
  
  char ** pictures = malloc(sizeof(char *));
  int nbPictures = 0;
  pictures[nbPictures] = NULL;

  struct dirent * entry;
  while ( entry = readdir(folder) )
  {
    if ( !strcmp(entry->d_name, "." ) ) continue;
    if ( !strcmp(entry->d_name, ".." ) ) continue;
    char * path = malloc(sizeof(char)*(strlen(foldername)+strlen(entry->d_name)+2));
    path[0] = '\0';
    strcat(path, foldername);
    strcat(path, "/");
    strcat(path, entry->d_name);
    if ( isPicture( path ) )
    {
      pictures[nbPictures] = strdup(path);
      nbPictures++;
      pictures = realloc(pictures, (nbPictures+1)*sizeof(char*));
      pictures[nbPictures] = NULL;
    }
    free(path);
  }

  closedir(folder);

  return pictures;
}


void freeList (char ** list)
{
  if ( !list ) return;

  for ( int i=0 ; list[i] ; i++ )
  {
    free(list[i]);
    list[i] = NULL;
  }
  
  free(list);
}
