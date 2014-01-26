#include "MyProcessManager.hh"

#include "globals.hh"

#include <iostream>
#include <sstream>
#include <unistd.h>

MyProcessManager* MyProcessManager::fMyProcessManager = 0;

MyProcessManager::MyProcessManager()
{
	if (fMyProcessManager){
		G4Exception("MyProcessManager::MyProcessManager()","Run0031",
				FatalException, "MyProcessManager constructed twice.");
	}
	fileOpened = false;
	fin_card = 0;
	MemoryConsumption = 0;
}

MyProcessManager::~MyProcessManager()
{
	CloseFile();
	delete &buff;
}

MyProcessManager* MyProcessManager::GetMyProcessManager(){
	if ( !fMyProcessManager ){
		fMyProcessManager = new MyProcessManager;
	}
	return fMyProcessManager;
}

int MyProcessManager::OpenFile(){
	CloseFile();
	pid_t id = getpid();
	buff.str("");
	buff.clear();
	buff<<"/proc/"<<id<<"/status";
	fin_card = new std::ifstream (buff.str().c_str());
	std::cout<<"In MyProcessManager::OpenFile: Opened file \""<<buff.str()<<"\""<<std::endl;
	if ( !fin_card ){
		fileOpened = false;
		return 1;
	}
	else{
		fileOpened = true;
		return 0;
	}
}

void MyProcessManager::CloseFile(){
	if (fileOpened&&fin_card){
		fin_card->close();
	}
	fileOpened = false;
	fin_card = 0;
}

int MyProcessManager::GetMemorySize(){
	if (!fileOpened){
		OpenFile();
	}
	if (fin_card&&fileOpened){
		std::string s_card;
		std::string temp;
		double size = -2;
		while(getline(*fin_card,s_card)){
			buff.str("");
			buff.clear();
			buff<<s_card;
			buff>>temp;
//			std::cout<<"In MyProcessManager::GetMemorySize: Found Line \""<<s_card<<"\""<<std::endl;
			if (temp=="VmSize:"){
				buff>>size>>temp;
				if (temp == "kB" || temp == "KB" || temp == "kb"){
					size /= 1024;
				}
				else if (temp == "mB" || temp == "MB" || temp == "mb"){
				}
				else if (temp == "gB" || temp == "GB" || temp == "gb"){
					size *= 1024;
				}
				else if (temp == "Byte" || temp == "B" || temp == "b"){
					size /= 1024*1024;
				}
				fin_card->clear();
				fin_card->seekg(0,std::ios::beg);
				break;
			}
		}
		return size;
	}
	else{
		return -1;
	}
}
