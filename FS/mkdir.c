#include "Block_Data.c"
#include "Inode_Data.c"
#include "Inode_Util.c"
#include "creat.c"
#include <time.h>

#ifndef MKDIR_C
#define MKDIR_C

//local globals
extern PROC *_running;

void mymkdir(char * pathname);
int createDir(MINODE *parentInode, char* dirName);

void mymkdir(char *pathname){
    //local variables
    char *path[16];
    char *dirPath = "";
    char *dirName;
    int numberOfDirs;
    INODE *dirPathInode;
    MINODE *dirPathMinode;
    int ino, bno;

    //tokenize pathname
    numberOfDirs = tokenize(path, pathname, "/");

		if(numberOfDirs > 1)
		{
    	//cat to dir path;
		  for(int i = 0; i < numberOfDirs-2; i++){
		      strcat(dirPath, "/");
		      strcat(dirPath, path[i]);
		  }

		  dirName = path[numberOfDirs - 1];
    }
    else
    {
    	dirPath = ".";
    	dirName = pathname;
    }

    //get ino and inode of dirPath
    if(dirPathMinode = iget(_running->cwd->dev, getino(_running->cwd->dev, dirPath))){
		  dirPathInode = &dirPathMinode->INODE;

		  //verify dirPathInode is a dir
      if(S_ISDIR(dirPathInode->i_mode)){
				//verify dirName does not exist
				if(!isearch_ino(dirPathMinode, dirName)){
					//mkdir
					createDir(dirPathMinode, dirName);

					//inc dirPathInode link count
					dirPathInode->i_links_count++;

					//update atime and mark dirty
					dirPathInode->i_atime = time(0L);
					dirPathMinode->dirty = 1;
					
					//write back to disk
					iput(dirPathMinode);

					//successful mkdir 
					return;
				}
				else
					printf("This directory already exists.\n");
		  }
		  else
		  	printf("The parent directory specified does not exist.\n");
    }
    else
    	printf("The path specified does not exist.\n");
}

int createDir(MINODE *parentInode, char* dirName){
    //local variables
    int ino, bno;
    MINODE *mip;
    INODE *ip;
    char *cp, buf[BLKSIZE];
    DIR *dp;

    //allocate inode and block
    ino = ialloc(_running->cwd->dev);
    bno = balloc(_running->cwd->dev);

    //load new inode
    mip = iget(_running->cwd->dev, ino);

    //write to inode
    ip = &(mip->INODE);

    ip->i_mode = 0x41ED; //mark as DIR, se premissions
    ip->i_uid = _running->uid; //owner's uid
    ip->i_gid = _running->gid; //group's id
    ip->i_size = BLKSIZE; //blocksize
    ip->i_links_count = 2; //links to . and ..
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L); //current time
    ip->i_blocks = 2;
    ip->i_block[0] = bno; //first direct block to allocated block
    for(int i = 1; i <= 14; i++)
        ip->i_block[i] = 0;
    
    //set inode in memory to dirty
    mip->dirty = 1;

    //write inode in memory back to disk
    iput(mip);

    //data block for . and .. 
    get_block(_running->cwd->dev, bno, buf);

    cp = buf;
    dp = (DIR*)cp;

    //write . and .. blocks SHOULD BE A FUNCTION LATER//
    dp->inode = ino;
    dp->rec_len = 4*((8 + 1 + 3)/4); //ideal length
    dp->name_len = 1;
    strcpy(dp->name, ".");

    cp += dp->rec_len;
    dp = (DIR*)cp;

    dp->inode = parentInode->ino;
    dp->rec_len = BLKSIZE - 12;
    dp->name_len = 2;
    strcpy(dp->name, "..");

    //write block
    put_block(_running->cwd->dev, bno, buf);

    //enter names . and ..
    enterName(parentInode, ino, dirName);

    //successful
    return 0;
}

#endif
