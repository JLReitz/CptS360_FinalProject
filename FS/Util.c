#ifndef UTIL_C
#define UTIL_C

// Prototypes *************************************************************************************

int tokenize(char * path[], char * pathname, char * delimiter);

// Funcions ***************************************************************************************

int tokenize(char * path[], char * pathname, char * delimiter)
{
	//Set up
	int tokens = 1;
	path[tokens-1] = strtok(pathname, delimiter);
	
	//Loop iteratively until the pathname has been toklenized completely
	while((tokens < BLKSIZE) && (path[tokens-1] = strtok(0, delimiter)))
		tokens++;
	
	return tokens;
}

#endif
