#include "type.h"

void searchFile(INODE * inode, char * filename, u32 ino)
{
	int i = 0;
	char buf[1024];
	
	//Check for if the supplied inode is a directory
	if(inode->i_mode == 16877)
	{
		//Access every data block that this directory contains
		for(i; i<12; i++)
		{
			//Use this INODE to find the data block number
			u32 iblock = inode->i_block[i];
			
			if(iblock) //If the block number is 0, that block does not exist
			{
				//Read in the block and print the directory listing
				get_block(dev, iblock, buf);
				
				char * cp = buf; //Character pointer to hold the current position within the block
				DIR * dp = (DIR *)buf;
				
				while(cp < &buf[BLKSIZE])
				{
					if(dp->inode == ino)
					{
						strcpy(filename, dp->name);
						return;
					}
					
					//Increment cp and set dp to the next file in the directory
					cp += dp->rec_len;
					dp = (DIR *)cp;
				}
				
				printf("\n");
			}
		}
	}
	else
	{
		printf("The file supplied was not a directory\n");
		exit(1);
	}
}

void list_file()
{
	INODE * curr_inode = &(running->cwd->INODE);
	
	printf("\nls results:\n\n");
	print_dir(curr_inode);
	printf("\n");
}

void change_dir()
{
	printf("cd to %s\n", pathname);
	
	u32 ino = kcwgetino(dev, pathname);
	
	if(ino)
		running->cwd = iget(dev, ino);
}

void pwd(MINODE * dir)
{
	char * filename;
	
	if(dir->ino == 2)
	{
		printf("/");
		return;
	}
	
	if(kcwsearch(dir, "..") == 2) //If your parent directory is root
	{
		ip = &(iget(dev, 2)->INODE);
		
		//Load the name of the current directory
		searchFile(ip, filename, 2);
		printf("/%s", filename);
		
		return;
	}
	else
		ip = &(iget(dev, kcwsearch(dir, ".."))->INODE);
		
	pwd(iget(dev, kcwsearch(dir, "..")));
	
	//Load the name of the current directory
	searchFile(ip, filename, kcwsearch(dir, "."));
	printf("/%s", filename);
	
	return;
}

#include "util.c"
//#include "ls_cd_pwd.c"


// load root INODE and set root pointer to it
int mount_root()
{  
  printf("mount_root()\n");
  root = iget(dev, 2);
  root->mounted = 1;
  root->mptr = &mntable;

  mntPtr = &mntable;
  mntPtr->dev = dev;
  mntPtr->ninodes = ninodes;
  mntPtr->nblocks = nblocks;
  mntPtr->bmap = bmap;
  mntPtr->imap = imap;
  mntPtr->iblk = iblk;
  mntPtr->mntDirPtr = root;
  strcpy(mntPtr->devName, "mydisk");
  strcpy(mntPtr->mntName, "/");
}

char *disk = "mydisk";
main(int argc, char *argv[ ])
{
  int ino;
  char buf[BLKSIZE];

  if (argc > 1)
    disk = argv[1];

  printf("checking EXT2 FS ....");
  if ((fd = open(disk, O_RDWR)) < 0){
    printf("open %s failed\n", disk);  exit(1);
  }
  dev = fd;

  /********** read super block at 1024 ****************/
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;

  /* verify it's an ext2 file system *****************/
  if (sp->s_magic != 0xEF53){
      printf("magic = %x is not an ext2 filesystem\n", sp->s_magic);
      exit(1);
  }     
  printf("OK\n");
  ninodes = sp->s_inodes_count;
  nblocks = sp->s_blocks_count;
  
  get_block(dev, 2, buf); 
  gp = (GD *)buf;

  bmap = gp->bg_block_bitmap;
  imap = gp->bg_inode_bitmap;
  iblk = gp->bg_inode_table;
  printf("bmp=%d imap=%d iblk = %d\n", bmap, imap, iblk);

  init();  
  mount_root();
  printf("root refCount = %d\n", root->refCount);

  printf("creating P0 as running process\n");
  running = &proc[0];
  running->cwd = iget(dev, 2);
  printf("root refCount = %d\n", root->refCount);

  //printf("hit a key to continue : "); getchar();
  while(1){
    printf("input command : [ls|cd|pwd|quit] ");
    fgets(line, 128, stdin);

    line[strlen(line)-1] = 0;

    if (line[0]==0)
      continue;
    pathname[0] = 0;

    sscanf(line, "%s %s", cmd, pathname);
    printf("cmd=%s pathname=%s\n", cmd, pathname);

    if (strcmp(cmd, "ls")==0)
       list_file();
    if (strcmp(cmd, "cd")==0)
       change_dir();
    if (strcmp(cmd, "pwd")==0)
    {
       printf("\npwd result:\n");
       pwd(running->cwd);
       printf("\n");
    }

    if (strcmp(cmd, "quit")==0)
       quit();
  }
}
 
int quit()
{
  int i;
  MINODE *mip;
  for (i=0; i<NMINODE; i++){
    mip = &minode[i];
    if (mip->refCount > 0)
      iput(mip);
  }
  exit(0);
}

