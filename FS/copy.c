#include "Type.h"

myCopy(char *source, char  *dest){
    int n;
    char *buf[1024];
    
    int fd = open_file(source, 0);

    int gd = open_file(dest, 1);

    while(n = read(fd, buf, 1024)){
        write(gd, buf, n);
    }
}