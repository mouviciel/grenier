#include "grenier.h"
#include <magic.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <openssl/md5.h>


///
/// @name Exiftool query engine
///    A collection of variables and functions to retrieve EXIF metadata from
///    picture files.
/// @{
///

/// The process ID of the _exiftool_ process. Its value is 0 when the process
/// has not been launched.

static pid_t ExifToolPid = 0;


/// The pipe channel from parent process (_Grenier_) to child (_exiftool_).

static int ExifToolIn[2];


/// The pipe channel from child process (_exiftool_) to parent (_Grenier_).

static int ExifToolOut[2];


/// How to name pipe bounds

enum PipeBounds
{
  PIPE_OUT = 0, ///< The outbound of a pipe channel
  PIPE_IN = 1   ///< The inbound of a pipe channel
};


/// Launch _exiftool_ process if it has not been launched yet.
///
/// Moreover a watchdog process is also launched to monitor the _Grenier_
/// process and kill _exiftool_ whenever _Grenier_ terminates.

static void ExifToolLaunch(void)
{
  if ( ExifToolPid != 0 )
  {
    return;
  }

  // First a child process is created by cloning the current process. Before
  // that, two pipes are created for parent <--> child communication.
  pipe(ExifToolIn);
  pipe(ExifToolOut);
  ExifToolPid = fork();

  if ( ExifToolPid == -1 )
  {
    // In case of error in spawning the child process, the program terminates.
    fprintf(stderr, "Error while creating child process: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  if ( ExifToolPid == 0 )
  {
    // The child process closes the bounds of the pipes that it doesn't need
    // and maps the other ones to its standard input and output.
    close(ExifToolIn[PIPE_IN]);
    close(ExifToolOut[PIPE_OUT]);

    dup2(ExifToolIn[PIPE_OUT], STDIN_FILENO);
    close(ExifToolIn[PIPE_OUT]);

    dup2(ExifToolOut[PIPE_IN], STDOUT_FILENO);
    close(ExifToolOut[PIPE_IN]);

    // Then it replaces its process image with a new _exiftool_ process image.
    execlp ( "exiftool", "exiftool", "-stay_open", "true", "-@", "-", NULL);
    
    // In case of error in launching _exiftool_, the program terminates.
    fprintf(stderr, "Error while launching exiftool process: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  if ( ExifToolPid > 0 )
  {
    // The parent process first closes the bounds of the pipes that it doesn't
    // need and sets the one that it reads from the child as non-blocking.
    close(ExifToolIn[PIPE_OUT]);
    close(ExifToolOut[PIPE_IN]);

    int flags = fcntl(ExifToolOut[PIPE_OUT], F_GETFL, 0);
    fcntl(ExifToolOut[PIPE_OUT], F_SETFL, flags | O_NONBLOCK);

    // Then it launches a watchdog process that will be in charge of killing
    // the _exiftool_ process, should the parent unexpectedly terminate.
    pid_t GrenierPid = getpid();
    pid_t WatchdogPid = fork();

    if ( WatchdogPid == -1 )
    {
      // In case of error in spawning the watchdog process, the _exiftool_
      // process is killed and the program terminates.
      fprintf(stderr, "Error while launching watchdog process: %s\n", strerror(errno));
      kill(ExifToolPid, SIGINT);
      exit(EXIT_FAILURE);
    }

    if ( WatchdogPid == 0 )
    {
      // The watchdog process periodically checks it parent process ID.
      for (;;)
      {
        sleep(1);
        // If it is different from the _Grenier_ parent process, it kills the
        // _exiftool_ child process and terminates.
        if ( getppid() != GrenierPid )
        {
          kill(ExifToolPid,SIGINT);
          exit(EXIT_SUCCESS);
        }
      }
    }
  }
}


/// Append a tag subquery to an ExifTool query
///
/// @param query The query to append the subquery to.
/// @param tag The tag name to query.
/// @return The modified query.

char * ExifToolQueryAppend ( char * query, const char * tag )
{
  if ( !query )
  {
    /// If the input query is NULL, it is created with global exiftool options.
    query = StringAppend ( query, "-m\n-q\n-q\n" );
  }

  if ( !tag )
  {
    return query;
  }

  query = StringAppend ( query, "-p\n$" );
  query = StringAppend ( query, tag );
  query = StringAppend ( query, "\n" );
}


/// Submit an ExifTool query to a picture file
///
/// @param filename The name of the file to check.
/// @param query The query to submit (see ExifToolQueryAppend() to build a
///              query).
/// @return A new string containing the tags values separated by newlines.

char * ExifToolQuerySubmit ( const char * filename, const char * query )
{
  if ( !filename || !query )
  {
    return NULL;
  }

  // Launch _exiftool_ process if it has not been launched yet.
  ExifToolLaunch();

  // Write the filename to _exiftool_ stdin.
  int nwritten;
  nwritten = write(ExifToolIn[PIPE_IN], filename, strlen(filename));
  if ( nwritten == -1 )
  {
    fprintf(stderr, "Error while writing filename to exitool process: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  nwritten = write(ExifToolIn[PIPE_IN], "\n", 1);
  if ( nwritten == -1 )
  {
    fprintf(stderr, "Error while writing newline to exitool process: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  // Write the query to _exiftool_ stdin.
  nwritten = write(ExifToolIn[PIPE_IN], query, strlen(query));
  if ( nwritten == -1 )
  {
    fprintf(stderr, "Error while writing query to exitool process: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  nwritten = write(ExifToolIn[PIPE_IN], "-execute\n", 9);
  if ( nwritten == -1 )
  {
    fprintf(stderr, "Error while writing command to exitool process: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  nwritten = write(ExifToolIn[PIPE_IN], "-execute\n", 9);
  if ( nwritten == -1 )
  {
    fprintf(stderr, "Error while writing command to exitool process: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  // Read the results from _exiftool_ stdout.
  char * result = NULL;
  const size_t bufsize = 1000;
  char * rbuf = malloc(sizeof(char)*bufsize);
  for (;;)
  {
    // Wait for _exiftool_ stdout to be ready.
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(ExifToolOut[PIPE_OUT], &rfds);
    int nfds = select(ExifToolOut[PIPE_OUT]+1, &rfds, NULL, NULL, NULL);
    if ( nfds == -1 )
    {
      fprintf(stderr, "Error while waiting for exitool process response: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }

    // Read from _exiftool_ stdout.
    int nread = read(ExifToolOut[PIPE_OUT], rbuf, bufsize-1);
    if ( nread == -1 )
    {
      fprintf(stderr, "Error while reading from exiftool process: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    if ( nread == 0 )
    {
      fprintf(stderr, "Error: unexpected EOF while reading from exiftool process\n");
      exit(EXIT_FAILURE);
    }
    rbuf[nread] = '\0';
    result = StringAppend ( result, rbuf );

    // Look for end of _exiftool_ processing.
    char * ready = strstr(result, "{ready}");
    if ( ready )
    {
      *ready = '\0';
      break;
    }
  }

  return result;
}


///
/// @}
///


///
/// @name File information
///    These functions retrieve information on files.
/// @{
///


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

  char * actualmime = MimeTypeGet ( filename );

  /// The file matches if its MIME type starts with expected MIME type.
  bool result = ( strncmp (actualmime, expectedmime, strlen(expectedmime)) == 0 );

  free(actualmime);

  return result;
}


/// Get MIME type of a file
///
/// @param filename The file name to query.
/// @return A new string containing the file's MIME type, or _NULL_ if an error
///         arises.

char * MimeTypeGet ( const char * filename )
{
  if ( !filename )
  {
    return NULL;
  }

  /// The function uses the **libmagic(3)** library to get the MIME type of
  /// the file.
  magic_t query = magic_open (MAGIC_MIME_TYPE);
  magic_load (query, NULL);
  char * mime = strdup ( magic_file (query, filename) );
  magic_close (query);

  return mime;
}


/// Get file size.
///
/// @param filename The file name to query.
/// @return The file size in bytes.

off_t FileSizeGet ( const char * filename )
{
  if ( !filename )
  {
    fprintf(stderr, "Error can't get file size of NULL\n");
    exit(EXIT_FAILURE);
  }

  struct stat buf;

  if ( stat(filename, &buf) == -1 )
  {
    fprintf(stderr, "Error while retrieving information about file \"%s\": %s\n",
        filename, strerror(errno));
    return -1;
  }

  return buf.st_size;
}


/// Get file MD5 hash.
///
/// @param filename The file name to query.
/// @return The MD5 hash expressed in hexadecimal and stored in a string.

char * FileMd5Get ( const char * filename )
{
  if ( !filename )
  {
    return NULL;
  }

  FILE * f = fopen(filename, "rb");
  if ( !f )
  {
    fprintf(stderr, "Error while opening file \"%s\": %s\n", filename, strerror(errno));
    exit(EXIT_FAILURE);
  }

  const int bufferSize = 1024;
  unsigned char buffer[bufferSize];
  unsigned char result[MD5_DIGEST_LENGTH];
  MD5_CTX context;
  int nread;

  MD5_Init (&context);
  while ((nread = fread (buffer, 1, bufferSize, f)) != 0)
  {
    MD5_Update (&context, buffer, nread);
  }
  MD5_Final (result, &context);

  fclose(f);

  char * md5hex = NULL;
  for ( int i = 0; i < MD5_DIGEST_LENGTH; i++)
  {
    char hex[3];
    snprintf(hex, sizeof(hex), "%02x", result[i]);
    md5hex = StringAppend ( md5hex, hex );
  }

  return md5hex;
}


///
/// @}
///


///
/// @name Pictures
///   These functions locate picture files and retrieve information about them.
/// @{
///


/// Iterate over pictures in a folder and run a function on each picture found.
///
/// @param foldername The name of the folder where to find pictures.
/// @param function The name of the function to run.
/// @param context A pointer to some context needed by @b function.

void foreachPicture (
    const char * foldername,
    void ( * function ) ( void *, void * ),
    void * context )
{
  if ( isPicture (foldername) )
  {
    char * path = strdup(foldername);
    function(path,context);
    free(path);
    return;
  }

  DIR * folder = opendir(foldername);
  if ( !folder )
  {
    return;
  }
  
  struct dirent * entry;
  while ( ( entry = readdir(folder) ) )
  {
    if ( !strcmp(entry->d_name, "." ) ) continue;
    if ( !strcmp(entry->d_name, ".." ) ) continue;
    char* path = NULL;
    path = StringAppend(path,foldername);
    path = StringAppend(path,"/");
    path = StringAppend(path,entry->d_name);
    if ( isPicture( path ) )
    {
      function(path,context);
    }
    if ( isFolder( path ) )
    {
      foreachPicture(path, function, context);
    }
    free(path);
  }

  closedir(folder);

  return;
}


/// Get various EXIF metadata from a picture file
///
/// @param filename The name of the file to check.
/// @param tags A list of tag names to fetch from the picture.
/// @return A list of tag values, in the same order as the list of tag names.

char ** ExifGet ( const char * filename, const char ** tags )
{
  if ( !filename || !tags )
  {
    return NULL;
  }

  // A query is built from the tag list passed in argument.
  char * query = NULL;
  for ( int i = 0 ; tags[i] ; i++ )
  {
    query = ExifToolQueryAppend ( query, tags[i] );
  }

  // The result is a newline-separated list of values in a single string.
  char * result = ExifToolQuerySubmit ( filename, query );
  char ** values = StringSplit ( result, '\n' );
  free(result);
  free(query);

  return values;
}


///
/// @}
///


///
/// @name Strings and string lists
///   These functions handle strings and string lists, including memory allocation.
/// @{
///


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
  body = realloc(body, sizeof(char)*(newlen+1));
  body[newlen] = '\0';
  for ( size_t i = 0 ; i < taillen ; i++ )
  {
    body[i+bodylen] = tail[i];
  }

  return body;
}


/// Split a string to a list of strings.
///
/// @param string The string to be splitted. This string is left untouched.
/// @param separator A character specifying a separator.
/// @return A list of strings.

char ** StringSplit ( const char * string, const char separator )
{
  if ( !string )
  {
    return NULL;
  }

  if ( !separator )
  {
    return StringListAdd ( NULL, string );
  }

  char ** list = NULL;
  const char * item = string;

  while ( item )
  {
    const char * endofitem = strchr(item, separator);
    if ( !endofitem )
    {
      endofitem = strchr(item, '\0');
    }
    char * clone = strndup(item, endofitem - item);
    list = StringListAdd ( list, clone );
    item = endofitem + 1;
    if ( !(*endofitem) )
    {
      item = NULL;
    }
  }

  return list;
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

int StringListCount ( char ** list )
{
  int count = 0;

  if ( !list )
  {
    return count;
  }

  while ( list[count] )
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

char ** StringListAdd ( char ** list, const char * string )
{
  if ( !string )
  {
    return list;
  }

  int nitems = StringListCount(list)+1;
  list = realloc(list, sizeof(char*)*(nitems+1));
  list[nitems] = NULL;

  list[nitems-1] = strdup(string);

  return list;
}


///
/// @}
///


struct Picture * PictureInformation ( const char * pathname )
{
  if ( !pathname )
  {
    return NULL;
  }

  struct Picture * picture = calloc(1, sizeof(struct Picture));

  picture->filesize = FileSizeGet ( pathname );
  if ( picture->filesize < 0 )
  {
    PictureFree(picture);
    return NULL;
  }

  picture->pathname = strdup(pathname);

  picture->mimetype = MimeTypeGet ( pathname );
  const char * expectedmime = "image/";
  if ( strncmp (picture->mimetype, expectedmime, strlen(expectedmime)) != 0 )
  {
    PictureFree(picture);
    return NULL;
  }

  picture->md5hash = FileMd5Get ( pathname );

  const char * tags[] = { "ImageWidth", "ImageHeight", NULL };
  char **exif = ExifGet ( pathname, tags );
  picture->width = atoi(exif[0]);
  picture->height = atoi(exif[1]);
  StringListFree ( exif );

  return picture;
}


void PictureFree ( struct Picture * picture )
{
  if ( picture )
  {
    free(picture->pathname);
    free(picture->mimetype);
    free(picture->md5hash);
  }

  free(picture);
}


int PictureListCount ( struct Picture ** list )
{
  int count = 0;

  if ( !list )
  {
    return count;
  }

  while ( list[count] )
  {
    count++;
  }

  return count;
}


struct Picture ** PictureListAdd ( struct Picture ** list, const struct Picture * picture )
{
  if ( !picture )
  {
    return list;
  }

  int nitems = PictureListCount ( list ) + 1;
  list = realloc(list, sizeof(struct Picture *)*(nitems+1));
  list[nitems] = NULL;

  list[nitems-1] = calloc(1, sizeof(struct Picture));
  list[nitems-1]->pathname = strdup(picture->pathname);
  list[nitems-1]->mimetype = strdup(picture->mimetype);
  list[nitems-1]->filesize = picture->filesize;
  list[nitems-1]->md5hash = strdup(picture->md5hash);
  list[nitems-1]->width = picture->width;
  list[nitems-1]->height = picture->height;

  return list;
}


// vim: set tw=79 ts=2 sw=2 sts=2 et ai si syn=c fo+=ro dip+=iwhite ff=unix:
