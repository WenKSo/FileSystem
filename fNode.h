// Name: Wenkang Su & Donald Tang
// I pledge my honor that I have abided by the Stevens Honor System.
#ifndef FNODE_H_
#define FNODE_H_

#include "dir.h"
#include "file.h"
#include <string>

using namespace std;

class fNode {

public:
    int startAddress;
    fNode *next;

    fNode(int startAddress) {
    	this->startAddress = startAddress;
    }
};

#endif /* FNODE_H_ */