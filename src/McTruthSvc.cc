//---------------------------------------------------------------------------//
//Description: In charge of McTruth
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "McTruthSvc.hh"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "globals.hh"
#include "G4Event.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"

#include "MyString2Anything.hh"

#include "MyRoot.hh"

McTruthSvc* McTruthSvc::fMcTruthSvc = 0;

McTruthSvc::McTruthSvc()
{
	if (fMcTruthSvc){
		G4Exception("McTruthSvc::McTruthSvc()","Run0031",
				FatalException, "McTruthSvc constructed twice.");
	}
	fMcTruthSvc = this;
	ReSet();
}

McTruthSvc::~McTruthSvc()
{
	printf("~McTruthSvc\n");
}

McTruthSvc* McTruthSvc::GetMcTruthSvc(){
	if ( !fMcTruthSvc ){
		fMcTruthSvc = new McTruthSvc;
	}
	return fMcTruthSvc;
}

void McTruthSvc::Initialize(){
	m_nTracks = 0;
	m_pid.clear();
	m_tid.clear();
	m_ptid.clear();
	m_time.clear();
	m_px.clear();
	m_py.clear();
	m_pz.clear();
	m_e.clear();
	m_x.clear();
	m_y.clear();
	m_z.clear();
	m_particleName.clear();
	m_charge.clear();
	m_process.clear();
	m_volume.clear();
}

void McTruthSvc::SetBranch(){
	if (!m_Switch) return;
	MyRoot* myRoot = MyRoot::GetMyRoot();
	if( flag_nTracks ) myRoot->SetBranch("McTruth_nTracks", &m_nTracks);
	if( flag_pid ) myRoot->SetBranch("McTruth_pid", &m_pid);
	if( flag_tid ) myRoot->SetBranch("McTruth_tid", &m_tid);
	if( flag_ptid ) myRoot->SetBranch("McTruth_ptid", &m_ptid);
	if( flag_time ) myRoot->SetBranch("McTruth_time", &m_time);
	if( flag_px ) myRoot->SetBranch("McTruth_px", &m_px);
	if( flag_py ) myRoot->SetBranch("McTruth_py", &m_py);
	if( flag_pz ) myRoot->SetBranch("McTruth_pz", &m_pz);
	if( flag_e ) myRoot->SetBranch("McTruth_e", &m_e);
	if( flag_x ) myRoot->SetBranch("McTruth_x", &m_x);
	if( flag_y ) myRoot->SetBranch("McTruth_y", &m_y);
	if( flag_z ) myRoot->SetBranch("McTruth_z", &m_z);
	if( flag_charge ) myRoot->SetBranch("McTruth_charge", &m_charge);
	if( flag_particleName ) myRoot->SetBranch("McTruth_particleName", &m_particleName);
	if( flag_process ) myRoot->SetBranch("McTruth_process", &m_process);
	if( flag_volume ) myRoot->SetBranch("McTruth_volume", &m_volume);
}

void McTruthSvc::ReadOutputCard(G4String filename){
	ReSet();
	std::ifstream fin_card(filename);
	if(!fin_card){
		std::cout<<"In McTruthSvc::ReadOutputCard, cannot open "<<filename<<"!!!"<<std::endl;
		G4Exception("McTruthSvc::ReadOutputCard()",
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
			if ( name == "MCTRUTH_SECTION" ){
				n_output_section_symbol++;
			}
		}
		else if ( n_output_section_symbol == 1 ){
			if ( name == "MCTRUTH_SECTION" ){
				n_output_section_symbol++;
			}
			else if( name == "nTracks" ) flag_nTracks = true;
			else if( name == "pid" ) flag_pid = true;
			else if( name == "tid" ) flag_tid = true;
			else if( name == "ptid" ) flag_ptid = true;
			else if( name == "time" ) {flag_time = true; buf_card>>unitName_time; unit_time = MyString2Anything::get_U(unitName_time);}
			else if( name == "px" ) {flag_px = true; buf_card>>unitName_px; unit_px = MyString2Anything::get_U(unitName_px);}
			else if( name == "py" ) {flag_py = true; buf_card>>unitName_py; unit_py = MyString2Anything::get_U(unitName_py);}
			else if( name == "pz" ) {flag_pz = true; buf_card>>unitName_pz; unit_pz = MyString2Anything::get_U(unitName_pz);}
			else if( name == "e" ) {flag_e = true; buf_card>>unitName_e; unit_e = MyString2Anything::get_U(unitName_e);}
			else if( name == "x" ) {flag_x = true; buf_card>>unitName_x; unit_x = MyString2Anything::get_U(unitName_x);}
			else if( name == "y" ) {flag_y = true; buf_card>>unitName_y; unit_y = MyString2Anything::get_U(unitName_y);}
			else if( name == "z" ) {flag_z = true; buf_card>>unitName_z; unit_z = MyString2Anything::get_U(unitName_z);}
			else if( name == "particleName" ) flag_particleName = true;
			else if( name == "charge" ) flag_charge = true;
			else if( name == "process" ) flag_process = true;
			else if( name == "volume" ) flag_volume = true;
			else{
				std::cout<<"In McTruthSvc::ReadOutputCard, unknown name: "<<name<<" in file "<<filename<<std::endl;
				std::cout<<"Will ignore this line!"<<std::endl;
			}
		}

		if ( n_filter_section_symbol == 0 ){
			if ( name == "MCTRUTHFILTER_SECTION" ){
				n_filter_section_symbol++;
			}
		}
		else if ( n_filter_section_symbol == 1 ){
			if ( name == "MCTRUTHFILTER_SECTION" ){
				n_filter_section_symbol++;
			}
			else if( name == "Switch" ) m_Switch = true;
			else if( name == "nTracks" ) buf_card>>m_maxnTracks;
			else if( name == "WL" ){
				int pid = 0;
				buf_card>>pid;
				white_list.push_back(pid);
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
					std::cout<<"In McTruthSvc::ReadOutputCard, unknown name: "<<name<<" in file "<<filename<<std::endl;
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
		std::cout<<"In McTruthSvc::ReadOutputCard, failed to find enough section seperators for output in file "<<filename<<std::endl;
		std::cout<<"Will use default settings."<<std::endl;
		std::cout<<"********************************************"<<std::endl;
	}
	if ( n_filter_section_symbol<= 1 ){
		std::cout<<"*****************WARNING********************"<<std::endl;
		std::cout<<"In McTruthSvc::ReadOutputCard, failed to find enough section seperators for filter in file "<<filename<<std::endl;
		std::cout<<"Will use default settings."<<std::endl;
		std::cout<<"********************************************"<<std::endl;
	}
	fin_card.close();
	ShowOutCard();
}

void McTruthSvc::ReSet(){
	flag_nTracks = false;
	flag_pid = false;
	flag_tid = false;
	flag_ptid = false;
	flag_time = false;
	flag_px = false;
	flag_py = false;
	flag_pz = false;
	flag_e = false;
	flag_x = false;
	flag_y = false;
	flag_z = false;
	flag_charge = false;
	flag_particleName = false;
	flag_process = false;
	flag_volume = false;
	m_Switch = false;
	m_minp = 0;
	m_mine = 0;
	m_maxnTracks = 0;
	m_mint = 0;
	m_maxt = 0;
	white_list.clear();
}

void McTruthSvc::ShowOutCard(){
	std::cout<<"*************************Output settings for McTruthSvc***************************"<<std::endl;
	std::cout<<"output nTracks?       "<<(flag_nTracks?" yes":" no")<<std::endl;
	std::cout<<"output pid?           "<<(flag_pid?" yes":" no")<<std::endl;
	std::cout<<"output tid?           "<<(flag_tid?" yes":" no")<<std::endl;
	std::cout<<"output ptid?          "<<(flag_ptid?" yes":" no")<<std::endl;
	std::cout<<"output time?          "<<(flag_time?" yes":" no")<<", unit: "<<unitName_time<<std::endl;
	std::cout<<"output px?            "<<(flag_px?" yes":" no")<<", unit: "<<unitName_px<<std::endl;
	std::cout<<"output py?            "<<(flag_py?" yes":" no")<<", unit: "<<unitName_py<<std::endl;
	std::cout<<"output pz?            "<<(flag_pz?" yes":" no")<<", unit: "<<unitName_pz<<std::endl;
	std::cout<<"output e?             "<<(flag_e?" yes":" no")<<", unit: "<<unitName_e<<std::endl;
	std::cout<<"output x?             "<<(flag_x?" yes":" no")<<", unit: "<<unitName_x<<std::endl;
	std::cout<<"output y?             "<<(flag_y?" yes":" no")<<", unit: "<<unitName_y<<std::endl;
	std::cout<<"output z?             "<<(flag_z?" yes":" no")<<", unit: "<<unitName_z<<std::endl;
	std::cout<<"output particleName?  "<<(flag_particleName?" yes":" no")<<std::endl;
	std::cout<<"output charge?        "<<(flag_charge?" yes":" no")<<std::endl;
	std::cout<<"output process?       "<<(flag_process?" yes":" no")<<std::endl;
	std::cout<<"output volume?        "<<(flag_volume?" yes":" no")<<std::endl;
	std::cout<<"Switch on?            "<<(m_Switch?"yes":"no")<<std::endl;
	std::cout<<"minp =                "<<m_minp/MeV<<"MeV"<<std::endl;
	std::cout<<"mine =                "<<m_mine/MeV<<"MeV"<<std::endl;
	std::cout<<"maxnTracks =          "<<m_maxnTracks<<std::endl;
	std::cout<<"mint =                "<<m_mint/ns<<"ns"<<std::endl;
	std::cout<<"maxt =                "<<m_maxt/ns<<"ns"<<std::endl;
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

void McTruthSvc::SetValuePre(const G4Track* aTrack){
	//switch
	if (!m_Switch) return;
	//nTracks
	if (m_maxnTracks&&m_nTracks>=m_maxnTracks) return;
	//momentum
	if (m_minp&&aTrack->GetMomentum().mag()<m_minp) return;
	//energy
	if (m_mine&&aTrack->GetTotalEnergy()<m_mine) return;
	//time window
	G4double globalT=aTrack->GetGlobalTime();//Time since the event in which the track belongs is created
	if (m_mint&&globalT<m_mint) return;
	if (m_maxt&&globalT>m_maxt) return;
	// white_list
	G4int pid = aTrack->GetParticleDefinition()->GetPDGEncoding();
	bool foundit = false;
	for (int i = 0; i<white_list.size(); i++){
		if (pid == white_list[i]) foundit=true;
	}
	if (!foundit) return;

	G4String processName;
	const G4VProcess* process = aTrack->GetCreatorProcess();
	if (process) {
		processName = process->GetProcessName();
	}
	else{
		processName = "NULL";
	}
	G4int trackID= aTrack->GetTrackID(); //G4 track ID of current track.
	G4int ptid = aTrack->GetParentID(); //parent G4 track ID of current track.
	G4ThreeVector mom_3vec = aTrack->GetMomentum();
	G4ThreeVector pos_3vec = aTrack->GetVertexPosition();
	std::string volume = aTrack->GetLogicalVolumeAtVertex()->GetName();
	std::string particleName = aTrack->GetParticleDefinition()->GetParticleName();
	int charge = aTrack->GetParticleDefinition()->GetPDGCharge();
	G4double energy = aTrack->GetTotalEnergy();

	m_nTracks++;
	if(flag_pid) m_pid.push_back(pid);
	if(flag_tid) m_tid.push_back(trackID);
	if(flag_ptid) m_ptid.push_back(ptid);
	if(flag_time) m_time.push_back(globalT/unit_time);
	if(flag_px) m_px.push_back(mom_3vec.x()/unit_px);
	if(flag_py) m_py.push_back(mom_3vec.y()/unit_py);
	if(flag_pz) m_pz.push_back(mom_3vec.z()/unit_pz);
	if(flag_e) m_e.push_back(energy/unit_e);
	if(flag_x) m_x.push_back(pos_3vec.x()/unit_x);
	if(flag_y) m_y.push_back(pos_3vec.y()/unit_y);
	if(flag_z) m_z.push_back(pos_3vec.z()/unit_z);
	if(flag_charge) m_charge.push_back(charge);
	if(flag_particleName) m_particleName.push_back(particleName);
	if(flag_process) m_process.push_back(processName);
	if(flag_volume) m_volume.push_back(volume);
}

void McTruthSvc::SetValuePost(const G4Track* aTrack){
	//	G4int   nbSteps = aTrack->GetCurrentStepNumber();
	//	G4double trackLength = aTrack->GetTrackLength();
}
