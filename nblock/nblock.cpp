// Dylan Richardson
#include "nblock.hpp"
#include <semaphore.h>
#include <map>
#include <iostream>

using namespace std;

map<int, NBlock*> NBID_NBLOCK;

NBlock* getNBlock(int id) {
    return NBID_NBLOCK[id];
}

void setNBlock(NBlock* nBlock) {
    NBID_NBLOCK[nBlock->id] = nBlock;
}

int CreateNBlock(int n) {
    NBlock* nBlock = new NBlock(n);
    nBlock->semaphore = new sem_t;
    if (sem_init(nBlock->semaphore, 0, n ? 0 : 1)) {
        return -1;
    }
    setNBlock(nBlock);
    return nBlock->id;
}

void DestroyNBlock(int id) {
    sem_destroy(getNBlock(id)->semaphore);
    delete getNBlock(id)->semaphore;
    delete getNBlock(id);
}

void WaitNBlock(int id) {
    sem_wait(getNBlock(id)->semaphore);
}

void SignalNBlock(int id) {
    NBlock* nBlock = getNBlock(id);
    nBlock->count--;
    if (nBlock->count == 0) {
        sem_post(nBlock->semaphore);
    }
}
