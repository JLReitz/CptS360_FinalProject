#ifndef LINK_UNLINK_C
#define LINK_UNLINK_C

#include <string.h>

extern PROC * _running;

int truncate(MINODE *mip){
    int buf[256], indirectBuf[256];
    INODE *inode = &mip->INODE;

    //release inode data blocks
    //direct blocks
    for(int i = 0; i < 12; i++){
        if(inode->i_block[i])
            bdealloc(_running->cwd->dev, inode->i_block[i]);
    }

    //indirect
    if(inode->i_block[12]){
        
        get_block(_running->cwd->dev, inode->i_block[12], (char *)buf);

        for (int i = 0; i < 256; i++){
            if(buf[i])
                bdealloc(_running->cwd->dev, buf[i]);
        }
    }

    //doube indirect
    if(inode->i_block[13]){

        get_block(_running->cwd->dev, inode->i_block[13], (char *)buf);

        for(int i = 0; i < 256; i++){

            get_block(_running->cwd->dev, buf[i], (char *)indirectBuf);
            for(int j = 0; j < 256; j++){

                if(indirectBuf[j])
                    bdealloc(_running->cwd->dev, indirectBuf[j]);
            }
        }
    }

    //update time field
    inode->i_atime = inode->i_mtime = time(0L);

    //inode size = 0, mark dirty
    inode->i_size = 0;
    mip->dirty = 1;

    return 0;
}

void link(char *oldPathname, char*newPathname){
  char *oldFile[16], *newFile[16], *newPath = "", *newName;
  int inoOld, inoNew, numberOfTokens;
  INODE *inodeOld, *inodeNew;
  MINODE *minodeOld, *minodeNew;

  //load old into memory
  inoOld = getino(_running->cwd->dev, oldPathname);
  minodeOld = iget(_running->cwd->dev, inoOld);
  inodeOld = &minodeOld->INODE;

  //check old is not dir
  if(S_ISDIR(inodeOld->i_mode)){
    printf("Cannot link to a directory.\n");
    return;
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
  if(!getino(_running->cwd->dev, newPath)){
    printf("Supplied pathname does not exist.\n");
    return;
  }

  //check that name doesnt exist
  if(getino(_running->cwd->dev, newPathname)){
    printf("Link name already exists\n");
    return;
  }

  //load new into memory
  inoNew = getino(_running->cwd->dev, newPath);
  minodeNew = iget(_running->cwd->dev, inoNew);
  inodeNew = &minodeNew->INODE;

  //add entry to to new path data block with old ino
  enterName(minodeNew, inoOld, newName);

  //increment i_links of inode
  inodeOld->i_links_count++;

  //write inode back to disk 
  minodeOld->dirty = 1;
  minodeNew->dirty = 1;

  iput(minodeNew);
  iput(minodeOld);
}

void unlink(char *pathname){
  char *path[16], filePath[BLKSIZE], *fileName, buf[BLKSIZE], pathnameTemp[BLKSIZE]; 
  int ino, parentIno;
  INODE *inode, *parentInode;
  MINODE *minode, *parentMinode;

 	//tokenize new pathname
  strcpy(pathnameTemp, pathname);
	int numberOfDirs = tokenize(path, pathname, "/");

	if(numberOfDirs > 1)
	{
		//cat to dir path;
		for(int i = 0; i < numberOfDirs-1; i++){
			  strcat(filePath, "/");
			  strcat(filePath, path[i]);
		}
		
		fileName = path[numberOfDirs - 1];
	}
	else
	{
		strcpy(filePath, ".");
		fileName = pathname;
	}

  ino = getino(_running->cwd->dev, pathname);
  minode = iget(_running->cwd->dev, ino);
  inode = &minode->INODE;

  if(S_ISDIR(inode->i_mode)){
    printf("cannot unlink dir\n");
    return;
  }  

  //decremet inode link count

  inode->i_links_count--;

  //remove inode if links = 0
  if(!inode->i_links_count){
  
  	truncate(minode);
  }

  //remove name from parent directory 
  parentIno = getino(_running->cwd->dev, filePath);
  parentMinode = iget(_running->cwd->dev, parentIno);
  parentInode = &parentMinode->INODE;

  rmChild(parentMinode, fileName);

  parentInode->i_atime = parentInode->i_mtime = time(0L);
  parentMinode->dirty = 1;
  minode->dirty = 1;
  iput(minode);
  iput(parentMinode);
}

void symlink(char *oldPathname, char *newPathname){
  int oldIno, linkIno;
  char linkTarget_pathname[60] = "/";
  INODE *oldInode, *linkInode;
  MINODE *oldMinode, *linkMinode;

  //verify old pathname
  if(!(oldIno = getino(_running->cwd->dev, oldPathname))){
    printf("path does not exist\n");
    return;
  }
  
  strncat(linkTarget_pathname, oldPathname, 59);
  
  oldMinode = iget(_running->cwd->dev, oldIno);
  oldInode = &oldMinode->INODE;

  //create new pathname file
  mycreat(newPathname);

  oldMinode->dirty = 1;

  linkIno = getino(_running->cwd->dev, newPathname);
  linkMinode = iget(_running->cwd->dev, linkIno);
  linkInode = &linkMinode->INODE;

  linkInode->i_mode = 0120000;
  
  for(int j=0; j<60; j+=4)
  {
  	char cbuf[4] = {linkTarget_pathname[j], linkTarget_pathname[j+1], linkTarget_pathname[j+2], linkTarget_pathname[j+3]};
  	
  	memcpy(&linkInode->i_block[j/4], cbuf, 4);
  }

  linkMinode->dirty = 1;

  iput(linkMinode);
}

void readlink(char *pathname){
  int ino, i = 0;
  char buf[4];
  MINODE * minode;
  INODE *inode;

  //load pathname inode into memory
  ino = getino(_running->cwd->dev, pathname);
  minode = iget(_running->cwd->dev, ino);
  inode = &minode->INODE;

  //check if sym link
  if(!S_ISLNK(inode->i_mode)){
    printf("not a link file\n");
    return;
  }

  //return contents of i blocks
  for(int i=0; i<15; i++)
  {
  	if(inode->i_block[i])
  	{
			memcpy(buf, &inode->i_block[i], 4);
			
			printf("%s", buf);
  	}
  }
  
  printf("\n");
}

#endif
