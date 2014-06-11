//---------------------------------------------------------------------------//
//Description: Deal with Log
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "LogSvc.hh"
#include "myglobals.hh"

#include <stdlib.h>
#include <time.h>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <iostream>

LogSvc* LogSvc::fLogSvc = 0;

LogSvc::LogSvc()
{
	if (fLogSvc){
		G4Exception("LogSvc::LogSvc()","Run0031",
				FatalException, "LogSvc constructed twice.");
	}
	fLogSvc = this;

}

LogSvc::~LogSvc()
{
	printf("~LogSvc\n");
}

LogSvc* LogSvc::GetLogSvc(){
	if ( !fLogSvc ){
		fLogSvc = new LogSvc;
	}
	return fLogSvc;
}

bool LogSvc::CreateFile(){
	std::ofstream outfile(fFileName.c_str());
	outfile<<"0 TEST";
	outfile.close();
}

bool LogSvc::CheckFileExist(){
	bool isOK = ( access( fFileName.c_str(), 0 ) == 0 );
	return isOK;
}

bool LogSvc::CheckFileAvailable(){
	bool isOK = ( access( fFileName.c_str(), 6 ) == 0 );
	return isOK;
}

void LogSvc::SetLogFile( const char* file_name ){
	fFileName = file_name;
}

int LogSvc::OpenFile(){
	fd = open(fFileName.c_str(), O_RDWR|O_CREAT);
	if ( fd<0 ){
		return -1;
	}
	return 0;
}

void LogSvc::CloseFile(){
	close(fd);
}

int LogSvc::LockFile(){
	lock.l_type = F_WRLCK;
	lock.l_whence = 0;
	lock.l_start = 0;
	lock.l_len = 0;
	int count = 0;
	while(fcntl(fd,F_SETLK,&lock)<0){
		if(errno==EAGAIN||errno==EACCES){//locked by other process
			if(++count<100){
				fcntl(fd,F_GETLK,&lock); //give up and output error message
				std::cout<<"Pid: "<<getpid()<<" process finds "<<lock.l_pid<<" locked the file "<<fFileName<<", "<<count<<"s."<<std::endl;
				sleep(1);//wait for 100 sec at most
			}
			else{
				fcntl(fd,F_GETLK,&lock); //give up and output error message
				std::cout<<"Pid: "<<getpid()<<" process finds "<<lock.l_pid<<" locked the file "<<fFileName<<std::endl;
				return 1;
			}
		}
		else{
			std::cout<<"Error: cannnot lock file "<<fFileName<<", errno = "<<errno<<std::endl;
			return 2;
		}
	}
	time_t t = time( 0 );
	char tmp[64];
	strftime( tmp, sizeof(tmp), "%Y/%m/%d %X %A",localtime(&t) );
	std::cout<<tmp<<std::endl;
	std::cout<<"Pid: "<<getpid()<<" process locked file "<<fFileName<<std::endl;
	return 0;
}

int LogSvc::UnLockFile(){
	lock.l_type = F_UNLCK;
	lock.l_whence = 0;
	lock.l_start = 0;
	lock.l_len = 0;
	int val = fcntl(fd,F_SETLK,&lock);
	if(val<0){
		std::cout<<"Pid: "<<getpid()<<" process cannnot unlock file "<<fFileName<<std::endl;
		return 1;
	}
	else{
		time_t t = time( 0 );
		char tmp[64];
		strftime( tmp, sizeof(tmp), "%Y/%m/%d %X %A",localtime(&t) );
		std::cout<<tmp<<std::endl;
		std::cout<<"Pid: "<<getpid()<<" process unlocked file "<<fFileName<<std::endl;
	}
	return 0;
}

int LogSvc::AddLog( const char* run_name ){
	if ( !CheckFileExist() ){
		std::cout<<"File "<<fFileName<<" dose not exist! Will generate a new one."<<std::endl;
		CreateFile();
	}
	int status;
	status = OpenFile();
	if ( status != 0 ){
		std::cout<<"In LogSvs::AddLog, cannnot open file "<<fFileName<<", will set run_num to -1"<<std::endl;
		return -1;
	}
	status = LockFile();
	if (status != 0){
		std::cout<<"In LogSvs::AddLog, cannnot lock file "<<fFileName<<", will set run_num to -1"<<std::endl;
		return -1;
	}
	std::ifstream fin_log(fFileName.c_str());
	if(!fin_log){
		std::cout<<"In LogSvs::AddLog, cannot open "<<fFileName<<" in ifstream format, will set run_num to -1"<<std::endl;
		return -1;
	}
	std::stringstream buf_log;
	buf_log.clear();
	buf_log.str("");
	std::string s_log;
	while(getline(fin_log,s_log)){
		buf_log<<s_log<<"\n";
	}
	//std::cout<<"before modification, logfile is like:"<<std::endl;
	//std::cout<<buf_log.str()<<std::endl;
	int run_num;
	buf_log >> run_num;
	run_num++;
	std::ofstream fout;
	fout.open(fFileName.c_str());
	if(!fout){
		std::cout<<"In LogSvs::AddLog, cannot open "<<fFileName<<" in ofstream format, will set run_num to -1"<<std::endl;
		return -1;
	}
	time_t t = time( 0 );
	char tmp[64];
	strftime( tmp, sizeof(tmp), "%Y/%m/%d %X %A",localtime(&t) );
	fout<<std::setiosflags(std::ios::left)<<std::setw(5)<<run_num<<" "<<std::setiosflags(std::ios::left)<<std::setw(30)<<run_name<<" "<<tmp<<std::endl;
	fout<<buf_log.str();
	//std::cout<<"after modification, logfile is like:"<<std::endl;
	//std::cout<<std::setiosflags(std::ios::left)<<std::setw(5)<<run_num<<" "<<std::setiosflags(std::ios::left)<<std::setw(30)<<run_name<<" "<<tmp<<std::endl;
	//std::cout<<buf_log.str()<<std::endl;
	buf_log.str("");
	buf_log.clear();
	//    sleep(3);
	status = UnLockFile();
	if ( status != 0 ){
		std::cout<<"Cannot unlock"<<fFileName<<", will set run number to -1."<<std::endl;
		CloseFile();
		return -1;
	}
	fout.close();
	//CloseFile();
	return run_num;
}
