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
#include "mkdir.c"
#include "rmdir.c"
#include "link_unlink.c"

// Global Variables ********************************************************************************

int _dev;
int _nblocks, _ninodes, _bmap, _imap, _iblk;
char _line[128], _buf[BLKSIZE];
char _cwd[BLKSIZE];
char * _cmd[32];


MINODE * _root;
MINODE _minode[NMINODE];

MNTABLE _mntable, * _mntPtr;

PROC   _proc[NPROC], * _running;

OFT _OpenFileTable[NOFT];

// Protoypes ***************************************************************************************

void init();
void mount_root(char * diskname);
void run_FS(char * disk);
void quit();

//Function pointers ********************************************************************************

void (*functions_0arg[])() = {pwd, quit};
void (*functions_1arg[])(char *) = {ls, cd, mycreat, mymkdir, myrmdir, unlink, readlink};
void (*functions_2arg[])(char *, char *) = {link, symlink};

// Functions ***************************************************************************************

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
	
	_mntPtr = &_mntable;
  _mntPtr->dev = _dev;
  _mntPtr->ninodes = _ninodes;
  _mntPtr->nblocks = _nblocks;
  _mntPtr->bmap = _bmap;
  _mntPtr->imap = _imap;
  _mntPtr->iblk = _iblk;
	
  _root = iget(_dev, 2);
  _root->mounted = 1;
  _root->mptr = &_mntable;
  _mntPtr->mntDirPtr = _root;
  
  strcpy(_mntPtr->devName, diskname);
  strcpy(_mntPtr->mntName, "/");
}

void run_FS(char * disk)
{
	int argc, function;

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
	_running->cwd = iget(_dev, 2);
	strcpy(_cwd, "/");
	printf("Root inodes refCount = %d\n\n", _root->refCount);
	
	//Now take user input commands
	while(1)
	{
		printf("%s >> ", _cwd);
		fgets(_line, 128, stdin);
		
		//Replace newline character
		_line[strlen(_line)-1] = 0;

		if (_line[0]==0)
			continue;
		
		argc = tokenize(_cmd, _line, " ");
		
		switch(argc)
		{
		case 1:
			
			if((function = find_0arg_function(_cmd[0])) >= 0)
				(*functions_0arg[function])();
			else
				printf("This command does not exist.\n");
			
			break;
		case 2:
			
			if((function = find_1arg_function(_cmd[0])) >= 0)
				(*functions_1arg[function])(_cmd[1]);
			else
				printf("This command does not exist.\n");
			
			break;
		case 3:
		
			if((function = find_2arg_function(_cmd[0])) >= 0)
				(*functions_2arg[function])(_cmd[1], _cmd[2]);
			else
				printf("This command does not exist.\n");
				
		default:
		
			break;
		}
	}
}

void quit()
{
  int i;
  MINODE *mip;
  
  //Close process0
  iput(_running->cwd);
  
  //Close the rest of the inodes
  for (i=0; i<NMINODE; i++)
  {
    mip = &_minode[i];
    
    iput(mip);
  }
  
  exit(0);
}

#endif
