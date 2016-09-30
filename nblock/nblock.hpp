#ifndef NBLOCK_H
#define NBLOCK_H

#include <semaphore.h>

struct NBlock {
    int id;
    sem_t* semaphore;
    int count;
    static int currentId;
    NBlock(int n) : id(currentId++), count(n) {}
};

void DestroyNBlock(int);
int CreateNBlock(int);
void WaitNBlock(int);
void SignalNBlock(int);

#endif
