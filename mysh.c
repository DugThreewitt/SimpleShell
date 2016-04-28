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
	char ** ps1; //array to hold ps1 search
	const char * prompt = getenv("PS1"); // User prompt if defined in PS1
	const char * usrPath; // path to search from PATH or MYPATH environment variable
	char * ps1Tmp = malloc(sizeof(char) * MAX_CANON * 4); // to get string from ps1
	char ** dir ; //to hold dir from PS1, Mara wanted me to put her name in the program
	char * dirTmp = malloc(sizeof(char) * PATH_MAX);
	char buff[PATH_MAX];
	char dirPrint[128];
	int cmdTokens, pathTokens, ps1Tokens, dirTokens, j = 0, i = 0, needDir = 0;

	if( ( usrPath = getenv("MYPATH") ) == NULL ) // check for MYPATH Env Var
	{
		usrPath = getenv("PATH");
	}

	if( (pathTokens = makeargv( usrPath, pathDelim, &pathArgs ) ) == -1 )
	{
		fprintf(stderr, "Failed to find search path. Exiting . . . \n");
		return EXIT_FAILURE;
	}
	for ( i = 0 ; i < strlen(prompt) ; i++)
	{
		if(prompt[i] == '\\')
		{
			if( prompt[i+1] == 'w' || prompt[i+1] == 'W' )
			{
				needDir = 1;
			}
		}
	} 

	ps1Tmp = parsePS( getenv("PS1") );

	if( (ps1Tokens = makeargv( ps1Tmp, pathDelim, &ps1 ) ) == -1 )
	{
		fprintf(stderr, "Failed to parse PS1. Exiting . . . \n");
		return EXIT_FAILURE;
	}

	while (1)
	{
		if(prompt != NULL)
		{
			if(needDir == 1)
			{
				dirTmp = getcwd(buff, PATH_MAX);

				if( (dirTokens = makeargv( dirTmp, "/", &dir ) ) == -1 )
				{
					fprintf(stderr, "Failed to parse PS1. Exiting . . . \n");
					return EXIT_FAILURE;
				}
			}
			
			//printf("%s\n", ps1Tmp);

			for( j = 0 ; j < ps1Tokens; j++)
			{
				if( strcmp(ps1[j], "DIR") == 0)
				{
					printf("%s ", dir[dirTokens - 1]);
					continue;
				}
			/*	else if( strcmp(ps1[j], "\n") == 0 )
				{
					printf("\n");
					continue;
				}*/
				else
				{
					printf("%s ", ps1[j]);
				}
			}
			/*if(needDir == 1)
			{
				dir = getcwd( buff, PATH_MAX );
		        	printf("%s ", dir);
			} */
			printf("$ ");
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
			continue;
		}
		else
		{	
			callCmd(cmdPath, myArgs);
		}


		free (input);
		free (cmdPath);		
		//free (ps1Tmp);
//		free (ps1);
	}
	

}
