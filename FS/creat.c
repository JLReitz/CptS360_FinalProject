#include "Block_Data.c"
#include "Inode_Data.c"
#include "Inode_Util.c"
#include <time.h>

#ifndef CREAT_C
#define CREAT_C

//local globals
extern PROC * _running;

//Prototypes
int mycreat(char *pathname);
int createFile(MINODE *parentMinode, char* fileName);
int enterName(MINODE *parentMinode, int ino, char *name);


int mycreat(char *pathname){
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
		
		if(numberOfDirs > 1)
		{
		  //cat to dir path;
		  for(int i = 0; i < numberOfDirs-2; i++){
		      strcat(filePath, "/");
		      strcat(filePath, path[i]);
		  }
		  
		  fileName = path[numberOfDirs - 1];
    }
    else
    {
    	filePath = ".";
    	fileName = pathname;
    }

    //get ino and inode of filePath
    if(filePathMinode = iget(_running->cwd->dev, getino(_running->cwd->dev, filePath)))
    {
		  filePathInode = &(filePathMinode->INODE);

		  //verify filePathInode is a dir
		  if(S_ISDIR(filePathInode->i_mode))
		  {
				//verify fileName does not exist
				if(!getino(_running->cwd->dev, pathname))
				{
					//create file
					createFile(filePathMinode, fileName);

					//inc filePathInode link count
					filePathInode->i_links_count++;

					//update atime and mark dirty
					filePathInode->i_atime = time(0L);
					filePathMinode->dirty = 1;
					//write back to disk
					iput(filePathMinode);

					//Successful creat
					return 0;
				}
				else
					printf("This file already exists.\n");
		  }
		  else
		  	printf("The parent directory specified does not exist.\n");
    }
    else
    	printf("The specified path does not exist.\n");
    	
    //Unsuccessful creat
    return 1;
}

int createFile(MINODE *parentMinode, char* fileName){
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

    ip->i_mode = 0x81A4; //mark as FILE, se premissions
    ip->i_uid = _running->uid; //owner's uid
    ip->i_gid = _running->gid; //group's id
    ip->i_size = 0; //0 for files
    ip->i_links_count = 1; //links to ..
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L); //current time
    ip->i_blocks = 0;
    ip->i_block[0] = bno; //first direct block to allocated block
    for(int i = 1; i <= 14; i++)
        ip->i_block[i] = 0;
    
    //set inode in memory to dirty
    mip->dirty = 1;

    //write inode in memory back to disk
    iput(mip);

    //enter name ..
    enterName(parentMinode, ino, fileName);

    //successful
    return 0;
}

int enterName(MINODE *parentMinode, int ino, char *name){
    INODE *parentInode = &parentMinode->INODE;
    char *cp, buf[BLKSIZE];
    DIR *dp;
    int bno, neededLen, idealLen, remainingLen;
    
    //find next block
    for(int i = 0; i < 12; i++){

        if(!parentInode->i_block[i])
            break;
        
        bno = parentInode->i_block[i];
        get_block(_running->cwd->dev, bno, buf);

        //go to last entry
        cp = buf;
        dp = (DIR*)cp;

        while((cp+dp->rec_len) < (buf+BLKSIZE)){
            
            cp += dp->rec_len;
            dp = (DIR*)cp;
        }

        //get ideal length and remaining length
        idealLen = 4*((8 + dp->name_len + 3)/4);
        remainingLen = dp->rec_len - idealLen;
        neededLen = 4*((8 + strlen(name) + 3)/4);

        //if there is room in the block
        if(remainingLen >= neededLen){
        		
        		//Set the OLD last entry's rec_length to the correct size and then incrememnt past it
        		dp->rec_len = idealLen;        		
        		cp += dp->rec_len;
        		dp = (DIR*)cp;
        		
        		//Now set the NEW last entry's information
            dp->rec_len = remainingLen;
            dp->inode = ino;
            dp->name_len = strlen(name);
            strcpy(dp->name, name);

            //write block back
            put_block(_running->cwd->dev, bno, buf);

            //success
            return 0;
        }
    }
    
    return 1;
}

#endif
