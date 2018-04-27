#include "Type.h"
#include "Allocate_Deallocate.c"

extern PROC *running;
extern _BLKSIZE;

int write(int fd, char *buf, int numberOfBytes){
    int blkNumber, start, offset, blk, ibuf[256], dibuf[256], remaining;
    MINODE *minode;
    INODE *inode;
    char *wbuf[_BLKSIZE], *bufPtr = buf, wBufPtr;

    minode = &running->fd[fd]->mptr;

    inode = &minode->INODE;

    offset = running->fd[fd]->offset;

    while(numberOfBytes > 0){

        blkNumber = offset / _BLKSIZE;

        start = offset % _BLKSIZE;

        //direct blocks
        if(blkNumber < 12){

            if(!inode->i_block[blkNumber]){
                //allocate blk
                inode->i_block[blkNumber] = balloc(minode->dev);
            }

            blk = inode->i_block[blkNumber];
        }
        //indirect blocks
        else if(blkNumber < 256 + 12){
            
            if(!inode->i_block[12]){

                inode->i_block[12] = balloc(minode->dev);    
            }

            get_block(minode->dev, inode->i_block[12], ibuf);
            blk = ibuf[blkNumber - 12];
            

        }
        //double indirect
        else{
            if(!inode->i_block[13])
                inode->i_block[13] = balloc(minode->dev);
            
            get_block(minode->dev, inode->i_block[13], ibuf);

            blk = ibuf[blkNumber - 12];

            get_block(minode->dev, ibuf[blk], dibuf);

            blk = dibuf[blkNumber - (256 + 12)];
        }

        get_block(minode->dev, blk, wbuf);
        wBufPtr = wbuf + start;
        remaining = _BLKSIZE - start;

        while(remaining > 0){

            wBufPtr++ = bufPtr++;
            numberOfBytes--;
            remaining--;
            offset++;
            if(offset > inode->i_size)
                inode->i_size++;
            
            if(numberOfBytes <= 0)
                break;
        }
    }

    minode->dirty = 1;

    printf("wrote %d char into file descriptor %d\n", numberOfBytes, fd);

    return numberOfBytes;
}