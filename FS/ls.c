//****************************************** ls.c **************************************************
#include "Inode_Util.c"
#include "Inode_Data.c"
#include "Block_Data.c"

#ifndef LS_C
#define LS_C

// Global Variables ********************************************************************************

extern PROC * _running;

// Prototypes **************************************************************************************

void ls(char * pathname);
void print_Dir(int dev, INODE * inode);
void print_file(int dev, int ino);

// Functions ***************************************************************************************

void ls(char * pathname)
{
	if(pathname) //If the pathname is specified
	{
		//Go to that pathname and then print the directory
		char ibuf[BLKSIZE];
		INODE * inode = get_inode(_running->cwd->dev, getino(_running->cwd->dev, pathname), ibuf);
		
		print_Dir(_running->cwd->dev, inode);
	}
	else //Otherwise print cwd
		print_Dir(_running->cwd->dev, &_running->cwd->INODE);
}

void print_Dir(int dev, INODE * inode)
{
	int i;
	char buf[BLKSIZE];
	
	//Check for if the supplied inode is a directory
	if(inode->i_mode == 16877)
	{
		//Access every data block that this directory contains
		for(i=0; i<12; i++)
		{
			//Use this INODE to find the data block number
			int iblock = inode->i_block[i];
			
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
		printf("The file supplied was not a directory.\n");
}

#endif
