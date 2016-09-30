// Dylan Richardson
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include "scheduler.hpp"
#include "node.hpp"

using namespace std;

bool getConfig(int, char*[], ifstream &config);
string getFileName(int, char*[]);
Scheduler* parseConfig(ifstream &config);
vector<vector<string> > configToVectors(ifstream &config);
vector<Node*> configToNodes(vector<vector<string> >);
Node* lineToNode(vector<string>, int);
NodeId idFromLine(vector<string>);
vector<NodeId> depsFromLine(vector<string>);
Expression exprFromLine(vector<string>, NodeId);
size_t findEqualSign(vector<string>);
int valueFromLine(vector<string>);
int durationFromLine(vector<string>);
bool validateConfig(vector<vector<string> >);
bool validateLine(vector<string>);
bool validateDeps(vector<string>);
bool validateNodeId(string);
bool validateDuration(string);
bool validateValue(string);
vector<string> split(const string &s, char);
void printResult(GraphResult);

const int NODE_ID_OFFSET = 0;
const int VALUE_OFFSET = 1;
const int DURATION_OFFSET = 2;
const int DEP_OFFSET = 3;

// run the program
int main(int argc, char* argv[]) {
    // get the config file
    ifstream config;
    if (!getConfig(argc, argv, config)) {
        exit(1);
    }
    // parse the config file
    Scheduler* scheduler;
    if (!(scheduler = parseConfig(config))) {
        cout << "The configuration file could not be parsed.\n";
        exit(1);
    }
    // run the scheduler
    GraphResult result = scheduler->run();
    printResult(result);
    // delete the scheduler
    delete scheduler;
    return 0;
}

bool getConfig(int argc, char* argv[], ifstream &config) {
    // get file name
    string fileName = getFileName(argc, argv);
    if (fileName == "") {
        return false;
    }
    // open the file
    config.open(fileName.c_str());
    if (!config) {
        cerr << "Could not find the configuration file: " << fileName << "\n";
        return false;
    }
    return true;
}

string getFileName(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Wrong number of arguments.\n";
        return "";
    }
    return argv[1];
}

// parse the configuration file and
Scheduler* parseConfig(ifstream &config) {
    // get config as 2D vector of strings
    vector<vector<string> > vecs = configToVectors(config);
    // validate the config vector
    if (!validateConfig(vecs)) {
        return NULL;
    }

    vector<Node*> nodes = configToNodes(vecs);

    return new Scheduler(nodes);
}

vector<vector<string> > configToVectors(ifstream &config) {
    vector<vector<string> > vecs;
    string line;
    // split each line by spaces and add to the 2D vector
    while (getline(config, line)) {
        vecs.push_back(split(line, ' '));
    }

    return vecs;
}

vector<Node*> configToNodes(vector<vector<string> > vecs) {
    vector<Node*> nodes;
    // convert each line to a node
    for (size_t i = 0, max = vecs.size(); i < max; i++) {
        nodes.push_back(lineToNode(vecs[i], i));
    }
    return nodes;
}

Node* lineToNode(vector<string> line, int index) {
    NodeId id = idFromLine(line);
    Node* node = new Node(
                    id,
                    index,
                    durationFromLine(line),
                    valueFromLine(line),
                    depsFromLine(line),
                    exprFromLine(line, id));
    return node;
}

NodeId idFromLine(vector<string> line) {
    return line[NODE_ID_OFFSET][0];
}

vector<NodeId> depsFromLine(vector<string> line) {
    vector<NodeId> dependencies;
    int endOfDeps = findEqualSign(line);
    for (int i = DEP_OFFSET; i < endOfDeps; i++) {
        dependencies.push_back(line[i][0]);
    }
    return dependencies;
}

Expression exprFromLine(vector<string> line, NodeId nodeId) {
    Expression expression;
    size_t startOfSymbols = findEqualSign(line) + 1;
    for (size_t i = startOfSymbols, max = line.size(); i < max; i++) {
        expression.push_back(Symbol(line[i], nodeId));
    }
    return expression;
}

size_t findEqualSign(vector<string> line) {
    return find(line.begin(), line.end(), "=") - line.begin();
}

int valueFromLine(vector<string> line) {
    return strToInt(line[1]);
}

int durationFromLine(vector<string> line) {
    return strToInt(line[2]);
}

bool validateConfig(vector<vector<string> > config) {
    // check for an empty config
    if (config.empty()) {
        cerr << "The configuration file is empty.\n";
        return false;
    }
    // validate every line of the config
    for (size_t i = 0, max = config.size(); i < max; i++) {
        if (!validateLine(config[i])) {
            return false;
        }
    }
    return true;
}

bool validateLine(vector<string> line) {
    // validate node, duration and value
    if (!validateNodeId(line[NODE_ID_OFFSET])
            || !validateValue(line[VALUE_OFFSET])
            || !validateDuration(line[DURATION_OFFSET])) {
        return false;
    }
    return validateDeps(line);
}

bool validateDeps(vector<string> line) {
    int endOfDeps = findEqualSign(line);
    for (int i = DEP_OFFSET; i < endOfDeps; i++) {
        if (!validateNodeId(line[i])) {
            return false;
        }
    }
    return true;
}

bool validateNodeId(string node) {
    if (node.length() != 1 || node[0] < CAPITAL_A || node[0] > CAPITAL_Z) {
        cerr << "Node '" << node << "' must be a single capitalized letter.\n";
        return false;
    }
    return true;
}

bool validateDuration(string duration) {
    if (!isInteger(duration) || atoi(duration.c_str()) < 0) {
        cerr << "Duration '" << duration << "' must be a nonnegative integer.\n";
        return false;
    }
    return true;
}

bool validateValue(string value) {
    if (!isInteger(value)) {
        cerr << "Value '" << value << "' must be an integer.\n";
        return false;
    }
    return true;
}

/**
* Split the string by the delimiter.
*
* @param  const string &s
* @param  char delim
*/
vector<string> split(const string &s, char delim) {
    // store the strings in a vector
    vector<string> elems;
    // create a stream of the string to read from
    stringstream ss(s);
    // token string
    string item;
    // read until the delimiter or end of file
    while (getline(ss, item, delim)) {
        // append the character to the vector
        if (!item.empty()) {
            elems.push_back(item);
        }
    }
    // return the vector of characters
    return elems;
}

void printResult(GraphResult result) {
    cout << "Total computation resulted in a value of " << result.value;
    cout << " after " << durationSeconds(result.duration) << ".\n";
}
