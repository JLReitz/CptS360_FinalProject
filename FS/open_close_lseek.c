#include "Type.h"

extern int _bufsize;
extern OFT _OpenFileTable[NOFT];

int open_file(char *pathname, int mode){
    char buf[_blksize]
    INODE *ip;
    MINODE *mip;
    OFT oft;
    int i;

    //get ino
    if(pathname[0] = "/")
        dev = root->dev;
    else   
        dev = running->cwd->dev;

    ino = getino(dev, pathname);

    //load into memory
    mip = iget(dev, ino);
    ip = &mip->INODE;

    //check if dir
    if(!S_ISREG(ip->i_mode)){
        printf("not a file\n");
        return 1;
    }

    //allocate free OFT with values
    for(i = 0; i < NOFT; i++){

        oft = &_OpenFileTable[i];

        if(!oft->refCount){

            of->mode = mode;

            //set offset based on mode
            switch(mode){
            case 0: //read
                oft->offset = 0;
                break;
            case 1: //write
                truncate(mip);
                oft->offset = 0;
                break;
            case 2: //read/write
                oft->offset = 0;
                break;
            case 3: //append
                oft->offset = ip->i_size;
                break;
            default: 
                printf("invalid mode\n");
                return 1;
            };

            oft->refCount = 1;

            oft->mptr = &mip;

            //find smallest, free fd, set to new OFT entry
             running->fd[i] = oft;  
        }
        else{
            printf("file bsuy\n");
            return -1;
        }

    }

    //upate inode time fields
    ip->i_atime = time(0L);
    mip->dirty = 1;

    //return fd
    return i;
}

int close_file(int fd){
    OFT *oft;

    //verify fd

    //verify fd is OFT
    for(int i = 0; i < NOFT; i++){

        oft = &_OpenFileTable[i];

        if(oft->mptr->INODE == running->fd[fd]->mptr->INODE){
            break;
        }

        if(i = NOFT - 1){

            printf("file not found\n");
            return 1;
        }
    }
    //
    fp = running->fd[fd];
    running->fd[fd] = 0;

    fp->refCount--;

    if(!fp->refCount){
        iput(fp->mptr);
    }

    return 0;
}

int lseek(int fd, int position){
    OFT *oft;
    int orgPos;

    //find oft entry
    if(running->fd[fd] && running->fd[fd]->refCount > 0){
        oft = running->fd[fd];

        //change offset without exceeding end of file
        if(position <= oft->mptr->INODE.i_size){

            orgPos = oft->offset;
            oft->offset = position;
            running->fd[fd] = oft;
            return orgPos;
        }
        else{

            printf("position out of bounds\n");
            return 1;
        }
    }
    printf("fd busy or could not be found\n");
    return 1;
}

int truncate(MINODE *mip){

    //release inode data blocks

    //update time field

    //inode size = 0, mark dirty
}

int printfd(){

}