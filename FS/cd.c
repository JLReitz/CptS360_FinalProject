#include "Type.h"
#include "Inode_Util.c"

#ifndef CD_C
#define CD_C

// Global Variables ********************************************************************************

extern char * _cwd;
extern PROC * _running;

// Prototypes **************************************************************************************

void cd(char * pathname);
void update_cwd(MINODE * dir);

// Functions ***************************************************************************************

void cd(char * pathname)
{	
	int ino = getino(_running->cwd->dev, pathname);
	MINODE * mip;
	
	if(ino)
	{
		mip = iget(_running->cwd->dev, ino);
		
		if(mip->INODE.i_mode == 16877)
		{
			_running->cwd = mip;
			update_cwd(_running->cwd);
		}
		else
			printf("The supplied location was not a valid directory.\n");
			
		return;
	}
	
	printf("The supplied path does not exist.\n");
}

void update_cwd(MINODE * dir)
{
	int ino;
	char * filename;
	INODE ip;
	MINODE * mip;
	
	if(dir->ino == 2)
	{
		_cwd = "/";
		return;
	}
	
	if((ino = isearch_ino(dir, "..")) == 2) //If your parent directory is root
	{
		mip = iget(dir->dev, 2);
		
		//Load the name of the current directory
		isearch_name(mip, 2, filename);
		_cwd = "/";
		
		return;
	}
	else
	{
		mip = iget(dir->dev, ino);
		
		pwd_recursive(mip);
		
		//Load the name of the current directory
		isearch_name(mip, isearch_ino(dir, "."), filename);
		strcat(_cwd, filename);
		
		return;
	}
}

#endif
