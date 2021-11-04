#include <stdbool.h>


/// Check if a file is a picture.
///
/// @param filename The name of the file to check.
/// @return _true_ if the file is a picture, _false_ otherwise.

bool isPicture (const char * filename);


/// Find picture files in a folder.
///
/// @param foldername The name of the folder where to find pictures.
/// @return A list of file names that are pictures.

char ** findPictures (const char * foldername);


/// Free memory held by an array of strings.
///
/// @param list The array of strings to be freed

void freeList (char ** list);
