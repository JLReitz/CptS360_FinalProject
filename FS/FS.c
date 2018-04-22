#ifndef FS_C
#define FS_C

#include "Block_Data.c"
#include "Inode_Data.c"
#include "Inode_Util.c"
#include "Allocate_Deallocate.c"
#include "Util.c"
#include "pwd.c"
#include "ls.c"
#include "cd.c"

// Global Variables *******************************************************************************

MINODE _minode[NMINODE];
MINODE * _root;

PROC   _proc[NPROC], * _running;
MNTABLE _mntable, * _mntPtr;

OFT _OpenFileTable[NOFT];

int _dev;
int _nblocks, _ninodes, _bmap, _imap, _iblk;
char _line[128], _cmd[32], _pathname[64], _buf[BLKSIZE];
char _gpath[128];   //Hold tokenized strings
char * _name[64];   //Token string pointers
int  _n;            //Number of token strings 

// Protoypes **************************************************************************************

void init();
void mount_root(char * diskname);
void run_FS(char * disk);
void quit();

// Functions **************************************************************************************

void init()
{
  int i, j;
  MINODE *mip;
  PROC   *p;

  printf("INITIALIZING...\n");

  for (i=0; i<NMINODE; i++)
  {
    mip = &_minode[i];
    mip->dev = mip->ino = 0;
    mip->refCount = 0;
    mip->mounted = 0;
    mip->mptr = 0;
  }
  
  for (i=0; i<NPROC; i++)
  {
  
    p = &_proc[i];
    p->pid = i;
    p->uid = 0;
    p->cwd = 0;
    
    for (j=0; j<NFD; j++)
      p->fd[j] = 0;
  }
} 

void mount_root(char * diskname)
{  
	printf("MOUNTING...\n");	
	
  iget(_dev, 2, _root);
  _root->mounted = 1;
  _root->mptr = &_mntable;

  _mntPtr = &_mntable;
  _mntPtr->dev = _dev;
  _mntPtr->ninodes = _ninodes;
  _mntPtr->nblocks = _nblocks;
  _mntPtr->bmap = _bmap;
  _mntPtr->imap = _imap;
  _mntPtr->iblk = _iblk;
  _mntPtr->mntDirPtr = _root;
  strcpy(_mntPtr->devName, diskname);
  strcpy(_mntPtr->mntName, "/");
}

void run_FS(char * disk)
{
	int ino;

	if ((_dev = open(disk, O_RDWR)) < 0)
	{
		printf("open %s failed\n", disk);
		return;
	}

	//Read super block at 1024
	get_block(_dev, 1, _buf);
	SUPER * sp = (SUPER *)_buf;
	
	//Verify that this is an ext32 filesystem
	printf("checking EXT2 FS...");
	if (sp->s_magic != 0xEF53)
	{
		printf("magic = %x is not an ext2 filesystem\n", sp->s_magic);
		return;
	}
	printf("EXT2 FS confirmed.\n"); 
	
	_ninodes = sp->s_inodes_count;
	_nblocks = sp->s_blocks_count;
	
	//Get group0 information
	get_block(_dev, 2, _buf); 
	GD * gp = (GD *)_buf;

	_bmap = gp->bg_block_bitmap;
	_imap = gp->bg_inode_bitmap;
	_iblk = gp->bg_inode_table;
	printf("bmp=%d _imap=%d _iblk = %d\n", _bmap, _imap, _iblk);
	
	//Initialize the filesystem and mount it
	init();  
	mount_root(disk);
	
	//Create Process 0
	printf("creating Process 0 as running process\n");
	_running = &_proc[0];
	iget(_dev, 2, _running->cwd);
	printf("Root inodes refCount = %d\n", _root->refCount);

	//printf("hit a key to continue : ");
	//getchar();
	
	//Now take user input commands
	while(1)
	{
		//TODO print out cwd instead
		printf("input command : [ls|cd|pwd|quit] ");
		fgets(_line, 128, stdin);

		_line[strlen(_line)-1] = 0;

		if (_line[0]==0)
			continue;
			
		_pathname[0] = 0;

		sscanf(_line, "%s %s", _cmd, _pathname);
		printf("_cmd=%s _pathname=%s\n", _cmd, _pathname);
		
		/*//TODO replace the following with a function stack
		if (strcmp(_cmd, "ls")==0)
			 ls(_pathname);
		if (strcmp(_cmd, "cd")==0)
			 cd(_pathname);
		if (strcmp(_cmd, "pwd")==0)
			 pwd(_pathname);
		if (strcmp(_cmd, "creat")==0)
			 creat(_pathname);
		if (strcmp(_cmd, "mkdir")==0)
			 mkdir(_pathname);
		if (strcmp(_cmd, "rmdir")==0)
			 rmdir(_pathname);
		if (strcmp(_cmd, "quit")==0)
			 quit();
			 
		printf("\_n");*/
	}
}

void quit()
{
  int i;
  MINODE *mip;
  
  for (i=0; i<NMINODE; i++)
  {
    mip = &_minode[i];
    if (mip->refCount > 0)
    	iput(mip);
  }
  
  exit(0);
}

#endif
