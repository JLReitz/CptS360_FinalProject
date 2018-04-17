#include "Block_Data.c"

extern _BLKSIZE;
extern _imap;
extern _bmap;
extern numberOfInodes;
extern numberOfBlocks;

int ialloc(int dev){
    char buf[BLKSIZE];

    get_block(dev, _imap, buf);

    for(int i = 0; i < numberOfInodes; i++){

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

    for(int i = 0; i < numberOfBlocks; i++){
        if(!tst_bit(buf, i)){

            set_bit(buf,i);
            put_block(dev, _bmap, buf);

            return i + 1;
        }
    }
    printf("No more free blocks on dev %d\n", dev);
    return 0;
}

int idealloc(int dev, int ino){
    char *buf[__BLKSIZE];
    int byte, bit;
    SUPER *sp;
    GD *gp;

    get_block(dev, _imap, buf);

    byte = ino / 8;
    bit = ino % 8;

    buf[byte] &= ~(1 << bit);

    put_block(dev, _imap, buf);

    get_block(dev, 1, buf);
    sp = (SUPER*)buf;
    sp->s_free_inodes_count++;
    put_block(dev, 1, buf);

    get_block(dev, 2, buf);
    gp->bg_free_inodes_count++;
    put_block(dev, 2, buf);
}

int bdealloc(int dev, int ino){
    char buf[_BLKSIZE];
    int byte, bit;
    SUPER *sp;
    GD gp;

    get_block(dev, _bmap, buf);

    byte = ino / 8;
    bit = ino % 8;

    buf[byte] &= ~(1 << bit);

    put_block(dev, _bmap, buf);

    get_block(dev, 1, buf);
    sp = (SUPER*)buf;
    sp->s_free_blocks_count++;
    put_block(dev, 1, buf);

    get_block(dev, 2, buf);
    gp = (GD*)buf;
    gp->bg_free_blocks_count++;
    put_block(dev, 2, buf);
}
