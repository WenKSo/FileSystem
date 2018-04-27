// Name: Wenkang Su & Donald Tang
// Login: wsu1, dtang4
// I pledge my honor that I have abided by the Stevens Honor System.
#ifndef DIR_H_
#define DIR_H_

#include "file.h"
#include <string>
#include <vector>

using namespace std;

//Dir class represents a directory 
class Dir {

public:
    string name;    //attribute name is the name of the directory
    Dir* parent;    //atribute parent is the parent of the current directory
    string timestamp;   //attribute timestamp is the timestamp of when the directory was last modified
    vector<Dir*> dBox;  //attribute dBox is a vector of directories of current directory's children
    vector<File*> fBox; //atribute fBox is a vector of files of the current directory's children

    //Dir constructor takes a name, parent, and a vector of directory children
    Dir(string name, Dir* parent, vector<Dir*> dBox) {
    	this->name = name;
    	this->parent = parent;
    	this->dBox = dBox;
    }

    void updateTimeStamp(){
            char buffer[80];
			time_t rawtime;
  			struct tm * timeinfo;
			time (&rawtime);
  			timeinfo = localtime (&rawtime);
    		struct tm *tmp ;
			strftime(buffer, sizeof(buffer), "%b %d %R", timeinfo);
			timestamp = string(buffer);
    }
};

#endif /* DIR_H_ */
