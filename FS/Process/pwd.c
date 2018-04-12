//****************************************** pwd.c *************************************************
#include "../Type.h"
#incude "../Utility/Inode_Util.c"

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
	char * filename;
	
	if(dir->ino == 2)
	{
		printf("/");
		return;
	}
	
	if(isearch(dir, "..") == 2) //If your parent directory is root
	{
		ip = &(iget(dev, 2)->INODE);
		
		//Load the name of the current directory
		isearch_name(ip, filename, 2);
		printf("/%s", filename);
		
		return;
	}
	else
		ip = &(iget(dev, kcwsearch(dir, ".."))->INODE);
		
	pwd(iget(dev, kcwsearch(dir, "..")));
	
	//Load the name of the current directory
	searchFile(ip, filename, kcwsearch(dir, "."));
	printf("/%s", filename);
	
	return;
}
