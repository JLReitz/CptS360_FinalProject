#include "FS/FS.c"

void main(int argc, char * argv[])
{
	if(argc > 1)
		run_FS(argv[1]);
	else
		printf("Please insert a virtual disk.\n");
		
	exit(0);
}
