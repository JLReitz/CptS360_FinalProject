#ifndef UTIL_C
#define UTIL_C

#include "Type.h"

#define NUM_0ARG_FUNCTIONS 2
#define NUM_1ARG_FUNCTIONS 2

// GLobal Variables ********************************************************************************

char * functions_0arg_str[NUM_0ARG_FUNCTIONS] = {"pwd", "quit"};
char * functions_1arg_str[NUM_1ARG_FUNCTIONS] = {"ls", "cd"};

// Prototypes **************************************************************************************

int tokenize(char * path[], char * pathname, char * delimiter);
int find_0arg_function(char * cmd);
int find_1arg_function(char * cmd);

// Funcions ****************************************************************************************

int tokenize(char * path[], char * pathname, char * delimiter)
{
	//Set up
	int tokens = 0;
	
	if(path[tokens] = strtok(pathname, delimiter))
	{
		tokens++;
		
		//Loop iteratively until the pathname has been toklenized completely
		while((tokens < BLKSIZE) && (path[tokens] = strtok(0, delimiter)))
			tokens++;
	}
	
	return tokens;
}

int find_0arg_function(char * cmd)
{
	int i = 0;
	
	for(i; i<NUM_0ARG_FUNCTIONS; i++)
	{
		if(!strcmp(cmd, functions_0arg_str[i]))
			return i;
	}
	
	return -1;
}

int find_1arg_function(char * cmd)
{
	int i = 0;
	
	for(i; i<NUM_1ARG_FUNCTIONS; i++)
	{
		if(!strcmp(cmd, functions_1arg_str[i]))
			return i;
	}
	
	return -1;
}

#endif
