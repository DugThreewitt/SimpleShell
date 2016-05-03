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
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "func.h"

struct sigaction mainHandler, cmdHandler;
static void handleMainSignal( int ignore );

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
	char buff[PATH_MAX]; // buffer for getcwd command
	int cmdTokens, pathTokens, ps1Tokens, dirTokens, j = 0, i = 0, needDir = 0;

	mainHandler.sa_handler = handleMainSignal;
	sigemptyset(&mainHandler.sa_mask);
	sigaddset(&mainHandler.sa_mask, SIGINT);
	cmdHandler.sa_flags=0;
	sigaction(SIGINT, &mainHandler, NULL);

	if( ( usrPath = getenv("MYPATH") ) == NULL ) // check for MYPATH Env Var
	{
		usrPath = getenv("PATH");
	}

	if( (pathTokens = makeargv( usrPath, pathDelim, &pathArgs ) ) == -1 ) // put each part of path into it's own array element for searching through
	{
		fprintf(stderr, "Failed to find search path. Exiting . . . \n");
		return EXIT_FAILURE;
	}

	if(prompt != NULL) // parse prompt
	{
		for ( i = 0 ; i < strlen(prompt) ; i++)
		{
			if(prompt[i] == '\\')
			{
				if( prompt[i+1] == 'w' || prompt[i+1] == 'W' ) // set flag for dir so it can change in each loop
				{
					needDir = 1;
				}
			}
		} 

		ps1Tmp = parsePS( getenv("PS1") ); // parse \u \h \w \W \n out of PS1

		if( (ps1Tokens = makeargv( ps1Tmp, pathDelim, &ps1 ) ) == -1 ) // put each part of PS1 into it's own array element
		{
			fprintf(stderr, "Failed to parse PS1. Exiting . . . \n");
			return EXIT_FAILURE;
		}
	}

	while (1)
	{

		//signal(SIGINT, SIG_IGN);
		if(prompt != NULL) // if PS1 is set
		{
			if(needDir == 1)
			{
				dirTmp = getcwd(buff, PATH_MAX); //get current working directory, getenv("PWD") does not work, its the last directory from bash

				if( (dirTokens = makeargv( dirTmp, "/", &dir ) ) == -1 ) // parse directory out of /'s to print just current directory
				{
					fprintf(stderr, "Failed to parse PS1. Exiting . . . \n");
					return EXIT_FAILURE;
				}
			}
			

			for( j = 0 ; j < ps1Tokens; j++) // print prompt
			{
				if( strcmp(ps1[j], "DIR") == 0)
				{
					printf("%s ", dir[dirTokens - 1]);
					continue;
				}
				else
				{
					printf("%s ", ps1[j]);
				}
			}
			printf("$ ");
		}
		else // if PS1 is not set
		{
			printf("$ ");
		}

		input = readLine();  // call function to read in user input

		if( strcmp(input, "exit") == 0 ) // if exit, quit mysh
		{
			break;
		}
		

		if( ( cmdTokens = makeargv( input, cmdDelim, &myArgs) ) == -1 ) // build array with each arg in it's own element
		{
			fprintf(stderr, "Parsing of commands failed. Exiting . . .");
			return EXIT_FAILURE;
		}

		cmdPath = makeCmd(pathTokens, pathArgs, myArgs);// create command for execv
		
		if( strcmp(cmdPath, "Command not found" ) == 0 )
		{
			printf("%s: %s\n", myArgs[0],  cmdPath);
			continue;
		}
		else
		{	
			callCmd(cmdPath, myArgs); // call function that executes the command
		}


		free (input); // free input pointer
		free (cmdPath);	// free comPath pointer	
	}
	

}

static void handleMainSignal( int ignore )
{
	printf("Ctrl-C called and ignored\n");
}

