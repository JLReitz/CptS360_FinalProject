#include "Block_Data.c"
#include "Inode_Data.c"
#include "Inode_Util.c"
#include <time.h>

#ifndef RMDIR_C
#define RMDIR_C

extern PROC * _running;

//Prototypes
int myrmdir(char *pathname);
int rmChild(MINODE *parentMinode, char *name);
int rmChild(MINODE *parentMinode, char *name);


int myrmdir(char *pathname){
    int ino, parentIno;
    MINODE *mip, *pmip;
    char * dirName, * path[16], * dirPath, * pathnameTemp = pathname;

   //tokenize pathname
   int numberOfDirs = tokenize(path, pathname, "/");

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
				for(int i = 0; i <= 14; i++){
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
				return 0;
		  }
		  else
		  printf("The specified directory does not exist.\n");
		  
    }
    else
    	printf("The specified path does not exist.\n");
    
    //Unsuccessful rmdir
    return 1;
}

int rmChild(MINODE *parentMinode, char *name){
    INODE *pip = &parentMinode->INODE;
    DIR *dp, *prevDir, *lastDir;
    char buf[BLKSIZE], *cp, *dirName, *lastChar;
    int ino, start, end;

    //search inode for name
    if(ino = isearch_ino(parentMinode, name)){
		  get_block(_running->cwd->dev, ino, buf);

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
		              
		              int i = 0;
		              bdealloc(_running->cwd->dev, ino);
		              pip->i_size -= BLKSIZE;
		              
		          //any blocks after need to be moved up
		        	while(pip->i_block[i + 1] && i+1 < 12){
								i++;
								get_block(_running->cwd->dev, pip->i_block[i], buf);
								put_block(_running->cwd->dev, pip->i_block[i-1], buf);
					  	}		
		          }
		          //if last entry
		          else if(cp + dp->rec_len == buf + 1024){

		              prevDir += dp->rec_len;
		              put_block(_running->cwd->dev, ino, buf);

		          }
		          //if in the middle
		          else{

		              lastChar = buf;
		              lastDir = (DIR*)lastChar;

		              //go to last entry
		              while(lastChar < buf + 1024){
		                  lastChar += lastDir->rec_len;
		                  lastDir = (DIR*)lastChar;
		              }

		              lastDir->rec_len = dp->rec_len;
		              
		              //location of last dir
		              start = cp + dp->rec_len;
		              end = buf + 1024;

		              //shift memory left, overwriting entry
		              memmove(cp, start, end-start);

		              //write back block
		              put_block(_running->cwd->dev, ino, buf);
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
    else
    
    return 1;
}

#endif
