//---------------------------------------------------------------------------//
//Description: Deal with persistency
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "MyAnalysisSvc.hh"
#include "MyAnalysisSvcMessenger.hh"

#include "globals.hh"
#include "G4Step.hh"
#include "G4Event.hh"
#include "G4Run.hh"

#include "MyRoot.hh"
#include "LogSvc.hh"
#include "MyDetectorManager.hh"
#include "EventHeaderSvc.hh"
#include "McTruthSvc.hh"
#include "ProcessCountingSvc.hh"
#include "MyTriggerSvc.hh"

#include "DEBUG.hh"

MyAnalysisSvc* MyAnalysisSvc::fMyAnalysisSvc = 0;

MyAnalysisSvc::MyAnalysisSvc()
{
	if (fMyAnalysisSvc){
		G4Exception("MyAnalysisSvc::MyAnalysisSvc()","Run0031",
				FatalException, "MyAnalysisSvc constructed twice.");
	}
	fMyAnalysisSvc = this;
	//get pointers
	pMyAnalysisSvcMessenger = new MyAnalysisSvcMessenger(this);
	pMyRoot = MyRoot::GetMyRoot();
	pMyDetectorManager = MyDetectorManager::GetMyDetectorManager();
	pEventHeaderSvc = EventHeaderSvc::GetEventHeaderSvc();
	pMcTruthSvc = McTruthSvc::GetMcTruthSvc();
	pProcessCountingSvc = ProcessCountingSvc::GetProcessCountingSvc();
	pMyTriggerSvc = MyTriggerSvc::GetMyTriggerSvc();

	//default logfile
	run_name = getenv("RUNNAMEROOT");
	//default output file
	ofile_name = getenv("OFILENAMEROOT");
	//set default output card, read it, and inform relative files.
	std::string out_card = getenv("OUTCARDROOT");
	set_out_card(out_card.c_str());
	//default trigger card
	m_minT = -1;
	m_maxT = -1;

}

MyAnalysisSvc::~MyAnalysisSvc()
{
	delete pMyAnalysisSvcMessenger;
}

MyAnalysisSvc* MyAnalysisSvc::GetMyAnalysisSvc(){
	if ( !fMyAnalysisSvc ){
		fMyAnalysisSvc = new MyAnalysisSvc;
	}
	return fMyAnalysisSvc;
}

void MyAnalysisSvc::set_out_card(G4String file_name){
	size_t sLast = file_name.last('/');
	if(sLast==std::string::npos){ // file name only
		G4String dir_name = getenv("CONFIGUREROOT");
		if (dir_name[dir_name.size()-1] != '/') dir_name.append("/");
		file_name = dir_name + file_name;
	}
	//inform relative files
	pEventHeaderSvc->ReadOutputCard(file_name);
	pMyDetectorManager->ReadOutputCard(file_name);
	pMcTruthSvc->ReadOutputCard(file_name);
	pProcessCountingSvc->ReadOutputCard(file_name);
	pMyTriggerSvc->SetMyTrigger(file_name);
	//read output card
	ReadOutputCard(file_name);
	//set verbose
	pMyRoot->SetVerbose(fVerbose);
	pMyRoot->SetPrintModule(fPrintModule);
}

void MyAnalysisSvc::BeginOfRunAction(){
	//deal with log
	std::string log_name = getenv("LOGFILEROOT");
	if ( std::strlen(log_name.c_str()) != 0 ){
		LogSvc::GetLogSvc()->SetLogFile( log_name.c_str() );
	}
	run_num = LogSvc::GetLogSvc()->AddLog( run_name.c_str() );
	std::cout<<"logfile finished for "<<run_name<<", "<<run_num<<std::endl;

	//set root file
	pMyRoot->OpenFile(ofile_name);
	pMyRoot->CreateTree(tree_name, fCircular);

	//set branches
	pMyDetectorManager->SetBranch();
	pEventHeaderSvc->SetBranch();
	pMcTruthSvc->SetBranch();
	pProcessCountingSvc->SetBranch();

	t_begin = (double)clock();
}

void MyAnalysisSvc::PreUserTrackingAction(const G4Track* aTrack){
	//set McTruth
	pMcTruthSvc->SetValuePre(aTrack);
}

void MyAnalysisSvc::PostUserTrackingAction(const G4Track* aTrack) {
	//set McTruth
	pMcTruthSvc->SetValuePost(aTrack);
}

void MyAnalysisSvc::EndOfRunAction(const G4Run* aRun){
	int NbOfEvents = aRun->GetNumberOfEvent();
	t_end = (double)clock();
	std::cout<<"\n##############################################"<<std::endl;
	std::cout<<"TOTAL TIME COST IS:  "<<(double)(t_end-t_begin)/CLOCKS_PER_SEC*1000<<"ms"<<std::endl;
	std::cout<<"TIME COST PER EVENT: "<<(double)(t_end-t_begin)/CLOCKS_PER_SEC/NbOfEvents*1000<<"ms"<<std::endl;
	std::cout<<"##############################################\n"<<std::endl;
	pMyRoot->Write();
	pMyRoot->Close();
}

void MyAnalysisSvc::BeginOfEventAction(){
	//Initialize
	pMcTruthSvc->Initialize();
	pProcessCountingSvc->Initialize();
}

void MyAnalysisSvc::EndOfEventAction(const G4Event* evt){
	//Digitze
	pMyDetectorManager->Digitize();
	//Set event header 
	pEventHeaderSvc->SetValue(evt,run_num);

	//Fill
	if ( pMyTriggerSvc->TriggerIt(evt) ) pMyRoot->Fill();

	//AutoSave
	if (fAutoSave){
		int evt_num = evt->GetEventID();
		if ( evt_num%fAutoSave == 0 ){
			pMyRoot->Save();
			int nBytes = pMyRoot->FlushBaskets();
			std::cout<<"Event "<<evt_num<<", "<<nBytes<<"Bytes data written"<<std::endl;
		}
	}
}

void MyAnalysisSvc::SteppingAction(const G4Step* aStep){
	//set ProcessCounting
	pProcessCountingSvc->SetValue(aStep);
	G4Track* aTrack = aStep->GetTrack() ;
	G4int nSteps = aTrack->GetCurrentStepNumber();
	if (nSteps>2e4)
		aTrack->SetTrackStatus(fStopAndKill);
	G4double globalT=aTrack->GetGlobalTime();//Time since the event in which the track belongs is created
	if ((m_minT>=0&&m_minT>globalT)&&(m_maxT>=0&&m_maxT<globalT))
		aTrack->SetTrackStatus(fStopAndKill);
}

void MyAnalysisSvc::InitialStepAction(const G4Step* aStep){
	pProcessCountingSvc->InitialStep(aStep);
}

void MyAnalysisSvc::ASDI(G4String name){
	pProcessCountingSvc->AddASDI(name);
}

void MyAnalysisSvc::PSDI(G4String name){
	pProcessCountingSvc->AddPSDI(name);
}

void MyAnalysisSvc::ReadOutputCard(G4String file_name){
	std::ifstream fin_card(file_name);
	if(!fin_card){
		std::cout<<"In MyAnalysisSvc::ReadOutputCard, cannot open "<<file_name<<"!!!"<<std::endl;
		G4Exception("MyAnalysisSvc::ReadOutputCard()",
				"InvalidSetup", FatalException,
				"cannot find output card");
	}
	std::stringstream buf_card;
	std::string s_card;
	bool find_tree_name = false;
	bool find_AutoSave = false;
	bool find_Circular = false;
	bool find_Verbose = false;
	bool find_PrintModule = false;
	while(getline(fin_card,s_card)){
		buf_card.str("");
		buf_card.clear();
		buf_card<<s_card;

		//eleminate useless lines
		const char* c_card = s_card.c_str();
		int length = strlen(c_card);
		int offset = 0;
		for ( ; offset < length; offset++ ){
			if ( c_card[offset] != ' ' ) break;
		}
		if ( c_card[offset] == '#' || (c_card[offset] == '/' && c_card[offset+1] == '/') || length - offset == 0 ){
			continue;
		}
		std::string name;
		buf_card>>name;
		if ( name == "tree_name" ){
			buf_card>>tree_name;
			std::cout<<"In MyAnalysisSvc::ReadOutputCard, tree_name will be set to "<<tree_name<<std::endl;
			find_tree_name = true;
		}
		else if ( name == "AutoSave" ){
			buf_card>>fAutoSave;
			std::cout<<"In MyAnalysisSvc::ReadOutputCard, fAutoSave will be set to "<<fAutoSave<<std::endl;
			find_AutoSave = true;
		}
		else if ( name == "Circular" ){
			buf_card>>fCircular;
			std::cout<<"In MyAnalysisSvc::ReadOutputCard, fCircular will be set to "<<fCircular<<std::endl;
			find_Circular = true;
		}
		else if ( name == "Verbose" ){
			buf_card>>fVerbose;
			std::cout<<"In MyAnalysisSvc::ReadOutputCard, fVerbose will be set to "<<fVerbose<<std::endl;
			find_Verbose = true;
		}
		else if ( name == "PrintModule" ){
			buf_card>>fPrintModule;
			std::cout<<"In MyAnalysisSvc::ReadOutputCard, fPrintModule will be set to "<<fPrintModule<<std::endl;
			find_PrintModule = true;
		}
		if ( find_Verbose && find_PrintModule && find_Circular && find_AutoSave && find_tree_name ) break;
	}
	if (!find_tree_name){
		std::cout<<"In MyAnalysisSvc::ReadOutputCard, tree_name not found in card, will be set to t as default"<<std::endl;
		tree_name = "t";
	}
	if (!find_AutoSave){
		std::cout<<"In MyAnalysisSvc::ReadOutputCard, AutoSave not found in card, will be set to 0 as default"<<std::endl;
		fAutoSave = 0;
	}
	if (!find_Circular){
		std::cout<<"In MyAnalysisSvc::ReadOutputCard, Circular not found in card, will be set to 0 as default"<<std::endl;
		fCircular = 0;
	}
	if (!find_Verbose){
		std::cout<<"In MyAnalysisSvc::ReadOutputCard, Verbose not found in card, will be set to 0 as default"<<std::endl;
		fVerbose = 0;
	}
	if (!find_PrintModule){
		std::cout<<"In MyAnalysisSvc::ReadOutputCard, PrintModule not found in card, will be set to 0 as default"<<std::endl;
		fPrintModule = 0;
	}
	fin_card.close();
	buf_card.str("");
	buf_card.clear();
}
