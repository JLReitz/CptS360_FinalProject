#include "../Type.h"
#include "Utility/*"
#include "Block/*"

#ifndef CD_C
#define CD_C

void cd(char * pathname)
{	
	int ino = getino(dev, pathname);
	
	if(ino)
		running->cwd = iget(dev, ino);
}

#endif