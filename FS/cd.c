#include "Type.h"
#include "Inode_Util.c"

#ifndef CD_C
#define CD_C

extern PROC * _running;

void cd(char * pathname)
{	
	int ino = getino(_running->cwd->dev, pathname);
	MINODE * mip;
	
	if(ino)
	{
		mip = iget(_running->cwd->dev, ino);
		
		if(mip->INODE.i_mode == 16877)
			_running->cwd = mip;
		else
			printf("The supplied location was not a valid directory.\n");
			
		return;
	}
	
	printf("The supplied path does not exist.\n");
}

#endif
