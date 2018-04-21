#include "FS/FS.c"

void main(int argc, char * argv[])
{
#ifdef DEBUG_BASEFS
	run_FS("../diskimage");
	
	exit(0);
#endif

	if(argc > 1)
		run_FS(argv[1]);
	else
		printf("Please insert a virtual disk.\n");
		
	exit(0);
}
