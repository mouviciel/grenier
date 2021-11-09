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


int ExifToolInit ( void )
{
  pid_t exiftoolpid = fork();

  if ( exiftoolpid == -1 )
  {
    // fork() failed
    fprintf(stderr, "DEBUGJBO: fork() failed: %s\n", strerror(errno));
    return -1;
  }
  else if ( exiftoolpid == 0 )
  {
    // Child process: launch exiftool
    printf("DEBUGJBO: Child here: spawning ExifTool\n");
    char * argv[] = { "-stay_open", "true", "-@", "-", NULL };
    char * envp[] = { NULL };
    execve("exiftool", argv, envp);
    // execve returns only if it fails
    fprintf(stderr, "DEBUGJBO: execve() failed: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  else
  {
    // Parent process: launch watchdog
    printf("DEBUGJBO: Parent here: launching Watchdog\n");
    pid_t grenierpid = getpid();
    pid_t watchdogpid = fork();
    if ( watchdogpid == 0 )
    {
      // Watchdog process: kill ExifTool process as soon as Grenier process
      // vanishes
      printf("DEBUGJBO: Watchdog here: monitoring Grenier\n");
      for (;;)
      {
        sleep(1);
        if ( getppid() != grenierpid )
        {
          kill ( exiftoolpid, SIGINT );
          printf("DEBUGJBO: Watchdog here: Grenier vanished, killing ExifTool\n");
          exit(0);
        }
      }
    }
  }
}
