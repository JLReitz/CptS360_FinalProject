#include "../Type.h"


int open_file(char *pathname, int mode){


    //get ino
    if(pathname[0] = "/")
        dev = root->dev;
    else   
        dev = running->cwd->dev;

    ino = getino(dev, pathname);

    //load into memory
    mip = iget(dev, ino);

    //check if dir

    //allcate free OFT with values

    //set offset based on mode

    //find smallest, free fd, set to new OFT entry

    //upate inode time fields

    //return fd
}

int close_file(int fd){

    //verify fd

    //verify fd is OFT

    //
}

int lseek(int fd, int position){

    //find oft entry

    //change offset without exceeding end of file

    //return original position 
}

int truncate(MINODE *mip){

    //release inode data blocks

    //update time field

    //inode size = 0, mark dirty
}

int printfd(){

}