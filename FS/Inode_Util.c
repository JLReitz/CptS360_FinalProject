#ifndef INODE_UTIL_C
#define INODE_UTIL_C

#include "Block_Data.c"
#include "Utility.c"

// Global Variables *******************************************************************************

extern PROC * _running;
extern MINODE _minode[NMINODE];

// Prototypes *************************************************************************************

int getino(int dev, char * pathname);
int iget(int dev, int ino, MINODE * minode);
int isearch_ino(MINODE * mip, char * filename);
int isearch_name(MINODE * mip, int ino, char * filename);

void iput(MINODE * mip);
void print_inode(int dev, int ino);

// Functions **************************************************************************************

int getino(int dev, char * pathname)
{	  
	int i, ino, blk, pathlength;
	char ibuf[BLKSIZE];
	char * filenames[BLKSIZE];
	MINODE * minode;

	if (!strcmp(pathname, "/")) //If the pathname points to root
		return 2; //Return the root inode number

	if (pathname[0]=='/') //If the pathname is absolute 
		iget(dev, 2, minode); //Start with minode = root
	else
		iget(_running->cwd->dev, _running->cwd->ino, minode); //Otherwise start with mip = cwd

	strcpy(ibuf, pathname);
	
	//Tokenize the string
	pathlength = tokenize(filenames, ibuf, "/");

	for (i=0; i<pathlength-2; i++) //Loop through the steps in the pathname
	{
		ino = isearch_ino(minode, filenames[i]); //Search for the next branch in the path

		if (!ino) //If the ino returned is 0, we've reached the end of the path
		{
			iput(minode); //Discard minode
		  return 0;
		}
		
		iget(dev, ino, minode); //Set minode to the next step that was found
	}
	
	return ino; //Return the found ino
}

int iget(int dev, int ino, MINODE * minode)
{  
  int i, blk, disp;
  char buf[BLKSIZE];
  INODE * ip;
  MINODE * mip;
  
	//Search the In-Memory Inode list (MINODE) list first to see if it's already been loaded
	for (i=0; i<NMINODE; i++)
	{
		mip = &_minode[i];
		
		if ((mip->dev == dev) && (mip->ino == ino)) //If both dev and ino match up, then we've found it
		{
			 mip->refCount++;
			 minode = mip;
			 
			 return 1;
		}
	}

	//Otherwise look for the first empty minode
	for (i=0; i<NMINODE; i++)
	{
		mip = &_minode[i];
		
		if (mip->refCount == 0)
		{
			mip->refCount = 1;
			mip->dev = dev;
			mip->ino = ino;

			//Get INODE of ino to buf    
			blk  = (ino-1)/8 + _iblk;
			disp = (ino-1)%8;
			
			get_block(dev, blk, buf);
			ip = (INODE *)buf + disp;
			
			//Copy INODE to mp->INODE
			mip->INODE = *ip;
			
			minode = mip;
			
			return 1;
		}
	}
	
	return 0;
}

int isearch_ino(MINODE * mip, char * filename)
{
	int i;
	char ibuf[BLKSIZE];
	INODE inode = mip->INODE;
		
	//Check for if the supplied inode is a directory
	if(inode.i_mode == 16877)
	{
		//Access every data block that this directory contains
		for(i=0; i<12; i++)
		{
			//Use this INODE to find the data block number
			u32 iblock = inode.i_block[i];
			
			if(iblock) //If the block number is 0, that block does not exist
			{
				//Read in the block and print the directory listing
				get_block(mip->dev, iblock, ibuf);
				
				char * cp = ibuf; //Character pointer to hold the current position within the block
				DIR * dp = (DIR *)ibuf;
				
				while(cp < &ibuf[BLKSIZE])
				{
					//Compare filenames
					if(!strncmp(dp->name, filename, dp->name_len))
						return dp->inode; //Return the inode number if this is the correct file
					
					//Increment cp and set dp to the next file in the directory
					cp += dp->rec_len;
					dp = (DIR *)cp;
				}
			}
		}
		
		return 0;
	}
	
	printf("The file supplied was not a directory\n");
	return 0;
}

int isearch_name(MINODE * mip, int ino, char * filename)
{
	int i = 0;
	char ibuf[1024];
	INODE inode = mip->INODE;
	
	//Check for if the supplied inode is a directory
	if(inode.i_mode == 16877)
	{
		//Access every data block that this directory contains
		for(i; i<12; i++)
		{
			//Use this INODE to find the data block number
			u32 iblock = inode.i_block[i];
			
			if(iblock) //If the block number is 0, that block does not exist
			{
				//Read in the block and print the directory listing
				get_block(mip->dev, iblock, ibuf);
				
				char * cp = ibuf; //Character pointer to hold the current position within the block
				DIR * dp = (DIR *)ibuf;
				
				while(cp < &ibuf[BLKSIZE])
				{
					//If this is the inode we're looking for
					if(dp->inode == ino)
					{
						strcpy(filename, dp->name); //Copy the 
						return 1;
					}
					
					//Increment cp and set dp to the next file in the directory
					cp += dp->rec_len;
					dp = (DIR *)cp;
				}
			}
		}
		
		return 0;
	}
	
	printf("The file supplied was not a directory\n");
	return 0;
}

void iput(MINODE * mip)
{
	int i, block, offset;
	char buf[BLKSIZE];
	INODE * ip;

	if(mip)
	{
		mip->refCount--; //Decrease the refcount

		if (mip->refCount > 0) //If the MINODE is supporting more than one inode 
		return; //Return
		
		if (!mip->dirty) //If the INODE hasn't been altered at all
			return; //Return
		else
		{
			//Write back
			put_inode(mip->dev, mip->ino, &(mip->INODE));
		}
	}
}

void print_inode(int dev, int ino)
{
	int i = 0;
	char ibuf[1024];
	INODE * inode;
}

#endif