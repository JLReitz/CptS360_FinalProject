#include "../FS/FS.c"

//#define DEBUG_TEST

// Global Variables ********************************************************************************

extern int _dev;
extern int _nblocks, _ninodes, _bmap, _imap, _iblk;
extern char _line[128], _buf[BLKSIZE];
extern char _cwd[BLKSIZE];
extern char * _cmd[32];


extern MINODE * _root;
extern MINODE _minode[NMINODE];

extern MNTABLE _mntable, * _mntPtr;

extern PROC   _proc[NPROC], * _running;

extern OFT _OpenFileTable[NOFT];

// Run Test ****************************************************************************************

int main(int argc, char * argv[])
{
	char * disk;	
	
	if(argc < 2)
	{
		disk = "diskimage_test";
	}	
	else
		disk = argv[1];
	
	if ((_dev = open(disk, O_RDWR)) < 0)
	{
		printf("open %s failed\n", disk);
		exit(0);
	}

	//Read super block at 1024
	get_block(_dev, 1, _buf);
	SUPER * sp = (SUPER *)_buf;
	
	//Verify that this is an ext32 filesystem
	printf("checking EXT2 FS...");
	if (sp->s_magic != 0xEF53)
	{
		printf("magic = %x is not an ext2 filesystem\n", sp->s_magic);
		exit(0);
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
	
	//Run test
	printf("mkdir\n");
	mymkdir("testdir");
	printf("ls: ");
	ls(0);
	
	printf("rmdir\n");
	myrmdir("testdir");
	printf("ls: ");
	ls(0);
	
	quit();
	
	exit(0);
}
