/* ************************************************************************ *
 * util.c
 * functions for mysh.c
 * Dug Threewitt - dstgh6
 * CS2750 - Mon/Wed
 * *********************************************************************** */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <memory.h>
#include <limits.h>
#include <signal.h>
#include "func.h"

#define IN
#define OUT
#define INOUT

struct sigaction cmdHandler, mainHandler;

int makeargv (
    IN	const char *   s, 		// String to be parsed
    IN	const char *   delimiters,	// String of delimiters
    OUT       char *** argvp		// Argument array
    )
{
    int		error;
    int		i;
    int		numtokens;
    const char * snew;
    char       * t;

    // Make sure that none of the inputs is NULL

    if ( ! ( s && delimiters && argvp ) )
    {
	errno = EINVAL;
	return ( -1 );
    }

    *argvp = NULL;
    snew = s + strspn ( s, delimiters );	// Real start of string

    t = ( char * ) malloc ( strlen ( snew ) + 1 );
    if ( !t )
    {
	fprintf ( stderr, "Error allocating memory\n" );
	return ( -1 );
    }

    strcpy ( t, snew );
    numtokens = 0;

    // Count the number of tokens

    if ( strtok ( t, delimiters ) )
	for ( numtokens = 1; strtok ( NULL, delimiters ); numtokens++ );

    // Create argument array for pointers to tokens

    *argvp = ( char ** ) malloc ( ( numtokens + 1 ) * sizeof ( char * ) );
    error = errno;
    if ( ! *argvp )
    {
	free ( t );
	errno = error;
	return ( -1 );
    }

    if ( ! numtokens )
	free ( t );
    else
    {
	strcpy ( t, snew );
	**argvp = strtok ( t, delimiters );
	for ( i = 1; i < numtokens; i++ )
	{
	    *((*argvp) + i ) = strtok ( NULL, delimiters );
	}
    }
    *(( *argvp ) + numtokens ) = NULL;
    return ( numtokens );
}


void freemakeargv (
    INOUT char ** argv
    )
{
    if ( ! argv )
	return;

    if ( ! *argv )
	free ( * argv );

    free ( argv );
}

char * readLine()
{
	char c;
	char * input = malloc(sizeof(char) * MAX_CANON);
	int pos = 0;

	while (1)
	{
		c = getchar();

		if( c == EOF || c == '\n' ) // user is done entering input
		{
			input[pos] = '\0'; // ends input string	
			return input;
		}
		else
		{
			input[pos] = c;
		}
		pos ++;
	}

}

char * makeCmd ( int pathTokens, char ** pathArgs, char ** myArgs )
{
	char * cmdPath = malloc( sizeof(char) * MAX_CANON ); 
	int i = 0;

	for( i ; i < pathTokens ; i++ )
	{
		strcpy(cmdPath, pathArgs[i]);
		snprintf(cmdPath, (sizeof(char) * MAX_CANON), "%s/%s", pathArgs[i], myArgs[0]); //instead of multiple strcat's


		if(access(cmdPath, F_OK) == 0) // check if file exists
		{
			if(access(cmdPath, X_OK) == 0) // check if file is executable
			{
				return cmdPath;
			}
		}
	}

	cmdPath = "Command not found";
	return cmdPath;



}

/*static void handleMainSignal( int ignore )
{
	printf("Ctrl-C called and ignored\n");
}*/

static void handleChildSignal( pid_t child )
{
	int status;

	if ( child > 0 )
	{
		printf("Child pid: %d Ctrl-C called\n", child);
		kill(child, SIGKILL);
		waitpid(child, status);
	}

	sigaction(SIGINT, &mainHandler, NULL);
}

void callCmd ( char * cmdPath, char ** myArgs )
{
	pid_t child, childWait;
	int status;

	cmdHandler.sa_handler = handleChildSignal;
	sigemptyset(&cmdHandler.sa_mask);
	sigaddset(&cmdHandler.sa_mask, SIGINT);
	cmdHandler.sa_flags=0;

	sigaction(SIGINT, &cmdHandler, &mainHandler);

	if( strcmp(myArgs[0], "cd") == 0 ) // if user calls cd to change directory, don't fork
	{
		if(chdir(myArgs[1]) != 0)
			fprintf(stderr, "Directory not available\n");
		return;
	}
	else // fork new process
	{
		child = fork();
		

		if( child == 0 )
		{

			//signal(SIGINT, SIG_DFL);
			execv(cmdPath, myArgs) < 0;
		
			fprintf(stderr, "Failed to execute\n");
			return;
		}
		else
		{
			do
			{
				childWait = wait(&status);
				if( childWait != child )
					fprintf(stderr, "Process Terminated.\n");
			} while (childWait != child);
		}	
	}
}

char * parsePS( const char * prompt )
{
	int i;
	char * ps1=malloc( sizeof(char) * MAX_CANON * 4 );

	for( i ; i < strlen(prompt) ; i++)
	{
		if(prompt[i] == '\\')
		{
			switch(prompt[i+1])
			{
				case 'u':
					strcat(ps1, getenv("USER"));
					strcat(ps1, ":");
					break;
				case 'h':
					strcat(ps1, getenv("HOSTNAME"));
					strcat(ps1, ":");
					break;
				case 'w':
				case 'W':
					strcat(ps1, "DIR:");//use DIR to replace in main
					break;
				case 'n':
					strcat(ps1, "\n");
					strcat(ps1, ":");
					break;
			}
		}
	}	
	

	return ps1;

}
