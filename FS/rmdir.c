#include "Block_Data.c"
#include "Inode_Data.c"
#include "Inode_Util.c"
#include <time.h>

#ifndef RMDIR_C
#define RMDIR_C

extern PROC * _running;

//Prototypes
void myrmdir(char *pathname);
int rmChild(MINODE *parentMinode, char *name);
int rmChild(MINODE *parentMinode, char *name);


void myrmdir(char *pathname){
    int ino, parentIno;
    MINODE *mip, *pmip;
    char * dirName, * path[16], * dirPath, * pathnameTemp = pathname;
    
   int numberOfDirs = tokenize(path, pathname, "/");

	  if(numberOfDirs > 1)
	  {
		  //cat to dir path;
		  for(int i = 0; i < numberOfDirs-1; i++){
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

    //get ino
    parentIno = getino(_running->cwd->dev, dirPath);

    //get minode 
    if(pmip = iget(_running->cwd->dev, parentIno))
    {
		  //get child ino and inode
		  if(ino = getino(_running->cwd->dev, pathnameTemp))
		  {
				mip = iget(_running->cwd->dev , ino);		
				
				//deallocate block and inode
				for(int i = 0; i <= 12; i++){
				    bdealloc(_running->cwd->dev, mip->INODE.i_block[i]);
				}

				idealloc(_running->cwd->dev, ino);

				//remove child's entry
				rmChild(pmip, dirName);

				//decrement parent's inode link by 1
				pmip->INODE.i_links_count--;

				//update parent time, make dirty
				pmip->INODE.i_atime = pmip->INODE.i_ctime = time(0L);
				pmip->dirty = 1;

				//write back to disk
				iput(mip);

				//success
				return;
		  }
		  else
		  printf("The specified directory does not exist.\n");
		  
    }
    else
    	printf("The specified path does not exist.\n");

}

int rmChild(MINODE *parentMinode, char *name){
    INODE *pip = &parentMinode->INODE;
    DIR *dp, *prevdp, *lastdp;
    char buf[BLKSIZE], dirName[255], *cp, *lastcp;
    int ino, start, end;
	  
	  //Check to make sure that the child exists within the parent directory
	  if(ino = isearch_ino(parentMinode, name))
	  {
			for(int i=0; i<12; i++)
			{
				if(pip->i_block[i])
				{
					//Get DIR info
					get_block(_running->cwd->dev, pip->i_block[i], buf);

					//erase name entry
					cp = buf;
					dp = (DIR*)buf;
					
					while(cp < (buf+1024)){
							strncpy(dirName, dp->name, dp->name_len);
							dirName[dp->name_len] = 0;
							
							//found entry
							if(!strcmp(dirName, name)){
							
							    //If last entry
							    if(cp + dp->rec_len == buf + 1024){

							        prevdp->rec_len += dp->rec_len;
							        put_block(_running->cwd->dev, pip->i_block[i], buf);

							    }
							    else{
											
											//Move all following entries up
							        lastcp = cp;
							        lastdp = (DIR*)lastcp;

							        //go to last entry
							        while(lastcp < buf+1024){
							            lastcp += lastdp->rec_len;
							            lastdp = (DIR*)lastcp;
							        }

							        lastdp->rec_len = dp->rec_len;
							        
							        //location of first dir to be moved over
							        start = cp + dp->rec_len;
							        end = buf + 1024;

							        //shift memory left, overwriting entry
							        memmove(cp, start, end-start);

							        //write back block
							        put_block(_running->cwd->dev, pip->i_block[i], buf);
							    }

							    //mark parent minode dirty
							    parentMinode->dirty = 1;

							    //write minode to disk
							    iput(parentMinode);
							    
							    //success
									return 0;
							}

							prevdp = dp;
							cp += dp->rec_len;
							dp = (DIR*)cp;
					}
				}
				
				return;
			}
	  }
	  
	  //unsuccessful rmdir
	  return 1;
}

#endif
