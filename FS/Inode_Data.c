#ifndef INODE_DATA_C
#define INODE_DATA_C

#include "Block_Data.c"

// Global Variables *******************************************************************************

extern MNTABLE * _mntPtr;

// Prototypes *************************************************************************************

INODE * get_inode(int dev, int ino, char buf[]);
int put_inode(int dev, int ino, INODE * inode);

// Functions **************************************************************************************

INODE * get_inode(int dev, int ino, char buf[])
{
	int blk = _mntPtr->iblk + (ino-1)/8;
	int offset = (ino-1)%8;
	
	//Get the block where the inode exists
	get_block(dev, blk, buf);
	
	//Now return the inode
	return (INODE *)buf + offset;
}

int put_inode(int dev, int ino, INODE * inode)
{
	int blk = _mntPtr->iblk + (ino-1)/8;
	int offset = (ino-1)%8;
	char ibuf[BLKSIZE];
	INODE * inode_stale;
	
	//Read in the old inode
	get_block(dev, ino, ibuf);
	inode_stale = (INODE *)ibuf;
	
	//Set the old inode equal to the new inode
	*inode_stale = *inode;
	
	//Write the inode to its respective block
	put_block(dev, ino, ibuf);
}

#endif
