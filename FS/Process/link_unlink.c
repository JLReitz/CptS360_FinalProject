#include "../Type.h"
#include "../Block/Block_Data.c"
#include "../Block/Inode_Data.c"
#include "../Utility/Util.c"
#include "mkdir.c"

extern PROC running;

int myLink(char *oldPathname, char*newPathname){
  char oldFile[16]; newFile[16], newPath[16], newName[16];
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

  //load new into memory
  inoNew = getino(running->cwd, newPathname);
  minodeNew = iget(dev, inoNew);
  //inodeNew = &minodeNew->INODE;

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

int myUnlink(char *pathname){
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

int mySymlink(char *oldPathname, char *newPathname){

}

int myReadlink(char *pathname){
  
}

