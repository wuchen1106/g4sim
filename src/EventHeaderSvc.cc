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

#include "MyRoot.hh"

EventHeaderSvc* EventHeaderSvc::fEventHeaderSvc = 0;

EventHeaderSvc::EventHeaderSvc()
{
  if (fEventHeaderSvc){
    G4Exception("EventHeaderSvc::EventHeaderSvc()","Run0031",
        FatalException, "EventHeaderSvc constructed twice.");
  }
  fEventHeaderSvc = this;

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
}

void EventHeaderSvc::ShowOutCard(){
	std::cout<<"*************************Output settings for EventHeaderSvc***************************"<<std::endl;
  std::cout<<"output evt_num?"<<(flag_evt_num?" yes":" no")<<std::endl;
  std::cout<<"output run_num?"<<(flag_run_num?" yes":" no")<<std::endl;
}

void EventHeaderSvc::SetValue(const G4Event* evt, int runNb){
  run_num = runNb;
  evt_num = evt->GetEventID();
}
