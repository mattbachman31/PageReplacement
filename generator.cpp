/*MUST BE COMPILED WITH -std=c++11
 Used to generate random input for the program*/
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <list>
#include <math.h>
#include <algorithm>
#include <iterator>

using namespace std;

typedef struct{
	int processID;
	int vmSize;
	int numReferences;
}process;

int main(int argc, char** argv){
	srand(time(NULL));
	if(argc != 6){
		cout << "Usage:\n./a.out <num-processes> <max-address-size-space> <max-num-references-per-process> <output-file-name> <locality-boolean>" << endl;
		exit(1);
	}
	int numProcesses = atoi(argv[1]);
	int maxAddressSpaceSize = atoi(argv[2]);
	int maxNumReferencesPerProcess = atoi(argv[3]);
	int locality = atoi(argv[5]);
	int numProcessesSinglePage = locality/2;
	ofstream outFile(argv[4]);
	list<process*> inProgress;
	list<process*> ended;
	process allProcesses[numProcesses];
	for(int i=0; i<numProcesses; i++){
		allProcesses[i].processID = i+1;
		if(numProcessesSinglePage){
			allProcesses[i].vmSize = 1;
			numProcessesSinglePage--;
		}
		else{allProcesses[i].vmSize = (rand() % maxAddressSpaceSize)+1;}
		allProcesses[i].numReferences = 0;
	}
	int processSelected = rand() % numProcesses;
	//int newProcessVMSize = rand() % maxAddressSpaceSize;
	//start initial process
	//inProgress.push_back(&allProcesses[numProcesses]);
	//cout << "START " << processSelected+1 << " " << newProcessVMSize << endl;
	while(!((inProgress.size() == 0) && (ended.size() == numProcesses))){
		processSelected = rand() % numProcesses;
		process* selectedPointer = &allProcesses[processSelected];
		list<process*>::iterator it = std::find(std::begin(inProgress), std::end(inProgress), selectedPointer);
		if(it != inProgress.end()){
			if(allProcesses[processSelected].numReferences < maxNumReferencesPerProcess){
				outFile << "REFERENCE " << processSelected+1 << " " << (rand() % allProcesses[processSelected].vmSize)+1 << endl;
				allProcesses[processSelected].numReferences++;
			}
			else{
				outFile << "TERMINATE " << processSelected+1 << endl;
				inProgress.erase(it);
				ended.push_back(&allProcesses[processSelected]);
			}
		}
		else{
			it = find(std::begin(ended), std::end(ended), selectedPointer);
			if(it == ended.end()){
				//newProcessVMSize = (rand() % maxAddressSpaceSize) + 1;
				outFile << "START " << processSelected+1 << " " << allProcesses[processSelected].vmSize << endl;
				inProgress.push_back(&allProcesses[processSelected]);
			}
		}
	}
	outFile.close();
	return 0;
}
