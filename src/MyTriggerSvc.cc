//---------------------------------------------------------------------------//
//Description: Setup MyTrigger
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "MyTriggerSvc.hh"

#include "G4UnitsTable.hh"
#include "globals.hh"
#include "G4Event.hh"

#include "MyDetectorManager.hh"
#include "CdcLayerSD.hh"
#include "CdcSD.hh"
#include "CdcSimpleSD.hh"
#include "MonitorSD.hh"
#include "KillerSD.hh"
#include "MyString2Anything.hh"
#include "McTruthSvc.hh"

#include <sstream>
#include <fstream>
#include <string.h>
#include <iostream>

#include "DEBUG.hh"

MyTriggerSvc* MyTriggerSvc::fMyTriggerSvc = 0;

MyTriggerSvc::MyTriggerSvc()
 :pMyDetectorManager(0),myCdcLayerSD(0),myCdcSD(0),myCdcSimpleSD(0),myMonitorSD(0),myTriggerSD(0),myMcTruthSvc(0),myKillerSD(0)
{
  if (fMyTriggerSvc){
    G4Exception("MyTriggerSvc::MyTriggerSvc()","Run0031",
        FatalException, "MyTriggerSvc constructed twice.");
  }
  fMyTriggerSvc = this;
}

MyTriggerSvc::~MyTriggerSvc()
{
  printf("~MyTriggerSvc\n");
}

MyTriggerSvc* MyTriggerSvc::GetMyTriggerSvc(){
  if ( !fMyTriggerSvc ){
    fMyTriggerSvc = new MyTriggerSvc;
  }
  return fMyTriggerSvc;
}

void MyTriggerSvc::SetMyTrigger( G4String filename ){
	ReSet();
	std::ifstream fin_card(filename);
	if(!fin_card){
		std::cout<<"In MyTriggerSvc::ReadOutputCard, cannot open "<<filename<<"!!!"<<std::endl;
    G4Exception("MyTriggerSvc::ReadOutputCard()",
        "InvalidSetup", FatalException,
        "cannot find output card");
	}
	std::stringstream buf_card;
	G4String s_card;
	int n_TRIGGER_section_symbol = 0;
	G4String TRIGGERSECTIONNAME = "TRIGGERSECTION";
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

		if ( n_TRIGGER_section_symbol == 0 ){
			if ( name == TRIGGERSECTIONNAME ){
				n_TRIGGER_section_symbol++;
			}
		}
		else if ( n_TRIGGER_section_symbol == 1 ){
			int para;
			buf_card>>para;
			if ( name == TRIGGERSECTIONNAME ){
				n_TRIGGER_section_symbol++;
			}
			else if ( name == "minCdcHits" ){
				minCdcHits = para;
			}
			else if ( name == "minCdcCellHits" ){
				minCdcCellHits = para;
			}
			else if ( name == "minTriggerHits" ){
				minTriggerHits = para;
			}
			else if ( name == "minEleMom" ){
				std::string unit;
				buf_card>>unit;
				minEleMom = para*MyString2Anything::get_U(unit);
			}
			else if ( name == "minAntipNum" ){
				minAntipNum = para;
			}
			else if ( name == "minTracks" ){
				minTracks = para;
			}
			else{
				std::cout<<"In MyTriggerSvc::SetMyTrigger, unknown name: "<<name<<" in file "<<filename<<std::endl;
				std::cout<<"Will ignore this line!"<<std::endl;
			}
		}

		if ( n_TRIGGER_section_symbol > 1 ){
			break;
		}
	}
	buf_card.str("");
	buf_card.clear();
	if ( n_TRIGGER_section_symbol<= 1 ){
	  std::cout<<"*****************WARNING********************"<<std::endl;
		std::cout<<"In MyTriggerSvc::ReadOutputCard, failed to find enough section seperators \""<<TRIGGERSECTIONNAME<<"\" for TRIGGER in file "<<filename<<std::endl;
		std::cout<<"Will use default settings."<<std::endl;
		std::cout<<"********************************************"<<std::endl;
	}
	fin_card.close();
	ShowOutCard();
  pMyDetectorManager = MyDetectorManager::GetMyDetectorManager();
	G4VSensitiveDetector* myVSD = 0;
	if ( minCdcHits != -1 ){ 
		myVSD = pMyDetectorManager->GetSD("CDCLayer","CdcLayerSD");
		if (!myVSD){
			myVSD = pMyDetectorManager->GetSD("CDCLayer","C/MonitorSD");
			myMonitorSD = dynamic_cast<MonitorSD*> (myVSD);
			std::cout<<"myVSD @ ["<<(void*) myVSD<<"]"<<std::endl;
			std::cout<<"myMonitorSD @ ["<<(void*) myMonitorSD<<"]"<<std::endl;
		}
		else{
			myCdcLayerSD = dynamic_cast<CdcLayerSD*> (myVSD);
			std::cout<<"myVSD @ ["<<(void*) myVSD<<"]"<<std::endl;
			std::cout<<"myCdcLayerSD @ ["<<(void*) myCdcLayerSD<<"]"<<std::endl;
		}
	}
	if ( minCdcCellHits != -1 ){ 
		myVSD = pMyDetectorManager->GetSD("CdcCell","CdcSD");
		if (!myVSD){
			myVSD = pMyDetectorManager->GetSD("CdcCell","C/MonitorSD");
			myMonitorSD = dynamic_cast<MonitorSD*> (myVSD);
			std::cout<<"myVSD @ ["<<(void*) myVSD<<"]"<<std::endl;
			std::cout<<"myMonitorSD @ ["<<(void*) myMonitorSD<<"]"<<std::endl;
		}
		else{
			myCdcSD = dynamic_cast<CdcSD*> (myVSD);
			std::cout<<"myVSD @ ["<<(void*) myVSD<<"]"<<std::endl;
			std::cout<<"myCdcSD @ ["<<(void*) myCdcSD<<"]"<<std::endl;
		}
	}
	if ( minTriggerHits != -1 ){ 
		myVSD = pMyDetectorManager->GetSD("Trigger","MonitorSD");
		std::cout<<"myVSD @ ["<<(void*) myVSD<<"]"<<std::endl;
		myTriggerSD = dynamic_cast<MonitorSD*> (myVSD);
		std::cout<<"myTriggerSD @ ["<<(void*)myTriggerSD<<"]"<<std::endl;
	}
	if ( minEleMom != -1*MeV ){
		myMcTruthSvc = McTruthSvc::GetMcTruthSvc();
	}
	if ( minAntipNum != -1 || minTracks != -1 ){
		myMcTruthSvc = McTruthSvc::GetMcTruthSvc();
	}
}

bool MyTriggerSvc::TriggerIt( const G4Event* evt ){

	if ( minCdcHits != -1 ){
		int nHits_CDC = 0;
		if (myCdcLayerSD){
			nHits_CDC = myCdcLayerSD->Get_nHits();
		}
		else{
			nHits_CDC = myMonitorSD->Get_nHits();
		}
		if ( nHits_CDC < minCdcHits ) return false;
	}
	if ( minCdcCellHits != -1 ){
		int nHits_CDC = 0;
		if (myCdcSD){
			nHits_CDC = myCdcSD->Get_nHits();
		}
		else{
			nHits_CDC = myMonitorSD->Get_nHits();
		}
		MYTRI_LINEVAR( nHits_CDC )
		if ( nHits_CDC < minCdcCellHits ) return false;
	}
	if ( minTriggerHits != -1 ){
		int nHits_Trigger = myTriggerSD->Get_nHits();
		if ( nHits_Trigger < minTriggerHits ) return false;
	}
	if ( minEleMom != -1*MeV ){
		bool foundit = false;
		int nTracks = myMcTruthSvc->get_nTracks();
		for ( int i = 0; i < nTracks; i++ ){
			int pid = myMcTruthSvc->get_pid(i);
			double px = myMcTruthSvc->get_px(i)*GeV;
			double py = myMcTruthSvc->get_py(i)*GeV;
			double pz = myMcTruthSvc->get_pz(i)*GeV;
			double pa = sqrt(px*px+py*py+pz*pz);
			MYTRI_LINEVAR( pa )
			MYTRI_LINEVAR( pid )
			if ( pid == 11 && pa > minEleMom ) foundit = true;
		}
		if (!foundit) return false;
	}
	if ( minAntipNum != -1 ){
		bool foundit = false;
		int nTracks = myMcTruthSvc->get_nTracks();
		for ( int i = 0; i < nTracks; i++ ){
			int pid = myMcTruthSvc->get_pid(i);
			if ( pid == -2212 ) foundit = true;
		}
		if (!foundit) return false;
	}
	if ( minTracks!= -1 ){
		int nTracks = myMcTruthSvc->get_nTracks();
		if (nTracks<minTracks) return false;
	}
	//std::cout<<"Passed Cut!"<<std::endl;
	MYTRI_LINECONT( "Passed Cut!" )

	return true;
}

void MyTriggerSvc::ReSet(){
	minCdcHits = -1;
	minCdcCellHits = -1;
	minTriggerHits = -1;
	minEleMom = -1*MeV;
	minAntipNum = -1;
	minTracks = -1;
}

void MyTriggerSvc::ShowOutCard(){
	std::cout<<"*************************Trigger settings"<<"***************************"<<std::endl;
	std::cout<<"minTriggerHits = "<<minTriggerHits<<std::endl;
	std::cout<<"minCdcHits =     "<<minCdcHits<<std::endl;
	std::cout<<"minCdcCellHits = "<<minCdcCellHits<<std::endl;
	std::cout<<"minEleMom=       "<<minEleMom/MeV<<", MeV"<<std::endl;
	std::cout<<"minAntipNum=     "<<minAntipNum<<std::endl;
	std::cout<<"minTracks =      "<<minTracks<<std::endl;
	std::cout<<"******************************************************************************"<<std::endl;
}
