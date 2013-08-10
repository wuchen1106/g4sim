//---------------------------------------------------------------------------
//Description: Sensitive detector definition for CDC
//Author: Yuan Ye(yuany@mail.ihep.ac.cn)
//Created: 22 July, 2012
//Modified: 29 Sep, 2012 by Wu Chen(wuchen@mail.ihep.ac.cn)
//Modified: 8 Oct, 2012 by Wu Chen(wuchen@mail.ihep.ac.cn)
//Modified: 21 Oct, 2012 by Wu Chen(wuchen@mail.ihep.ac.cn)
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
#include "G4VPhysicalVolume.hh"
#include "G4RotationMatrix.hh"

#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Point3D.h"

#include "TVector3.h"
#include "TRandom.h"

#include <iostream>

#include "CdcGeometryParameter.hh"
//#include "CdcDigitizer.hh"
#include "MyRoot.hh"
#include "MySD.hh"
#include "MyDetectorManager.hh"
#include "MyVGeometrySvc.hh"
#include "MyString2Anything.hh"

#include "KillerSD.hh"

#include "DEBUG.hh"

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
	//initialize for root objects
	// nHits = 0;
	//initialize for filter
}

//-----------------------------------SetBranch----------------------------------------------
//Should be called at the beginning of each run (now in MyAnalysisSvc::BeginOfRunAction())
void KillerSD::SetBranch(){
	//MyRoot* myRoot = MyRoot::GetMyRoot();
	//G4String volName = get_VolName();
	//if( flag_nHits ) myRoot->SetBranch(volName+"_nHits", &nHits);
}

//-----------------------------------ReadOutputCard----------------------------------------------
//Should be called before the begin of each run (now in the constructor of MyAnalysisSvc)
void KillerSD::ReadOutputCard(G4String filename){
//	ReSet();
//	G4String volName = get_VolName();
//	std::ifstream fin_card(filename);
//	if(!fin_card){
//		std::cout<<"In KillerSD::ReadOutputCard, cannot open "<<filename<<"!!!"<<std::endl;
//		G4Exception("KillerSD::ReadOutputCard()",
//				"InvalidSetup", FatalException,
//				"cannot find output card");
//	}
//	std::stringstream buf_card;
//	std::string s_card;
//	int n_output_section_symbol = 0;
//	int n_filter_section_symbol = 0;
//	G4String BRANCHSECTIONNAME = volName + "_SECTION";
//	G4String FILTERSECTIONNAME = volName + "_FILTERSECTION";
//	while(getline(fin_card,s_card)){
//		buf_card.str("");
//		buf_card.clear();
//		buf_card<<s_card;
//
//		//eleminate useless lines
//		const char* c_card = s_card.c_str();
//		int length = strlen(c_card);
//		int offset = 0;
//		for ( ; offset < length; offset++ ){
//			if ( c_card[offset] != ' ' ) break;
//		}
//		if ( c_card[offset] == '#' || (c_card[offset] == '/' && c_card[offset+1] == '/') || length - offset == 0 ){
//			continue;
//		}
//
//		std::string name;
//		buf_card>>name;
//		if ( n_output_section_symbol == 0 ){
//			if ( name == BRANCHSECTIONNAME ){
//				n_output_section_symbol++;
//			}
//		}
//		else if ( n_output_section_symbol == 1 ){
//			if ( name == BRANCHSECTIONNAME ){
//				n_output_section_symbol++;
//			}
//			else if( name == "nHits" ) flag_nHits = true;
//			else{
//				std::cout<<"In KillerSD::ReadOutputCard, unknown name: "<<name<<" in file "<<filename<<std::endl;
//				std::cout<<"Will ignore this line!"<<std::endl;
//			}
//		}
//
//		if ( n_filter_section_symbol == 0 ){
//			if ( name == FILTERSECTIONNAME ){
//				n_filter_section_symbol++;
//			}
//		}
//		else if ( n_filter_section_symbol == 1 ){
//			if ( name == FILTERSECTIONNAME ){
//				n_filter_section_symbol++;
//			}
//			else if( name == "Switch" ) Switch = true;
//			else if( name == "neutralCut" ) neutralCut = true;
//			else{
//				G4double para;
//				std::string unit;
//				buf_card>>para>>unit;
//				para *= MyString2Anything::get_U(unit);
//				if( name == "minp" ) minp = para;
//				else if( name == "mint" ) mint = para;
//				else if( name == "maxt" ) maxt = para;
//				else{
//					std::cout<<"In KillerSD::ReadOutputCard, unknown name: "<<name<<" in file "<<filename<<std::endl;
//					std::cout<<"Will ignore this line!"<<std::endl;
//				}
//			}
//		}
//
//		if ( n_output_section_symbol > 1 && n_filter_section_symbol > 1 ){
//			break;
//		}
//	}
//	buf_card.str("");
//	buf_card.clear();
//	if ( n_output_section_symbol <= 1 ){
//		std::cout<<"*****************WARNING********************"<<std::endl;
//		std::cout<<"In KillerSD::ReadOutputCard, failed to find enough section seperators for output in file "<<filename<<std::endl;
//		std::cout<<"Will use default settings."<<std::endl;
//		std::cout<<"********************************************"<<std::endl;
//	}
//	if ( n_filter_section_symbol<= 1 ){
//		std::cout<<"*****************WARNING********************"<<std::endl;
//		std::cout<<"In KillerSD::ReadOutputCard, failed to find enough section seperators for filter in file "<<filename<<std::endl;
//		std::cout<<"Will use default settings."<<std::endl;
//		std::cout<<"********************************************"<<std::endl;
//	}
//	fin_card.close();
//	ShowOutCard();
}

//-----------------------------------ReSet----------------------------------------------
//Private
void KillerSD::ReSet(){
	//default output set
//	flag_nHits = false;
	//for fileter
//	Switch = false;
//	neutralCut = false;
//	minp = 1.5*MeV;
//	mint = 0;
//	maxt = 0;
}

//-----------------------------------ShowOutCard----------------------------------------------
//Private
void KillerSD::ShowOutCard(){
	std::cout<<"*************************Output settings for "<<get_VolName()<<"***************************"<<std::endl;
	//default output set
	std::cout<<"This is Killer!"<<std::endl;
//	std::cout<<"output nHits?  "<<(flag_nHits?" yes":" no")<<std::endl;
	//for fileter
//	std::cout<<"Switch on?     "<<(Switch?" yes":" no")<<std::endl;
//	std::cout<<"neutralCut on? "<<(neutralCut?"yes":"no")<<std::endl;
//	std::cout<<"minp =         "<<minp/MeV<<"MeV"<<std::endl;
//	std::cout<<"mint =         "<<mint/ns<<"ns"<<std::endl;
//	std::cout<<"maxt =         "<<maxt/ns<<"ns"<<std::endl;
//	std::cout<<"VerboseLevel = "<<get_VerboseLevel()<<std::endl;
	std::cout<<"******************************************************************************"<<std::endl;
}

//-----------------------------------ProcessHits----------------------------------------------
//Will be called by geant4 automatically everytime a step in KillerSD generated
G4bool KillerSD::ProcessHits(G4Step* aStep,G4TouchableHistory* touchableHistory)
{
	G4Track* aTrack = aStep->GetTrack() ;
	aTrack->SetTrackStatus(fStopAndKill);
	return true;
}

//-----------------------------------EndOfEvent----------------------------------------------
//Will be called by geant4 automatically at the end of each event
void KillerSD::EndOfEvent(G4HCofThisEvent*){
}
