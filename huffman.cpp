#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <map>

struct Node
{
    Node()
    {}
    
    Node(Node* L, Node* R)
    {
        left = L;
        right = R;
        weight = L->weight + R->weight;
    }
    
    int weight;
    char ch;
    
    Node *left, *right; 
};

struct CompareNodes
{
    bool operator()(Node* left, Node* right) const
    {
        return left->weight < right->weight;
    }
};

void print(Node* rootNode, unsigned count=0)
{
    
    if(nullptr != rootNode)
    {
        print(rootNode->left, count + 3);
        
        for(unsigned i = 0; i < count; i++)
        {
            std::cout << "  ";
        }
        if(rootNode->ch) std::cout << rootNode->weight << " (" << rootNode->ch << ") " << std::endl;
        else std::cout << rootNode->weight << std::endl;
        print(rootNode->right, count+3);
    }
}

std::vector<bool> code;
std::map<char,std::vector<bool> > table;
void buildTable(Node* root)
{
    if(nullptr != root->left)
    {
        code.push_back(0);
        buildTable(root->left);
    }
    
    if(nullptr != root->right)
    {
        code.push_back(1);
        buildTable(root->right);
    }
    
    if(root->ch) table[root->ch] = code;
    
    code.pop_back();
}

int main(int argc, char** argv)
{
    std::string inputString = "abacabaaaaa";

// function code started here        
    std::map<char, int> tree;

std::list<Node*> nodesTree;
std::map<char, int>::iterator it;
    for(int i =0; i < inputString.size(); i++)
    {
        char c = inputString[i];
        tree[c]++;
    }
    

    for(it = tree.begin(); it != tree.end(); it++ )
    {
        Node *p = new Node;
        p->ch = it->first;
        p->weight = it->second;
        nodesTree.push_back(p);
    }

    while(nodesTree.size() > 1 )
    {
        nodesTree.sort(CompareNodes());
        
        Node* sonLeft = nodesTree.front();
        nodesTree.pop_front();
        Node* sonRight = nodesTree.front();
        nodesTree.pop_front();
        
        Node* parent = new Node(sonLeft, sonRight);
        nodesTree.push_back(parent);
    }   
    
    Node* rootTree = nodesTree.front();
    
    buildTable(rootTree);
    print(rootTree);
    
    Node* rootTreeForDecode = rootTree;
    
    
    return 0;
}
