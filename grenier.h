#include <stdbool.h>


bool isPicture ( const char * filename );
bool isFolder ( const char * pathname );
bool MimeTypeMatches ( const char * filename, const char * expectedmime );
char * MimeTypeGet ( const char * filename );

void foreachPicture ( const char * foldername, void (*function)(void*,void*), void *context );
char ** ExifGet ( const char * filename, char ** tags );

char * StringAppend ( char * body, const char * tail );
char ** StringSplit ( const char * string, const char separator );
void StringListFree (char ** list);
int StringListCount ( char ** list );
char ** StringListAdd ( char ** list, const char * string );


// vim: set tw=79 ts=2 sw=2 sts=2 et ai si syn=c fo+=ro dip+=iwhite ff=unix:
