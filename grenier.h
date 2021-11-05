#include <stdbool.h>


bool isPicture (const char * filename);
bool isFolder (const char * pathname);
void foreachPicture (const char * foldername, void (*function)(void*,void*), void *context);


bool MimeTypeMatches ( const char * filename, const char * expectedmime );


char * StringAppend(char * body, const char * tail);
