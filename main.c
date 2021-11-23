#include "grenier.h"
#include <stdio.h>
#include <magic.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>


/// Print information on picture file.
///
/// @param picture The filename of the picture.
/// @param context The context of printPictureInformation() in the
///                foreachPicture() loop.

void printPictureInformation(void * picture, void * context)
{
  struct Picture ** album = *((struct Picture ***)context);
  struct Picture asset;

  int count = PictureListCount ( album );

  char * mime = MimeTypeGet ( picture );
  char * tags[] = { "MimeType", "ImageWidth", "ImageHeight", NULL };
  char ** exif = ExifGet ( picture, tags );
  off_t size = FileSizeGet ( picture );
  char * md5hash = FileMd5Get ( picture );

  printf("- (%s, :%s:%s:%s:, %zd, %s) %s\n",
      mime,
      exif[0], exif[1], exif[2],
      (intmax_t)size,
      md5hash,
      picture);

  asset.filename = strdup(picture);
  asset.mimetype = strdup(mime);
  asset.filesize = size;
  asset.md5hash = strdup(md5hash);
  asset.width = atoi(exif[1]);
  asset.height = atoi(exif[2]);

  album = PictureListAdd ( album, &asset );

  *((struct Picture ***)context) = album;

  free(md5hash);
  StringListFree(exif);
  free(mime);
}


/// Entry point of the application.
///
/// @param argc The number of command line arguments.
/// @param argv The list of command line arguments, starting with command name
///             at index #0.
/// @return The execution status of the application. 0 means _success_.
int main (int argc, char * argv[])
{
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
        album[i]->filename);
  }

  return 0;
}

// vim: set tw=79 ts=2 sw=2 sts=2 et ai si syn=c fo+=ro dip+=iwhite ff=unix:
