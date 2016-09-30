// Dylan Richardson
#include "node.hpp"
#include <iostream>
#include <stack>

using namespace std;

extern int TOTAL;

const string OP_ID = "I";
const string OP_TOTAL = "V";

Symbol::Symbol(string raw, NodeId id) {
    this->raw = raw;
    this->id = id - CAPITAL_A;
    this->operand = isInteger(raw) || raw == OP_ID || raw == OP_TOTAL;
}

int Symbol::getValue() {
    if (raw == OP_ID) {
        return id;
    } else if (raw == OP_TOTAL) {
        return TOTAL;
    } else {
        return strToInt(raw);
    }
}

Node::Node(NodeId id, int index, int duration, int value,
            vector<NodeId> dependencies, Expression expression) {
    this->id = id;
    this->index = index;
    this->duration = duration;
    this->totalDuration = -1;
    this->value = value;
    this->dependencies = dependencies;
    this->depCount = dependencies.size();
    this->expression = expression;
}

Node::~Node() {}

const void Node::print() {
    cout << "Node\n";
    cout << "\tid: " << id << "\n";
    cout << "\tvalue: " << value << "\n";
    cout << "\tduration: " << duration << "\n";
    cout << "\tdep count: " << depCount << "\n";
}

const NodeId Node::getId() {
    return id;
}

const int Node::getDuration() {
    return duration;
}

const int Node::getTotalDuration() {
    return totalDuration;
}

void Node::setTotalDuration(int duration) {
    totalDuration = duration;
}

const bool Node::hasTotalDuration() {
    return totalDuration != -1;
}

const int Node::getValue() {
    if (expression.size() == 0) {
        return value;
    } else {
        return evalExpr(expression);
    }

}

int Node::evalExpr(Expression expression) {
    stack<int> stack;
    int arg1, arg2;
    for (size_t i = 0, max = expression.size(); i < max; i++) {
        Symbol symbol = expression[i];
        if (symbol.operand) {
            stack.push(symbol.getValue());
        } else {
            arg2 = stack.top();
            stack.pop();
            arg1 = stack.top();
            stack.pop();
            int result = calculate(symbol, arg1, arg2);
            stack.push(result);
        }
    }
    return stack.top();
}

int calculate(Symbol symbol, int arg1, int arg2) {
    switch (symbol.raw[0]) {
        case '+':
            return arg1 + arg2;
        case '-':
            return arg1 - arg2;
        case '*':
            return arg1 * arg2;
        case '/':
            return arg1 / arg2;
        case '%':
            return arg1 % arg2;
        default:
            return 0;
    }
}

const vector<NodeId> Node::getDependencies() {
    return dependencies;
}

const Expression Node::getExpression() {
    return expression;
}

const vector<Node*> Node::getNextNodes() {
    return nextNodes;
}

void Node::addNextNode(Node* node) {
    nextNodes.push_back(node);
}

const int Node::getDepCount() {
    return depCount;
}

int strToInt(string str) {
    return atoi(str.c_str());
}

bool isInteger(const string &str) {
    if(str.empty() || ((!isdigit(str[0]))
        && (str[0] != '-') && (str[0] != '+'))) {
        return false ;
    }

    char* c;
    strtol(str.c_str(), &c, 10);
    return *c == 0;
}
