#include "Type.h"
#include "Block_Data.c"
#include "Inode_Data.c"
#include "Util.c"
#include "mkdir.c"

extern PROC * _running;
extern int dev = running->cwd->dev;

int Link(char *oldPathname, char*newPathname){
  char *oldFile[16]; *newFile[16], *newPath = "", *newName;
  int inoOld, inoNew, numberOfTokens;
  INODE *inodeOld, *inodeNew;
  MINODE *minodeOld, *minodeNew;

  //load old into memory
  inoOld = getino(running->cwd, oldPathname);
  minodeOld = iget(dev, inoOld);
  inodeOld = &minodeOld->INODE;

  //check old is not dir
  if(S_ISDIR(inodeOld->i_mode)){
    printf("Cannot link to a directory.\n");
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
    printf("Supplied pathname does not exist.\n");
    return 1;
  }

  //check that name doesnt exist
  if(getino(dev, newPathname)){
    printf("Link name already exists\n");
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
  char *path[16], *dirPath = "", *dirName, buf[_BLKSIZE]; 
  int ino, parentIno;
  INODE *inode, *parentInode;
  MINODE *minode, *parentMinode;

  
  //make sure it's not a dir
 /* //tokenize new pathname
  numberOfTokens = tokenize(path, Pathname, "/");

  //cat to new dir path
  for(int i = 0; i < numberOfTokens-2; i++){

    strcat(dirPath, "/");
    strcat(dirPath, path[i]);
  }

  dirName = path[numberOfTokens-1];*/

  ino = getino(dev, pathname);
  minode = iget(dev, ino);
  inode = &minode->INODE;

  if(S_ISDIR(inode->i_mode)){
    printf("cannot unlink dir\n");
    return 1;
  }  

  //decremet inode link count

  inode->i_links_count--;

  //remove inode if links = 0
  if(!inode->i_links_count){

    //deallocate datablocks, inode
    for(int i = 0; i < 12 && inode->i_block[i]; i++){

      bdeallocate(dev, inode->i_block[i]);
    }

    idealloc(dev, ino);
  }

  //remove name from parent directory 
  parentIno = getino(dev, pathname);
  parentMinode = iget(dev, parentIno);
  parentInode = parentMinode->INODE;

  rm_child(parentMinode, dirPath);

  parentInode->i_atime = parentInode->i_mtime = time(0L);
  parentMinode->dirty = 1;
  minode->dirty = 1;
  iput(minode);
  iput(parentMinode);

  return 0;
}

int Symlink(char *oldPathname, char *newPathname){
  int oldIno, linkIno;
  INODE *oldInode, *linkInode;
  MINODE *oldMinode, *linkMinode;

  //verify old pathname
  if(!oldIno = getino(dev, oldPathname)){
    printf("path does not exist\n");
    return 1;
  }
  oldMinode = iget(dev, oldIno);
  oldInode = oldMinode->INODE;

  //create new pathname file
  if(creat(newPathname)){
    printf("failed to create file\n");
    return 1;
  }

  oldMinode->dirty = 1;

  linkIno = getino(dev, newPathname);
  linkMinode = iget(dev, linkIno);
  linkInode = linkMinode->INODE;

  linkInode->i_mode = 0120000;

  linkInode->i_block[0] = oldPathname; //NOT SURE ABOUT THIS

  strcpy(buf, oldPathname);

  put_block(dev, linkInode->i_block[0], buf);

  linkMinode->dirty = 1;

  iput(linkMinode);
}

int Readlink(char *pathname, char* container){
  int ino, i = 0;
  MINODE * minode;
  INODE *inode;
  container = "";

  //load pathname inode into memory
  ino = getino(dev, pathname);
  minode = iget(dev, ino);
  inode = &minode->INODE;

  //check if sym link
  if(!S_ISLNK(inode->i_mode)){
    printf("not a link file\n");
    return 1;
  }

  //return contents of i blocks
  while(inode->i_block[i]){
    get_block(dev, inode->i_block[i], buf);  
    strcat(container, buf);
    i++;
  }
  return 0;
}

