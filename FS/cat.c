#include "Type.h"


int catfile(char *pathname){

    char buf[1024];
    int fd, n;

    //open file for read
    fd = open_file(pathname, 0);

    while(n = myRead(fd, buf, 1024)){
        buf[n] = 0;
        printf("%s", buf);
    }

    close_file(fd);
}