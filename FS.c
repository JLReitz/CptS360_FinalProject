#include "Type.h"
#include "Util.c"

// Global Variables *******************************************************************************

MINODE minode[NMINODE];
MINODE *root;

PROC   proc[NPROC], *running;
MNTABLE mntable, *mntPtr;

SUPER *sp;
GD    *gp;
INODE *ip;

int fd, dev;
int nblocks, ninodes, bmap, imap, iblk;
char line[128], cmd[32], pathname[64];

char gpath[128];   // hold tokenized strings
char *name[64];    // token string pointers
int  n;            // number of token strings 

// Protoypes **************************************************************************************

int mount_root();
int run_FS(char * disk);

void init();
void quit();

// Functions **************************************************************************************

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

int run_FS(char * disk)
{
	int ino;
	char buf[BLKSIZE];

	if (argc > 1)
	disk = argv[1];

	printf("checking EXT2 FS ....");
	if ((fd = open(disk, O_RDWR)) < 0)
	{
		printf("open %s failed\n", disk);
		exit(1);
	}
	
	dev = fd;

	//Read super block at 1024
	get_block(dev, 1, buf);
	sp = (SUPER *)buf;

	//Verify that this is an ext32 filesystem
	if (sp->s_magic != 0xEF53)
	{
		printf("magic = %x is not an ext2 filesystem\n", sp->s_magic);
		exit(1);
	}  
	   
	printf("OK\n");
	
	ninodes = sp->s_inodes_count;
	nblocks = sp->s_blocks_count;
	
	//Get group0 information
	get_block(dev, 2, buf); 
	gp = (GD *)buf;

	bmap = gp->bg_block_bitmap;
	imap = gp->bg_inode_bitmap;
	iblk = gp->bg_inode_table;
	printf("bmp=%d imap=%d iblk = %d\n", bmap, imap, iblk);
	
	//Initialize the filesystem and mount it
	init();  
	mount_root();
	printf("root refCount = %d\n", root->refCount);

	printf("creating P0 as running process\n");
	running = &proc[0];
	running->cwd = iget(dev, 2);
	printf("root refCount = %d\n", root->refCount);

	//printf("hit a key to continue : "); 
	//getchar();
	
	//Now take user input commands
	while(1)
	{
		printf("input command : [ls|cd|pwd|quit] ");
		fgets(line, 128, stdin);

		line[strlen(line)-1] = 0;

		if (line[0]==0)
			continue;
			
		pathname[0] = 0;

		sscanf(line, "%s %s", cmd, pathname);
		printf("cmd=%s pathname=%s\n", cmd, pathname);
		
		//TODO replace the following with a function stack
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

void init()
{
  int i, j;
  MINODE *mip;
  PROC   *p;

  printf("init()\n");

  for (i=0; i<NMINODE; i++)
  {
    mip = &minode[i];
    mip->dev = mip->ino = 0;
    mip->refCount = 0;
    mip->mounted = 0;
    mip->mptr = 0;
  }
  for (i=0; i<NPROC; i++){
  
    p = &proc[i];
    p->pid = i;
    p->uid = 0;
    p->cwd = 0;
    
    for (j=0; j<NFD; j++)
      p->fd[j] = 0;
  }
} 

void quit()
{
  int i;
  MINODE *mip;
  
  for (i=0; i<NMINODE; i++)
  {
    mip = &minode[i];
    if (mip->refCount > 0)
    	iput(mip);
  }
  
  exit(0);
}
