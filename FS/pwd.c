//****************************************** pwd.c *************************************************
#include "Type.h"
#include "Inode_Util.c"

#ifndef PWD_C
#define PWD_C

// Global Variables ********************************************************************************

extern PROC * _running;

// Prototypes **************************************************************************************

void pwd(char * pathname);
void pwd_recursive(MINODE * dir);

// Functions ***************************************************************************************

void pwd(char * pathname)
{
	pwd_recursive(_running->cwd);
}

void pwd_recursive(MINODE * dir)
{
	int ino;
	char * filename;
	INODE ip;
	MINODE * mip;
	
	if(dir->ino == 2)
	{
		printf("/");
		return;
	}
	
	if((ino = isearch_ino(dir, "..")) == 2) //If your parent directory is root
	{
		mip = iget(dir->dev, 2);
		
		//Load the name of the current directory
		isearch_name(mip, 2, filename);
		printf("/%s", filename);
		
		return;
	}
	else
	{
		mip = iget(dir->dev, ino);
		
		pwd_recursive(mip);
		
		//Load the name of the current directory
		isearch_name(mip, isearch_ino(dir, "."), filename);
		printf("/%s", filename);
		
		return;
	}
}

#endif
