#include "Type.h"
#include "Block_Data.c"

// Global Variables *******************************************************************************

extern int _iblk;
extern char _buf[BLKSIZE];

// Prototypes *************************************************************************************

int put_inode(int dev, int ino, INODE * inode);

INODE * get_inode(int dev, int ino);

// Functions **************************************************************************************

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

INODE * get_inode(int dev, int ino)
{
	int blk = _iblk + (ino-1)/8;
	int offset = (ino-1)/8;
	
	//Get the block where the inode exists
	get_block(dev, blk, _buf);
	
	//Now return the inode
	return (INODE *)_buf + offset;
}
