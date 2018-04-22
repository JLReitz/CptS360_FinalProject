#include "Type.h"
#include "Block_Data.c"
#include "Inode_Data.c"
#include "Util.c"
#include "mkdir.c"

extern PROC running;
extern int dev = running->cwd->dev;

int Link(char *oldPathname, char*newPathname){
  char oldFile[16]; newFile[16], *newPath = "", *newName;
  int inoOld, inoNew, numberOfTokens;
  INODE *inodeOld, *inodeNew;
  MINODE *minodeOld, *minodeNew;

  //load old into memory
  inoOld = getino(running->cwd, oldPathname);
  minodeOld = iget(dev, inoOld);
  inodeOld = &minodeOld->INODE;

  //check old is not dir
  if(S_ISDIR(inodeOld->i_mode)){
    printf("cannot link to dir\n");
    return 1;
  }

  //check new path exists but name does not
  //tokenize new pathname
  numberOfTokens = tokenize(newFile, newPathname, "/");

  //cat to new dir path
  for(int i = 0; i < numberOfTokens-2; i++){

    strcat(newPath, "/");
    strcat(newPath, newFile[i]);
  }

  newName = newFile[numberOfTokens-1];

  //check if path exists
  if(!getino(dev, newPath)){
    printf("new pathname does not exist\n");
    return 1;
  }

  //check that name doesnt exist
  if(getino(dev, newPathname)){
    printf("link name already exists\n");
    return 1;
  }

  //load new into memory
  inoNew = getino(dev, newPath);
  minodeNew = iget(dev, inoNew);
  inodeNew = &minodeNew->INODE;

  //add entry to to new path data block with old ino
  enterName(minodeNew, inoOld, newName)

  //increment i_links of inode
  inodeOld->i_links_count++;

  //write inode back to disk 
  minodeOld->dirty = 1;
  minodeNew->dirty = 1;

  iput(minodeNew);
  iput(minodeOld);

  return 0;
}

int Unlink(char *pathname){
  //char 
  int ino;
  INODE *inode;
  MINODE *minode;

  
  //make sure it's not a dir

  //decremet inode link count

  //remove inode if links = 0

    //deallocate datablocks, inode

  //remove name from parent directory 
}

int Symlink(char *oldPathname, char *newPathname){

}

int Readlink(char *pathname){
  
}

