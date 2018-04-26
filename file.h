// Name: Wenkang Su & Donald Tang
// wsu1, dtang4
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
	string name; // file name
    int size; // file size
    int freeBytes; // left bytes that are free
    string timestamp;
    Dir* directory;
    fNode* froot; //root of lfile

    // File constructor
    File(string name, int size, string timestamp, Dir* directory){
    	this->name = name;
    	this-> size = size;
    	this->timestamp = timestamp;
    	this->directory = directory;
        this->froot = NULL;
        this->freeBytes = 0;
    }

    // To get the number of fnodes in lfile
    int getNumOfNodes(){
        fNode* curr = froot;
        int count = 0;
        while(curr){
            curr = curr->next;
            count++; 
        }
        return count;
    }

    // To get the last fnode of lfile
    fNode* getLastNode(){
        fNode* curr = froot;
        fNode* tmp = froot;
         while(curr){
            tmp = curr;
            curr = curr->next;
        }
        return tmp;
    }

    // Update and get the freeBytes
    int getLeftBytes(int blockSize){
       freeBytes = this->getNumOfNodes() * blockSize - size;
       return freeBytes;
    }
};

#endif /* FILE_H_ */