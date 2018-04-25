#ifndef INODE_DATA_C
#define INODE_DATA_C

#include "Block_Data.c"

// Global Variables *******************************************************************************

extern char _buf[BLKSIZE];
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

int put_inode(int dev, int ino, INODE * inode_fresh)
{
	int blk = _mntPtr->iblk + (ino-1)/8;
	int offset = (ino-1)%8;
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
