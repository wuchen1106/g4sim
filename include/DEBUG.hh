#include <iostream>
#include <time.h>
#include <sys/time.h>

#ifndef DEBUG_h
#define DEBUG_h 1

//#define DEBUG_MYTRI //MyTriggerSvc
//#define DEBUG_MYDETM //MyDetectorManager
#define DEBUG_EM //G4EventManager
//#define DEBUG_CDCSD //CdcSD
//#define DEBUG_MTA //MyTrackingAction
//#define DEBUG_SA //SteppingAction
//#define DEBUG_EA //EventAction

#define SHOWTIME() if(1){\
	struct timeval tv;\
	gettimeofday(&tv,NULL);\
	double timesys = (double)clock();\
	timesys *= 1000000./CLOCKS_PER_SEC;\
	printf("%u:%u,%f",tv.tv_sec,tv.tv_usec,timesys);\
}
//#define GETMYTIME( var ) if(1){time_t GETMYTIME_t = time(0); strftime( var, sizeof(var), "%Y/%m/%d %X %A",localtime(&GETMYTIME_t) );}

#ifdef DEBUG_MYTRI
	#define MYTRI_LINEINFO() if (1) {std::cout << __FILE__ << ":" << __FUNCTION__ << "[" << __LINE__ << "] @"; SHOWTIME(); std::cout<<": " << std::endl;}
	#define MYTRI_LINEVAR( var ) if (1) {std::cout << __FILE__ << ":" << __FUNCTION__ << "[" << __LINE__ << "] @"; SHOWTIME(); std::cout<<": "<< #var << " = " << var << std::endl;}
	#define MYTRI_LINECONT( var ) if (1) {std::cout << __FILE__ << ":" << __FUNCTION__ << "[" << __LINE__ << "] @"; SHOWTIME(); std::cout<<": "<< var << std::endl;}
#else
	#define MYTRI_LINEINFO()
	#define MYTRI_LINEVAR( var )
	#define MYTRI_LINECONT( var )
#endif

#ifdef DEBUG_MYDETM
	#define MYDETM_LINEINFO() if (1) {std::cout << __FILE__ << ":" << __FUNCTION__ << "[" << __LINE__ << "] @"; SHOWTIME(); std::cout<<": " << std::endl;}
	#define MYDETM_LINEVAR( var ) if (1) {std::cout << __FILE__ << ":" << __FUNCTION__ << "[" << __LINE__ << "] @"; SHOWTIME(); std::cout<<": "<< #var << " = " << var << std::endl;}
	#define MYDETM_LINECONT( var ) if (1) {std::cout << __FILE__ << ":" << __FUNCTION__ << "[" << __LINE__ << "] @"; SHOWTIME(); std::cout<<": "<< var << std::endl;}
#else
	#define MYDETM_LINEINFO()
	#define MYDETM_LINEVAR( var )
	#define MYDETM_LINECONT( var )
#endif

#ifdef DEBUG_CDCSD
	#define CDCSD_LINEINFO() if (1) {std::cout << __FILE__ << ":" << __FUNCTION__ << "[" << __LINE__ << "] @"; SHOWTIME(); std::cout<<": " << std::endl;}
	#define CDCSD_LINEVAR( var ) if (1) {std::cout << __FILE__ << ":" << __FUNCTION__ << "[" << __LINE__ << "] @"; SHOWTIME(); std::cout<<": "<< #var << " = " << var << std::endl;}
	#define CDCSD_LINECONT( var ) if (1) {std::cout << __FILE__ << ":" << __FUNCTION__ << "[" << __LINE__ << "] @"; SHOWTIME(); std::cout<<": "<< var << std::endl;}
#else
	#define CDCSD_LINEINFO()
	#define CDCSD_LINEVAR( var )
	#define CDCSD_LINECONT( var )
#endif

#ifdef DEBUG_EM
	#define EM_LINEINFO() if (1) {std::cout << __FILE__ << ":" << __FUNCTION__ << "[" << __LINE__ << "] @"; SHOWTIME(); std::cout<<": " << std::endl;}
	#define EM_LINEVAR( var ) if (1) {std::cout << __FILE__ << ":" << __FUNCTION__ << "[" << __LINE__ << "] @"; SHOWTIME(); std::cout<<": "<< #var << " = " << var << std::endl;}
	#define EM_LINECONT( var ) if (1) {std::cout << __FILE__ << ":" << __FUNCTION__ << "[" << __LINE__ << "] @"; SHOWTIME(); std::cout<<": "<< var << std::endl;}
#else
	#define EM_LINEINFO()
	#define EM_LINEVAR( var )
	#define EM_LINECONT( var )
#endif

#ifdef DEBUG_MTA
	#define MTA_LINEINFO() if (1) {std::cout << __FILE__ << ":" << __FUNCTION__ << "[" << __LINE__ << "] @"; SHOWTIME(); std::cout<<": " << std::endl;}
	#define MTA_LINEVAR( var ) if (1) {std::cout << __FILE__ << ":" << __FUNCTION__ << "[" << __LINE__ << "] @"; SHOWTIME(); std::cout<<": "<< #var << " = " << var << std::endl;}
	#define MTA_LINECONT( var ) if (1) {std::cout << __FILE__ << ":" << __FUNCTION__ << "[" << __LINE__ << "] @"; SHOWTIME(); std::cout<<": "<< var << std::endl;}
#else
	#define MTA_LINEINFO()
	#define MTA_LINEVAR( var )
	#define MTA_LINECONT( var )
#endif

#ifdef DEBUG_SA
	#define SA_LINEINFO() if (1) {std::cout << __FILE__ << ":" << __FUNCTION__ << "[" << __LINE__ << "] @"; SHOWTIME(); std::cout<<": " << std::endl;}
	#define SA_LINEVAR( var ) if (1) {std::cout << __FILE__ << ":" << __FUNCTION__ << "[" << __LINE__ << "] @"; SHOWTIME(); std::cout<<": "<< #var << " = " << var << std::endl;}
	#define SA_LINECONT( var ) if (1) {std::cout << __FILE__ << ":" << __FUNCTION__ << "[" << __LINE__ << "] @"; SHOWTIME(); std::cout<<": "<< var << std::endl;}
#else
	#define SA_LINEINFO()
	#define SA_LINEVAR( var )
	#define SA_LINECONT( var )
#endif

#ifdef DEBUG_EA
	#define EA_LINEINFO() if (1) {std::cout << __FILE__ << ":" << __FUNCTION__ << "[" << __LINE__ << "] @"; SHOWTIME(); std::cout<<": " << std::endl;}
	#define EA_LINEVAR( var ) if (1) {std::cout << __FILE__ << ":" << __FUNCTION__ << "[" << __LINE__ << "] @"; SHOWTIME(); std::cout<<": "<< #var << " = " << var << std::endl;}
	#define EA_LINECONT( var ) if (1) {std::cout << __FILE__ << ":" << __FUNCTION__ << "[" << __LINE__ << "] @"; SHOWTIME(); std::cout<<": "<< var << std::endl;}
#else
	#define EA_LINEINFO()
	#define EA_LINEVAR( var )
	#define EA_LINECONT( var )
#endif

#endif
