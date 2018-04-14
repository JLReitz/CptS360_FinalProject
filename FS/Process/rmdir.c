#include "../Type.h"
#include "Utility/*"
#include "Block/*"
#include <time.h>

extern _PROC *running;
extern int _blocksize;
int dev = running->cwd->dev;

int myrmdir(char *pathname){
    int ino, parentIno;
    MINODE *mip, *pmip;
    INODE *ip, *pip;
    char dirName, path[16], dirPath, pathnameTemp = pathname;

    //tokenize pathname
   int numberOfDirs = tokenize(path, pathname, "/");

    //cat to dir path;
    for(int i = 0; i < numberOfDirs-2; i++){
        strcat(dirPath, "/");
        strcat(dirPath, path[i]);
    }

    //get ino
    parentIno = getino(dev, dirPath);

    //get minode 
    pmip = iget(dev, parentIno);

    //check for dir, check if busy, check if empty
        //if failed check, return -1
    
    //get child ino and inode
    ino = getino(dev, pathnameTemp);
    mip = iget(dev , ino);

    //make sure that exits

    //deallocate block and inode
    for(int i = 0; i <= 14; i++){
        bdealloc(dev, ip->i_block[i]);
    }

    idealloc(dev, ino);

    //remove child's entry
    rmChild(pmip, dirName);

    //decrement parent's inode link by 1
    pip->i_links_count--;

    //update parent time, make dirty
    pip->i_atime = pip->i_ctime = time(0L)
    pmip->dirty = 1;

    //write back to disk
    iput(mip);

    //success
    
    return 0;
}

int rmChild(MINODE *parentMinode, char *name){
    INODE *pip = parentMinode->INODE;
    DIR *dp, *prevDir, *lastDir;
    char buf[_blocksize], *cp, *dirName, *lastChar;
    int ino, start, end;

    //search inode for name
    //ino = search()

    get_block(dev, ino, buf);

    //erase name entry
    cp = buf;
    dp = (DIR*)buf;
    
    while(cp < buf + 1024){
        strncpy(dirName, dp->name, dp->name_len);
        dirName[dp->name_len] = 0;
        //found entry
        if(!strcmp(dirName, name)){
        
            //if first entry
            if(cp == buf && dp->rec_len == buf + 1024){
                
                free(buf);
                bdealloc(dev, ino);
                pip->i_size -= _blocksize;
                
                //any blocks after need to be moved up COME BACK TO THIS
            
            }
            //if last entry
            else if(cp + dp->rec_len == buf + 1024){

                prevDir += dp->rec_len;
                put_block(dev, ino, buf);

            }
            //if in the middle
            else{

                lastChar = buf;
                lastDir = (DIR*)lastChar;

                //go to last entry
                while(lastChar < buf + 1024){
                    lastChar += lastDir->rec_len;
                    lastDir = (DIRr*)lastChar;
                }

                lastDir->rec_len = dp->rec_len;
                
                //location of last dir
                start = cp + dp->rec_len;
                end = buf + 1024;

                //shift memory left, overwriting entry
                memmove(cp, start, end-start);

                //write back block
                put_block(dev, ino, buf);
            }

            //mark parent minode dirty
            parentMinode->dirty = 1;

            //write minode to disk
            iput(parentMinode);
        }

        prevDir = dp;
        cp += dp->rec_len;
        dp = (DIR*)cp;
    }
    //success
    return 0;
}
