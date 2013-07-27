//---------------------------------------------------------------------------
//Description: Sensitive detector definition for Monitor
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "globals.hh"
#include "G4DigiManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4TransportationManager.hh"
#include "G4FieldManager.hh"
#include "G4MagneticField.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"
#include "G4RunManager.hh"
#include "G4TrackStatus.hh"

#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Point3D.h"

#include "TVector3.h"

#include <iostream>

#include "MyString2Anything.hh"
#include "SimpleGeometryParameter.hh"
#include "MonitorHit.hh"
#include "MyRoot.hh"
#include "MySD.hh"

#include "MonitorSD.hh"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
typedef HepGeom::Vector3D<double> HepVector3D;
#endif

	MonitorSD::MonitorSD(G4String name, MyVGeometryParameter* pointer)
:MySD(name, pointer), hitsCollection(0)
{}

MonitorSD::~MonitorSD(){
}

//-----------------------------------Initialize----------------------------------------------
//Will be called by geant4 automatically at the beginning of each event
void MonitorSD::Initialize(G4HCofThisEvent* HCE)
{
	hitsCollection = new MonitorHitsCollection
		(SensitiveDetectorName,collectionName[0]);
	//	std::cout<<"====>In MonitorSD::Initialize(), created hitsCollection at ("<<(void*)hitsCollection<<std::endl;
	G4int HCID = -1;
	HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
	if ( HCID < 0 ){
		G4Exception("MonitorSD::Initialize()",
				"InvalidSetup", FatalException,
				"Cann not find hitsCollection of this event");
	}
	HCE->AddHitsCollection( HCID, hitsCollection );
	//initialize for root objects
	nHits = 0;
	m_x.clear();
	m_y.clear();
	m_z.clear();
	m_t.clear();
	m_px.clear();
	m_py.clear();
	m_pz.clear();
	m_e.clear();
	m_edep.clear();
	m_stepL.clear();
	m_volID.clear();
	m_volName.clear();
	m_tid.clear();
	m_pid.clear();
	m_charge.clear();
	m_stopped.clear();
	m_stop_time.clear();
	m_killed.clear();
	m_kill_time.clear();
}

//-----------------------------------SetBranch----------------------------------------------
//Should be called at the beginning of each run (now in MyAnalysisSvc::BeginOfRunAction())
void MonitorSD::SetBranch(){
	if (!Switch) return;
	MyRoot* myRoot = MyRoot::GetMyRoot();
	G4String volName = get_VolName();
	if( flag_nHits ) myRoot->SetBranch(volName+"_nHits", &nHits);
	if( flag_x ) myRoot->SetBranch(volName+"_x", &m_x);
	if( flag_y ) myRoot->SetBranch(volName+"_y", &m_y);
	if( flag_z ) myRoot->SetBranch(volName+"_z", &m_z);
	if( flag_t ) myRoot->SetBranch(volName+"_t", &m_t);
	if( flag_px ) myRoot->SetBranch(volName+"_px", &m_px);
	if( flag_py ) myRoot->SetBranch(volName+"_py", &m_py);
	if( flag_pz ) myRoot->SetBranch(volName+"_pz", &m_pz);
	if( flag_e ) myRoot->SetBranch(volName+"_e", &m_e);
	if( flag_edep ) myRoot->SetBranch(volName+"_edep", &m_edep);
	if( flag_stepL ) myRoot->SetBranch(volName+"_stepL", &m_stepL);
	if( flag_volID ) myRoot->SetBranch(volName+"_volID", &m_volID);
	if( flag_volName ) myRoot->SetBranch(volName+"_volName", &m_volName);
	if( flag_tid ) myRoot->SetBranch(volName+"_tid", &m_tid);
	if( flag_pid ) myRoot->SetBranch(volName+"_pid", &m_pid);
	if( flag_charge ) myRoot->SetBranch(volName+"_charge", &m_charge);
	if( flag_stopped ) myRoot->SetBranch(volName+"_stopped", &m_stopped);
	if( flag_stop_time ) myRoot->SetBranch(volName+"_stop_time", &m_stop_time);
	if( flag_killed ) myRoot->SetBranch(volName+"_killed", &m_killed);
	if( flag_kill_time ) myRoot->SetBranch(volName+"_kill_time", &m_kill_time);
}

//-----------------------------------ReadOutputCard----------------------------------------------
//Should be called before the begin of each run (now in the constructor of MyAnalysisSvc)
void MonitorSD::ReadOutputCard(G4String filename){
	ReSet();
	std::ifstream fin_card(filename);
	if(!fin_card){
		std::cout<<"In MonitorSD::ReadOutputCard, cannot open "<<filename<<"!!!"<<std::endl;
		G4Exception("MonitorSD::ReadOutputCard()",
				"InvalidSetup", FatalException,
				"cannot find output card");
	}
	std::stringstream buf_card;
	G4String s_card;
	G4String volName = get_VolName();
	int n_output_section_symbol = 0;
	int n_filter_section_symbol = 0;
	G4String BRANCHSECTIONNAME = volName + "_SECTION";
	G4String FILTERSECTIONNAME = volName + "_FILTERSECTION";
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

		G4String name, unit;
		buf_card>>name;
		if ( n_output_section_symbol == 0 ){
			if ( name == BRANCHSECTIONNAME ){
				n_output_section_symbol++;
			}
		}
		else if ( n_output_section_symbol == 1 ){
			if ( name == BRANCHSECTIONNAME ){
				n_output_section_symbol++;
			}
			else if( name == "nHits" ) {flag_nHits = true;}
			else if( name == "x" ) {{flag_x = true; buf_card>>unitName_x; unit_x = MyString2Anything::get_U(unitName_x);}}
			else if( name == "y" ) {{flag_y = true; buf_card>>unitName_y; unit_y = MyString2Anything::get_U(unitName_y);}}
			else if( name == "z" ) {{flag_z = true; buf_card>>unitName_z; unit_z = MyString2Anything::get_U(unitName_z);}}
			else if( name == "t" ) {{flag_t = true; buf_card>>unitName_t; unit_t = MyString2Anything::get_U(unitName_t);}}
			else if( name == "px" ) {{flag_px = true; buf_card>>unitName_px; unit_px = MyString2Anything::get_U(unitName_px);}}
			else if( name == "py" ) {{flag_py = true; buf_card>>unitName_py; unit_py = MyString2Anything::get_U(unitName_py);}}
			else if( name == "pz" ) {{flag_pz = true; buf_card>>unitName_pz; unit_pz = MyString2Anything::get_U(unitName_pz);}}
			else if( name == "e" ) {{flag_e = true; buf_card>>unitName_e; unit_e = MyString2Anything::get_U(unitName_e);}}
			else if( name == "edep" ) {{flag_edep = true; buf_card>>unitName_edep; unit_edep = MyString2Anything::get_U(unitName_edep);}}
			else if( name == "stepL" ) {{flag_stepL = true; buf_card>>unitName_stepL; unit_stepL = MyString2Anything::get_U(unitName_stepL);}}
			else if( name == "volID" ) {flag_volID = true;}
			else if( name == "volName" ) {flag_volName = true;}
			else if( name == "tid" ) {flag_tid = true;}
			else if( name == "pid" ) {flag_pid = true;}
			else if( name == "charge" ) {flag_charge = true;}
			else if( name == "stopped" ) {flag_stopped = true;}
			else if( name == "stop_time" ) {{flag_stop_time = true; buf_card>>unitName_stop_time; unit_stop_time = MyString2Anything::get_U(unitName_stop_time);}}
			else if( name == "killed" ) {flag_killed = true;}
			else if( name == "kill_time" ) {{flag_kill_time = true; buf_card>>unitName_kill_time; unit_kill_time = MyString2Anything::get_U(unitName_kill_time);}}
			else{
				std::cout<<"In MonitorSD::ReadOutputCard, unknown name: "<<name<<" in file "<<filename<<std::endl;
				std::cout<<"Will ignore this line!"<<std::endl;
			}
		}

		if ( n_filter_section_symbol == 0 ){
			if ( name == FILTERSECTIONNAME ){
				n_filter_section_symbol++;
			}
		}
		else if ( n_filter_section_symbol == 1 ){
			if ( name == FILTERSECTIONNAME ){
				n_filter_section_symbol++;
			}
			else if( name == "Switch" ) Switch = true;
			else if( name == "neutralCut" ) neutralCut = true;
			else if( name == "maxn" ) buf_card>>maxn;
			else if( name == "WL" ){
				int pid = 0;
				buf_card>>pid;
				white_list.push_back(pid);
			}
			else{
				G4double para;
				G4String unit;
				buf_card>>para>>unit;
				para *= MyString2Anything::get_U(unit);
				if( name == "minp" ) minp = para;
				else if ( name == "minedep" ) minedep = para;
				else if( name == "mint" ) mint = para;
				else if( name == "maxt" ) maxt = para;
				else if( name == "tres" ) tres = para;
				else{
					std::cout<<"In MonitorSD::ReadOutputCard, unknown name: "<<name<<" in file "<<filename<<std::endl;
					std::cout<<"Will ignore this line!"<<std::endl;
				}
			}
		}

		if ( n_output_section_symbol > 1 && n_filter_section_symbol > 1 ){
			break;
		}
	}
	buf_card.str("");
	buf_card.clear();
	if ( n_output_section_symbol <= 1 ){
		std::cout<<"*****************WARNING********************"<<std::endl;
		std::cout<<"In MonitorSD::ReadOutputCard, failed to find enough section seperators \""<<BRANCHSECTIONNAME<<"\" for output in file "<<filename<<std::endl;
		std::cout<<"Will use default settings."<<std::endl;
		std::cout<<"********************************************"<<std::endl;
	}
	if ( n_filter_section_symbol<= 1 ){
		std::cout<<"*****************WARNING********************"<<std::endl;
		std::cout<<"In MonitorSD::ReadOutputCard, failed to find enough section seperators \""<<FILTERSECTIONNAME<<"\" for filter in file "<<filename<<std::endl;
		std::cout<<"Will use default settings."<<std::endl;
		std::cout<<"********************************************"<<std::endl;
	}
	fin_card.close();
	ShowOutCard();
}

//-----------------------------------ReSet----------------------------------------------
//Private
void MonitorSD::ReSet(){
	//default output set
	flag_nHits = false;
	flag_x = false;
	flag_y = false;
	flag_z = false;
	flag_t = false;
	flag_px = false;
	flag_py = false;
	flag_pz = false;
	flag_e = false;
	flag_edep = false;
	flag_stepL = false;
	flag_volID = false;
	flag_volName = false;
	flag_tid = false;
	flag_pid = false;
	flag_charge = false;
	flag_stopped = false;
	flag_stop_time = false;
	flag_killed = false;
	flag_kill_time = false;
	//for fileter
	Switch = false;
	neutralCut = false;
	minp = 0;
	maxn = 0;
	mint = 0;
	maxt = 0;
	tres = 0;
	minedep = -1*MeV;
	white_list.clear();
	//for units
	unitName_x = "cm";
	unitName_y = "cm";
	unitName_z = "cm";
	unitName_t = "ns";
	unitName_px = "GeV";
	unitName_py = "GeV";
	unitName_pz = "GeV";
	unitName_e = "GeV";
	unitName_edep = "GeV";
	unitName_stepL = "cm";
	unitName_stop_time = "ns";
	unitName_kill_time = "ns";
	unit_x = MyString2Anything::get_U(unitName_x);
	unit_y = MyString2Anything::get_U(unitName_y);
	unit_z = MyString2Anything::get_U(unitName_z);
	unit_t = MyString2Anything::get_U(unitName_t);
	unit_px = MyString2Anything::get_U(unitName_px);
	unit_py = MyString2Anything::get_U(unitName_py);
	unit_pz = MyString2Anything::get_U(unitName_pz);
	unit_e = MyString2Anything::get_U(unitName_e);
	unit_edep = MyString2Anything::get_U(unitName_edep);
	unit_stepL = MyString2Anything::get_U(unitName_stepL);
	unit_stop_time = MyString2Anything::get_U(unitName_stop_time);
	unit_kill_time = MyString2Anything::get_U(unitName_kill_time);
}

//-----------------------------------ShowOutCard----------------------------------------------
//Private
void MonitorSD::ShowOutCard(){
	std::cout<<"*************************Output settings for "<<get_VolName()<<"***************************"<<std::endl;
	//default output set
	std::cout<<"output nHits?   "<<(flag_nHits?" yes":" no")<<std::endl;
	std::cout<<"output x?       "<<(flag_x?" yes":" no")<<", unit: "<<unitName_x<<std::endl;
	std::cout<<"output y?       "<<(flag_y?" yes":" no")<<", unit: "<<unitName_y<<std::endl;
	std::cout<<"output z?       "<<(flag_z?" yes":" no")<<", unit: "<<unitName_z<<std::endl;
	std::cout<<"output t?       "<<(flag_t?" yes":" no")<<", unit: "<<unitName_t<<std::endl;
	std::cout<<"output px?      "<<(flag_px?" yes":" no")<<", unit: "<<unitName_px<<std::endl;
	std::cout<<"output py?      "<<(flag_py?" yes":" no")<<", unit: "<<unitName_py<<std::endl;
	std::cout<<"output pz?      "<<(flag_pz?" yes":" no")<<", unit: "<<unitName_pz<<std::endl;
	std::cout<<"output e?       "<<(flag_e?" yes":" no")<<", unit: "<<unitName_e<<std::endl;
	std::cout<<"output edep?    "<<(flag_edep?" yes":" no")<<", unit: "<<unitName_edep<<std::endl;
	std::cout<<"output stepL?   "<<(flag_stepL?" yes":" no")<<", unit: "<<unitName_stepL<<std::endl;
	std::cout<<"output volID?   "<<(flag_volID?" yes":" no")<<std::endl;
	std::cout<<"output volName? "<<(flag_volName?" yes":" no")<<std::endl;
	std::cout<<"output tid?     "<<(flag_tid?" yes":" no")<<std::endl;
	std::cout<<"output pid?     "<<(flag_pid?" yes":" no")<<std::endl;
	std::cout<<"output charge?  "<<(flag_charge?" yes":" no")<<std::endl;
	std::cout<<"output stopped? "<<(flag_stopped?" yes":" no")<<std::endl;
	std::cout<<"output stop_time?"<<(flag_stop_time?" yes":" no")<<", unit: "<<unitName_stop_time<<std::endl;
	std::cout<<"output killed?  "<<(flag_killed?" yes":" no")<<std::endl;
	std::cout<<"output kill_time?"<<(flag_kill_time?" yes":" no")<<", unit: "<<unitName_kill_time<<std::endl;
	//for fileter
	std::cout<<"Switch on?      "<<(Switch?"yes":"no")<<std::endl;
	std::cout<<"neutralCut on?  "<<(neutralCut?"yes":"no")<<std::endl;
	std::cout<<"minp =          "<<minp/MeV<<"MeV"<<std::endl;
	std::cout<<"maxn =          "<<maxn<<std::endl;
	std::cout<<"mint =          "<<mint/ns<<"ns"<<std::endl;
	std::cout<<"maxt =          "<<maxt/ns<<"ns"<<std::endl;
	std::cout<<"tres =          "<<tres/ns<<"ns"<<std::endl;
	std::cout<<"minedep =       "<<minedep/MeV<<"MeV"<<std::endl;
	std::cout<<"white list:     "<<std::endl;
	for ( int i = 0; i< white_list.size(); i++){
		std::cout <<"  Only tracks with these following PDGCodes will be recorded:"<<std::endl;
		std::cout<<"            "<<i<<": "<<white_list[i]<<std::endl;
	}
	if ( white_list.size() == 0 ){
		std::cout <<"  Empty! So all tracks will be recorded!"<<std::endl;
	}
	std::cout<<"******************************************************************************"<<std::endl;
}

//-----------------------------------ProcessHits----------------------------------------------
//Will be called by geant4 automatically everytime a step in MonitorSD generated
G4bool MonitorSD::ProcessHits(G4Step* aStep,G4TouchableHistory* touchableHistory)
{

	//*************************get useful variables***********************

	// get track info
	G4Track* gTrack = aStep->GetTrack() ;
	G4int trackID= gTrack->GetTrackID(); //G4 track ID of current track.
	G4int charge = gTrack->GetDefinition()->GetPDGCharge();
	G4int pid = gTrack->GetDefinition()->GetPDGEncoding();

	// get information at the beginning and at the end of step
	G4StepPoint* prePoint  = aStep->GetPreStepPoint() ;
	G4double total_e = prePoint->GetTotalEnergy();
	G4ThreeVector pointIn_mom = prePoint->GetMomentum();
	G4double pointIn_pa = pointIn_mom.mag();
	G4ThreeVector pointIn_pos = prePoint->GetPosition();
	G4double pointIn_time = prePoint->GetGlobalTime();//Time since the event in which the track belongs is created
	G4StepPoint* postPoint  = aStep->GetPostStepPoint() ;
	G4double pointOut_time = postPoint->GetGlobalTime();//Time since the event in which the track belongs is created

	// get volume info
	const G4VTouchable *touchable = prePoint->GetTouchable();
	G4int ReplicaNo = touchable->GetVolume(0)->GetCopyNo();
	G4String VolName = touchable->GetVolume(0)->GetName();

	// get step info
	G4double edep = aStep->GetTotalEnergyDeposit();
	G4double stepL = aStep->GetStepLength();

	//*************************filter***********************
	//switch
	if (!Switch) return false;

	//neutralCut
	if ( charge == 0 && neutralCut ) return false;

	//maxn
	if ( maxn && nHits >= maxn ) return false;

	// white_list
	bool foundit = false;
	for (int i = 0; i<white_list.size(); i++){
		if (pid == white_list[i]) foundit=true;
	}
	if (!foundit&&white_list.size()) return false;

	//minp
	if ( minp && pointIn_pa < minp ) return false;

	//time_window
	if(isnan(pointIn_time)){
		G4cout<<"MonitorSD:error, pointIn_time is nan "<<G4endl;
		return false;
	}
	if ( pointIn_time < mint && mint ) return false;
	if ( pointIn_time > maxt && maxt ) return false;

	//minedep
	if( edep <= minedep ) return false;

	//****************************Stopped or not****************************************
	//std::cout<<"tid("<<trackID
	//         <<"), pid("<<pid
	//         <<"), e = "<<total_e/MeV
	//         <<"MeV, edep = "<<edep/MeV
	//         <<"MeV, t_in = "<<pointIn_time/ns
	//         <<"ns, t_out = "<<pointOut_time/ns
	//         <<"ns"
	//         <<std::endl;
	G4TrackStatus fTrackStatus = gTrack->GetTrackStatus();
	//if (fTrackStatus == fAlive) std::cout<<"fAlive"<<std::endl;
	//else if (fTrackStatus == fStopButAlive) std::cout<<"fStopButAlive"<<std::endl;
	//else if (fTrackStatus == fStopAndKill) std::cout<<"fStopAndKill"<<std::endl;
	//else if (fTrackStatus == fKillTrackAndSecondaries) std::cout<<"fKillTrackAndSecondaries"<<std::endl;
	//else if (fTrackStatus == fSuspend) std::cout<<"fSuspend"<<std::endl;
	//else if (fTrackStatus == fPostponeToNextEvent) std::cout<<"fPostponeToNextEvent"<<std::endl;
	//else std::cout<<"What?!"<<std::endl;

	bool stopped = false;
	double stop_time = 0;
	bool killed = false;
	double kill_time = 0;
	if (fTrackStatus == fStopButAlive){
		stopped = true;
		stop_time = pointIn_time;
	}
	else if (fTrackStatus == fStopAndKill || fTrackStatus == fKillTrackAndSecondaries){
		killed = true;
		kill_time = pointIn_time;
	}
	else if (fTrackStatus == fKillTrackAndSecondaries) std::cout<<"fKillTrackAndSecondaries"<<std::endl;

	//****************************generate new hit or modify old one********************************
	bool willPush = true;
	//look for last hit in the same volume.
	G4int index = -1;
	G4double dt = 1e10*s;
	//std::cout<<"At first, dt = "<<dt/ns<<"ns"<<std::endl;
	//std::cout<<"pointIn_time = "<<pointIn_time/ns<<"ns"<<std::endl;
	for ( int i = 0; i < nHits; i++ ){
		if ( m_volName[i] == VolName && m_volID[i] == ReplicaNo ){
			//std::cout<<"m_t["<<i<<"] = "<<m_t[i]*unit_t/ns<<"ns"<<std::endl;
			G4double dtime = pointIn_time - m_t[i]*unit_t;
			if ( dtime < dt ){
				index = i;
				dt = dtime;
				//std::cout<<"dtime = "<<dtime/ns<<"ns < dt = "<<dt/ns<<"ns"<<std::endl;
			}
		}
	}
	//std::cout<<"At last, dt = "<<dt/ns<<"ns"<<std::endl;
	if ( index != -1 ){// found a previous hit in the same volume
		if ( fabs(dt) < tres ){// dt too small, will not push
			willPush = false;
		}
		if ( m_pid[index] == pid && prePoint->GetStepStatus() != fGeomBoundary ){ // If this particle was in this volume in last step, don't generate a new hit
			willPush = false;
		}
		//if ( m_tid[index] == trackID ){
		//	willPush = false;
		//}
	}

	if (willPush){
		MonitorHit* newHit = new MonitorHit();
		newHit->SetTrackID(trackID);
		newHit->SetVolID(ReplicaNo);
		newHit->SetEdep(edep);
		newHit->SetPos(pointIn_pos);
		newHit->SetGlobalT(pointIn_time);
		hitsCollection->insert(newHit);
		//Set for root objects
		if(flag_x) m_x.push_back(pointIn_pos.x()/unit_x);
		if(flag_y) m_y.push_back(pointIn_pos.y()/unit_y);
		if(flag_z) m_z.push_back(pointIn_pos.z()/unit_z);
		m_t.push_back(pointIn_time/unit_t);
		//std::cout<<"m_t = pointIn_time/"<<unitName_t<<" = pointIn_time/"<<unit_t/ns<<"ns = "<<pointIn_time/unit_t<<unitName_t<<std::endl;
		if(flag_px) m_px.push_back(pointIn_mom.x()/unit_px);
		if(flag_py) m_py.push_back(pointIn_mom.y()/unit_py);
		if(flag_pz) m_pz.push_back(pointIn_mom.z()/unit_pz);
		if(flag_e) m_e.push_back(total_e/unit_e);
		if(flag_edep) m_edep.push_back(edep/unit_edep);
		if(flag_stepL) m_stepL.push_back(stepL/unit_stepL);
		m_volID.push_back(ReplicaNo);
		m_volName.push_back(VolName);
		if(flag_tid) m_tid.push_back(trackID);
		if(flag_pid) m_pid.push_back(pid);
		if(flag_charge) m_charge.push_back(charge);
		if(flag_stopped) m_stopped.push_back(stopped); // always false unless stopped at the first step
		if(flag_killed) m_killed.push_back(killed); // always false unless killed at the first step
		if(flag_stop_time) m_stop_time.push_back(stop_time/unit_stop_time); // always 0 unless stopped at the first step
		if(flag_kill_time) m_kill_time.push_back(kill_time/unit_kill_time); // always 0 unless killed at the first step
		nHits++;
	}
	else {
		G4double edepTemp = (*hitsCollection)[index]->GetEdep();
		(*hitsCollection)[index]->SetEdep(edepTemp  + edep);
		if(flag_edep) m_edep[index] += edep/unit_edep;
		if(flag_stepL) m_stepL[index] += stepL/unit_stepL;
		if(flag_stop_time&&stopped) m_stop_time[index] = stop_time/unit_stop_time; // modify only if it got stopped at this step
		if(flag_kill_time&&killed) m_kill_time[index] = kill_time/unit_kill_time;// modify only if it got killed at this step
		if(flag_stopped&&stopped) m_stopped[index] = stopped;// modify only if it got stopped at this step
		if(flag_killed&&killed) m_killed[index] = killed;// modify only if it got killed at this step
	}
	return true;
}

//-----------------------------------EndOfEvent----------------------------------------------
//Will be called by geant4 automatically at the end of each event
void MonitorSD::EndOfEvent(G4HCofThisEvent*){
	if (verboseLevel>0) {
		//    hitsCollection->PrintAllHits();
		/*
		   G4int NbHits = hitsCollection->entries();
		   G4cout << "\n-------->Hits Collection: in this event they are " << NbHits
		   << " hits in the MONITOR chambers: " << G4endl;
		   for (G4int i=0;i<NbHits;i++) (*hitsCollection)[i]->Print();
		 */
	}
}
