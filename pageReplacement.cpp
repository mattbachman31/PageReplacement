#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <queue>
#include <math.h>

using namespace std;

typedef struct{
	int idNum;
	int* pageTable;
	int pageTableSize;
}process;


typedef struct{
	int counter;
	int processID;
	int pageIndex;
}frame;

int main(int argc, char** argv){
	
	int frames = atoi(argv[1]);
	vector<process> activeProcesses;
	vector<frame> physicalMemoryRandom;
	vector<frame> physicalMemoryFIFO;
	vector<frame> physicalMemoryLRU;
	int numTimes = 1;
	cout << "Page Fault Rates:" << endl;
	while(numTimes <= 3){
		//cout << "Run number " << numTimes << endl;	
		int currentSlotsFull=0;
		int totalRef=0, faults=0;
		srand(time(NULL));
		string currentLine, word;
		ifstream inFile(argv[2]);
		while(getline(inFile, currentLine)){
			stringstream thisLine;
			thisLine << currentLine;
			thisLine >> word;
			if(!word.compare("START")){
				thisLine >> word;
				process newProcess;
				newProcess.idNum = atoi(word.c_str());
				thisLine >> word;
				newProcess.pageTable = new int[atoi(word.c_str())+1];
				newProcess.pageTableSize = atoi(word.c_str())+1;
				for(int i=1; i<atoi(word.c_str())+1; i++){
					newProcess.pageTable[i] = -1;
				}
				activeProcesses.push_back(newProcess);
				//cout << "Created Process " << newProcess.idNum << endl;
			}
			else if(!word.compare("TERMINATE")){
				thisLine >> word;
				for(int i=0; i<activeProcesses.size(); i++){
					if(activeProcesses[i].idNum == atoi(word.c_str())){
						//cout << "Terminated process " << activeProcesses[i].idNum << endl;
						for(int j=1; j<activeProcesses[i].pageTableSize; j++){
							if(activeProcesses[i].pageTable[j] != -1){
								currentSlotsFull--;
								if(numTimes == 1){
									physicalMemoryRandom[activeProcesses[i].pageTable[j]].processID = -1;
								}
								else if(numTimes == 2){
									physicalMemoryLRU[activeProcesses[i].pageTable[j]].processID = -1;
								}
								else if(numTimes == 3){

								}
							}
						}
						delete [] activeProcesses[i].pageTable;
						activeProcesses.erase(activeProcesses.begin()+i);
						//cout << "Terminated" << endl;
						break;
					}
				}
			}
			else if(!word.compare("REFERENCE")){
				thisLine >> word;
				int referenceID = atoi(word.c_str());
				thisLine >> word;
				int virtualPageNumber = atoi(word.c_str());
				totalRef++;
				for(int i=0; i<activeProcesses.size(); i++){
					if(activeProcesses[i].idNum == referenceID){
						if((virtualPageNumber < 1) || (virtualPageNumber > activeProcesses[i].pageTableSize)){cout << "Invalid Virtual Page Number" << endl;}
						if(activeProcesses[i].pageTable[virtualPageNumber] == -1){
							faults++;
							if(numTimes == 1){
								if(currentSlotsFull == frames) {
									//cout << "Page Fault" << endl;
									int randomNum = rand() % frames;
									frame newFrame;
									newFrame.processID = activeProcesses[i].idNum;
									newFrame.pageIndex = virtualPageNumber;
									activeProcesses[i].pageTable[virtualPageNumber] = randomNum;
									frame oldFrame = physicalMemoryRandom[randomNum];
									for(int k = 0; k<activeProcesses.size(); k++){
										if(activeProcesses[k].idNum == oldFrame.processID){
											activeProcesses[k].pageTable[oldFrame.pageIndex] = -1;
											//cout << "Evicting Process " << activeProcesses[k].idNum << " Page " << physicalMemoryRandom[randomNum].pageIndex << endl;
											break;
										}
										
									}
									physicalMemoryRandom.erase(physicalMemoryRandom.begin() + randomNum);
									physicalMemoryRandom.insert(physicalMemoryRandom.begin() + randomNum, newFrame);
								}
								else{
									//activeProcesses[i].pageTable[virtualPageNumber] = currentSlotsFull; //our array is still 0 based (phys mem)
									int q=0;
									for(q=0; q<physicalMemoryRandom.size(); q++){
										if(physicalMemoryRandom[q].processID == -1){
											activeProcesses[i].pageTable[virtualPageNumber] = q;
											break;
										}
										if(q+1 == physicalMemoryRandom.size()){
											activeProcesses[i].pageTable[virtualPageNumber] = q+1;
										}
									}
									currentSlotsFull++;
									frame newFrame;
									newFrame.processID = activeProcesses[i].idNum;
									newFrame.pageIndex = virtualPageNumber;
									physicalMemoryRandom.insert(physicalMemoryRandom.begin()+q, newFrame);
									//cout << "Page Fault - Loading into memory slot " << q << endl;	
								}
							}
							else if(numTimes == 2){
								if(currentSlotsFull == frames) {
									//cout << "Page Fault" << endl;
									int highestCounter=0;
									int randomNum;
									for(int q=0; q<physicalMemoryLRU.size(); q++){
										if(physicalMemoryLRU[q].processID != -1){
											if(physicalMemoryLRU[q].counter > highestCounter){
												highestCounter = physicalMemoryLRU[q].counter;
												randomNum = q;
											}
										}
									}
									frame newFrame;
									newFrame.processID = activeProcesses[i].idNum;
									newFrame.pageIndex = virtualPageNumber;
									newFrame.counter = 0;
									activeProcesses[i].pageTable[virtualPageNumber] = randomNum;
									frame oldFrame = physicalMemoryLRU[randomNum];
									for(int k = 0; k<activeProcesses.size(); k++){
										if(activeProcesses[k].idNum == oldFrame.processID){
											activeProcesses[k].pageTable[oldFrame.pageIndex] = -1;
											//cout << "Evicting Process " << activeProcesses[k].idNum << " Page " << physicalMemoryLRU[randomNum].pageIndex << endl;
											break;
										}
										
									}
									physicalMemoryLRU.erase(physicalMemoryLRU.begin() + randomNum);
									physicalMemoryLRU.insert(physicalMemoryLRU.begin() + randomNum, newFrame);
								}
								else{
									int q=0;
									for(q=0; q<physicalMemoryLRU.size(); q++){
										if(physicalMemoryLRU[q].processID == -1){
											activeProcesses[i].pageTable[virtualPageNumber] = q;
											break;
										}
										if(q+1 == physicalMemoryLRU.size()){
											activeProcesses[i].pageTable[virtualPageNumber] = q+1;
										}
									}
									currentSlotsFull++;
									frame newFrame;
									newFrame.processID = activeProcesses[i].idNum;
									newFrame.pageIndex = virtualPageNumber;
									newFrame.counter = 0;
									activeProcesses[i].pageTable[virtualPageNumber] = q;
									physicalMemoryLRU.insert(physicalMemoryLRU.begin()+q, newFrame);
									//cout << "Page Fault - Loading into memory slot " << q << endl;	
								}
								for(int q=0; q<physicalMemoryLRU.size(); q++){
									physicalMemoryLRU[q].counter++;
								}
							}
							else if(numTimes == 3){
								if(currentSlotsFull == frames) {
									//cout << "Page Fault" << endl;
									int highestCounter=0;
									int randomNum;
									for(int q=0; q<physicalMemoryFIFO.size(); q++){
										if(physicalMemoryFIFO[q].processID != -1){
											if(physicalMemoryFIFO[q].counter > highestCounter){
												highestCounter = physicalMemoryFIFO[q].counter;
												randomNum = q;
											}
										}
									}
									frame newFrame;
									newFrame.processID = activeProcesses[i].idNum;
									newFrame.pageIndex = virtualPageNumber;
									newFrame.counter = 0;
									activeProcesses[i].pageTable[virtualPageNumber] = randomNum;
									frame oldFrame = physicalMemoryFIFO[randomNum];
									for(int k = 0; k<activeProcesses.size(); k++){
										if(activeProcesses[k].idNum == oldFrame.processID){
											activeProcesses[k].pageTable[oldFrame.pageIndex] = -1;
											//cout << "Evicting Process " << activeProcesses[k].idNum << " Page " << physicalMemoryFIFO[randomNum].pageIndex << endl;
											break;
										}
										
									}
									physicalMemoryFIFO.erase(physicalMemoryFIFO.begin() + randomNum);
									physicalMemoryFIFO.insert(physicalMemoryFIFO.begin() + randomNum, newFrame);
								}
								else{
									int q=0;
									for(q=0; q<physicalMemoryFIFO.size(); q++){
										if(physicalMemoryFIFO[q].processID == -1){
											activeProcesses[i].pageTable[virtualPageNumber] = q;
											break;
										}
										if(q+1 == physicalMemoryFIFO.size()){
											activeProcesses[i].pageTable[virtualPageNumber] = q+1;
										}
									}
									currentSlotsFull++;
									frame newFrame;
									newFrame.processID = activeProcesses[i].idNum;
									newFrame.pageIndex = virtualPageNumber;
									newFrame.counter = 0;
									activeProcesses[i].pageTable[virtualPageNumber] = q;							
									physicalMemoryFIFO.insert(physicalMemoryFIFO.begin()+q, newFrame);
									//cout << "Page Fault - Loading into memory slot " << q << endl;	
								}
								for(int q=0; q<physicalMemoryFIFO.size(); q++){
									physicalMemoryFIFO[q].counter++;
								}
							}
						}
						else{
							//cout << "Already in Memory" << endl;
							if(numTimes == 2){
								physicalMemoryLRU[activeProcesses[i].pageTable[virtualPageNumber]].counter = 0;
								for(int q=0; q<physicalMemoryLRU.size(); q++){
									physicalMemoryLRU[q].counter++;
								}
							}
						}
						break;
					}
					else if(i == activeProcesses.size()-1){cout << "Not an Active Process ID" << endl;}
				}
			}
		}
		if(numTimes == 1){
			cout << "RANDOM: ";
		}
		else if(numTimes == 2){
			cout << "LRU: ";
		}
		else if(numTimes == 3){
			cout << "FIFO: ";
		}
		numTimes++;
		inFile.close();
		float rate = (float)faults/(float)totalRef;
		cout << rate*100 << "%" << endl;
	}
	return 0;
}
