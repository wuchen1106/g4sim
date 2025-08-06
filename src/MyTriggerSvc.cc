//---------------------------------------------------------------------------//
//Description: Setup MyTrigger
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "MyTriggerSvc.hh"

#include "G4UnitsTable.hh"
#include "myglobals.hh"
#include "G4Event.hh"

#include "MyDetectorManager.hh"
#include "CdcLayerSD.hh"
#include "CdcSD.hh"
#include "CdcSimpleSD.hh"
#include "CdcCellSD.hh"
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
:pMyDetectorManager(0),myCdcLayerSD(0),myCdcSD(0),myCdcSimpleSD(0),myCdcCellSD(0),myMonitorSD(0),myMonitorSD2(0),myMcTruthSvc(0),myKillerSD(0)
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
			else if ( name == "minM_Hits" ){
				minM_Hits = para;
			}
			else if ( name == "minV_Hits" ){
				minV_Hits = para;
			}
			else if ( name == "minCorM_Hits" ){
				minCorM_Hits = para;
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
			else if ( name == "minVorTracks" ){
				minVorTracks = para;
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
	G4VSensitiveDetector* myVSD2 = 0;
	if ( minM_Hits != -1 ){ 
		myVSD = pMyDetectorManager->GetSD("","M/MonitorSD");
		if (myVSD){
			myMonitorSD = dynamic_cast<MonitorSD*> (myVSD);
			std::cout<<"myVSD @ ["<<(void*) myVSD<<"]"<<std::endl;
			std::cout<<"myMonitorSD @ ["<<(void*) myMonitorSD<<"]"<<std::endl;
		}
	}
	if ( minV_Hits != -1 ){ 
		myVSD = pMyDetectorManager->GetSD("","V/MonitorSD");
		if (myVSD){
			myMonitorSD = dynamic_cast<MonitorSD*> (myVSD);
			std::cout<<"myVSD @ ["<<(void*) myVSD<<"]"<<std::endl;
			std::cout<<"myMonitorSD @ ["<<(void*) myMonitorSD<<"]"<<std::endl;
		}
	}
	if ( minCorM_Hits != -1 ){ 
		myVSD2 = pMyDetectorManager->GetSD("","M/MonitorSD");
		if (myVSD2){
			myMonitorSD2 = dynamic_cast<MonitorSD*> (myVSD2);
			std::cout<<"myVSD2 @ ["<<(void*) myVSD2<<"]"<<std::endl;
			std::cout<<"myMonitorSD2 @ ["<<(void*) myMonitorSD2<<"]"<<std::endl;
		}
		else{
			std::cout<<"Cannot find M sensitive detector!"<<std::endl;
		}
	}
	if ( minCdcHits != -1 || minCorM_Hits != -1 ){ 
		myVSD = pMyDetectorManager->GetSD("CdcCell","CdcSD");
		if (myVSD){
			myCdcSD = dynamic_cast<CdcSD*> (myVSD);
			std::cout<<"myVSD @ ["<<(void*) myVSD<<"]"<<std::endl;
			std::cout<<"myCdcSD @ ["<<(void*) myCdcSD<<"]"<<std::endl;
		}
		else{
			myVSD = pMyDetectorManager->GetSD("CDCContainer","CdcSD");
			if (myVSD){
				myCdcSD = dynamic_cast<CdcSD*> (myVSD);
				std::cout<<"myVSD @ ["<<(void*) myVSD<<"]"<<std::endl;
				std::cout<<"myCdcSD @ ["<<(void*) myCdcSD<<"]"<<std::endl;
			}
			else{
				myVSD = pMyDetectorManager->GetSD("CDCLayer","CdcLayerSD");
				if (myVSD){
					myCdcLayerSD = dynamic_cast<CdcLayerSD*> (myVSD);
					std::cout<<"myVSD @ ["<<(void*) myVSD<<"]"<<std::endl;
					std::cout<<"myCdcLayerSD @ ["<<(void*) myCdcLayerSD<<"]"<<std::endl;
				}
				else{
					myVSD = pMyDetectorManager->GetSD("","C/MonitorSD");
					if (myVSD){
						myMonitorSD = dynamic_cast<MonitorSD*> (myVSD);
						std::cout<<"myVSD @ ["<<(void*) myVSD<<"]"<<std::endl;
						std::cout<<"myMonitorSD @ ["<<(void*) myMonitorSD<<"]"<<std::endl;
					}
					else{
						std::cout<<"Cannot find cdc sensitive detector!"<<std::endl;
					}
				}
			}
		}
	}
	if ( minEleMom != -1*MeV ){
		myMcTruthSvc = McTruthSvc::GetMcTruthSvc();
	}
	if ( minAntipNum != -1 || minTracks != -1 || minVorTracks !=-1 ){
		myMcTruthSvc = McTruthSvc::GetMcTruthSvc();
	}
}

bool MyTriggerSvc::TriggerIt( const G4Event* evt ){

	if ( minCdcHits != -1 ){
		int nHits_CDC = 0;
		if (myCdcLayerSD){
			nHits_CDC = myCdcLayerSD->Get_nHits();
		}
		else if (myCdcSD){
			nHits_CDC = myCdcSD->Get_nHits();
		}
		else if (myCdcSimpleSD){
			nHits_CDC = myCdcSimpleSD->Get_nHits();
		}
		else if (myCdcCellSD){
			nHits_CDC = myCdcCellSD->Get_nHits();
		}
		else if (myMonitorSD){
			nHits_CDC = myMonitorSD->Get_nHits();
		}
		if ( nHits_CDC < minCdcHits) return false;
	}
	if ( minM_Hits != -1 ){
		int nHits = 0;
		if (myMonitorSD){
			nHits = myMonitorSD->Get_nHits();
		}
		if ( nHits < minM_Hits ) return false;
	}
	if ( minV_Hits != -1 ){
		int nHits = 0;
		if (myMonitorSD){
			nHits = myMonitorSD->Get_nHits();
		}
		if ( nHits < minV_Hits ) return false;
	}
	if ( minCorM_Hits != -1 ){
		int nHits = 0;
		if (myMonitorSD2){
			nHits = myMonitorSD2->Get_nHits();
		}
		int nHits_CDC = 0;
		if (myCdcLayerSD){
			nHits_CDC = myCdcLayerSD->Get_nHits();
		}
		else if (myCdcSD){
			nHits_CDC = myCdcSD->Get_nHits();
		}
		else if (myCdcSimpleSD){
			nHits_CDC = myCdcSimpleSD->Get_nHits();
		}
		else if (myCdcCellSD){
			nHits_CDC = myCdcCellSD->Get_nHits();
		}
		else if (myMonitorSD){
			nHits_CDC = myMonitorSD->Get_nHits();
		}
		if ( nHits+nHits_CDC < minCorM_Hits ) return false;
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
	if ( minVorTracks!= -1 ){
		int nTracks = myMcTruthSvc->get_nTracks();
		int nHits = 0;
		if (myMonitorSD){
			nHits = myMonitorSD->Get_nHits();
		}
		if (nTracks<minVorTracks&&nHits<minVorTracks) return false;
	}
	//std::cout<<"Passed Cut!"<<std::endl;

	return true;
}

void MyTriggerSvc::ReSet(){
	minCdcHits = -1;
	minM_Hits = -1;
	minV_Hits = -1;
	minCorM_Hits = -1;
	minEleMom = -1*MeV;
	minAntipNum = -1;
	minTracks = -1;
	minVorTracks = -1;
}

void MyTriggerSvc::ShowOutCard(){
	std::cout<<"*************************Trigger settings"<<"***************************"<<std::endl;
	std::cout<<"minCdcHits =     "<<minCdcHits<<std::endl;
	std::cout<<"minM_Hits = "<<minM_Hits<<std::endl;
	std::cout<<"minV_Hits = "<<minV_Hits<<std::endl;
	std::cout<<"minCorM_Hits = "<<minCorM_Hits<<std::endl;
	std::cout<<"minEleMom=       "<<minEleMom/MeV<<", MeV"<<std::endl;
	std::cout<<"minAntipNum=     "<<minAntipNum<<std::endl;
	std::cout<<"minTracks =      "<<minTracks<<std::endl;
	std::cout<<"minVorTracks =      "<<minVorTracks<<std::endl;
	std::cout<<"******************************************************************************"<<std::endl;
}
