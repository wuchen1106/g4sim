//---------------------------------------------------------------------------//
//Description: In charge of EventHeader
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "EventHeaderSvc.hh"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "globals.hh"
#include "G4Event.hh"
#include "Randomize.hh"

#include "MyRoot.hh"

EventHeaderSvc* EventHeaderSvc::fEventHeaderSvc = 0;

EventHeaderSvc::EventHeaderSvc()
{
	if (fEventHeaderSvc){
		G4Exception("EventHeaderSvc::EventHeaderSvc()","Run0031",
				FatalException, "EventHeaderSvc constructed twice.");
	}
	fEventHeaderSvc = this;
	R0 = 0;
	R1 = 0;
	ipx = 0;
	ipy = 0;
	ipz = 0;
}

EventHeaderSvc::~EventHeaderSvc()
{
	printf("~EventHeaderSvc\n");
}

EventHeaderSvc* EventHeaderSvc::GetEventHeaderSvc(){
	if ( !fEventHeaderSvc ){
		fEventHeaderSvc = new EventHeaderSvc;
	}
	return fEventHeaderSvc;
}

void EventHeaderSvc::SetBranch(){
	MyRoot::GetMyRoot()->SetBranch("run_num", &run_num);
	MyRoot::GetMyRoot()->SetBranch("evt_num", &evt_num);
	MyRoot::GetMyRoot()->SetBranch("R0", &R0);
	MyRoot::GetMyRoot()->SetBranch("R1", &R1);
	MyRoot::GetMyRoot()->SetBranch("ipx", &ipx);
	MyRoot::GetMyRoot()->SetBranch("ipy", &ipy);
	MyRoot::GetMyRoot()->SetBranch("ipz", &ipz);
	MyRoot::GetMyRoot()->SetBranch("weight", &weight);
}

void EventHeaderSvc::ReadOutputCard(G4String filename){
	ReSet();
	std::ifstream fin_card(filename);
	if(!fin_card){
		std::cout<<"In EventHeaderSvc::ReadOutputCard, cannot open "<<filename<<"!!!"<<std::endl;
		G4Exception("EventHeaderSvc::ReadOutputCard()",
				"InvalidSetup", FatalException,
				"cannot find output card");
	}
	std::stringstream buf_card;
	std::string s_card;
	int n_output_section_symbol = 0;
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
		if ( n_output_section_symbol == 0 ){
			if ( name == "EVENTHEADER_SECTION" ){
				n_output_section_symbol++;
			}
		}
		else if ( n_output_section_symbol == 1 ){
			if ( name == "EVENTHEADER_SECTION" ){
				n_output_section_symbol++;
			}
			else if( name == "evt_num" ) flag_evt_num = true;
			else if( name == "run_num" ) flag_run_num = true;
			else if( name == "R0" ) flag_R0 = true;
			else if( name == "R1" ) flag_R1 = true;
			else if( name == "ipx" ) flag_ipx = true;
			else if( name == "ipy" ) flag_ipy = true;
			else if( name == "ipz" ) flag_ipz = true;
			else if( name == "weight" ) flag_weight = true;
			else{
				std::cout<<"In EventHeaderSvc::ReadOutputCard, unknown name: "<<name<<" in file "<<filename<<std::endl;
				std::cout<<"Will ignore this line!"<<std::endl;
			}
		}

		if ( n_output_section_symbol > 1 ){
			break;
		}
	}
	buf_card.str("");
	buf_card.clear();
	if ( n_output_section_symbol <= 1 ){
		std::cout<<"*****************WARNING********************"<<std::endl;
		std::cout<<"In EventHeaderSvc::ReadOutputCard, failed to find enough section seperators for output in file "<<filename<<std::endl;
		std::cout<<"Will use default settings."<<std::endl;
		std::cout<<"********************************************"<<std::endl;
	}
	fin_card.close();
	ShowOutCard();
}

void EventHeaderSvc::ReSet(){
	flag_evt_num = false;
	flag_run_num = false;
	flag_R0 = false;
	flag_R1 = false;
	flag_R1 = false;
}

void EventHeaderSvc::ShowOutCard(){
	std::cout<<"*************************Output settings for EventHeaderSvc***************************"<<std::endl;
	std::cout<<"output evt_num?"<<(flag_evt_num?" yes":" no")<<std::endl;
	std::cout<<"output run_num?"<<(flag_run_num?" yes":" no")<<std::endl;
	std::cout<<"output R0?"<<(flag_R0?" yes":" no")<<std::endl;
	std::cout<<"output R1?"<<(flag_R1?" yes":" no")<<std::endl;
	std::cout<<"output ipx?"<<(flag_ipx?" yes":" no")<<std::endl;
	std::cout<<"output ipy?"<<(flag_ipy?" yes":" no")<<std::endl;
	std::cout<<"output ipz?"<<(flag_ipz?" yes":" no")<<std::endl;
	std::cout<<"output weight?"<<(flag_weight?" yes":" no")<<std::endl;
}

void EventHeaderSvc::SetValue(const G4Event* evt, int runNb, double w){
	run_num = runNb;
	evt_num = evt->GetEventID();
	weight = w;
}
void EventHeaderSvc::SetSeedsValue(){
    R0 = CLHEP::HepRandom::getTheSeeds()[0];
    R1 = CLHEP::HepRandom::getTheSeeds()[1];
}
void EventHeaderSvc::SetInitialMomentum(double tipx, double tipy, double tipz){
	ipx = tipx;
	ipy = tipy;
	ipz = tipz;
}
