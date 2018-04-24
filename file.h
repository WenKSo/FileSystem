// Name: Wenkang Su & Donald Tang
// I pledge my honor that I have abided by the Stevens Honor System.
#ifndef FILE_H
#define FILE_H

#include "dir.h"
#include "fNode.h"
#include <string>

using namespace std;

class Dir;

class File{

public:
	string name;
    int size;
    string timestamp;
    Dir* directory;
    fNode* froot;

    int getNumOfNodes(){
        int count = 0;
        while(froot){
            froot = froot->next;
            count++; 
        }
        return count;
    }

    fNode* getLastNode(){
        fNode* curr = froot;
        fNode* tmp = froot;
         while(curr){
            tmp = curr;
            curr = curr->next;
        }
        return tmp;
    }

     File(string name, int size, string timestamp, Dir* directory){
    	this->name = name;
    	this-> size = size;
    	this->timestamp = timestamp;
    	this->directory = directory;
        this->froot = NULL;
    }

    int getLeftBytes(int blockSize){
       return (size - this->getNumOfNodes() * blockSize);
    }
};

#endif /* FILE_H_ */