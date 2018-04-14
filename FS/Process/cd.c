#include "../Type.h"
#include "Utility/*"
#include "Block/*"

void cd(char * pathname)
{	
	int ino = getino(dev, pathname);
	
	if(ino)
		running->cwd = iget(dev, ino);
}
