//---------------------------------------------------------------------------//
//Description: Deal with Log
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#ifndef LogSvc_h
#define LogSvc_h 1

#include <G4String.hh>

//for locking file
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
//#include <sys/file.h>

class LogSvc
{
  public:
    LogSvc();
     ~LogSvc();

    static LogSvc* GetLogSvc();

    bool CreateFile();

    bool CheckFileExist();

    bool CheckFileAvailable();

    void SetLogFile( const G4String& file_name );

    int AddLog( const char* run_name);

  private:

		int OpenFile();

		void CloseFile();

    int LockFile();

    int UnLockFile();

    static LogSvc* fLogSvc;

    G4String fFileName;

		int fd;

		struct flock lock;

};

#endif

