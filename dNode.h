// Name: Wenkang Su & Donald Tang
// I pledge my honor that I have abided by the Stevens Honor System.
#ifndef DNODE_H_
#define DNODE_H_

#include "dir.h"
#include "file.h"
#include <string>
#include <vector>

using namespace std;

class dNode {

public:
    int startID;
    int endID;
    bool status;
    dNode* next;

    dNode(int start, int end, bool status) {
    	this->startID = start;
        this->endID = end;
        this->status = false;
        this->next = NULL;
    }
};

#endif /* DNODE_H_ */