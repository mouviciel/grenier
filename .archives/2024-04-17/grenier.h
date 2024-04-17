#ifndef GRENIER_H
#define GRENIER_H


#include <stdbool.h>
#include <sys/types.h>


/// @name Operations on pictures and arrays of pictures
/// @{

/// Picture data structure.
///
/// This data structure gathers information about a picture stored in a
/// file.

struct Picture
{
  char * pathname;
  char * mimetype;
  off_t filesize;
  char * md5hash;
  int width;
  int height;
};


/// Retrieve picture information.
///
/// @param pathname The pathname of a picture file.
/// @return A pointer to a new Picture data structure containing all relevant
///         information about the picture file. If the file does not exist, is
///         not readable or is not a picture, _NULL_ is returned. It is the
///         caller responsibility to free memory by calling PictureFree().

struct Picture * PictureInformation ( const char * pathname );


/// Free memory held by a Picture data structure.
///
/// @param picture A pointer to a dynamically allocated Picture data structure.

void PictureFree ( struct Picture * picture );


/// Count the number of pictures in a list of pictures.
///
/// @param list The list to count.
/// @return The number of pictures in the list. If the list is _NULL_, the
///         count is 0;
///
/// A list of pictures is an array of pointers to Picture data structures
/// terminated by a _NULL_ pointer.

int PictureListCount ( struct Picture ** list );


/// Add a picture at the end of a list of pictures.
///
/// @param list The list of pictures to which the picture is added.
/// @param picture The picture to add to the list of pictures.
/// @return The modified list of pictures.

struct Picture ** PictureListAdd ( struct Picture ** list, const struct Picture * picture );


/// Free memory held by a list of pictures.
///
/// @param list A pointer to a dynamically allocated array of pointers to
///             dynamically allocated Picture data structures.

void PictureListFree ( struct Picture ** list );


/// @}


bool isPicture ( const char * pathname );
bool isFolder ( const char * pathname );
bool MimeTypeMatches ( const char * pathname, const char * expectedmime );
char * MimeTypeGet ( const char * pathname );
off_t FileSizeGet ( const char * pathname );
char * FileMd5Get ( const char * pathname );

void foreachPicture ( const char * pathname, void (*function)(void*,void*), void *context );
char ** ExifGet ( const char * pathname, const char ** tags );

char * StringAppend ( char * body, const char * tail );
char ** StringSplit ( const char * string, const char separator );
void StringListFree (char ** list);
int StringListCount ( char ** list );
char ** StringListAdd ( char ** list, const char * string );


#endif // GRENIER_H


// vim: set tw=79 ts=2 sw=2 sts=2 et ai si syn=c fo+=ro dip+=iwhite ff=unix:
