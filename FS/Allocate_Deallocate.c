#ifndef ALLOCATE_DEALLOCATE_C
#define ALLOCATE_DEALLOCATE_C

#include "Block_Data.c"

extern int _imap;
extern int _bmap;
extern int _ninodes;
extern int _nblocks;

int ialloc(int dev){
    char buf[BLKSIZE];

    get_block(dev, _imap, buf);

    for(int i = 0; i < _ninodes; i++){

        if(!tst_bit(buf, i)){

            set_bit(buf, i);
            dec_FreeInodes(dev);
            put_block(dev, _imap, buf);

            return i+1;
        }
    }
    printf("No more inodes remaining on dev %d", dev);
    return 0;
}

int balloc(int dev){
    char buf[BLKSIZE];

    get_block(dev, _bmap, buf);

    for(int i = 0; i < _nblocks; i++){
        if(!tst_bit(buf, i)){

            set_bit(buf,i);
            put_block(dev, _bmap, buf);
            dec_FreeBlocks(dev);

            return i + 1;
        }
    }
    printf("No more free blocks on dev %d\n", dev);
    return 0;
}

int idealloc(int dev, int ino){
    char buf[BLKSIZE];
    //int byte, bit;
    //SUPER *sp;
    //GD *gp;

    get_block(dev, _imap, buf);
		
		free_bit(buf, ino);
		//Same below as this function?
		/*
    byte = ino / 8;
    bit = ino % 8;

    buf[byte] &= ~(1 << bit);
    */

    put_block(dev, _imap, buf);
		
		inc_FreeInodes(dev);
		//Same below as this function?
		/*
    get_block(dev, 1, buf);
    sp = (SUPER*)buf;
    sp->s_free_inodes_count++;
    put_block(dev, 1, buf);

    get_block(dev, 2, buf);
    gp->bg_free_inodes_count++;
    put_block(dev, 2, buf);
    */
}

int bdealloc(int dev, int blk){
    char buf[BLKSIZE];
    int byte, bit;
    SUPER *sp;
    GD *gp;

    get_block(dev, _bmap, buf);
    
    free_bit(buf, blk);
    //Same below as this function?
		/*
    byte = ino / 8;
    bit = ino % 8;

    buf[byte] &= ~(1 << bit);
    */

    put_block(dev, _bmap, buf);
		
		inc_FreeBlocks(dev);
}

#endif
