#include <stdbool.h>


bool isPicture (const char * filename);
bool isFolder (const char * pathname);
char ** findPictures (const char * foldername);


bool MimeTypeMatches ( const char * filename, const char * expectedmime );

void StringListFree (char ** list);
int StringListCount(char ** list);
char ** StringListAdd(char ** list, const char * string);
char ** StringListAppend(char ** body, char ** tail);
char ** StringListDup(char ** list);


char * StringAppend(char * body, const char * tail);
