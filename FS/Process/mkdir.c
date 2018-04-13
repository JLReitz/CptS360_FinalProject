#include "../Type.h"
#include <time.h>

//local globals
extern _PROC *running;
extern int _blocksize;
int dev = running->cwd->dev;

int mymkdir(char *pathname){
    //local variables
    char *path[16];
    char *dirPath = "";
    char *dirName;
    int numberOfDirs;
    MINODE *dirPathInode;
    int ino, bno;

    //tokenize pathname
    numberOfDirs = tokenize(path, pathname, "/");

    //cat to dir path;
    for(int i = 0; i < numberOfDirs-2; i++){
        strcat(dirPath, "/");
        strcat(dirPath, path[i]);
    }

    //get ino and inode of dirPath
    dirPathInode = iget(dev, getino(dev, dirPath));

    //verify dirPathInode is a dir

    //verify dirName does not exist

    //mkdir
    createDir(dirPathInode, dirName);

    //inc dirPathInode link count

    //update atime and mark dirty

    //write back to disk
    iput(dirPathInode);

    //successful mkdir 
    return 0
}

int createDir(MINODE *parentInode, char* dirName){
    //local variables
    int ino, bno;
    MINODE *mip;
    INODE *ip;
    char *cp, buf[_blocksize];
    DIR *dp;

    //allocate inode and block
    ino = ialloc(dev);
    bno = balloc(dev);

    //load new inode
    mip = iget(dev, ino);

    //write to inode
    ip = &mip->inode;

    ip->i_mode = 0x41ED; //mark as DIR, se premissions
    ip->i_uid = running->uid; //owner's uid
    ip->i_gid = running->gid; //group's id
    ip->i_size = _blocksize; //blocksize
    ip->i_links_count = 2; //links to . and ..
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L); //current time
    ip->i_blocks = 2;
    ip->i_block[0] = bno; //first direct block to allocated block
    for(int i = 1; i <= 14; i++)
        ip->i_blocks[i] = 0;
    
    //set inode in memory to dirty
    mip->dirty = 1;

    //write inode in memory back to disk
    iput(mip);

    //data block for . and .. 
    getBlock(dev, bno, buf);

    cp = buf;
    dp = (DIR*)cp;

    //write . and ,, blocks SHOULD BE A FUNCTION LATER//
    dp->inode = ino;
    dp->rec_len = 4*((8 + 1 + 3)/4); //ideal length
    dp->name_len = 1;
    dp->name[0] = ".";

    cp += dp->rec_len;
    dp = (DIR*)cp;

    dp->inode = parentInode->ino;
    dp->rec_len = _blocksize - 12;
    dp->name_len = 2;
    dp->name[0] = ".";
    dp->name[1] = ".";

    //write block
    putBlock(dev, bno, buf);

    //enter names . and ..
    enterName(parentInode, ino, ".");
    enterName(parentInode, ino, "..");

    //successful
    return 0;
}

int enterName(MINODE *parentInode, int ino, char *name){

    //find next block

    //get name length

    //get ideal length

    

    //successful
    return 0;
}

int myCreat(char *pathname){
    
}