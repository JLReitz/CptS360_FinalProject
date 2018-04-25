#ifndef BLOCK_DATA_C
#define BLOCK_DATA_C

#include "Type.h"

// Prototypes *************************************************************************************

int free_bit(char buf[], int bit);
int set_bit(char buf[], int bit);
int tst_bit(char buf[], int bit);

void dec_FreeBlocks(int dev);
void inc_FreeBlocks(int dev);
void dec_FreeInodes(int dev);
void inc_FreeInodes(int dev);
void get_block(int dev, int blk, char buf[]);
void put_block(int dev, int blk, char buf[]);

// Functions **************************************************************************************

int free_bit(char buf[], int bit)
{
  int i = bit / 8; 
  int j = bit % 8;
  
  buf[i] &= ~(1 << j);
}

int set_bit(char buf[], int bit)
{
  int i = bit / 8; 
  int j = bit % 8;
  
  buf[i] |= (1 << j);
}

int tst_bit(char buf[], int bit)
{
  int i = bit / 8;
  int j = bit % 8;
  
  if (buf[i] & (1 << j))
     return 1;
  else 
  	return 0;
}

void dec_FreeBlocks(int dev)
{
	char bbuf[BLKSIZE];
  SUPER * sp;
  GD * gp;
  
  get_block(dev, 1, bbuf);
  sp = (SUPER*)bbuf;
  sp->s_free_blocks_count--;
  put_block(dev, 1, bbuf);

  get_block(dev, 2, bbuf);
  gp = (GD*)bbuf;
  gp->bg_free_blocks_count--;
  put_block(dev, 2, bbuf);
}

void inc_FreeBlocks(int dev)
{
	char bbuf[BLKSIZE];
  SUPER * sp;
  GD * gp;	
	
  get_block(dev, 1, bbuf);
  sp = (SUPER*)bbuf;
  sp->s_free_blocks_count++;
  put_block(dev, 1, bbuf);

  get_block(dev, 2, bbuf);
  gp = (GD*)bbuf;
  gp->bg_free_blocks_count++;
  put_block(dev, 2, bbuf);
}

void dec_FreeInodes(int dev)
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

void inc_FreeInodes(int dev)
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

void get_block(int dev, int blk, char buf[])
{
	lseek(dev, (long)blk*BLKSIZE, 0);
  read(dev, buf, BLKSIZE);
}

void put_block(int dev, int blk, char buf[])
{
	lseek(dev, (long)blk*BLKSIZE, 0);
  write(dev, buf, BLKSIZE);
}

#endif
