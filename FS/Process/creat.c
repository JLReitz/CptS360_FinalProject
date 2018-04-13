#include "../Type.h"
#include <time.h>

//local globals
extern _PROC *running;
extern int _blocksize;
int dev = running->cwd->dev;

int mymkdir(char *pathname){
    //local variables
    char *path[16];
    char *filePath = "";
    char *fileName;
    int numberOfDirs;
    INODE *filePathInode;
    MINODE *filePathMinode;
    int ino, bno;

    //tokenize pathname
    numberOfDirs = tokenize(path, pathname, "/");

    //cat to dir path;
    for(int i = 0; i < numberOfDirs-2; i++){
        strcat(filePath, "/");
        strcat(filePath, path[i]);
    }

    //get ino and inode of filePath
    filePathMinode = iget(dev, getino(dev, filePath));

    filePathInode = &filePathMinode->INODE;

    //verify filePathInode is a file

    //verify fileName does not exist

    //create file
    createFile(filePathMinode, fileName);

    //inc filePathInode link count
    filePathInode->i_links_count++;

    //update atime and mark dirty
    filePathInode->i_atime = time(0L);
    filePathMinode->dirty = 1;
    //write back to disk
    iput(filePathMinode);

    //successful mkdir 
    return 0
}

int createFile(MINODE *parentMinode, char* fileName){
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

    ip->i_mode = 0x81A4; //mark as FILE, se premissions
    ip->i_uid = running->uid; //owner's uid
    ip->i_gid = running->gid; //group's id
    ip->i_size = 0; //0 for files
    ip->i_links_count = 1; //links to ..
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L); //current time
    ip->i_blocks = 0;
    ip->i_block[0] = bno; //first direct block to allocated block
    for(int i = 1; i <= 14; i++)
        ip->i_blocks[i] = 0;
    
    //set inode in memory to dirty
    mip->dirty = 1;

    //write inode in memory back to disk
    iput(mip);

    //enter name ..
    enterName(parentMinode, ino, "..");

    //successful
    return 0;
}

int enterName(MINODE *parentMinode, int ino, char *name){
    INODE *parentInode = &parentMinode->INODE;
    char *cp, buf[_blocksize];
    DIR *dp;
    int bno, neededLen, idealLen, remainingLen;
    
    //find next block
    for(int i = 0; i < parentInode->i_size/_blocksize; i++){

        if(!parentInode->i_block[i])
            break;
        
        bno = parentInode->i_block[i];
        get_block(dev, bno, buf);

        //go to last entry
        cp = buf;
        dp = (DIR*)cp;

        while(cp < buf + _blocksize){
            
            cp += dp->rec_len;
            dp = (DIR*)cp;
        }

        cp = (char*)dp;

        //get ideal length and remaining length
        idealLen = 4*((8 + dp->name_len + 3)/4);
        remainingLen = dp->rec_len - idealLen;

        //if there is room in the block
        if(remainingLen >= idealLen){
            dp->rec_len = remainingLen
        

            dp->inode = ino;
            dp->name_len = strlen(name);
            strcpy(dp->name, name);

            //write block back
            put_block(dev, bno, buf);

            //success
            return 0;
        }
    }
}
