#include "Type.h"
#include "Inode_Util.c"

extern PROC *running;
extern _BLKSIZE;

int myRead(int fd, char *buf, int numberOfBytes){
    int count = 0, offset, blknumber, blk, start, remaining, available, ibuf[256], dibuf[256];
    MINODE *minode;
    INODE *inode;
    char *bufPtr = buf, readBuf[_BLKSIZE], *readBufPtr;

    offset = running->fd[fd]->offset;

    remaining = _BLKSIZE - offset;

    available = lseek(fd, 0, SEEK_END) - offset;

    minode = &running->fd[fd]->mptr;

    inode = &minode->INODE;

    while(numberOfBytes && available){

        blknumber = offset/_BLKSIZE;

        start = offset & _BLKSIZE;
        
        //direct blocks
        if(blknumber < 12)
            blk = inode->i_block[blknumber];
            
        //indirect blocks
        else if(blknumber < 256 + 12){
            
            if(inode->i_block[12]){

                get_block(dev, inode->i_block[12], ibuf);
                blk = ibuf[blkNumber - 12];
            }
        }
        //double indirect blocks
        else{
            
            if(inode->i_block[13]){

                get_block(dev, inode->i_block[13], ibuf);

                blk = ibuf[blknumber - 12];

                get_block(dev, ibuf[blk], dibuf);

                blk = dibuf[blknumber - (256 + 12)];
            }
        }

        get_block(dev, blk, readBuf);

        readBufPtr = readBuf + start;

        while(remaining > 0){
            bufPtr++ = readBufPtr++;
            offset++;
            count++;
            available--;
            numberOfBytes--;
            remaining--;

            if(numberOfBytes <= 0 || available <= 0)
                break;
        }
    }

    printf("my read: read %d char from file descriptor %d\n", count, fd);
    return count;
}