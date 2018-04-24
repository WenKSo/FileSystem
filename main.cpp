// Name: Wenkang Su & Donald Tang
// I pledge my honor that I have abided by the Stevens Honor System.
#include "dir.h"
#include "file.h"
#include "dNode.h"
#include "fNode.h"
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <deque>
#include <typeinfo>

using namespace std;

//vector of the file information from file_list.txt
vector<Dir*> tmp;
Dir* root = new Dir(".",NULL,tmp);

void genDirByPath(deque<string> parsed){
	Dir* parent = root;
	Dir* curr = root;
	bool isChild;
	string endDir;
	//cout << "Reach dGen" << endl;
	while (parsed.size() != 0) {
		endDir = parsed.front();
		parsed.pop_front();
		isChild = false;
		for(int i=0; i < parent->dBox.size(); i++){
			if (parent->dBox[i]->name == endDir){
				curr = parent->dBox[i];
				isChild = true;
			}
		}
		if (!isChild) {
			vector<Dir*> dBox;
			curr = new Dir(endDir, parent, dBox);
			parent->dBox.push_back(curr);
			parent = curr;
		}
		parent = curr;
	}
}

File* genFileByPath(deque<string> parsed, int size, string timeStamp){
	Dir* parent = root;
	Dir* curr = root;
	bool isChild;
	string endDir;
	//cout << "Reach fGen" << endl;
	while (parsed.size() != 1) {
		endDir = parsed.front();
		parsed.pop_front();
		for(int i=0; i < parent->dBox.size(); i++){
			if (parent->dBox[i]->name == endDir){
				curr = parent->dBox[i];
			}
		}
		parent = curr;
	}
	File* newFile = new File(parsed[0],size,timeStamp,parent);
	parent->fBox.push_back(newFile);
	return newFile;
}

deque<string> parser(string dir){
	deque<string> path;
	int i = 0;
	char c;
	string subDir;
	while (i < dir.length()) {
		c = dir[i];
		if (c == '/') {
			path.push_back(subDir);
			subDir = "";
		}
		else if (i == dir.length()-1) {
			subDir += c;
			if (subDir.length() > 0) {
				path.push_back(subDir);
			}
		}
		else subDir += c;
		i++;
	}
	return path;
}

// Pre-Check if there are enough memory to allocate 
bool enoughMemory(int useBlocks, dNode* droot, int blockSize){
	int count = 0;
	while(droot){
		if(droot->status == false){
		count = count + droot->endID - droot->startID + 1; 
		}
		droot = droot->next;
	}
	return (count > useBlocks);
}

int allocateMemory(File* file, dNode* droot, int blockSize, int numOfBlocks){
	int useBlocks;
	// Calculate the num of blocks to be used
	if(file->size % blockSize > 0) {useBlocks = file->size / blockSize + 1;}
	else useBlocks = file->size / blockSize;
	if(enoughMemory(useBlocks,droot,blockSize)){
		dNode* curr = droot;
		// Try to find a free dNode
		while(useBlocks > 0){
			while(droot->status == true){
				curr = droot;
				droot = droot->next;
			}
			// If ther is only one dNode (FREE);
			if(curr == droot){
				droot->next = new dNode(useBlocks + 1, numOfBlocks,false);
				droot->endID = useBlocks;
				droot->status = true;
				droot = droot->next;
				useBlocks = 0;
			}
			// If this node is not enough to allocate
			else if(useBlocks > (droot->endID - droot->startID + 1)){
				// Change the start&end IDs & Merging the nodes
				curr->endID = droot->next->endID;
				curr->next = droot->next->next;
				useBlocks = useBlocks - (droot->endID - droot->startID + 1);
				droot = curr;
			}
			// If this node has enough blocks to allocate
			else{
				// Change the start&end IDs
				curr->endID = curr->endID + useBlocks;
				droot->startID = droot->startID + useBlocks;
				useBlocks = 0;
			}
			// adding LFile
			if(file->froot == NULL){
				file->froot = new fNode((droot->startID - 1) * blockSize);
			}else file->getLastNode()->next = new fNode(droot->startID * blockSize);
		}
		return 1;
	}
	return 0;
}

vector<string> cmdParser(string cmd){
	vector<string> result;
	int i = 0;
	string subInput;
	while(i < cmd.length()){
		char c = cmd[i];
		if (c == ' ') {
			result.push_back(subInput);
			subInput = "";
		}
		else if (i == cmd.length()-1) {
			subInput += c;
			if (subInput.length() > 0) {
				result.push_back(subInput);
			}
		}
		else subInput += c;
		i++;
	}
	return result;
}
int main(int argc, char * const argv[]) {

	int diskSize;
	int blockSize;
	int numBlocks;
	istringstream iss;
	
	if (argc != 5) {
		cerr << "Error: Please enter the correct amount of arguments" << endl;
		return 1;
	}
	//initialize LDisk
	diskSize = atoi(argv[3]);
	blockSize = atoi(argv[4]);
	//check validity of size of disk argument
	if (diskSize <= 0) {
		cerr << "Error: Please enter a positive integer for size of disk" << endl;
		return 1;
	}
	//check validity of size of block argument
	if (blockSize <= 0) {
		cerr << "Error: Please enter a positive integer for size of block" << endl;
		return 1;
	}
	//number of blocks in memory
	int numOfBlocks = diskSize/blockSize;

	//initial node in LDisk is one node of all blocks that are free
	dNode* newNode = new dNode(1, numOfBlocks, false);

	//ERROR CHECKING FOR PARAMETERS
	//error check for dir_list.txt
	ifstream in_1(argv[1]);
	if (!(in_1)) {
		cerr << "Error: Please input a valid directory file" << endl;
		return 1;
	}
	//push each directory to the global vector of directories
	string dir;
	while(in_1 >> dir){
		deque<string> parsed = parser(dir);
		parsed.pop_front();
		//cout << "After Pop" << endl;
		if(parsed.size() == 0) continue;
		genDirByPath(parsed);
	}
	//error check for file_list.txt
	ifstream in_2(argv[2]);
	if (!(in_2)) {
		cerr << "Error: Please input a valid file list file" << endl;
		return 1;
	}
	//put each line in filelist into vector of fileinfo
	int size;
	string timeStamp;
	string file1, file2, file3, file4, file5, file6, file7, file8, file9, file10, file11;
	while(in_2 >> file1 >> file2 >> file3 >> file4 >> file5 >> file6 >> file7 >> file8 >> file9 >> file10 >> file11) {
		deque<string> parsed = parser(file11);
		size = stoi(file7);
		timeStamp = file8 + " " + file9 + " " + file10;
		File* newFile = genFileByPath(parsed, size, timeStamp);
		int bit = allocateMemory(newFile,newNode,blockSize,numOfBlocks);
		if(!bit) cerr << "Out Of Space" << endl;
	}

	while(true){
		Dir* curr = new Dir("home",NULL,tmp);
		cout << curr->name << "/";
		string inputText;
		getline(cin,inputText);
		vector<string> cmd = cmdParser(inputText);
		if (cmd[0] == "exit") break;
		if (cmd[0] == "-s") cout << diskSize << endl;
		if (cmd[0] == "-b") cout << blockSize << endl;
		if (cmd[0] == "cd") {
			for(int i = 0; i < curr->dBox.size(); i++)
				if(curr->dBox[i]->name == cmd[1]) {curr = curr->dBox[i]; break;}
		}
		if (cmd[0] == "cd..") try {curr = curr->parent;} catch(exception) {cerr << "Already in the top directory." << endl;}
		if (cmd[0] == "ls"){
			for(int i = 0; i < curr->dBox.size(); i++) cout << curr->dBox[i]->name << " " << endl;
			for(int i = 0; i < curr->fBox.size(); i++) cout << curr->fBox[i]->name << " " << endl;
		}
		if (cmd[0] == "create"){
			File* newFile = new File(cmd[1],0,"current time",curr);
			curr->fBox.push_back(newFile);
		}
		if (cmd[0] == "mkdir"){
			Dir* newDir = new Dir(cmd[1],curr,tmp);
			curr->dBox.push_back(newDir);
		}
		if (cmd[0] == "delete"){
			for(int i = 0; i < curr->dBox.size(); i++) 
				if(curr->dBox[i]->name == cmd[1]) curr->dBox.erase(curr->dBox.begin()+i);
			for(int i = 0; i < curr->fBox.size(); i++) 
				if(curr->fBox[i]->name == cmd[1]) curr->fBox.erase(curr->fBox.begin()+i);
		}
		if (cmd[0] == "prdisk"){
			dNode* temp = newNode;
			while(temp){
				if(temp->status == false) cout << "Free: " << (temp->startID - 1) * blockSize << "-" << endl;
			}
		}
		if (cmd[0] == "append"){
			cout << "TO DO" << endl;
		}
		if (cmd[0] == "remove"){
			cout << "TO DO" << endl;
		}
		if (cmd[0] == "prfiles"){
			cout << "TO DO" << endl;
		}
	}
}


