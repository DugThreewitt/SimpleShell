/* ************************************************************************ *
 * mysh.c
 * main for mysh.c
 * Dug Threewitt - dstgh6
 * CS2750 - Mon/Wed
 * *********************************************************************** */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "func.h"

int main ()
{
	char c;
	char * input; // gets input from user
	char cmdDelim[] = " \t"; // seperators for commands
	char pathDelim[] = ":"; // seperator for PATH environment variable
	char * cmdPath; // full path with command to be sent to exec
	char ** myArgs; // cmd and args array of strings
	char ** pathArgs; // array of paths to search
	const char * prompt = getenv("PS1"); // User prompt if defined in PS1
	const char * usrPath; // path to search from PATH or MYPATH environment variable
	int cmdTokens, pathTokens, i = 0, pos = 0;

	if( ( usrPath = getenv("MYPATH") ) == NULL ) // check for MYPATH Env Var
	{
		usrPath = getenv("PATH");
	}

	if( (pathTokens = makeargv( usrPath, pathDelim, &pathArgs ) ) == -1 )
	{
		fprintf(stderr, "Failed to find search path. Exiting . . . \n");
		return EXIT_FAILURE;
	}

	while (1)
	{
		if(prompt != NULL)
		{
			printf("%s ", prompt);
		}
		else
		{
			printf("$ ");
		}

		input = readLine();

		if( strcmp(input, "exit") == 0 )
		{
			break;
		}
		
		if( ( cmdTokens = makeargv( input, cmdDelim, &myArgs) ) == -1 )
		{
			fprintf(stderr, "Parsing of commands failed. Exiting . . .");
			return EXIT_FAILURE;
		}

		cmdPath = makeCmd(pathTokens, pathArgs, myArgs);
		
		if( strcmp(cmdPath, "Command not found" ) == 0 )
		{
			printf("%s: %s\n", myArgs[0],  cmdPath);
		//	free (cmdPath);
			continue;
		}
		else
		{	
		//	printf("cmdPath in Main = %s\n", cmdPath);
			callCmd(cmdPath, myArgs);
		}
/*
		for( i = 0 ; i < cmdTokens ; i++)
		{
			printf("cmd %d: %s\n", i, myArgs[i]  );
		}
*/
		free (input);
		free (cmdPath);		
	}
	

}
