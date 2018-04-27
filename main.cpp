// Name: Wenkang Su & Donald Tang
// Login: wsu1, dtang4
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
#include <queue>
#include <stdlib.h>
#include <algorithm>
#include <ctype.h>

using namespace std;

//initialize the root directory
vector<Dir*> tmp;
Dir* root = new Dir(".",NULL,tmp);

//genDirByPath is a function that takes in a vector of directories and constructs a directory object and places it in the tree
void genDirByPath(deque<string> parsed){
	//initialize variables to traverse tree to find appropriate position for new directory
	Dir* parent = root;
	Dir* curr = root;
	bool isChild;
	string endDir;
	//loop until all directories have been looked at
	while (parsed.size() != 0) {
		endDir = parsed.front();
		parsed.pop_front();
		isChild = false;
		//loop through current directory's sub directories
		for(int i=0; i < parent->dBox.size(); i++){
			//if sub directory matches the directory we are trying to place, then we are traversing the right path to place the new directory in
			if (parent->dBox[i]->name == endDir){
				curr = parent->dBox[i];
				isChild = true;
			}
		}
		//if directory is not a child, then we must create a new root and initialize it
		if (!isChild) {
			vector<Dir*> dBox;
			curr = new Dir(endDir, parent, dBox);
			parent->dBox.push_back(curr);
			parent = curr;
		}
		parent = curr;
	}
}

//genFileByPath is a function that takes in a directory, file size, and timestamp, and creates a file object and puts it in the corresponding position in the directory tree
File* genFileByPath(deque<string> parsed, int size, string timeStamp){
	//initialize variables used to traverse the tree
	Dir* parent = root;
	Dir* curr = root;
	bool isChild;
	string endDir;
	//loop through the vector of directories
	while (parsed.size() != 1) {
		endDir = parsed.front();
		parsed.pop_front();
		//loop through the vector of current directory's subdirectories to try to find the approrpriate position for new file
		for(int i=0; i < parent->dBox.size(); i++){
			//keep traversing the tree until we have found the correct location to put the new file in
			if (parent->dBox[i]->name == endDir){
				curr = parent->dBox[i];
			}
		}
		parent = curr;
	}
	//at this point curr should be the correct immediate directory that file belongs to
	//make a new file and push it to the directory's fbox
	File* newFile = new File(parsed[0],size,timeStamp,parent);
	parent->fBox.push_back(newFile);
	return newFile;
}

// Used by cmd "dir", to print out the whole diretory tree in Bread First Order
void printDir(Dir* curr){
	queue<Dir*> Q;
	vector<Dir*> children;
	Q.push(curr);
	while(!Q.empty()){
		Dir* tmp = Q.front();
		cout << tmp->name << endl;
		Q.pop();
		children = tmp->dBox;
		for (int i = 0; i < children.size(); ++i) Q.push(children[i]);
	}
}

// To parse the string directory and return the path
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

//allocateMemory is a function that checks for free space in memory to allocate files to
int allocateMemory(File* file, dNode* droot, int blockSize, int numOfBlocks, vector<int>& freeBytes, int numOfBytes, int obit){
	int useBlocks;
	int allocBlocks = 0;
	int fid;
	int tmp_num = numOfBytes;
	//If obit == 1, that means this function is used by cmd "append" but not initializing file_list
	if(obit == 1){
		int fbytes = file->getLeftBytes(blockSize);
		// If freeBytes can handle the numOfBytes
		if(numOfBytes <= fbytes){
			file->freeBytes -= numOfBytes;
			if(file->froot != NULL){
			int blockID = file->getLastNode()->startAddress/blockSize;
			freeBytes[blockID] = file->freeBytes;
			}
			return 1;
		}
		numOfBytes -= fbytes;
		file->freeBytes = 0;
		if(file->froot != NULL){
		int blockID = file->getLastNode()->startAddress/blockSize;
		freeBytes[blockID] = 0;
		}
	}
	// Calculate the num of blocks to be used
	int modBytes = numOfBytes % blockSize;
	if(modBytes > 0) useBlocks = numOfBytes / blockSize + 1;
	else useBlocks = numOfBytes / blockSize;
	if(enoughMemory(useBlocks,droot,blockSize)){
		dNode* curr = droot;
		
		while(useBlocks > 0){
			// Try to find a free dNode
			while(droot->status == true){
				curr = droot;
				droot = droot->next;
			}
			fid = droot->startID;
			// If ther is only one dNode (FREE);
			if(curr == droot){
				droot->next = new dNode(useBlocks + 1, numOfBlocks,false);
				droot->endID = useBlocks;
				droot->status = true;
				//droot = droot->next;
				allocBlocks = useBlocks;
				useBlocks = 0;
			}
			// If this node is not enough to allocate
			else if(useBlocks >= (droot->endID - droot->startID + 1)){
				// Change the start&end IDs & Merging the nodes
				curr->endID = droot->next->endID;
				curr->next = droot->next->next;
				allocBlocks = droot->endID - droot->startID + 1;
				useBlocks = useBlocks - allocBlocks;
				droot = curr;
			}
			// If this node has enough blocks to allocate
			else{
				// Change the start&end IDs
				curr->endID += useBlocks;
				droot->startID += useBlocks;
				allocBlocks = useBlocks;
				useBlocks = 0;
			}
			// adding LFile
			for(int i = 0; i < allocBlocks - 1; i++){
				if(file->froot == NULL){
					file->froot = new fNode((fid - 1) * blockSize);
					freeBytes[fid - 1] = 0;
				}
				else{
					file->getLastNode()->next = new fNode((fid + i - 1) * blockSize);
					freeBytes[fid + i - 1] = 0;
				}
			}
			if(file->froot == NULL){
					file->froot = new fNode((fid - 1) * blockSize);
					freeBytes[fid - 1] = modBytes;
				}
				else{
					file->getLastNode()->next = new fNode((fid + allocBlocks - 2) * blockSize);
					freeBytes[fid + allocBlocks - 2] = modBytes;
				}
		}
		return 1;
	}
	return 0;
}

// Helper function to handle the LDisk: freed the specific block in LDISK with blockID
void freeDisk(dNode* droot, int blockID){
	dNode* curr = droot;
	dNode* tmp = droot;	
	// Try to find the target node 
	while(curr){
		if(blockID <= curr->endID && blockID >= curr->startID){
			break;
		}
		tmp = curr;
		curr = curr->next;
	}
	if(blockID == 1) {
		// if(droot->next->status) droot->status = false;
		// else {droot->next->startID = 1; droot = droot->next;}
		if(droot->next->status) {dNode* newNode = new dNode(1,1,false); newNode->next = droot; droot = newNode;}
		else droot->next->startID = 1;
		cout << droot->startID << " " << droot->endID << " " << droot->status <<endl;
		cout << droot->next->startID << " " << droot->next->endID << " " << droot->next->status <<endl;
	}
	// If there is only one block in the node
	else if(curr->startID == curr->endID){
		cout << curr->startID << " " << tmp->endID << endl;
		if(curr->next) tmp->next = curr->next->next;
		tmp->endID = curr->next->endID;
	}
	// If the target block is the first block
	else if(blockID == curr->startID){
		tmp->endID++;
		curr->startID++;
	}
	// If the target block is the last block
	else if(blockID == curr->endID){
		if(curr->next) curr->next->startID--;
		else curr->next = new dNode(blockID,blockID,false);
		curr->endID--;
	}
	// If the target block is in the middle
	else{
		dNode* newdNode_1 = new dNode(blockID,blockID,false);
		dNode* newdNode_2 = new dNode(blockID + 1, curr->endID, true);
		dNode* temp = tmp->next; // use dNode* temp to store the tmp->next
		newdNode_1->next = newdNode_2;
		tmp->endID = blockID - 1;
		tmp->next = newdNode_1;
		newdNode_2->next = temp;
	}
}

//Helper function to freed the memory, numOfBytes is the num of bytes that used to free
int freeMemory(File* file, int numOfBytes, int blockSize, vector<int>& freeBytes, dNode* droot){
	int tmp_num = numOfBytes;
	// If numOfBytes does not exceed file size
	if(numOfBytes <= file->size){
		fNode* parent = file->froot;
		fNode* curr = parent;
		// Stop until numOfBytes is done
		while(numOfBytes > 0){
			if(numOfBytes < blockSize - file->freeBytes){
				file->freeBytes += numOfBytes;
				int blockID = file->getLastNode()->startAddress / blockSize;
				freeBytes[blockID] = file->freeBytes;
				numOfBytes = 0;
			}
			else{
				curr = file->froot;
				//Try to find the last node of Lfile and the one before it
				while(curr->next){
					parent = curr;
					curr = curr->next;
				}
				int blockID = curr->startAddress / blockSize;
				freeBytes[blockID] = blockSize; // update vector freeBytes
				parent->next = NULL; // remove last fnode		
				freeDisk(droot, blockID + 1); // free disk
				numOfBytes -= (blockSize - file->freeBytes);
				file->freeBytes = 0; //update file freeBytes
			}
		}
		if(tmp_num == file->size) file->froot = NULL;
		file->size -= tmp_num; // update file size
		//cout << file->size << endl;
		return 1;
	}
	return 0;
}

// remove function that can used by cmd "delete" and "remove"
int remove(Dir* curr, vector<string> cmd, int blockSize, vector<int>& freeBytes, dNode* droot){
	File* tmpFile = NULL;
	// To get the target file
	for(int i = 0; i < curr->fBox.size(); i++)
		if(curr->fBox[i]->name == cmd[1]){  
			tmpFile = curr->fBox[i];
			break;
		}
	if(tmpFile == NULL) {cerr << "File Not found!" << endl; return 1;}
	// free the memory and return the rbit
	int rbit = freeMemory(tmpFile,stoi(cmd[2]),blockSize,freeBytes,droot);
	if(!rbit) {cerr << "Error: Exceed file size!" << endl; return 1;}
	tmpFile->updateTimeStamp();
	return 0;	
}

//prFiles is a function for the command prfiles that prints out the information for all files
void prFiles(Dir* curr) {
	//initialize variables to traverse tree and print information of files in current directory
	queue<Dir*> parentDir;
	vector<Dir*> children;
	string addresses;
	parentDir.push(curr);
	while(!parentDir.empty()){
		Dir* tmp = parentDir.front();
		//loop through curent directory's fbox to access files that we need to print information about
		for (int i = 0; i < tmp->fBox.size(); i++) {
			File* newFile = tmp->fBox[i];
			fNode* lfile = newFile->froot;
			//loop through file's LFile to find the memory addresses that the file is located in
			while (lfile) {
				addresses += to_string(lfile->startAddress) + " ";
				lfile = lfile->next;
			}
			//print out the information of the current file
			cout << "----------------------------------------" << endl;
			cout << "File name: " << newFile->name << endl;
			cout << "File size: " << newFile->size << endl;
			cout << "Time stamp: " << newFile->timestamp << endl;
			cout << "Addresses: " << addresses << endl;
			cout << "----------------------------------------" << endl;
			addresses = "";
		}
		parentDir.pop();
		children = tmp->dBox;
		//push children elements so that we can traverse through them in the next iteration
		for (int i = 0; i < children.size(); ++i) {
			parentDir.push(children[i]);
		}
	}
}
// Almost the same as parse, but parse the string by sapce " ", used for command lines
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
// parse the file information into a vector
vector<string> fileParser(string fileInfo){
	vector<string> result;
	while(fileInfo[0] == ' ') fileInfo = fileInfo.substr(1);
	for(int i = 0; i < 10; i++){
		string doc;
		while(fileInfo[0] != ' ') {doc += fileInfo[0];fileInfo = fileInfo.substr(1);}
		result.push_back(doc);
		while(fileInfo[0] == ' ') fileInfo = fileInfo.substr(1);
	}
	result.push_back(fileInfo);
	return result;
}
int main(int argc, char * const argv[]) {

	//our program will take in 4 parameters: a directory list, a file list, disk size, and block size
	int diskSize;
	int blockSize;
	int numBlocks;
	istringstream iss;
	string dirList;
	string fileList;
	
	//error checking for arguments 
	//if total number of arguments is not 5
	if (argc != 9) {
		cerr << "Error: Please enter the correct amount of arguments" << endl;
		return 1;
	}

	//checking flags
	for(int i = 1; i < 8; i= i + 2){
		string arg = argv[i];	
		if(!arg.compare("-f")) fileList = argv[i+1];
		else if(!arg.compare("-d")) dirList = argv[i+1];
		else if(!arg.compare("-s"))diskSize = stoi(argv[i+1]);
		else if (!arg.compare("-b")) blockSize = stoi(argv[i+1]);
	}

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
	dNode* droot = new dNode(1, numOfBlocks, false);

	//initialize freeBytes
	vector<int> freeBytes;
	for(int i = 0; i < numOfBlocks; i++){
		freeBytes.push_back(blockSize);
	}

	//error check for dir_list.txt
	ifstream in_1(dirList);
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
	ifstream in_2(fileList);
	if (!(in_2)) {
		cerr << "Error: Please input a valid file list file" << endl;
		return 1;
	}
	//initialize variables needed for file
	int size = 0;
	int blockID = 0;
	string timeStamp;
	string input;
	vector<string> fileInfo;
	while(getline(in_2,input)){
		if(input != ""){
		fileInfo = fileParser(input);
		//for each line we read we will 
		//pass in the directory to our parser function
		//generate a new file
		//allocate the file in memory
		//calculate the freebytes of the file after allocating
		size = stoi(fileInfo[6]);
		timeStamp = fileInfo[7] + " " + fileInfo[8] + " " + fileInfo[9];
		deque<string> parsed = parser(fileInfo[10]);
		//create a new file object
		File* newFile = genFileByPath(parsed, size, timeStamp);
		//allocate the file in memory
		if (size > 0) {
			int bit = allocateMemory(newFile,droot,blockSize,numOfBlocks,freeBytes,size,0);
			//check if we have enough memory
			if(!bit) {
				cerr << "Out Of Space" << endl;
				return 1;
			}
		}
		int fbytes = newFile->getLeftBytes(blockSize);	
		//cout << newFile->size << endl;
		fNode* lastNode = newFile->getLastNode();	
		//cout << newFile->getNumOfNodes() << endl;
		if (lastNode != NULL) {
			blockID = lastNode->startAddress/blockSize;
			freeBytes[blockID] = fbytes;
		}
	}
	}
	//implementation of commands, start at root
	Dir* curr = root;
	while(true){
		//initial state of program when we are waiting for user input
		cout << curr->name << "/";
		string inputText;
		getline(cin,inputText);
		if(inputText == "") continue;
		//get user input
		vector<string> cmd = cmdParser(inputText);
		if (cmd[0] == "exit") break;
		//exit will exit the program
		//cd will set the current directory to the directory given by the user if it exists in our directory tree
		if (cmd[0] == "cd") {
			bool foundDir = false;
			//loop through vector of subdirectory to check for directory given by user
			for(int i = 0; i < curr->dBox.size(); i++) {
				if(curr->dBox[i]->name == cmd[1]) {
					curr = curr->dBox[i]; 
					foundDir = true;
					break;
				}
			}
			//if directory is not found we print error
			if (foundDir == false) {
				cerr << "No such directory" << endl;;
			}
		}
		//cd.. will set the current directory to the parent directory
		if (cmd[0] == "cd..") {
			//if parent of current directory is null, then we are already at the root
			//else we set the current directory to the parent directory
			if (curr->parent == NULL) {
				curr = root;
			} else {
				curr = curr->parent;
			}
		}
		//ls will list all the files and subdirectories of the current directory
		if (cmd[0] == "ls"){
			for(int i = 0; i < curr->dBox.size(); i++) cout << curr->dBox[i]->name << " " ;
			for(int i = 0; i < curr->fBox.size(); i++) cout << curr->fBox[i]->name << " " ;
			cout << endl;
		}
		//create will create a new file of name given by user and put it in the directory tree
		if (cmd[0] == "create"){
			if (cmd.size() == 1) {
				cerr << "Must provide a filename" << endl;
			}
			//create new file object of name given by user
			File* newFile = new File(cmd[1],0,"",curr);
			newFile->updateTimeStamp();			
			curr->updateTimeStamp();
			curr->fBox.push_back(newFile);
		}
		//mkdir creates a new directory in the current directory
		if (cmd[0] == "mkdir"){
			Dir* newDir = new Dir(cmd[1],curr,tmp);
			curr->dBox.push_back(newDir);
		}
		//delete will delete a file or directory
		if (cmd[0] == "delete"){
			//check if name is directory and delete if so
			for(int i = 0; i < curr->dBox.size(); i++) 
				if(curr->dBox[i]->name == cmd[1]){
					if(curr->dBox[i]->fBox.size() == 0 || curr->dBox[i]->dBox.size() == 0) curr->dBox.erase(curr->dBox.begin()+i);
					else cerr << "Error: Directory is not empty!" << endl;
				}
			//check if name is file and delete if so and update timestamp of parent directory
			for(int i = 0; i < curr->fBox.size(); i++) 
				if(curr->fBox[i]->name == cmd[1]){
					cmd.push_back(to_string(curr->fBox[i]->size));
					if(remove(curr, cmd, blockSize, freeBytes, droot)) break;
					curr->fBox.erase(curr->fBox.begin()+i);
					curr->updateTimeStamp();
					cout << "Current Directory Updated: " << curr->timestamp << endl;
				}
		}
		
		//append will append a number of bytes to the file
		if (cmd[0] == "append"){
			if (cmd.size() == 1) {
				cerr << "Must provide a filename" << endl;
				continue;
			}
			if (cmd.size() == 2) {
				cerr << "Must provide num of bytes you want to append" << endl;
				continue;
			}
			File* tmpFile = NULL;
			for(int i = 0; i < curr->fBox.size(); i++) 
				if(curr->fBox[i]->name == cmd[1]){ 
					tmpFile = curr->fBox[i];
					break;
				}
			if(tmpFile == NULL) {cerr << "File Not found!" << endl; continue;}
			int bit = allocateMemory(tmpFile,droot,blockSize,numOfBlocks,freeBytes,stoi(cmd[2]),1);
			if (bit == 0) cerr << "Out Of Space" << endl;
			else {tmpFile->updateTimeStamp(); tmpFile->size += stoi(cmd[2]);}
		}
		//remove will delete a number of byte from the file
		if (cmd[0] == "remove"){
			if (cmd.size() == 1) {
				cerr << "Must provide a filename" << endl;
				continue;
			}
			if (cmd.size() == 2) {
				cerr << "Must provide num of bytes you want to remove" << endl;
				continue;
			}
			if(remove(curr, cmd, blockSize, freeBytes, droot) == 1) continue;
		}
		//prfiles will print out file information for every file in the directory tree
		if (cmd[0] == "prfiles"){
			prFiles(root);
		}

		//Initialize disk
		vector<int> disk;
		for(int i = 0; i < diskSize; i++) disk.push_back(0);
		// Print out the disk stastus
		if (cmd[0] == "prdisk"){
			//cout << "Reach prdisk" << endl;
			for(int i = 0; i < numOfBlocks; i++){
				if(freeBytes[i] == blockSize) continue;
				for(int j = 0; j < blockSize - freeBytes[i]; j++) disk[i*blockSize + j] = 1;
			}
			int i = 0;
			int tbit = disk[0];
			int useCounter = 0;
			int freeCounter = 0;
			int fragmentation = 0;
			//for(int i = 0; i < freeBytes.size(); i++) fragmentation += freeBytes[i];
			// Stop unitil i >= diskSize
			while(i < diskSize){
				// looping for consecutive 1s
				while(tbit == 1 && i < diskSize){
					//If we meet 0, we change to other loop by changing tbit
					if(disk[i] == 0){
						tbit = 0;
						cout << "In use: " << i - useCounter << "-" << i - 1 << endl;
						useCounter = 0;
					}
					else if(i == diskSize - 1) {cout << "In use: " << i - useCounter << "-" << i << endl; i++;}
					else {
				 		useCounter++;
						i++;
					}
				}
				// looping for consecutive 0s
				while(tbit == 0 && i < diskSize){
					//If we meet 1, we change to other loop by changing tbit
					if(disk[i] == 1){
						tbit = 1;
						cout << "Free: " << i - freeCounter << "-" << i - 1 << endl;
						fragmentation += freeCounter;
						freeCounter = 0;
					}
					else if(i == diskSize - 1) {cout << "Free: " << i - freeCounter << "-" << i << endl;fragmentation += freeCounter;i++;}
					else {
						freeCounter++;
						i++;
					}
				}
			}
			cout << "fragmentation: " << fragmentation+1 << " bytes" << endl;
		}
		//dir will print out the directory tree in breadth first order
		if (cmd[0] == "dir"){
			printDir(root);
		}
		// Test for debugging
		// Print the file
		if (cmd[0] == "show"){
			File* tmpFile;
			for(int i = 0; i < curr->fBox.size(); i++) 
				if(curr->fBox[i]->name == cmd[1]){ 
					tmpFile = curr->fBox[i];
					break;
				}
			cout << "----------------------------------------" << endl;
			cout << "File Name: " << tmpFile->name << endl;
			cout << "Size: " << tmpFile->size << endl;
			cout << "Num of fnodes: " << tmpFile->getNumOfNodes() << endl;
			cout << "Left Bytes: " << tmpFile->getLeftBytes(blockSize) << endl;
			if(tmpFile->froot != NULL){
			cout << "First Address: " << tmpFile->froot->startAddress << endl;
			cout << "Last Address: " << tmpFile->getLastNode()->startAddress << endl;
			}
			cout << "----------------------------------------" << endl;
		}

		if (cmd[0] == "display"){
			cout << "----------------------------------------" << endl;
			dNode* tmpCurr = droot;
			while(tmpCurr){
				if (tmpCurr->status) cout << "In Use: " << tmpCurr->startID << " " << tmpCurr->endID << endl;
				else cout << "Free: " << tmpCurr->startID << " " << tmpCurr->endID << endl;
				tmpCurr = tmpCurr->next;
			}
			cout << "----------------------------------------" << endl;
		}
	}
}


