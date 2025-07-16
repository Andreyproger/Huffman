#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <fstream>
#include <memory>

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

std::vector<bool> encode(const std::string& input, const std::map<char, std::vector<bool>>& table)
{
    std::vector<bool> output;
    for(char c : input)
    {
        const auto& code = table.at(c);
        output.insert(output.end(), code.begin(), code.end());
    }
    return output;
}

std::string decode(const std::vector<bool>& encoded, Node* root)
{
    std::string output;
    Node* current = root;
    
    for(bool bit : encoded)
    {
        current = bit ? current->right : current->left;
        
        if(current->ch != '\0')
        {
            output += current->ch;
            current = root;
        }
    }
    
    return output;
}

void freeTree(Node* root)
{
    if(root != nullptr)
    {
        freeTree(root->left);
        freeTree(root->right);
        delete root;
    }
}

int main(int argc, char** argv)
{
    std::string inputString = "abacabaaaaa";
    
    // Build frequency table
    std::map<char, int> freqTable;
    for(char c : inputString)
        freqTable[c]++;
    
    // Create initial nodes
    std::list<Node*> nodes;
    for(const auto& pair : freqTable)
    {
        Node* p = new Node;
        p->ch = pair.first;
        p->weight = pair.second;
        nodes.push_back(p);
    }
    
    // Build Huffman tree
    while(nodes.size() > 1)
    {
        nodes.sort(CompareNodes());
        
        Node* left = nodes.front();
        nodes.pop_front();
        Node* right = nodes.front();
        nodes.pop_front();
        
        Node* parent = new Node(left, right);
        nodes.push_back(parent);
    }
    
    Node* root = nodes.front();
    
    // Build encoding table
    std::vector<bool> code;
    std::map<char, std::vector<bool>> encodingTable;
    buildTable(root, code, encodingTable);
    
    // Print the tree and codes
    std::cout << "Huffman Tree:\n";
    print(root);
    
    std::cout << "\nHuffman Codes:\n";
    for(const auto& pair : encodingTable)
    {
        std::cout << "'" << pair.first << "': ";
        for(bool bit : pair.second)
            std::cout << bit;
        std::cout << "\n";
    }
    
    // Encode the input string
    std::vector<bool> encoded = encode(inputString, encodingTable);
    std::cout << "\nEncoded: ";
    for(bool bit : encoded)
        std::cout << bit;
    std::cout << "\n";
    
    // Decode back
    std::string decoded = decode(encoded, root);
    std::cout << "\nDecoded: " << decoded << "\n";
    
    // Verify
    if(inputString == decoded)
        std::cout << "\nDecoding successful!\n";
    else
        std::cout << "\nDecoding failed!\n";
    
    // Clean up
    freeTree(root);
    
    return 0;
    
    
    return 0;
}
