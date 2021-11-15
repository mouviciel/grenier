#include "cexiftool.h"
#include "grenier.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/time.h>


char * exiftool ( char * argv[] )
{
  char * command = NULL;

  command = StringAppend(command, "exiftool -m -q -q");

  for ( int i = 0 ; argv[i] ; i++ )
  {
    command = StringAppend(command, " ");
    command = StringAppend(command, argv[i]);
  }

  FILE * CommandOutput = popen(command, "r");
  char * result = NULL;
  if ( CommandOutput )
  {
    char buffer[1000];
    while ( fgets(buffer, sizeof(buffer), CommandOutput) )
    {
      char * newline = strrchr(buffer, '\n');
      newline[0] = '\0';
      result = StringAppend(result, buffer);
    }
  }
  free(command);
  wait(NULL);
  return result;
}


static int childin[2];
static int childout[2];
enum
{
  PIPE_OUT = 0,
  PIPE_IN = 1
};


void ExifToolLaunch ( void )
{
  pipe(childin);
  pipe(childout);

  pid_t pid = fork();

  if ( pid == 0 )
  {
    // Child process: launch ExifTool
    printf("DEBUGJBO: Child here: launching ExifTool\n");
    // Set up parent<->child pipes for child
    close(childin[PIPE_IN]);
    close(childout[PIPE_OUT]);
    // Map child's stdin & stdout to parent<->child pipes
    dup2(childin[PIPE_OUT], STDIN_FILENO);
    close(childin[PIPE_OUT]);
    dup2(childout[PIPE_IN], STDOUT_FILENO);
    close(childout[PIPE_IN]);
    // Launch ExifTool
    execlp("exiftool", "exiftool", "-stay_open", "true", "-@", "-", NULL);
    // execlp returns only if it fails
    exit(EXIT_FAILURE);
  }
  else
  {
    // Parent process: launch Watchdog
    printf("DEBUGJBO: Grenier here: launching Watchdog\n");
    // Set up parent<->child pipes for parent
    close(childin[PIPE_OUT]);
    close(childout[PIPE_IN]);
    int flags = fcntl(childout[PIPE_OUT], F_GETFL, 0);
    fcntl(childout[PIPE_OUT], F_SETFL, flags | O_NONBLOCK);
    // Launch Watchdog
    pid_t ppid = getpid();
    if ( fork() == 0 )
    {
      // Watchdog process: kill ExifTool as soon as Grenier vanishes
      printf("DEBUGJBO: Watchdog here: monitoring Grenier\n");
      for (;;)
      {
        sleep(1);
        if ( getppid() != ppid )
        {
          kill ( pid, SIGINT );
          printf("DEBUGJBO: Watchdog here: Grenier vanished, killing ExifTool\n");
          exit(0);
        }
      }
    }
    printf("DEBUGJBO: Grenier here: going on\n");
  }
}


char * ExifToolGet ( char * file, char * name )
{
  size_t len;
  char * buf;

  buf = file;
  len = strlen(buf);
  write(childin[PIPE_IN], buf, len);

  buf = "\n-m\n-q\n-q\n-p\n";
  len = strlen(buf);
  write(childin[PIPE_IN], buf, len);

  buf = name;
  len = strlen(buf);
  write(childin[PIPE_IN], buf, len);

  buf = "\n-execute\n-execute\n";
  len = strlen(buf);
  write(childin[PIPE_IN], buf, len);

  const size_t maxlen = 1000;
  buf = calloc(sizeof(char), maxlen);

  fd_set readfds;
  int nread = 0;
  char * pread = buf;
  for (;;)
  {
    FD_ZERO(&readfds);
    FD_SET(childout[PIPE_OUT], &readfds);
    select (childout[PIPE_OUT]+1, &readfds, NULL, NULL, NULL);
    nread = read(childout[PIPE_OUT], pread, maxlen);
    char * ready = strstr(buf, "{ready}");
    if ( ready )
    {
      *ready = '\0';
      break;
    }
    pread += nread;
  }
  char * newline = strrchr(buf, '\n');
  if ( newline )
  {
    *newline = '\0';
  }

  char * value = strdup(buf);
  free(buf);
  return value;
}
