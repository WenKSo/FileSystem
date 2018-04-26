// Name: Wenkang Su & Donald Tang
// wsu1, dtang4
// I pledge my honor that I have abided by the Stevens Honor System.
#ifndef DNODE_H_
#define DNODE_H_

#include "dir.h"
#include "file.h"
#include <string>
#include <vector>

using namespace std;

//dNode class represents each node of LDIsk
class dNode {

public:
    int startID;    //attribute startID indicates the id of the beginning block in dNode
    int endID;      //attribute endID indicates the id of the end block in dNode
    bool status;    //attribute status indicates whether or not the node is free or used
    dNode* next;    //attribute next is a pointer that points to the next node in LDisk

    //dNode constructor takes in a start id, end id, and the status of the node
    dNode(int start, int end, bool status) {
    	this->startID = start;
        this->endID = end;
        this->status = status;
        this->next = NULL;
    }
};

#endif /* DNODE_H_ */