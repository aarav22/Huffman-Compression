#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <unordered_map>
#include <queue>
#include "pqueue.h"
#include<algorithm>
#include "console.h"
#include "filelib.h"
#include "grid.h"
#include "gwindow.h"
#include "simpio.h"
#include "bitstream.h"
using namespace std;

struct HuffmanNode{
    char ch;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;
};

void initializationFromFile(string file_name, PriorityQueue<HuffmanNode*> &nodeQueue)
{
    unordered_map<char, int> charToFreq;
    ifstream fin;
    string readline;
    openFile(fin, file_name);

   while(!fin.eof())
    {
        getline(fin, readline);
        unsigned int t = readline.size();
        for(unsigned int j = 0; j < t; j++)
        {
            charToFreq[readline[j]]++;
        }
    }
   charToFreq[PSEUDO_EOF] = 1;
    fin.close();
    unordered_map<char, int> :: iterator iter;
    
    for(iter = charToFreq.begin(); iter != charToFreq.end(); ++ iter) {
       HuffmanNode* node = new HuffmanNode;
       node -> ch = iter -> first;
       node -> freq = iter -> second;
       node -> left = nullptr;
       node -> right = nullptr;
       nodeQueue.enqueue(node, node -> freq);
    }
}

HuffmanNode* buildTree(PriorityQueue<HuffmanNode*>& nodeQueue) {
  while(nodeQueue.size() >= 2) {
      HuffmanNode* firstNode = nodeQueue.dequeue();
      HuffmanNode* secNode = nodeQueue.dequeue();
      //cout << "data : " << firstNode -> ch << ' ' << firstNode -> freq << ' ' << secNode -> ch << secNode -> freq<<endl;
      HuffmanNode* newSubTree = new HuffmanNode;
      newSubTree -> freq = firstNode -> freq + secNode -> freq;
      newSubTree -> ch = '$';  /** todo : decide appropriate character**/
      newSubTree -> left = firstNode;
      newSubTree -> right = secNode;
      nodeQueue.enqueue(newSubTree, newSubTree -> freq);
  }
  
  HuffmanNode* root = nodeQueue.dequeue();

  return root;
}

bool isLeaf(HuffmanNode * root) {
    return root -> ch != '$';
}
void traverseTreeAndEncode(HuffmanNode * root, unordered_map<char, string> & charMap, string HuffCode = "") {
    if(!root){
        return;
    }
    if(isLeaf(root)) {
        //cout << root -> ch << ": " << HuffCode << "\n";
        charMap[root -> ch] = HuffCode;
    }
    traverseTreeAndEncode(root -> left, charMap, HuffCode + '0');
    traverseTreeAndEncode(root -> right, charMap, HuffCode + '1');
}

void fileEncode(string file_name, unordered_map<char, string> charMap) {
    fstream fin(file_name, ios :: in);
    ofbitstream fout("temp.dat");
    char ch;
    while (!fin.eof()) {
         fin >> ch;
         string alphabetCode = charMap[ch];
         for(int i = 0; i < alphabetCode.length(); i++) {
             if(alphabetCode[i] == '0') {
                 fout.writeBit(0);
             } else {
                 fout.writeBit(1);
             }
         }
         }

    fin.close();
    fout.close();
}
void fileDecode(unordered_map<string, char> stringMap) {
    ifbitstream fin("temp.dat");
    ofstream fout("decompressed.txt");
    string alphabetDecoded = "";
    while(!fin.eof()) {
        int firstBit = fin.readBit();
        alphabetDecoded += integerToString(firstBit);
        cout << firstBit << ' ';
        if(stringMap.find(alphabetDecoded) != stringMap.end()) {
            fout << stringMap[alphabetDecoded];
            //fout << " done this!";
//            fout << "t";
         //   fout << 'a';
            alphabetDecoded = "";
        }
    }
}

int main(){

    string file_name;
    file_name = getLine("Enter File Name");
    while (!fileExists(file_name)) {
        cout << "Please Enter Again: "<< endl;
        file_name = getLine("Enter File Name");
    }
    PriorityQueue<HuffmanNode*> nodeQueue;
    initializationFromFile(file_name, nodeQueue); // Maintains frequency of a character and creates a priority queue.
    HuffmanNode * root = buildTree(nodeQueue);
    unordered_map<char, string> charMap; 
    traverseTreeAndEncode(root, charMap, "");
    unordered_map<char, string> :: iterator itr = charMap.begin();
    unordered_map<string, char> stringMap;
    for(;itr != charMap.end(); itr++) {
        stringMap[itr -> second] = itr -> first;
    }
     unordered_map<string, char> :: iterator itr2 = stringMap.begin();
     for(;itr2 != stringMap.end(); itr2++) {
         cout << itr2 -> second << "   :" << itr2 -> first << endl;
     }
    fileEncode(file_name, charMap);
    fileDecode(stringMap);
}
