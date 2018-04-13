#include "../Type.h"

int myrmdir(char *pathname){

    //get ino

    //get minode 

    //check for dir, check if busy, check if empty
        //if failed check, return -1
    
    //deallocate block and inode

    //write back

    //get parent inode

    //remove child's entry
    //rmchild()

    //decrement parent's inode link by 1

    //update parent time, make dirty

    //write back to disk

    //success
    return 0;
}

int rmChild(MINODE *parentInode, char *name){
    //search inode for name

    //erase name entry

        //if last entry

        //if first entry

        //if in the middle
    
    //write parent block back to disk

    //mark parent minode dirty

    //success
    return 0;
}