// Name: Wenkang Su & Donald Tang
// I pledge my honor that I have abided by the Stevens Honor System.
#ifndef DIR_H_
#define DIR_H_

#include "file.h"
#include <string>
#include <vector>

using namespace std;

class Dir {

public:
    string name;
    Dir* parent;
    //vector of directories of current directory's children
    vector<Dir*> dBox;
    vector<File*> fBox;

    Dir(string name, Dir* parent, vector<Dir*> dBox) {
    	this->name = name;
    	this->parent = parent;
    	this->dBox = dBox;
    }
};

#endif /* DIR_H_ */
