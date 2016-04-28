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

#define IN
#define OUT
#define INOUT

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

		if( c == EOF || c == '\n' )
		{
			input[pos] = '\0';	
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
		snprintf(cmdPath, (sizeof(char) * MAX_CANON), "%s/%s", pathArgs[i], myArgs[0]);

		//printf("cmdPath = %s\n", cmdPath);

		if(access(cmdPath, F_OK) == 0)
		{
			//printf("%s exists\n", cmdPath);
			if(access(cmdPath, X_OK) == 0)
			{
				//printf("%s can execute.\n", cmdPath);
				return cmdPath;
			}
		}
	}

	cmdPath = "Command not found";
	return cmdPath;



}


void callCmd ( char * cmdPath, char ** myArgs )
{
	pid_t child, childWait;
	int status;

	if( strcmp(myArgs[0], "cd") == 0 )
	{
		chdir(myArgs[1]);
		return;
	}
	else
	{

		child = fork();

		if( child == 0 )
		{
			execv(cmdPath, myArgs) < 0;
		
			fprintf(stderr, "Failed to execute\n");
			return;
		}
		else
		{
			do
			{
				childWait = wait(&status);
				if( childWait != child)
					fprintf(stderr, "Process Terminated.\n");
			} while (childWait != child);
		}	
	}
}

char * parsePS( const char * prompt )
{
	int i;
	char * ps1=malloc( sizeof(char) * MAX_CANON * 4 );
	char buf[PATH_MAX];
	char * dir;

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
					dir = getcwd(buf, PATH_MAX);
					strcat(ps1, dir);
					strcat(ps1, ":");
					break;
				case 'n':
					strcat(ps1, "/n");
					strcat(ps1, ":");
					break;
			}
		}
	}	
	

	return ps1;

}
