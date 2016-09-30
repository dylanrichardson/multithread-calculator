#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "node.hpp"
#include <map>
#include <string>
#include <vector>
#include <semaphore.h>

typedef struct {
    int value;
    int duration;
} GraphResult;

class Scheduler {
    public:
        Scheduler(std::vector<Node*>);
        ~Scheduler();
        GraphResult run();
        static void* _runNode(void*);
    private:
        std::vector<Node*> nodes;
        std::vector<pthread_t> threads;

        void setupNodes(std::vector<Node*>);
        void initSemCtrls();
        void initSemCtrl(Node*);
        void initTotalMutex();
        sem_t* getSemaphore(Node*);
        void deleteNodes();
        void runThread(int);
        void runNode(Node*);
        void waitForThreads();
        void waitForDependencies(Node*);
        int computeValue(Node*);
        void incrementTotal(int);
        void waitForTotal();
        void signalTotal();
        void signalNextNodes(Node*);
        void signalNode(Node*);
        bool signalSemCtrl(Node*);
        Node* getNodeById(NodeId);
        int getGraphDuration();
        int getNodeTotalDuration(Node*);
        int getNodeDependentDuration(Node*);
        void printComputation(Node*, int);
};

struct Noduler {
    Scheduler* scheduler;
    Node* node;
    Noduler(Scheduler* _scheduler, Node* _node) : scheduler(_scheduler), node(_node) {}
};

struct SemCtrl {
    sem_t* semaphore;
    int count;
};

std::string durationSeconds(int);

#endif
