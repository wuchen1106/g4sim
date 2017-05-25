//---------------------------------------------------------------------------
//Description: Sensitive detector definition for Monitor
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "myglobals.hh"
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
#include "MyRoot.hh"
#include "MySD.hh"

#include "KillerSD.hh"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
typedef HepGeom::Vector3D<double> HepVector3D;
#endif

	KillerSD::KillerSD(G4String name, MyVGeometryParameter* pointer)
:MySD(name, pointer)
{
}

KillerSD::~KillerSD(){
}

//-----------------------------------Initialize----------------------------------------------
//Will be called by geant4 automatically at the beginning of each event
void KillerSD::Initialize(G4HCofThisEvent* HCE)
{
}

//-----------------------------------SetBranch----------------------------------------------
//Should be called at the beginning of each run (now in MyAnalysisSvc::BeginOfRunAction())
void KillerSD::SetBranch(){
}

//-----------------------------------ReadOutputCard----------------------------------------------
//Should be called before the begin of each run (now in the constructor of MyAnalysisSvc)
void KillerSD::ReadOutputCard(G4String filename){
	ReSet();
	std::ifstream fin_card(filename);
	if(!fin_card){
		std::cout<<"In KillerSD::ReadOutputCard, cannot open "<<filename<<"!!!"<<std::endl;
		G4Exception("KillerSD::ReadOutputCard()",
				"InvalidSetup", FatalException,
				"cannot find output card");
	}
	std::stringstream buf_card;
	G4String s_card;
	G4String volName = get_VolName();
	int n_filter_section_symbol = 0;
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
			else if( name == "BL" ){
				int pid = 0;
				buf_card>>pid;
				black_list.push_back(pid);
			}
			else{
				G4double para;
				G4String unit;
				buf_card>>para>>unit;
				para *= MyString2Anything::get_U(unit);
				if( name == "minp" ) minp = para;
				else if( name == "mine" ) mine = para;
				else if ( name == "minedep" ) minedep = para;
				else if( name == "mint" ) mint = para;
				else if( name == "maxt" ) maxt = para;
				else if( name == "tres" ) tres = para;
				else{
					std::cout<<"In KillerSD::ReadOutputCard, unknown name: "<<name<<" in file "<<filename<<std::endl;
					std::cout<<"Will ignore this line!"<<std::endl;
				}
			}
		}

		if ( n_filter_section_symbol > 1 ){
			break;
		}
	}
	buf_card.str("");
	buf_card.clear();
	if ( n_filter_section_symbol<= 1 ){
		std::cout<<"*****************WARNING********************"<<std::endl;
		std::cout<<"In KillerSD::ReadOutputCard, failed to find enough section seperators \""<<FILTERSECTIONNAME<<"\" for filter in file "<<filename<<std::endl;
		std::cout<<"Will use default settings."<<std::endl;
		std::cout<<"********************************************"<<std::endl;
	}
	fin_card.close();
	ShowOutCard();
}

//-----------------------------------ReSet----------------------------------------------
//Private
void KillerSD::ReSet(){
	//for fileter
	Switch = true;
	neutralCut = false;
	minp = 0;
	mine = 0;
	maxn = 0;
	mint = 0;
	maxt = 0;
	tres = 0;
	minedep = -1*MeV;
	white_list.clear();
	black_list.clear();
}

//-----------------------------------ShowOutCard----------------------------------------------
//Private
void KillerSD::ShowOutCard(){
	std::cout<<"*************************Output settings for "<<get_VolName()<<"***************************"<<std::endl;
	//for fileter
	std::cout<<"Switch on?      "<<(Switch?"yes":"no")<<std::endl;
	std::cout<<"neutralCut on?  "<<(neutralCut?"yes":"no")<<std::endl;
	std::cout<<"minp =          "<<minp/MeV<<"MeV"<<std::endl;
	std::cout<<"mine =          "<<mine/MeV<<"MeV"<<std::endl;
	std::cout<<"maxn =          "<<maxn<<std::endl;
	std::cout<<"mint =          "<<mint/ns<<"ns"<<std::endl;
	std::cout<<"maxt =          "<<maxt/ns<<"ns"<<std::endl;
	std::cout<<"tres =          "<<tres/ns<<"ns"<<std::endl;
	std::cout<<"minedep =       "<<minedep/MeV<<"MeV"<<std::endl;
	std::cout<<"white list:     "<<std::endl;
	for ( int i = 0; i< white_list.size(); i++){
		std::cout <<"  Only tracks with these following PDGCodes will be killed:"<<std::endl;
		std::cout<<"            "<<i<<": "<<white_list[i]<<std::endl;
	}
	if ( white_list.size() == 0 ){
		std::cout <<"  Empty! So all tracks will be killed!"<<std::endl;
	}
	std::cout<<"black list:     "<<std::endl;
	for ( int i = 0; i< black_list.size(); i++){
		std::cout <<"  Tracks with these following PDGCodes will NOT be killed:"<<std::endl;
		std::cout<<"            "<<i<<": "<<black_list[i]<<std::endl;
	}
	std::cout<<"******************************************************************************"<<std::endl;
}

//-----------------------------------ProcessHits----------------------------------------------
//Will be called by geant4 automatically everytime a step in KillerSD generated
G4bool KillerSD::ProcessHits(G4Step* aStep,G4TouchableHistory* touchableHistory)
{

	//*************************get useful variables***********************

	// get track info
	G4Track* aTrack = aStep->GetTrack() ;
	G4int trackID= aTrack->GetTrackID(); //G4 track ID of current track.
	G4int charge = aTrack->GetDefinition()->GetPDGCharge();
	G4int pid = aTrack->GetDefinition()->GetPDGEncoding();

	// get information at the beginning and at the end of step
	G4StepPoint* prePoint  = aStep->GetPreStepPoint() ;
	G4double total_e = prePoint->GetTotalEnergy();
	G4ThreeVector pointIn_mom = prePoint->GetMomentum();
	G4double pointIn_pa = pointIn_mom.mag();
	G4ThreeVector pointIn_pos = prePoint->GetPosition();
	G4double pointIn_time = prePoint->GetGlobalTime();//Time since the event in which the track belongs is created
	G4StepPoint* postPoint  = aStep->GetPostStepPoint() ;
	G4double pointOut_time = postPoint->GetGlobalTime();//Time since the event in which the track belongs is created
	G4ThreeVector pointOut_mom = postPoint->GetMomentum();
	G4double pointOut_pa = pointOut_mom.mag();
	G4ThreeVector pointOut_pos = postPoint->GetPosition();

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
//	if ( maxn && nHits >= maxn ) return false;

	// white_list
	bool foundit = false;
	for (int i = 0; i<white_list.size(); i++){
		if (pid == white_list[i]) foundit=true;
		if (white_list[i]==0&&pid<1e7) foundit = true;
		if (white_list[i]==-1&&trackID==1) foundit = true;
	}
	if (!foundit&&white_list.size()) return false;
	// black_list
	foundit = false;
	for (int i = 0; i<black_list.size(); i++){
		if (pid == black_list[i]) foundit=true;
		if (black_list[i]==0&&pid<1e7) foundit = true;
		if (black_list[i]==-1&&trackID==1) foundit = true;
	}
	if (foundit) return false;

	//minp
	if ( minp && pointOut_pa < minp ) return false;
	//energy
	if (mine&&aTrack->GetTotalEnergy()<mine) return false;

	//time_window
	if(std::isnan(pointOut_time)){
		G4cout<<"KillerSD:error, pointOut_time is nan "<<G4endl;
		return false;
	}
	if ( pointOut_time < mint && mint ) return false;
	if ( pointOut_time > maxt && maxt ) return false;

	//minedep
	if( edep <= minedep ) return false;

	aTrack->SetTrackStatus(fStopAndKill);
	return true;
}

//-----------------------------------EndOfEvent----------------------------------------------
//Will be called by geant4 automatically at the end of each event
void KillerSD::EndOfEvent(G4HCofThisEvent*){
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
