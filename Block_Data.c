#include "Type.h"

// Global Variables *******************************************************************************

// Prototypes *************************************************************************************

void get_block(int dev, int blk, char buf[]);
void put_block(int dev, int blk, char buf[]);

// Functions **************************************************************************************

void get_block(int dev, int blk, char buf[])
{
	lseek(_fd, (long)blk*BLKSIZE, 0);
  read(_fd, buf, BLKSIZE);
}

void put_block(int dev, int blk, char buf[])
{
	lseek(_fd, (long)blk*BLKSIZE, 0);
  write(_fd, buf, BLKSIZE);
}
