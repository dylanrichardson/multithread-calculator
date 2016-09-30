#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>

const int CAPITAL_A = 65;
const int CAPITAL_Z = 90;

typedef char NodeId;

struct Symbol {
    std::string raw;
    bool operand;
    int id;
    Symbol(std::string, NodeId);
    int getValue();
};

typedef std::vector<Symbol> Expression;

class Node {
    public:
        Node(NodeId, int, int, int, std::vector<NodeId>, Expression);
        ~Node();
        const NodeId getId();
        const int getDuration();
        const int getTotalDuration();
        void setTotalDuration(int);
        const bool hasTotalDuration();
        const int getValue();
        const std::vector<NodeId> getDependencies();
        const Expression getExpression();
        const std::vector<Node*> getNextNodes();
        void addNextNode(Node*);
        const int getDepCount();
        int evalExpr(Expression);
        const void print();
        NodeId id; // should be private
    private:
        int index;
        int duration;
        int totalDuration;
        int value;
        Expression expression;
        std::vector<NodeId> dependencies; // this node depends on these nodes
        std::vector<Node*> nextNodes; // theses nodes depend on this node
        int depCount;
};

int calculate(Symbol, int, int);
int strToInt(std::string);
bool isInteger(const std::string &str);

#endif
