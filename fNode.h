// Name: Wenkang Su & Donald Tang
// wsu1, dtang4
// I pledge my honor that I have abided by the Stevens Honor System.
#ifndef FNODE_H_
#define FNODE_H_

#include "dir.h"
#include "file.h"
#include <string>

using namespace std;

//fNode class is used to represent a node in LFile
class fNode {

public:
    int startAddress;   //indicates the first address of the block that the file is located in
    fNode *next;    //pointer to the next node in LFile

    fNode(int startAddress) {
    	this->startAddress = startAddress;
        this->next = NULL;
    }
};

#endif /* FNODE_H_ */