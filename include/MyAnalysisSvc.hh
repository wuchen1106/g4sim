//---------------------------------------------------------------------------//
//Description: Deal with persistency
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#ifndef MyAnalysisSvc_h
#define MyAnalysisSvc_h 1

#include "myglobals.hh"

class G4Step;
class G4Event;
class G4Track;
class G4Run;

class MyAnalysisSvcMessenger;
class MyRoot;
class EventHeaderSvc;
class MyDetectorManager;
class McTruthSvc;
class ProcessCountingSvc;
class MyTriggerSvc;
class PrimaryGeneratorAction;
class MyProcessManager;

class MyAnalysisSvc
{
	public:
		MyAnalysisSvc();
		virtual ~MyAnalysisSvc();

		static MyAnalysisSvc* GetMyAnalysisSvc();
		void set_out_card(G4String filename);

		void set_PrintModulo(int i){fPrintModulo = i;};

		void set_ofile_name(const G4String& val){ ofile_name = val; };
		void set_tree_name(const G4String& val){ tree_name = val; };
		void set_run_name(const G4String& val){ run_name = val; };

		void set_minT(double val){m_minT=val;};
		void set_maxT(double val){m_maxT=val;};

		int get_evt_num(){return evt_num;};

		void ReadOutputCard(const G4String& filename);
		int  GetRunID(){return run_num;};

		void BeginOfRunAction();
		void EndOfRunAction(const G4Run*);
		void PreUserTrackingAction(const G4Track*);
		void PostUserTrackingAction(const G4Track*);
		void BeginOfEventAction();
		void EndOfEventAction(const G4Event*);
		void InitialStepAction(const G4Step*);
		void SteppingAction(const G4Step*);
		void ASDI(const G4String&);
		void PSDI(const G4String&);

	private:
		static MyAnalysisSvc* fMyAnalysisSvc;
		MyAnalysisSvcMessenger* pMyAnalysisSvcMessenger;

		G4String out_card;
		G4String run_name;
		G4String ofile_name;
		G4String tree_name;
		int run_num;
		int fAutoSave;
		int fCircular;
		int fVerbose;
		int fPrintModulo;
		int evt_num;

		double m_minT;
		double m_maxT;

		clock_t t_begin;
		clock_t t_end;
		double event_start_time;

		MyRoot* pMyRoot;
		EventHeaderSvc* pEventHeaderSvc;
		MyDetectorManager* pMyDetectorManager;
		McTruthSvc* pMcTruthSvc;
		ProcessCountingSvc* pProcessCountingSvc;
		MyTriggerSvc* pMyTriggerSvc;
		PrimaryGeneratorAction* pPrimaryGeneratorAction;
		MyProcessManager* pMyProcessManager;
};

#endif
