#include "grenier.h"
#include <stdio.h>
#include <magic.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include "Obj.h"
#include "Number.h"


/// Print information on picture file.
///
/// @param pathname The filename of the picture.
/// @param context The context of printPictureInformation() in the
///                foreachPicture() loop.

void printPictureInformation(void * pathname, void * context)
{
  struct Picture ** album = *((struct Picture ***)context);

  struct Picture * picture = PictureInformation ( pathname );

  printf("- (%s, :%d:%d:, %zd, %s) %s\n",
      picture->mimetype,
      picture->width,
      picture->height,
      picture->filesize,
      picture->md5hash,
      picture->pathname);

  album = PictureListAdd ( album, picture );

  PictureFree ( picture );

  *((struct Picture ***)context) = album;
}


/// Entry point of the application.
///
/// @param argc The number of command line arguments.
/// @param argv The list of command line arguments, starting with command name
///             at index #0.
/// @return The execution status of the application. 0 means _success_.
int main (int argc, char * argv[])
{
  Obj myobj = Obj_create ();
  printf ( "myobj: %s\n", Obj_string ( myobj ) );
  Obj_destroy ( myobj );

  Number mynum = Number_create ();
  printf ( "mynum: %s\n", Obj_string ( (Obj)mynum ) );
  Obj_destroy ( (Obj)mynum );

  struct Picture ** album = NULL;

  /// The application scans arguments and prints them only if the file is
  /// a picture. If an argument is a directory, the application scans its
  /// contents and prints directory entries that are pictures.
  for (int i=1 ; argv[i] ; i++ )
  {
    foreachPicture(argv[i], printPictureInformation, &album);
  }

  for ( int i=0 ; i<PictureListCount(album) ; i++ )
  {
    printf("%s, %zd, %dx%d, %s --- %s\n",
        album[i]->mimetype,
        (intmax_t)album[i]->filesize,
        album[i]->width,
        album[i]->height,
        album[i]->md5hash,
        album[i]->pathname);
  }

  PictureListFree ( album );

  return 0;
}

// vim: set tw=79 ts=2 sw=2 sts=2 et ai si syn=c fo+=ro dip+=iwhite ff=unix:
