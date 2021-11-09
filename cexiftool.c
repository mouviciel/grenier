#include "cexiftool.h"
#include "grenier.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>


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


void ExifToolLaunch ( void )
{
  pid_t pid = fork();

  if ( pid == 0 )
  {
    // Child process: launch ExifTool
    printf("DEBUGJBO: Child here: launching ExifTool\n");
    execlp("exiftool", "-stay_open", "true", "-@", "-", NULL);
    // execlp returns only if it fails
    exit(EXIT_FAILURE);
  }
  else
  {
    // Parent process: launch Watchdog
    printf("DEBUGJBO: Grenier here: launching Watchdog\n");
    pid_t ppid = getpid();
    if ( fork() == 0 )
    {
      // Watchdog process: kill ExifTool as soon as Grenier vanishes
      printf("DEBUGJBO: Watchdog here: monitoring Grenier\n");
      extern char * __progname;
      __progname = "WatchDog4Grenier";
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
