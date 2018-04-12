//****************************************** ls.c **************************************************
#include "../Type.h"
#include "../Utility/Inode_Util.c"

// Global Variables ********************************************************************************

extern PROC * _running;

// Prototypes **************************************************************************************

void ls(char * pathname);
void print_dir(int dev, int ino);
void print_file(int dev, int ino);

// Functions ***************************************************************************************

void ls(char * pathname)
{
	if(pathname) //If the pathname is specified
	{
		//Go to that pathname and then print the directory
		int ino = getino(_running->cwd->dev, pathname);
		
		print_Dir(_running->cwd->dev, ino);
	}
	else //Otherwise print cwd
		print_Dir(_running->cwd->dev, _running->cwd->ino);
}

void print_Dir(int dev, int ino)
{
	int i;
	INODE * inode;
	
	//Grab inode
	inode = get_inode(dev, ino);
	
	//Check for if the supplied inode is a directory
	if(inode->i_mode == 16877)
	{
		//Access every data block that this directory contains
		for(i=0; i<12; i++)
		{
			//Use this INODE to find the data block number
			u32 iblock = inode->i_block[i];
			
			if(iblock) //If the block number is 0, that block does not exist
			{
				//Read in the block and print the directory listing
				get_block(dev, iblock, buf);
				
				char * cp = buf; //Character pointer to hold the current position within the block
				DIR * dp = (DIR *)buf;
				
				while(cp < &buf[BLKSIZE])
				{
					//TODO create print_file()
					//print_file(dev, ino);
					printf("%s ", dp->name);
					
					//Increment cp and set dp to the next file in the directory
					cp += dp->rec_len;
					dp = (DIR *)cp;
				}
				
				printf("\n");
			}
		}
	}
	
	else
		printf("The file supplied was not a directory\n");
}
