// Dylan Richardson
#include "scheduler.hpp"
#include "node.hpp"
#include "nblock.hpp"
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <pthread.h>
#include <semaphore.h>

using namespace std;

int TOTAL = 0;
sem_t TOTAL_MUTEX;
map<NodeId, int> NODEID_NBID;
int NBlock::currentId = 10;

class MatchesNodeId {
    NodeId id;

    public:
        MatchesNodeId(NodeId Id) : id(Id) {}

        bool operator()(const Node* node) const {
            return node->id == id;
        }
};

Scheduler::Scheduler(vector<Node*> nodes) {
    setupNodes(nodes);
    threads.resize(nodes.size());
    initNBlocks();
    initTotalMutex();
}

Scheduler::~Scheduler() {
    for (size_t i = 0, max = nodes.size(); i < max; i++) {
        DestroyNBlock(getNBlockId(nodes[i]));
        delete nodes[i];
    }
}

void Scheduler::setupNodes(vector<Node*> nodes) {
    this->nodes = nodes;
    Node* next;
    Node* dep;
    vector<NodeId> deps;
    for (size_t i = 0, maxi = nodes.size(); i < maxi; i++) {
        next = nodes[i];
        deps = next->getDependencies();
        for (size_t j = 0, maxj = deps.size(); j < maxj; j++) {
            dep = getNodeById(deps[j]);
            dep->addNextNode(next);
        }
    }
}

void Scheduler::initNBlocks() {
    for (size_t i = 0, max = nodes.size(); i < max; i++) {
        initNBlock(nodes[i]);
    }
}

void Scheduler::initNBlock(Node* node) {
    NodeId nodeId = node->getId();
    int depCount = node->getDepCount();
    int id = CreateNBlock(depCount);
    if (!id) {
        cout << "unable to create NBlock for node " << nodeId << ".\n";
    }
    setNBlockId(node, id);
}

int Scheduler::getNBlockId(Node* node) {
    return NODEID_NBID[node->getId()];
}

void Scheduler::setNBlockId(Node* node, int id) {
    NODEID_NBID[node->getId()] = id;
}

void Scheduler::initTotalMutex() {
    sem_init(&TOTAL_MUTEX, 0, 1);
}

GraphResult Scheduler::run() {
    // run each node
    for (size_t i = 0, max = threads.size(); i < max; i++) {
        runThread(i);
    }
    // wait for threads to exit
    waitForThreads();
    // return graph results
    GraphResult result;
    result.value = TOTAL;
    result.duration = getGraphDuration();
    return result;
}

void Scheduler::runThread(int i) {
    Node* node = nodes[i];
    // package this scheduler object and the current node into one struct
    Noduler* noduler = new Noduler(this, node);
    // create a new thread for the node
    if (pthread_create(&threads[i], NULL, _runNode, (void*) noduler)) {
        cerr << "Failed to create a thread for node " << node->getId() << ".\n";
    }
}

void Scheduler::waitForThreads() {
    for (size_t i = 0, max = threads.size(); i < max; i++) {
        if (pthread_join(threads[i], NULL)) {
            cerr << "Failed to join the thread for node " << nodes[i]->getId() << ".\n";
        }
    }
}

void* Scheduler::_runNode(void* context) {
    Noduler* noduler = (Noduler*) context;
    Scheduler* scheduler = noduler->scheduler;
    Node* node = noduler->node;
    scheduler->runNode(node);
    delete noduler;
    return NULL;
}

void Scheduler::runNode(Node* node) {
    // wait for completion of dependent nodes
    waitForDependencies(node);
    // compute value
    int value = computeValue(node);
    // increment computed value in shared global variable.
    incrementTotal(value);
    // print info
    printComputation(node, value);
    // signal completion for all dependent nodes
    signalNextNodes(node);
}

void Scheduler::printComputation(Node* node, int value) {
    int duration = getNodeTotalDuration(node);
    waitForTotal();
    cout << "Node " << node->getId() << " computed a value of " << value;
    cout << " after " << durationSeconds(duration) << ".\n";
    signalTotal();
}

void Scheduler::waitForDependencies(Node* node) {
    int id = getNBlockId(node);
    WaitNBlock(id);
}

int Scheduler::computeValue(Node* node) {
    sleep(node->getDuration());
    return node->getValue();
}

void Scheduler::incrementTotal(int value) {
    waitForTotal();
    TOTAL += value;
    signalTotal();
}

void Scheduler::waitForTotal() {
    sem_wait(&TOTAL_MUTEX);
}

void Scheduler::signalTotal() {
    sem_post(&TOTAL_MUTEX);
}

void Scheduler::signalNextNodes(Node* node) {
    vector<Node*> nextNodes = node->getNextNodes();
    for (size_t i = 0, max = nextNodes.size(); i < max; i++) {
        signalNode(nextNodes[i]);
    }
}

void Scheduler::signalNode(Node* node) {
    int id = getNBlockId(node);
    SignalNBlock(id);
}

Node* Scheduler::getNodeById(NodeId id) {
    vector<Node*>::iterator it = find_if(nodes.begin(), nodes.end(), MatchesNodeId(id));
    return *it;
}

int Scheduler::getGraphDuration() {
    int maxDur = 0;
    int duration;
    for (size_t i = 0, max = nodes.size(); i < max; i++) {
        duration = getNodeTotalDuration(nodes[i]);
        if (duration > maxDur) {
            maxDur = duration;
        }
    }
    return maxDur;
}

int Scheduler::getNodeTotalDuration(Node* node) {
    if (node->hasTotalDuration()) {
        return node->getTotalDuration();
    } else {
        int duration = node->getDuration() + getNodeDependentDuration(node);
        node->setTotalDuration(duration);
        return duration;
    }
}

int Scheduler::getNodeDependentDuration(Node* node) {
    int maxDur = 0;
    int duration;
    Node* depNode;
    vector<NodeId> deps = node->getDependencies();
    for (size_t i = 0, max = deps.size(); i < max; i++) {
        depNode = getNodeById(deps[i]);
        duration = getNodeTotalDuration(depNode);
        if (duration > maxDur) {
            maxDur = duration;
        }
    }
    return maxDur;
}

string durationSeconds(int duration) {
    stringstream ss;
    ss << duration << " second" << ((duration == 1) ? "" : "s");
    return ss.str();
}
