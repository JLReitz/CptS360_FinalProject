#ifndef INODE_DATA_C
#define INODE_DATA_C

#include "Block_Data.c"

// Global Variables *******************************************************************************

extern int _iblk;

// Prototypes *************************************************************************************

int get_inode(int dev, int ino, INODE * inode);
int put_inode(int dev, int ino, INODE * inode);

// Functions **************************************************************************************

int get_inode(int dev, int ino, INODE * inode)
{
	int blk = _iblk + (ino-1)/8;
	int offset = (ino-1)/8;
	char ibuf[BLKSIZE];
	
	//Get the block where the inode exists
	get_block(dev, blk, ibuf);
	
	//Now return the inode
	*inode = *((INODE *)ibuf + offset);
}

int put_inode(int dev, int ino, INODE * inode_fresh)
{
	int blk = _iblk + (ino-1)/8;
	int offset = (ino-1)/8;
	char ibuf[BLKSIZE];
	INODE * inode_stale;
	
	//Get the block where the inode exists
	get_block(dev, blk, ibuf);
	inode_stale = (INODE *)ibuf + offset;
	
	//Rewrite the inode
	*inode_stale = *inode_fresh;
	
	//Replace the block
	put_block(dev, blk, ibuf);
}

#endif
