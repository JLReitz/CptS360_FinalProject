#include "../Type.h"

// Prototypes *************************************************************************************

int dec_FreeInodes(int dev);
int inc_FreeInodes(int dev);
int set_bit(char * buf, int bit);
int tst_bit(char * buf, int bit);

void get_block(int dev, int blk, char buf[]);
void put_block(int dev, int blk, char buf[]);

// Functions **************************************************************************************

int decFreeInodes(int dev)
{
  char bbuf[BLKSIZE];
  SUPER * sp;
  GD * gp;

  //Decrement free_inodes count in SUPER and GD
  get_block(dev, 1, bbuf);
  sp = (SUPER *)bbuf;
  sp->s_free_inodes_count--;
  put_block(dev, 1, bbuf);

  get_block(dev, 2, bbuf);
  gp = (GD *)bbuf;
  gp->bg_free_inodes_count--;
  put_block(dev, 2, bbuf);
}

int inc_FreeInodes(int dev)
{
	char bbuf[BLKSIZE];
  SUPER * sp;
  GD * gp;

  //Decrement free_inodes count in SUPER and GD
  get_block(dev, 1, bbuf);
  sp = (SUPER *)bbuf;
  sp->s_free_inodes_count++;
  put_block(dev, 1, bbuf);

  get_block(dev, 2, bbuf);
  gp = (GD *)bbuf;
  gp->bg_free_inodes_count++;
  put_block(dev, 2, bbuf);
}

int set_bit(char * buf, int bit)
{
  int i = bit / 8; 
  int j = bit % 8;
  
  buf[i] |= (1 << j);
}

int tst_bit(char * buf, int bit)
{
  int i = bit m 8;
  int j = bit % 8;
  
  if (buf[i] & (1 << j))
     return 1;
  else 
  	return 0;
}

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
