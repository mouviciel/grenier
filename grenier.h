#include <stdbool.h>
#include <sys/types.h>


bool isPicture ( const char * filename );
bool isFolder ( const char * pathname );
bool MimeTypeMatches ( const char * filename, const char * expectedmime );
char * MimeTypeGet ( const char * filename );
off_t FileSizeGet ( const char * filename );
char * FileMd5Get ( const char * filename );

void foreachPicture ( const char * foldername, void (*function)(void*,void*), void *context );
char ** ExifGet ( const char * filename, char ** tags );
struct Picture
{
  char * filename;
};
int PictureListCount ( struct Picture ** list );
struct Picture ** PictureListAdd ( struct Picture ** list, struct Picture * picture );

char * StringAppend ( char * body, const char * tail );
char ** StringSplit ( const char * string, const char separator );
void StringListFree (char ** list);
int StringListCount ( char ** list );
char ** StringListAdd ( char ** list, const char * string );


// vim: set tw=79 ts=2 sw=2 sts=2 et ai si syn=c fo+=ro dip+=iwhite ff=unix:
