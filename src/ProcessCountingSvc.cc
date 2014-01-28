//---------------------------------------------------------------------------//
//Description: In charge of ProcessCounting
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "ProcessCountingSvc.hh"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "globals.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4VProcess.hh"

#include "MyString2Anything.hh"

#include "MyRoot.hh"

ProcessCountingSvc* ProcessCountingSvc::fProcessCountingSvc = 0;

ProcessCountingSvc::ProcessCountingSvc()
{
	if (fProcessCountingSvc){
		G4Exception("ProcessCountingSvc::ProcessCountingSvc()","Run0031",
				FatalException, "ProcessCountingSvc constructed twice.");
	}
	fProcessCountingSvc = this;
	ReSet();
}

ProcessCountingSvc::~ProcessCountingSvc()
{
	printf("~ProcessCountingSvc\n");
}

ProcessCountingSvc* ProcessCountingSvc::GetProcessCountingSvc(){
	if ( !fProcessCountingSvc ){
		fProcessCountingSvc = new ProcessCountingSvc;
	}
	return fProcessCountingSvc;
}

void ProcessCountingSvc::Initialize(){
	m_TrackIDs.clear();
	m_nSteps = 0;
	m_pid.clear();
	m_tid.clear();
	m_nSec.clear();
	m_time.clear();
	m_stepL.clear();
	m_prePx.clear();
	m_prePy.clear();
	m_prePz.clear();
	m_postPx.clear();
	m_postPy.clear();
	m_postPz.clear();
	m_dTheta.clear();
	m_dE.clear();
	m_edepT.clear();
	m_edepN.clear();
	m_edepI.clear();
	m_e.clear();
	m_preX.clear();
	m_preY.clear();
	m_preZ.clear();
	m_postX.clear();
	m_postY.clear();
	m_postZ.clear();
	m_particleName.clear();
	m_charge.clear();
	m_process.clear();
	m_ASDI_msc.clear();
	m_ASDI_hPairProd.clear();
	m_ASDI_ProtonInelastic.clear();
	m_ASDI_Transportation.clear();
	m_ASDI_hBrems.clear();
	m_ASDI_eBrem.clear();
	m_ASDI_eIoni.clear();
	m_PSDI_eIoni.clear();
	m_PSDI_eBrem.clear();
	m_PSDI_msc.clear();
	m_PSDI_hPairProd.clear();
	m_PSDI_ProtonInelastic.clear();
	m_PSDI_hIoni.clear();
	m_PSDI_hadElastic.clear();
	m_PSDI_ionIoni.clear();
	m_PSDI_Decay.clear();
	m_PSDI_Transportation.clear();
	m_PSDI_hBrems.clear();
	m_volume.clear();
	VolName = "";
}

void ProcessCountingSvc::SetBranch(){
	if (!m_Switch) return;
	MyRoot* myRoot = MyRoot::GetMyRoot();
	if( flag_nSteps ) myRoot->SetBranch("ProcessCounting_nSteps", &m_nSteps);
	if( flag_pid ) myRoot->SetBranch("ProcessCounting_pid", &m_pid);
	if( flag_tid ) myRoot->SetBranch("ProcessCounting_tid", &m_tid);
	if( flag_nSec ) myRoot->SetBranch("ProcessCounting_nSec", &m_nSec);
	if( flag_time ) myRoot->SetBranch("ProcessCounting_time", &m_time);
	if( flag_stepL ) myRoot->SetBranch("ProcessCounting_stepL", &m_stepL);
	if( flag_prePx ) myRoot->SetBranch("ProcessCounting_prePx", &m_prePx);
	if( flag_prePy ) myRoot->SetBranch("ProcessCounting_prePy", &m_prePy);
	if( flag_prePz ) myRoot->SetBranch("ProcessCounting_prePz", &m_prePz);
	if( flag_postPx ) myRoot->SetBranch("ProcessCounting_postPx", &m_postPx);
	if( flag_postPy ) myRoot->SetBranch("ProcessCounting_postPy", &m_postPy);
	if( flag_postPz ) myRoot->SetBranch("ProcessCounting_postPz", &m_postPz);
	if( flag_dTheta ) myRoot->SetBranch("ProcessCounting_dTheta", &m_dTheta);
	if( flag_dE ) myRoot->SetBranch("ProcessCounting_dE", &m_dE);
	if( flag_edepT ) myRoot->SetBranch("ProcessCounting_edepT", &m_edepT);
	if( flag_edepN ) myRoot->SetBranch("ProcessCounting_edepN", &m_edepN);
	if( flag_edepI ) myRoot->SetBranch("ProcessCounting_edepI", &m_edepI);
	if( flag_e ) myRoot->SetBranch("ProcessCounting_e", &m_e);
	if( flag_preX ) myRoot->SetBranch("ProcessCounting_preX", &m_preX);
	if( flag_preY ) myRoot->SetBranch("ProcessCounting_preY", &m_preY);
	if( flag_preZ ) myRoot->SetBranch("ProcessCounting_preZ", &m_preZ);
	if( flag_postX ) myRoot->SetBranch("ProcessCounting_postX", &m_postX);
	if( flag_postY ) myRoot->SetBranch("ProcessCounting_postY", &m_postY);
	if( flag_postZ ) myRoot->SetBranch("ProcessCounting_postZ", &m_postZ);
	if( flag_charge ) myRoot->SetBranch("ProcessCounting_charge", &m_charge);
	if( flag_particleName ) myRoot->SetBranch("ProcessCounting_particleName", &m_particleName);
	if( flag_process ) myRoot->SetBranch("ProcessCounting_process", &m_process);
	if( flag_ASDI_msc ) myRoot->SetBranch("ProcessCounting_ASDI_msc", &m_ASDI_msc);
	if( flag_ASDI_hPairProd ) myRoot->SetBranch("ProcessCounting_ASDI_hPairProd", &m_ASDI_hPairProd);
	if( flag_ASDI_ProtonInelastic ) myRoot->SetBranch("ProcessCounting_ASDI_ProtonInelastic", &m_ASDI_ProtonInelastic);
	if( flag_ASDI_hIoni ) myRoot->SetBranch("ProcessCounting_ASDI_hIoni", &m_ASDI_hIoni);
	if( flag_ASDI_hadElastic ) myRoot->SetBranch("ProcessCounting_ASDI_hadElastic", &m_ASDI_hadElastic);
	if( flag_ASDI_ionIoni ) myRoot->SetBranch("ProcessCounting_ASDI_ionIoni", &m_ASDI_ionIoni);
	if( flag_ASDI_Decay ) myRoot->SetBranch("ProcessCounting_ASDI_Decay", &m_ASDI_Decay);
	if( flag_ASDI_Transportation ) myRoot->SetBranch("ProcessCounting_ASDI_Transportation", &m_ASDI_Transportation);
	if( flag_ASDI_hBrems ) myRoot->SetBranch("ProcessCounting_ASDI_hBrems", &m_ASDI_hBrems);
	if( flag_ASDI_eBrem ) myRoot->SetBranch("ProcessCounting_ASDI_eBrem", &m_ASDI_eBrem);
	if( flag_ASDI_eIoni ) myRoot->SetBranch("ProcessCounting_ASDI_eIoni", &m_ASDI_eIoni);
	if( flag_PSDI_eBrem ) myRoot->SetBranch("ProcessCounting_PSDI_eBrem", &m_PSDI_eBrem);
	if( flag_PSDI_eIoni ) myRoot->SetBranch("ProcessCounting_PSDI_eIoni", &m_PSDI_eIoni);
	if( flag_PSDI_msc ) myRoot->SetBranch("ProcessCounting_PSDI_msc", &m_PSDI_msc);
	if( flag_PSDI_hPairProd ) myRoot->SetBranch("ProcessCounting_PSDI_hPairProd", &m_PSDI_hPairProd);
	if( flag_PSDI_ProtonInelastic ) myRoot->SetBranch("ProcessCounting_PSDI_ProtonInelastic", &m_PSDI_ProtonInelastic);
	if( flag_PSDI_hIoni ) myRoot->SetBranch("ProcessCounting_PSDI_hIoni", &m_PSDI_hIoni);
	if( flag_PSDI_hadElastic ) myRoot->SetBranch("ProcessCounting_PSDI_hadElastic", &m_PSDI_hadElastic);
	if( flag_PSDI_ionIoni ) myRoot->SetBranch("ProcessCounting_PSDI_ionIoni", &m_PSDI_ionIoni);
	if( flag_PSDI_Decay ) myRoot->SetBranch("ProcessCounting_PSDI_Decay", &m_PSDI_Decay);
	if( flag_PSDI_Transportation ) myRoot->SetBranch("ProcessCounting_PSDI_Transportation", &m_PSDI_Transportation);
	if( flag_PSDI_hBrems ) myRoot->SetBranch("ProcessCounting_PSDI_hBrems", &m_PSDI_hBrems);
	if( flag_volume ) myRoot->SetBranch("ProcessCounting_volume", &m_volume);
}

void ProcessCountingSvc::ReadOutputCard(G4String filename){
	ReSet();
	std::ifstream fin_card(filename);
	if(!fin_card){
		std::cout<<"In ProcessCountingSvc::ReadOutputCard, cannot open "<<filename<<"!!!"<<std::endl;
		G4Exception("ProcessCountingSvc::ReadOutputCard()",
				"InvalidSetup", FatalException,
				"cannot find output card");
	}
	std::stringstream buf_card;
	std::string s_card;
	int n_output_section_symbol = 0;
	int n_filter_section_symbol = 0;
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
			if ( name == "PROCESSCOUNTING_SECTION" ){
				n_output_section_symbol++;
			}
		}
		else if ( n_output_section_symbol == 1 ){
			if ( name == "PROCESSCOUNTING_SECTION" ){
				n_output_section_symbol++;
			}
			else if( name == "nSteps" ) flag_nSteps = true;
			else if( name == "pid" ) flag_pid = true;
			else if( name == "tid" ) flag_tid = true;
			else if( name == "nSec" ) flag_nSec = true;
			else if( name == "time" ) {flag_time = true; buf_card>>unitName_time; unit_time = MyString2Anything::get_U(unitName_time);}
			else if( name == "time" ) {flag_time = true; buf_card>>unitName_time; unit_time = MyString2Anything::get_U(unitName_time);}
			else if( name == "stepL" ) {flag_stepL = true; buf_card>>unitName_stepL; unit_stepL = MyString2Anything::get_U(unitName_stepL);}
			else if( name == "prePx" ) {flag_prePx = true; buf_card>>unitName_prePx; unit_prePx = MyString2Anything::get_U(unitName_prePx);}
			else if( name == "prePy" ) {flag_prePy = true; buf_card>>unitName_prePy; unit_prePy = MyString2Anything::get_U(unitName_prePy);}
			else if( name == "prePz" ) {flag_prePz = true; buf_card>>unitName_prePz; unit_prePz = MyString2Anything::get_U(unitName_prePz);}
			else if( name == "postPx" ) {flag_postPx = true; buf_card>>unitName_postPx; unit_postPx = MyString2Anything::get_U(unitName_postPx);}
			else if( name == "postPy" ) {flag_postPy = true; buf_card>>unitName_postPy; unit_postPy = MyString2Anything::get_U(unitName_postPy);}
			else if( name == "postPz" ) {flag_postPz = true; buf_card>>unitName_postPz; unit_postPz = MyString2Anything::get_U(unitName_postPz);}
			else if( name == "dTheta" ) {flag_dTheta = true; buf_card>>unitName_dTheta; unit_dTheta = MyString2Anything::get_U(unitName_dTheta);}
			else if( name == "dE" ) {flag_dE = true; buf_card>>unitName_dE; unit_dE = MyString2Anything::get_U(unitName_dE);}
			else if( name == "edepT" ) {flag_edepT = true; buf_card>>unitName_edepT; unit_edepT = MyString2Anything::get_U(unitName_edepT);}
			else if( name == "edepN" ) {flag_edepN = true; buf_card>>unitName_edepN; unit_edepN = MyString2Anything::get_U(unitName_edepN);}
			else if( name == "edepI" ) {flag_edepI = true; buf_card>>unitName_edepI; unit_edepI = MyString2Anything::get_U(unitName_edepI);}
			else if( name == "e" ) {flag_e = true; buf_card>>unitName_e; unit_e = MyString2Anything::get_U(unitName_e);}
			else if( name == "preX" ) {flag_preX = true; buf_card>>unitName_preX; unit_preX = MyString2Anything::get_U(unitName_preX);}
			else if( name == "preY" ) {flag_preY = true; buf_card>>unitName_preY; unit_preY = MyString2Anything::get_U(unitName_preY);}
			else if( name == "preZ" ) {flag_preZ = true; buf_card>>unitName_preZ; unit_preZ = MyString2Anything::get_U(unitName_preZ);}
			else if( name == "postX" ) {flag_postX = true; buf_card>>unitName_postX; unit_postX = MyString2Anything::get_U(unitName_postX);}
			else if( name == "postY" ) {flag_postY = true; buf_card>>unitName_postY; unit_postY = MyString2Anything::get_U(unitName_postY);}
			else if( name == "postZ" ) {flag_postZ = true; buf_card>>unitName_postZ; unit_postZ = MyString2Anything::get_U(unitName_postZ);}
			else if( name == "particleName" ) flag_particleName = true;
			else if( name == "charge" ) flag_charge = true;
			else if( name == "process" ) flag_process = true;
			else if( name == "ASDI_msc" ) flag_ASDI_msc = true;
			else if( name == "ASDI_hPairProd" ) flag_ASDI_hPairProd = true;
			else if( name == "ASDI_ProtonInelastic" ) flag_ASDI_ProtonInelastic = true;
			else if( name == "ASDI_hIoni" ) flag_ASDI_hIoni = true;
			else if( name == "ASDI_hadElastic" ) flag_ASDI_hadElastic = true;
			else if( name == "ASDI_ionIoni" ) flag_ASDI_ionIoni = true;
			else if( name == "ASDI_Decay" ) flag_ASDI_Decay = true;
			else if( name == "ASDI_Transportation" ) flag_ASDI_Transportation = true;
			else if( name == "ASDI_hBrems" ) flag_ASDI_hBrems = true;
			else if( name == "ASDI_eBrem" ) flag_ASDI_eBrem = true;
			else if( name == "ASDI_eIoni" ) flag_ASDI_eIoni = true;
			else if( name == "PSDI_eBrem" ) flag_PSDI_eBrem = true;
			else if( name == "PSDI_eIoni" ) flag_PSDI_eIoni = true;
			else if( name == "PSDI_msc" ) flag_PSDI_msc = true;
			else if( name == "PSDI_hPairProd" ) flag_PSDI_hPairProd = true;
			else if( name == "PSDI_ProtonInelastic" ) flag_PSDI_ProtonInelastic = true;
			else if( name == "PSDI_hIoni" ) flag_PSDI_hIoni = true;
			else if( name == "PSDI_hadElastic" ) flag_PSDI_hadElastic = true;
			else if( name == "PSDI_ionIoni" ) flag_PSDI_ionIoni = true;
			else if( name == "PSDI_Decay" ) flag_PSDI_Decay = true;
			else if( name == "PSDI_Transportation" ) flag_PSDI_Transportation = true;
			else if( name == "PSDI_hBrems" ) flag_PSDI_hBrems = true;
			else if( name == "volume" ) flag_volume = true;
			else{
				std::cout<<"In ProcessCountingSvc::ReadOutputCard, unknown name: "<<name<<" in file "<<filename<<std::endl;
				std::cout<<"Will ignore this line!"<<std::endl;
			}
		}

		if ( n_filter_section_symbol == 0 ){
			if ( name == "PROCESSCOUNTINGFILTER_SECTION" ){
				n_filter_section_symbol++;
			}
		}
		else if ( n_filter_section_symbol == 1 ){
			if ( name == "PROCESSCOUNTINGFILTER_SECTION" ){
				n_filter_section_symbol++;
			}
			else if( name == "Switch" ) m_Switch = true;
			else if( name == "nTracks" ) buf_card>>m_maxnTracks;
			else if( name == "nSteps" ) buf_card>>m_maxnSteps;
			else if( name == "Volume" ){
				G4String VolName;
				while (buf_card>>VolName){
					if(VolName.substr(0,1) == "#" || VolName.substr(0,2) == "//") break;
					m_Volumes.push_back(VolName);
				}
			}
			else{
				G4double para;
				std::string unit;
				buf_card>>para>>unit;
				para *= MyString2Anything::get_U(unit);
				if( name == "minp" ) m_minp = para;
				else if( name == "mine" ) m_mine = para;
				else if( name == "mint" ) m_mint = para;
				else if( name == "maxt" ) m_maxt = para;
				else{
					std::cout<<"In ProcessCountingSvc::ReadOutputCard, unknown name: "<<name<<" in file "<<filename<<std::endl;
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
		std::cout<<"In ProcessCountingSvc::ReadOutputCard, failed to find enough section seperators for output in file "<<filename<<std::endl;
		std::cout<<"Will use default settings."<<std::endl;
		std::cout<<"********************************************"<<std::endl;
	}
	if ( n_filter_section_symbol<= 1 ){
		std::cout<<"*****************WARNING********************"<<std::endl;
		std::cout<<"In ProcessCountingSvc::ReadOutputCard, failed to find enough section seperators for filter in file "<<filename<<std::endl;
		std::cout<<"Will use default settings."<<std::endl;
		std::cout<<"********************************************"<<std::endl;
	}
	fin_card.close();
	ShowOutCard();
}

void ProcessCountingSvc::ReSet(){
	flag_nSteps = false;
	flag_pid = false;
	flag_tid = false;
	flag_nSec = false;
	flag_time = false;
	flag_stepL = false;
	flag_prePx = false;
	flag_prePy = false;
	flag_prePz = false;
	flag_postPx = false;
	flag_postPy = false;
	flag_postPz = false;
	flag_dTheta = false;
	flag_dE = false;
	flag_edepT = false;
	flag_edepN = false;
	flag_edepI = false;
	flag_e = false;
	flag_preX = false;
	flag_preY = false;
	flag_preZ = false;
	flag_postX = false;
	flag_postY = false;
	flag_postZ = false;
	flag_charge = false;
	flag_particleName = false;
	flag_process = false;
	flag_ASDI_msc = false;
	flag_ASDI_hPairProd = false;
	flag_ASDI_ProtonInelastic = false;
	flag_ASDI_hIoni = false;
	flag_ASDI_hadElastic = false;
	flag_ASDI_ionIoni = false;
	flag_ASDI_Decay = false;
	flag_ASDI_Transportation = false;
	flag_ASDI_hBrems = false;
	flag_ASDI_eBrem = false;
	flag_ASDI_eIoni = false;
	flag_PSDI_eIoni = false;
	flag_PSDI_eBrem = false;
	flag_PSDI_msc = false;
	flag_PSDI_hPairProd = false;
	flag_PSDI_ProtonInelastic = false;
	flag_PSDI_hIoni = false;
	flag_PSDI_hadElastic = false;
	flag_PSDI_ionIoni = false;
	flag_PSDI_Decay = false;
	flag_PSDI_Transportation = false;
	flag_PSDI_hBrems = false;
	flag_volume = false;
	m_Switch = false;
	m_maxnTracks = 0;
	m_minp = 0;
	m_mine = 0;
	m_maxnSteps = 0;
	m_mint = 0;
	m_maxt = 0;
	m_Volumes.clear();
	unitName_time	="s";
	unitName_stepL	="cm";
	unitName_prePx	="GeV";
	unitName_prePy	="GeV";
	unitName_prePz	="GeV";
	unitName_postPx	="GeV";
	unitName_postPy	="GeV";
	unitName_postPz	="GeV";
	unitName_dTheta	="rad";
	unitName_dE	="GeV";
	unitName_edepT	="GeV";
	unitName_edepN	="GeV";
	unitName_edepI	="GeV";
	unitName_e	="GeV";
	unitName_preX	="cm";
	unitName_preY	="cm";
	unitName_preZ	="cm";
	unitName_postX	="cm";
	unitName_postY	="cm";
	unitName_postZ	="cm";
	unit_time	=s;
	unit_stepL	=cm;
	unit_prePx	=GeV;
	unit_prePy	=GeV;
	unit_prePz	=GeV;
	unit_postPx	=GeV;
	unit_postPy	=GeV;
	unit_postPz	=GeV;
	unit_dTheta	=rad;
	unit_dE	=GeV;
	unit_edepT	=GeV;
	unit_edepN	=GeV;
	unit_edepI	=GeV;
	unit_e	=GeV;
	unit_preX	=cm;
	unit_preY	=cm;
	unit_preZ	=cm;
	unit_postX	=cm;
	unit_postY	=cm;
	unit_postZ	=cm;
}

void ProcessCountingSvc::ShowOutCard(){
	std::cout<<"*************************Output settings for ProcessCountingSvc***************************"<<std::endl;
	std::cout<<"output nSteps?        "<<(flag_nSteps?" yes":" no")<<std::endl;
	std::cout<<"output pid?           "<<(flag_pid?" yes":" no")<<std::endl;
	std::cout<<"output tid?           "<<(flag_tid?" yes":" no")<<std::endl;
	std::cout<<"output nSec?          "<<(flag_nSec?" yes":" no")<<std::endl;
	std::cout<<"output time?          "<<(flag_time?" yes":" no")<<", unit: "<<unitName_time<<std::endl;
	std::cout<<"output stepL?         "<<(flag_stepL?" yes":" no")<<", unit: "<<unitName_stepL<<std::endl;
	std::cout<<"output prePx?         "<<(flag_prePx?" yes":" no")<<", unit: "<<unitName_prePx<<std::endl;
	std::cout<<"output prePy?         "<<(flag_prePy?" yes":" no")<<", unit: "<<unitName_prePy<<std::endl;
	std::cout<<"output prePz?         "<<(flag_prePz?" yes":" no")<<", unit: "<<unitName_prePz<<std::endl;
	std::cout<<"output postPx?        "<<(flag_postPx?" yes":" no")<<", unit: "<<unitName_postPx<<std::endl;
	std::cout<<"output postPy?        "<<(flag_postPy?" yes":" no")<<", unit: "<<unitName_postPy<<std::endl;
	std::cout<<"output postPz?        "<<(flag_postPz?" yes":" no")<<", unit: "<<unitName_postPz<<std::endl;
	std::cout<<"output dTheta?        "<<(flag_dTheta?" yes":" no")<<", unit: "<<unitName_dTheta<<std::endl;
	std::cout<<"output dE?            "<<(flag_dE?" yes":" no")<<", unit: "<<unitName_dE<<std::endl;
	std::cout<<"output edepT?         "<<(flag_edepT?" yes":" no")<<", unit: "<<unitName_edepT<<std::endl;
	std::cout<<"output edepN?         "<<(flag_edepN?" yes":" no")<<", unit: "<<unitName_edepN<<std::endl;
	std::cout<<"output edepI?         "<<(flag_edepI?" yes":" no")<<", unit: "<<unitName_edepI<<std::endl;
	std::cout<<"output e?             "<<(flag_e?" yes":" no")<<", unit: "<<unitName_e<<std::endl;
	std::cout<<"output preX?          "<<(flag_preX?" yes":" no")<<", unit: "<<unitName_preX<<std::endl;
	std::cout<<"output preY?          "<<(flag_preY?" yes":" no")<<", unit: "<<unitName_preY<<std::endl;
	std::cout<<"output preZ?          "<<(flag_preZ?" yes":" no")<<", unit: "<<unitName_preZ<<std::endl;
	std::cout<<"output postX?         "<<(flag_postX?" yes":" no")<<", unit: "<<unitName_postX<<std::endl;
	std::cout<<"output postY?         "<<(flag_postY?" yes":" no")<<", unit: "<<unitName_postY<<std::endl;
	std::cout<<"output postZ?         "<<(flag_postZ?" yes":" no")<<", unit: "<<unitName_postZ<<std::endl;
	std::cout<<"output particleName?  "<<(flag_particleName?" yes":" no")<<std::endl;
	std::cout<<"output charge?        "<<(flag_charge?" yes":" no")<<std::endl;
	std::cout<<"output process?       "<<(flag_process?" yes":" no")<<std::endl;
	std::cout<<"output ASDI_msc?      "<<(flag_ASDI_msc?" yes":" no")<<std::endl;
	std::cout<<"output ASDI_hPairProd?"<<(flag_ASDI_hPairProd?" yes":" no")<<std::endl;
	std::cout<<"output ASDI_ProtonInelastic?"<<(flag_ASDI_ProtonInelastic?" yes":" no")<<std::endl;
	std::cout<<"output ASDI_hIoni?    "<<(flag_ASDI_hIoni?" yes":" no")<<std::endl;
	std::cout<<"output ASDI_hadElastic?    "<<(flag_ASDI_hadElastic?" yes":" no")<<std::endl;
	std::cout<<"output ASDI_ionIoni?    "<<(flag_ASDI_ionIoni?" yes":" no")<<std::endl;
	std::cout<<"output ASDI_Decay?    "<<(flag_ASDI_Decay?" yes":" no")<<std::endl;
	std::cout<<"output ASDI_Transportation?    "<<(flag_ASDI_Transportation?" yes":" no")<<std::endl;
	std::cout<<"output ASDI_hBrems?   "<<(flag_ASDI_hBrems?" yes":" no")<<std::endl;
	std::cout<<"output ASDI_eBrem?    "<<(flag_ASDI_eBrem?" yes":" no")<<std::endl;
	std::cout<<"output ASDI_eIoni?    "<<(flag_ASDI_eIoni?" yes":" no")<<std::endl;
	std::cout<<"output PSDI_eIoni?    "<<(flag_PSDI_eIoni?" yes":" no")<<std::endl;
	std::cout<<"output PSDI_eBrem?    "<<(flag_PSDI_eBrem?" yes":" no")<<std::endl;
	std::cout<<"output PSDI_msc?      "<<(flag_PSDI_msc?" yes":" no")<<std::endl;
	std::cout<<"output PSDI_hPairProd?"<<(flag_PSDI_hPairProd?" yes":" no")<<std::endl;
	std::cout<<"output PSDI_ProtonInelastic?"<<(flag_PSDI_ProtonInelastic?" yes":" no")<<std::endl;
	std::cout<<"output PSDI_hIoni?    "<<(flag_PSDI_hIoni?" yes":" no")<<std::endl;
	std::cout<<"output PSDI_hadElastic?    "<<(flag_PSDI_hadElastic?" yes":" no")<<std::endl;
	std::cout<<"output PSDI_ionIoni?    "<<(flag_PSDI_ionIoni?" yes":" no")<<std::endl;
	std::cout<<"output PSDI_Decay?    "<<(flag_PSDI_Decay?" yes":" no")<<std::endl;
	std::cout<<"output PSDI_Transportation?    "<<(flag_PSDI_Transportation?" yes":" no")<<std::endl;
	std::cout<<"output PSDI_hBrems?   "<<(flag_PSDI_hBrems?" yes":" no")<<std::endl;
	std::cout<<"output volume?        "<<(flag_volume?" yes":" no")<<std::endl;
	std::cout<<"Switch on?            "<<(m_Switch?"yes":"no")<<std::endl;
	std::cout<<"minp =                "<<m_minp/MeV<<"MeV"<<std::endl;
	std::cout<<"mine =                "<<m_mine/MeV<<"MeV"<<std::endl;
	std::cout<<"maxnSteps =           "<<m_maxnSteps<<std::endl;
	std::cout<<"maxnTracks =          "<<m_maxnTracks<<std::endl;
	std::cout<<"mint =                "<<m_mint/ns<<"ns"<<std::endl;
	std::cout<<"maxt =                "<<m_maxt/ns<<"ns"<<std::endl;
	std::cout<<m_Volumes.size()<<" Volumes Selected:";
	for ( int i = 0; i< m_Volumes.size(); i++ ){
		std::cout<<" "<<m_Volumes[i];
	}
	std::cout<<std::endl;
	std::cout<<"******************************************************************************"<<std::endl;
}

void ProcessCountingSvc::InitialStep(const G4Step* aStep){
	std::cout<<"In ProcessCountingSvc::InitialStep"<<std::endl;
	// get volume info
	const G4VTouchable *touchable = aStep->GetPreStepPoint()->GetTouchable();
	VolName = touchable->GetVolume(0)->GetName();
	//std::cout<<"In ProcessCountingSvc, VolName = \""<<VolName<<"\""<<std::endl;
	CheckTrack(aStep);
	CheckFilter(aStep);
	if (!PASSEDFILTER) return;
	//std::cout<<"--------------------In ProcessCountingSvc::InitialStep()-----------------------"<<std::endl;
	//std::cout<<"size = "<<m_ASDI_msc.size()<<std::endl;
	m_ASDI_msc.push_back(0);
	m_ASDI_hPairProd.push_back(0);
	m_ASDI_ProtonInelastic.push_back(0);
	m_ASDI_hIoni.push_back(0);
	m_ASDI_hadElastic.push_back(0);
	m_ASDI_ionIoni.push_back(0);
	m_ASDI_Decay.push_back(0);
	m_ASDI_Transportation.push_back(0);
	m_ASDI_hBrems.push_back(0);
	m_ASDI_eBrem.push_back(0);
	m_ASDI_eIoni.push_back(0);
	m_PSDI_eBrem.push_back(0);
	m_PSDI_eIoni.push_back(0);
	m_PSDI_msc.push_back(0);
	m_PSDI_hPairProd.push_back(0);
	m_PSDI_ProtonInelastic.push_back(0);
	m_PSDI_hIoni.push_back(0);
	m_PSDI_hadElastic.push_back(0);
	m_PSDI_ionIoni.push_back(0);
	m_PSDI_Decay.push_back(0);
	m_PSDI_Transportation.push_back(0);
	m_PSDI_hBrems.push_back(0);
}

void ProcessCountingSvc::AddASDI(G4String name){
	if (!PASSEDFILTER) return;
	int i = m_nSteps;
	if ( name == "msc" || name == "CoulombScat" ) m_ASDI_msc[i]++;
	else if ( name == "eIoni" ) m_ASDI_eIoni[i]++;
	else if ( name == "eBrem" ) m_ASDI_eBrem[i]++;
	else if ( name == "hIoni" ) m_ASDI_hIoni[i]++;
	else if ( name == "hadElastic" ) m_ASDI_hadElastic[i]++;
	else if ( name == "ionIoni" ) m_ASDI_ionIoni[i]++;
	else if ( name == "Decay" ) m_ASDI_Decay[i]++;
	else if ( name == "Transportation" ) m_ASDI_Transportation[i]++;
	else if ( name == "hBrems" ) m_ASDI_hBrems[i]++;
	else if ( name == "hPairProd" ) m_ASDI_hPairProd[i]++;
	else if ( name == "ProtonInelastic" ) m_ASDI_ProtonInelastic[i]++;
	//else std::cout<<"In AddASDI: Process Name = \""<<name<<"\""<<std::endl;
}

void ProcessCountingSvc::AddPSDI(G4String name){
	if (!PASSEDFILTER) return;
	int i = m_nSteps;
	if ( name == "msc" || name == "CoulombScat" ) m_PSDI_msc[i]++;
	else if ( name == "eIoni" ) m_PSDI_eIoni[i]++;
	else if ( name == "eBrem" ) m_PSDI_eBrem[i]++;
	else if ( name == "hIoni" ) m_PSDI_hIoni[i]++;
	else if ( name == "hadElastic" ) m_PSDI_hadElastic[i]++;
	else if ( name == "ionIoni" ) m_PSDI_ionIoni[i]++;
	else if ( name == "Decay" ) m_PSDI_Decay[i]++;
	else if ( name == "Transportation" ) m_PSDI_Transportation[i]++;
	else if ( name == "hBrems" ) m_PSDI_hBrems[i]++;
	else if ( name == "hPairProd" ) m_PSDI_hPairProd[i]++;
	else if ( name == "ProtonInelastic" ) m_PSDI_ProtonInelastic[i]++;
	//else std::cout<<"In AddPSDI: Process Name = \""<<name<<"\""<<std::endl;
}

void ProcessCountingSvc::SetValue(const G4Step* aStep){
	const G4VTouchable *touchable = aStep->GetPreStepPoint()->GetTouchable();
	VolName = touchable->GetVolume(0)->GetName();
	CheckFilter(aStep);
	if (!PASSEDFILTER) return;

	//*************************get useful variables***********************

	// get track info
	G4Track* aTrack = aStep->GetTrack() ;
	G4int trackID= aTrack->GetTrackID(); //G4 track ID of current track.
	G4int pid = aTrack->GetParticleDefinition()->GetPDGEncoding();
	G4int charge = aTrack->GetParticleDefinition()->GetPDGCharge();
	std::string particleName = aTrack->GetParticleDefinition()->GetParticleName();

	// get information at the beginning and at the end of step
	G4StepPoint* prePoint  = aStep->GetPreStepPoint() ;
	G4ThreeVector pointIn_pos = prePoint->GetPosition();
	G4ThreeVector pointIn_mom = prePoint->GetMomentum();
	G4double pointIn_e = prePoint->GetTotalEnergy();
	G4double pointIn_pa = pointIn_mom.mag();
	G4double pointIn_time = prePoint->GetGlobalTime();//Time since the event in which the track belongs is created
	G4StepPoint* postPoint = aStep->GetPostStepPoint() ;
	G4double pointOut_e = postPoint->GetTotalEnergy();
	G4ThreeVector pointOut_pos = postPoint->GetPosition();
	G4ThreeVector pointOut_mom = postPoint->GetMomentum();
	G4String processName;
	const G4VProcess* process = postPoint->GetProcessDefinedStep();
	if (process) {
		processName = process->GetProcessName();
	}
	else{
		processName = "NULL";
	}

	// get step info
	G4double stepL = aStep->GetStepLength();
	G4int nSec;
	const std::vector<const G4Track*>* pVTracks =	aStep->GetSecondaryInCurrentStep();
	if (pVTracks){
		nSec = pVTracks->size();
	}
	else{
		nSec = -1;
	}

	//	if ( nSec > 0 ){
	//		std::cout<<"==["<<nSec<<"] @ ("<<pointIn_pos.x()/mm<<","<<pointIn_pos.y()/mm<<","<<pointIn_pos.z()/mm<<")/mm"<<std::endl;
	//		G4String tProcessName, tParticleName;
	//		G4int tTid;
	//		G4double tEnergy;
	//		G4ThreeVector tPoint;
	//		for ( int i = 0; i < nSec; i++ ){
	//			tTid = (*pVTracks)[i]->GetTrackID(); //G4 track ID of current track.
	//			tParticleName = (*pVTracks)[i]->GetParticleDefinition()->GetParticleName();
	//			const G4VProcess* process = (*pVTracks)[i]->GetCreatorProcess();
	//			if (process) {
	//				tProcessName = process->GetProcessName();
	//			}
	//			else{
	//				tProcessName = "NULL";
	//			}
	//			tEnergy = (*pVTracks)[i]->GetTotalEnergy();
	//			tPoint = (*pVTracks)[i]->GetVertexPosition();
	//			std::cout<<"    "<<i<<": "<<tTid<<", "<<tParticleName<<", "<<tProcessName<<", "<<tEnergy/MeV<<"MeV, @ ("<<tPoint.x()/mm<<","<<tPoint.y()/mm<<","<<tPoint.z()/mm<<")/mm"<<std::endl;
	//		}
	//	}

	G4double deltaTheta = pointOut_mom.theta(pointIn_mom);
	G4double deltaE = pointIn_e - pointOut_e;
	G4double edep_total = aStep->GetTotalEnergyDeposit();
	G4double edep_nonIoni = aStep->GetNonIonizingEnergyDeposit();
	G4double edep_Ioni = edep_total - edep_nonIoni;

	m_nSteps++;
	if(flag_pid) m_pid.push_back(pid);
	if(flag_tid) m_tid.push_back(trackID);
	if(flag_nSec) m_nSec.push_back(nSec);
	if(flag_time) m_time.push_back(pointIn_time/unit_time);
	if(flag_stepL) m_stepL.push_back(stepL/unit_stepL);
	if(flag_prePx) m_prePx.push_back(pointIn_mom.x()/unit_prePx);
	if(flag_prePy) m_prePy.push_back(pointIn_mom.y()/unit_prePy);
	if(flag_prePz) m_prePz.push_back(pointIn_mom.z()/unit_prePz);
	if(flag_postPx) m_postPx.push_back(pointOut_mom.x()/unit_postPx);
	if(flag_postPy) m_postPy.push_back(pointOut_mom.y()/unit_postPy);
	if(flag_postPz) m_postPz.push_back(pointOut_mom.z()/unit_postPz);
	if(flag_dTheta) m_dTheta.push_back(deltaTheta/unit_dTheta);
	if(flag_dE) m_dE.push_back(deltaE/unit_dE);
	if(flag_edepT) m_edepT.push_back(edep_total/unit_edepT);
	if(flag_edepN) m_edepN.push_back(edep_Ioni/unit_edepN);
	if(flag_edepI) m_edepI.push_back(edep_nonIoni/unit_edepI);
	if(flag_e) m_e.push_back(pointIn_e/unit_e);
	if(flag_preX) m_preX.push_back(pointIn_pos.x()/unit_preX);
	if(flag_preY) m_preY.push_back(pointIn_pos.y()/unit_preY);
	if(flag_preZ) m_preZ.push_back(pointIn_pos.z()/unit_preZ);
	if(flag_postX) m_postX.push_back(pointOut_pos.x()/unit_postX);
	if(flag_postY) m_postY.push_back(pointOut_pos.y()/unit_postY);
	if(flag_postZ) m_postZ.push_back(pointOut_pos.z()/unit_postZ);
	if(flag_charge) m_charge.push_back(charge);
	if(flag_particleName) m_particleName.push_back(particleName);
	if(flag_process) m_process.push_back(processName);
	if(flag_volume) m_volume.push_back(VolName);
}

void ProcessCountingSvc::CheckFilter(const G4Step* aStep){
	PASSEDFILTER = true;
	//switch
	if (!m_Switch) {PASSEDFILTER = false; return;}

	// get information at the beginning and at the end of step
	G4StepPoint* prePoint  = aStep->GetPreStepPoint() ;
	G4ThreeVector pointIn_mom = prePoint->GetMomentum();
	G4double pointIn_e = prePoint->GetTotalEnergy();
	G4double pointIn_pa = pointIn_mom.mag();
	G4double pointIn_time = prePoint->GetGlobalTime();//Time since the event in which the track belongs is created

	//*************************filter***********************
	//nTracks
	if (m_maxnTracks&&m_TrackIDs.size()>m_maxnTracks) {PASSEDFILTER = false; return;}
	//nSteps
	if (m_maxnSteps&&m_nSteps>=m_maxnSteps) {PASSEDFILTER = false; return;}
	//momentum
	if (m_minp&&pointIn_pa<m_minp) {PASSEDFILTER = false; return;}
	//energy
	if (m_mine&&pointIn_e<m_mine) {PASSEDFILTER = false; return;}
	//time window
	if (m_mint&&pointIn_time<m_mint) {PASSEDFILTER = false; return;}
	if (m_maxt&&pointIn_time>m_maxt) {PASSEDFILTER = false; return;}
	if (m_Volumes.size()>0){
		PASSEDFILTER = false;
		for ( int i = 0; i< m_Volumes.size(); i++ ){
			if ( VolName == m_Volumes[i] ){
				PASSEDFILTER = true;
				break;
			}
		}
	}
}

void ProcessCountingSvc::CheckTrack(const G4Step* aStep){
	G4Track* aTrack = aStep->GetTrack() ;
	G4int trackID= aTrack->GetTrackID(); //G4 track ID of current track.
	//std::cout<<"trackID = "<<trackID<<std::endl;
	int i = 0;
	for ( ; i < m_TrackIDs.size(); ){
		//std::cout<<"m_TrackIDs["<<i<<"] = "<<m_TrackIDs[i]<<std::endl;
		if ( trackID == m_TrackIDs[i] ){
			//std::cout<<"The Same!"<<std::endl;
			break;
		}
		i++;
	}
	//std::cout<<"i = "<<i<<std::endl;
	if ( i == m_TrackIDs.size() ){
		//std::cout<<"Added track ["<<trackID<<"]"<<std::endl;
		m_TrackIDs.push_back(trackID);
	}
}
