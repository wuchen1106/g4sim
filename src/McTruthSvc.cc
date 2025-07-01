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

#include "myglobals.hh"
#include "G4Event.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4Step.hh"

#include "MyString2Anything.hh"
#include "Hstar10Converter.hh"

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

	m_fluence2H10_neutron  = new Hstar10Converter();
	m_fluence2H10_photon   = new Hstar10Converter();
	m_fluence2H10_proton   = new Hstar10Converter();
	m_fluence2H10_electron = new Hstar10Converter();
	m_fluence2H10_positron = new Hstar10Converter();
	m_fluence2H10_muonp    = new Hstar10Converter();
	m_fluence2H10_muonm    = new Hstar10Converter();
	m_fluence2H10_pionp    = new Hstar10Converter();
	m_fluence2H10_pionm    = new Hstar10Converter();
	m_fluence2H10_directory = "";
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
	m_nTracksAll = 0;
	m_nTracks = 0;
	m_dictpid.clear();
	m_dicttid.clear();
	m_dicttime.clear();
	m_pid.clear();
	m_tid.clear();
	m_ptid.clear();
	m_ppid.clear();
	m_time.clear();
	m_tcost.clear();
	m_px.clear();
	m_py.clear();
	m_pz.clear();
	m_e.clear();
	m_ekin.clear();
	m_x.clear();
	m_y.clear();
	m_z.clear();
	m_step_t.clear();
	m_step_x.clear();
	m_step_y.clear();
	m_step_z.clear();
	m_step_px.clear();
	m_step_py.clear();
	m_step_pz.clear();
	m_step_ekin.clear();
	m_particleName.clear();
	m_charge.clear();
	m_process.clear();
	m_volume.clear();
}

void McTruthSvc::InitializeRun(){
    m_xyz2edep.clear();
    m_flux_surfaces.clear();
    if (m_fluence2H10_directory!=""){
        m_fluence2H10_neutron ->LoadFromFile(m_fluence2H10_directory+"/adene.dat");  
        m_fluence2H10_photon  ->LoadFromFile(m_fluence2H10_directory+"/adeph.dat");  
        m_fluence2H10_proton  ->LoadFromFile(m_fluence2H10_directory+"/adepr.dat");  
        m_fluence2H10_electron->LoadFromFile(m_fluence2H10_directory+"/adeel.dat");  
        m_fluence2H10_positron->LoadFromFile(m_fluence2H10_directory+"/adepo.dat");  
        m_fluence2H10_muonp   ->LoadFromFile(m_fluence2H10_directory+"/ademp.dat");  
        m_fluence2H10_muonm   ->LoadFromFile(m_fluence2H10_directory+"/ademm.dat");  
        m_fluence2H10_pionp   ->LoadFromFile(m_fluence2H10_directory+"/adepp.dat");  
        m_fluence2H10_pionm   ->LoadFromFile(m_fluence2H10_directory+"/adepm.dat");  

        SurfacePlane* myPlane = new SurfacePlane(
                "XY",
                G4ThreeVector(0, 0, 0),
                G4ThreeVector(0, 0, 1),
                G4ThreeVector(1, 0, 0),
                G4ThreeVector(0, 1, 0),
                -2000,2000,-1600,4000,
                400,560
                );
        m_flux_surfaces.push_back(myPlane);

        myPlane = new SurfacePlane(
                "ZY",
                G4ThreeVector(0, 0, 0),
                G4ThreeVector(1, 0, 0),
                G4ThreeVector(0, 0, 1),
                G4ThreeVector(0, 1, 0),
                -1700,3300,-1600,4000,
                500,560
                );
        m_flux_surfaces.push_back(myPlane);

        myPlane = new SurfacePlane(
                "ZX",
                G4ThreeVector(0, 0, 0),
                G4ThreeVector(0, 1, 0),
                G4ThreeVector(0, 0, 1),
                G4ThreeVector(1, 0, 0),
                -1700,3300,-2000,2000,
                500,400
                );
        m_flux_surfaces.push_back(myPlane);

        myPlane = new SurfacePlane(
                "ZX2200",
                G4ThreeVector(0, 2200, 0),
                G4ThreeVector(0, 1, 0),
                G4ThreeVector(0, 0, 1),
                G4ThreeVector(1, 0, 0),
                -1700,3300,-2000,2000,
                500,400
                );
        m_flux_surfaces.push_back(myPlane);
    }
}

void McTruthSvc::SetBranch(){
	if (!m_Switch) return;
	MyRoot* myRoot = MyRoot::GetMyRoot();
	if( flag_nTracks ) myRoot->SetBranch("McTruth_nTracks", &m_nTracks);
	if( flag_pid ) myRoot->SetBranch("McTruth_pid", &m_pid);
	if( flag_tid ) myRoot->SetBranch("McTruth_tid", &m_tid);
	if( flag_ptid ) myRoot->SetBranch("McTruth_ptid", &m_ptid);
	if( flag_ppid ) myRoot->SetBranch("McTruth_ppid", &m_ppid);
	if( flag_time ) myRoot->SetBranch("McTruth_time", &m_time);
	if( flag_tcost ) myRoot->SetBranch("McTruth_tcost", &m_tcost);
	if( flag_px ) myRoot->SetBranch("McTruth_px", &m_px);
	if( flag_py ) myRoot->SetBranch("McTruth_py", &m_py);
	if( flag_pz ) myRoot->SetBranch("McTruth_pz", &m_pz);
	if( flag_e ) myRoot->SetBranch("McTruth_e", &m_e);
	if( flag_ekin ) myRoot->SetBranch("McTruth_ekin", &m_ekin);
	if( flag_x ) myRoot->SetBranch("McTruth_x", &m_x);
	if( flag_y ) myRoot->SetBranch("McTruth_y", &m_y);
	if( flag_z ) myRoot->SetBranch("McTruth_z", &m_z);
	if( flag_step_t ) myRoot->SetBranch("McTruth_step_t", &m_step_t);
	if( flag_step_x ) myRoot->SetBranch("McTruth_step_x", &m_step_x);
	if( flag_step_y ) myRoot->SetBranch("McTruth_step_y", &m_step_y);
	if( flag_step_z ) myRoot->SetBranch("McTruth_step_z", &m_step_z);
	if( flag_step_px ) myRoot->SetBranch("McTruth_step_px", &m_step_px);
	if( flag_step_py ) myRoot->SetBranch("McTruth_step_py", &m_step_py);
	if( flag_step_pz ) myRoot->SetBranch("McTruth_step_pz", &m_step_pz);
	if( flag_step_ekin ) myRoot->SetBranch("McTruth_step_ekin", &m_step_ekin);
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
			else if( name == "tid2pid" ) flag_tid2pid= true;
			else if( name == "tid2time" ) flag_tid2time= true;
			else if( name == "tid" ) flag_tid = true;
			else if( name == "ptid" ) flag_ptid = true;
			else if( name == "ppid" ) flag_ppid = true;
			else if( name == "time" ) {flag_time = true; buf_card>>unitName_time; unit_time = MyString2Anything::get_U(unitName_time);}
			else if( name == "tcost" ) {flag_tcost = true; buf_card>>unitName_time;}
			else if( name == "px" ) {flag_px = true; buf_card>>unitName_px; unit_px = MyString2Anything::get_U(unitName_px);}
			else if( name == "py" ) {flag_py = true; buf_card>>unitName_py; unit_py = MyString2Anything::get_U(unitName_py);}
			else if( name == "pz" ) {flag_pz = true; buf_card>>unitName_pz; unit_pz = MyString2Anything::get_U(unitName_pz);}
			else if( name == "e" ) {flag_e = true; buf_card>>unitName_e; unit_e = MyString2Anything::get_U(unitName_e);}
			else if( name == "ekin" ) {flag_ekin = true; buf_card>>unitName_ekin; unit_ekin = MyString2Anything::get_U(unitName_ekin);}
			else if( name == "x" ) {flag_x = true; buf_card>>unitName_x; unit_x = MyString2Anything::get_U(unitName_x);}
			else if( name == "y" ) {flag_y = true; buf_card>>unitName_y; unit_y = MyString2Anything::get_U(unitName_y);}
			else if( name == "z" ) {flag_z = true; buf_card>>unitName_z; unit_z = MyString2Anything::get_U(unitName_z);}
			else if( name == "step_t" ) {flag_step_t = true; buf_card>>unitName_step_t; unit_step_t = MyString2Anything::get_U(unitName_step_t);}
			else if( name == "step_x" ) {flag_step_x = true; buf_card>>unitName_step_x; unit_step_x = MyString2Anything::get_U(unitName_step_x);}
			else if( name == "step_y" ) {flag_step_y = true; buf_card>>unitName_step_y; unit_step_y = MyString2Anything::get_U(unitName_step_y);}
			else if( name == "step_z" ) {flag_step_z = true; buf_card>>unitName_step_z; unit_step_z = MyString2Anything::get_U(unitName_step_z);}
			else if( name == "step_px" ) {flag_step_px = true; buf_card>>unitName_step_px; unit_step_px = MyString2Anything::get_U(unitName_step_px);}
			else if( name == "step_py" ) {flag_step_py = true; buf_card>>unitName_step_py; unit_step_py = MyString2Anything::get_U(unitName_step_py);}
			else if( name == "step_pz" ) {flag_step_pz = true; buf_card>>unitName_step_pz; unit_step_pz = MyString2Anything::get_U(unitName_step_pz);}
			else if( name == "step_ekin" ) {flag_step_ekin = true; buf_card>>unitName_step_ekin; unit_step_ekin = MyString2Anything::get_U(unitName_step_ekin);}
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
			else if( name == "BL" ){
				int pid = 0;
				buf_card>>pid;
				black_list.push_back(pid);
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
	flag_tid2pid = false;
	flag_tid2time = false;
	flag_tid = false;
	flag_ptid = false;
	flag_ppid = false;
	flag_time = false;
	flag_tcost = false;
	flag_px = false;
	flag_py = false;
	flag_pz = false;
	flag_e = false;
	flag_ekin = false;
	flag_x = false;
	flag_y = false;
	flag_z = false;
	flag_step_t = false;
	flag_step_x = false;
	flag_step_y = false;
	flag_step_z = false;
	flag_step_px = false;
	flag_step_py = false;
	flag_step_pz = false;
	flag_step_ekin = false;
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
	black_list.clear();
	unitName_time="s";
	unitName_px	="GeV";
	unitName_py	="GeV";
	unitName_pz	="GeV";
	unitName_e	="GeV";
	unitName_x	="cm";
	unitName_y	="cm";
	unitName_z	="cm";
	unit_time=s;
	unit_px	=GeV;
	unit_py	=GeV;
	unit_pz	=GeV;
	unit_e	=GeV;
	unit_x	=cm;
	unit_y	=cm;
	unit_z	=cm;
	// Below are put into SetMapEdep function, not available in card. Consider to move them to card-setting style in the future.
//	m_map_step = 0*mm;
//	m_map_xmin = 0;
//	m_map_xmax = 0;
//	m_map_ymin = 0;
//	m_map_ymax = 0;
//	m_map_zmin = 0;
//	m_map_zmax = 0;
}

void McTruthSvc::ShowOutCard(){
	std::cout<<"*************************Output settings for McTruthSvc***************************"<<std::endl;
	std::cout<<"output nTracks?       "<<(flag_nTracks?" yes":" no")<<std::endl;
	std::cout<<"output pid?           "<<(flag_pid?" yes":" no")<<std::endl;
	std::cout<<"enable tid2pid?       "<<(flag_tid2pid?" yes":" no")<<std::endl;
	std::cout<<"enable tid2time?      "<<(flag_tid2time?" yes":" no")<<std::endl;
	std::cout<<"output tid?           "<<(flag_tid?" yes":" no")<<std::endl;
	std::cout<<"output ptid?          "<<(flag_ptid?" yes":" no")<<std::endl;
	std::cout<<"output ppid?          "<<(flag_ppid?" yes":" no")<<std::endl;
	std::cout<<"output time?          "<<(flag_time?" yes":" no")<<", unit: "<<unitName_time<<std::endl;
	std::cout<<"output CPU time cost? "<<(flag_tcost?" yes":" no")<<", unit: CPU second"<<std::endl;
	std::cout<<"output px?            "<<(flag_px?" yes":" no")<<", unit: "<<unitName_px<<std::endl;
	std::cout<<"output py?            "<<(flag_py?" yes":" no")<<", unit: "<<unitName_py<<std::endl;
	std::cout<<"output pz?            "<<(flag_pz?" yes":" no")<<", unit: "<<unitName_pz<<std::endl;
	std::cout<<"output e?             "<<(flag_e?" yes":" no")<<", unit: "<<unitName_e<<std::endl;
	std::cout<<"output ekin?          "<<(flag_ekin?" yes":" no")<<", unit: "<<unitName_e<<std::endl;
	std::cout<<"output x?             "<<(flag_x?" yes":" no")<<", unit: "<<unitName_x<<std::endl;
	std::cout<<"output y?             "<<(flag_y?" yes":" no")<<", unit: "<<unitName_y<<std::endl;
	std::cout<<"output z?             "<<(flag_z?" yes":" no")<<", unit: "<<unitName_z<<std::endl;
	std::cout<<"output step t?        "<<(flag_step_t?" yes":" no")<<", unit: "<<unitName_step_t<<std::endl;
	std::cout<<"output step x?        "<<(flag_step_x?" yes":" no")<<", unit: "<<unitName_step_x<<std::endl;
	std::cout<<"output step y?        "<<(flag_step_y?" yes":" no")<<", unit: "<<unitName_step_y<<std::endl;
	std::cout<<"output step z?        "<<(flag_step_z?" yes":" no")<<", unit: "<<unitName_step_z<<std::endl;
	std::cout<<"output step px?        "<<(flag_step_px?" yes":" no")<<", unit: "<<unitName_step_px<<std::endl;
	std::cout<<"output step py?        "<<(flag_step_py?" yes":" no")<<", unit: "<<unitName_step_py<<std::endl;
	std::cout<<"output step pz?        "<<(flag_step_pz?" yes":" no")<<", unit: "<<unitName_step_pz<<std::endl;
	std::cout<<"output step ekin?        "<<(flag_step_ekin?" yes":" no")<<", unit: "<<unitName_step_ekin<<std::endl;
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
	std::cout<<"black list:     "<<std::endl;
	for ( int i = 0; i< black_list.size(); i++){
		std::cout <<"  Tracks with these following PDGCodes will NOT be recorded:"<<std::endl;
		std::cout<<"            "<<i<<": "<<black_list[i]<<std::endl;
	}
        std::cout<<"edep map step         "<<m_map_step/mm<<" mm"<<std::endl;
        std::cout<<"edep map x range      "<<m_map_xmin/mm<<" ~ "<<m_map_xmax<<" mm"<<std::endl;
        std::cout<<"edep map y range      "<<m_map_ymin/mm<<" ~ "<<m_map_ymax<<" mm"<<std::endl;
        std::cout<<"edep map z range      "<<m_map_zmin/mm<<" ~ "<<m_map_zmax<<" mm"<<std::endl;
	std::cout<<"******************************************************************************"<<std::endl;
}

void McTruthSvc::SetValuePre(const G4Track* aTrack){
	G4int pid = aTrack->GetParticleDefinition()->GetPDGEncoding();
	G4double globalT=aTrack->GetGlobalTime();//Time since the event in which the track belongs is created
	G4int trackID= aTrack->GetTrackID(); //G4 track ID of current track.
	if (flag_tid2pid||flag_tid2time){
		m_dicttid.push_back(trackID);
		if (flag_tid2pid){
			m_dictpid.push_back(pid);
		}
		if (flag_tid2time){
			m_dicttime.push_back(globalT);
		}
	}
	m_nTracksAll++;
	//switch
	if (!m_Switch) return;
	//nTracks
	if (m_maxnTracks&&m_nTracks>=m_maxnTracks) return;
	//momentum
	if (m_minp&&aTrack->GetMomentum().mag()<m_minp) return;
	//energy
	if (m_mine&&aTrack->GetTotalEnergy()<m_mine) return;
	//time window
	if (m_mint&&globalT<m_mint) return;
	if (m_maxt&&globalT>m_maxt) return;
	// white_list
	bool foundit = false;
	for (int i = 0; i<white_list.size(); i++){
		if (pid == white_list[i]) foundit=true;
		if (white_list[i]==0&&pid<1e6) foundit = true;
		if (white_list[i]==1e6&&pid>=1e6) foundit = true;
		if (white_list[i]==-1&&trackID==1) foundit = true;
	}
	if (!foundit&&white_list.size()) return;
	// black_list
	foundit = false;
	for (int i = 0; i<black_list.size(); i++){
		if (pid == black_list[i]) foundit=true;
		if (black_list[i]==0&&pid<1e6) foundit = true;
		if (black_list[i]==1e6&&pid>=1e6) foundit = true;
		if (black_list[i]==-1&&trackID==1) foundit = true;
	}
	if (foundit) return;

	G4String processName;
	const G4VProcess* process = aTrack->GetCreatorProcess();
	if (process) {
		processName = process->GetProcessName();
	}
	else{
		processName = "NULL";
	}
	G4int ptid = aTrack->GetParentID(); //parent G4 track ID of current track.
	G4ThreeVector mom_3vec = aTrack->GetMomentum();
	G4ThreeVector pos_3vec = aTrack->GetVertexPosition();
	std::string volume = aTrack->GetLogicalVolumeAtVertex()?aTrack->GetLogicalVolumeAtVertex()->GetName():"NULL";
	std::string particleName = aTrack->GetParticleDefinition()->GetParticleName();
	int charge = aTrack->GetParticleDefinition()->GetPDGCharge();
	G4double energy = aTrack->GetTotalEnergy();
	G4double kinenergy = aTrack->GetKineticEnergy();

	m_nTracks++;
	if(flag_pid) m_pid.push_back(pid);
	if(flag_tid) m_tid.push_back(trackID);
	if(flag_ptid) m_ptid.push_back(ptid);
	if(flag_ppid){
		int ptid = aTrack->GetParentID();
		int ppid = McTruthSvc::GetMcTruthSvc()->tid2pid(ptid);
		m_ppid.push_back(ppid);
	}
	if(flag_time) m_time.push_back(globalT/unit_time);
	if(flag_tcost) m_tcost.push_back(0);
	if(flag_px) m_px.push_back(mom_3vec.x()/unit_px);
	if(flag_py) m_py.push_back(mom_3vec.y()/unit_py);
	if(flag_pz) m_pz.push_back(mom_3vec.z()/unit_pz);
	if(flag_e) m_e.push_back(energy/unit_e);
	if(flag_ekin) m_ekin.push_back(kinenergy/unit_ekin);
	if(flag_x) m_x.push_back(pos_3vec.x()/unit_x);
	if(flag_y) m_y.push_back(pos_3vec.y()/unit_y);
	if(flag_z) m_z.push_back(pos_3vec.z()/unit_z);
	if(flag_step_t) {std::vector<double> step_t;m_step_t.push_back(step_t);}
	if(flag_step_x) {std::vector<double> step_x;m_step_x.push_back(step_x);}
	if(flag_step_y) {std::vector<double> step_y;m_step_y.push_back(step_y);}
	if(flag_step_z) {std::vector<double> step_z;m_step_z.push_back(step_z);}
	if(flag_step_px) {std::vector<double> step_px;m_step_px.push_back(step_px);}
	if(flag_step_py) {std::vector<double> step_py;m_step_py.push_back(step_py);}
	if(flag_step_pz) {std::vector<double> step_pz;m_step_pz.push_back(step_pz);}
	if(flag_step_ekin) {std::vector<double> step_ekin;m_step_ekin.push_back(step_ekin);}
	if(flag_charge) m_charge.push_back(charge);
	if(flag_particleName) m_particleName.push_back(particleName);
	if(flag_process) m_process.push_back(processName);
	if(flag_volume) m_volume.push_back(volume);
}

void McTruthSvc::SetValuePost(const G4Track* aTrack,double dt){
	//	G4int   nbSteps = aTrack->GetCurrentStepNumber();
	//	G4double trackLength = aTrack->GetTrackLength();
    if (flag_tcost) m_tcost.back() = dt;
}

void McTruthSvc::AddStep(const G4StepPoint* aStepPoint){
    if (flag_step_t) m_step_t.back().push_back(aStepPoint->GetGlobalTime()/unit_step_t);
    if (flag_step_x) m_step_x.back().push_back(aStepPoint->GetPosition().x()/unit_step_x);
    if (flag_step_y) m_step_y.back().push_back(aStepPoint->GetPosition().y()/unit_step_y);
    if (flag_step_z) m_step_z.back().push_back(aStepPoint->GetPosition().z()/unit_step_z);
    if (flag_step_px) m_step_px.back().push_back(aStepPoint->GetMomentum().x()/unit_step_px);
    if (flag_step_py) m_step_py.back().push_back(aStepPoint->GetMomentum().y()/unit_step_py);
    if (flag_step_pz) m_step_pz.back().push_back(aStepPoint->GetMomentum().z()/unit_step_pz);
    if (flag_step_ekin) m_step_ekin.back().push_back(aStepPoint->GetKineticEnergy()/unit_step_ekin);
}

void McTruthSvc::AddEdep2Map(double edep, double x, double y, double z){
    if (m_map_step<=0) return;
    if (edep>0){
        if (m_map_xmin!=m_map_xmax&&(x<m_map_xmin||x>m_map_xmax)) return;
        if (m_map_ymin!=m_map_ymax&&(y<m_map_ymin||y>m_map_ymax)) return;
        if (m_map_zmin!=m_map_zmax&&(z<m_map_zmin||z>m_map_zmax)) return;
        if (x<0) x-=m_map_step/2; else x+=m_map_step/2;
        if (y<0) y-=m_map_step/2; else y+=m_map_step/2;
        if (z<0) z-=m_map_step/2; else z+=m_map_step/2;
        short int ix = x/m_map_step;
        short int iy = y/m_map_step;
        short int iz = z/m_map_step;
        m_xyz2edep[ix][iy][iz] += edep;
    }
    return;
}

void McTruthSvc::RegisterStep2Dose(const G4Step* step){
    double E_MeV = step->GetPreStepPoint()->GetKineticEnergy() / MeV;
    int pid = step->GetTrack()->GetParticleDefinition()->GetPDGEncoding();
    double coeff_pSvcm2 = 0;
    if (pid==2112) coeff_pSvcm2 = m_fluence2H10_neutron->GetHstar10(E_MeV);
    else if (pid==22) coeff_pSvcm2 = m_fluence2H10_photon->GetHstar10(E_MeV);
    else if (pid==2212) coeff_pSvcm2 = m_fluence2H10_proton->GetHstar10(E_MeV);
    else if (pid==11) coeff_pSvcm2 = m_fluence2H10_electron->GetHstar10(E_MeV);
    else if (pid==-11) coeff_pSvcm2 = m_fluence2H10_positron->GetHstar10(E_MeV);
    else if (pid==13) coeff_pSvcm2 = m_fluence2H10_muonm->GetHstar10(E_MeV);
    else if (pid==-13) coeff_pSvcm2 = m_fluence2H10_muonp->GetHstar10(E_MeV);
    else if (pid==211) coeff_pSvcm2 = m_fluence2H10_pionp->GetHstar10(E_MeV);
    else if (pid==-211) coeff_pSvcm2 = m_fluence2H10_pionm->GetHstar10(E_MeV);
    else return;
    G4ThreeVector midPoint = 0.5 * (step->GetPreStepPoint()->GetPosition() + step->GetPostStepPoint()->GetPosition());

    for (auto plane : m_flux_surfaces) {
        if (plane->IsCrossing(step)) {
            // Use midpoint for binning
            plane->Fill(midPoint, coeff_pSvcm2); // 1 particle crossing → add H*(10)
        }
    }

    return;

}

void McTruthSvc::EndOfRunAction(){
    for (auto plane : m_flux_surfaces) {
        plane->Write();
    }

    if (m_map_step<=0) return;
    TTree * otree = new TTree("mapEdep","map of energy deposit");
    short int x,y,z;
    float edep;
    otree->Branch("edep",&edep);
    otree->Branch("x",&x);
    otree->Branch("y",&y);
    otree->Branch("z",&z);
    for (std::map<short int, std::map<short int, std::map<short int, float> > >::const_iterator itx = m_xyz2edep.begin(); itx!=m_xyz2edep.end(); itx++){
        short int ix = itx->first;
        for (std::map<short int, std::map<short int, float> >::const_iterator ity = itx->second.begin(); ity!=itx->second.end(); ity++){
            short int iy = ity->first;
            for (std::map<short int, float>::const_iterator itz = ity->second.begin(); itz!=ity->second.end(); itz++){
                short int iz = itz->first;
                x = ix*m_map_step;
                y = iy*m_map_step;
                z = iz*m_map_step;
                edep = itz->second;
                otree->Fill();
            }
        }
    }
    otree->Write();

    return;
}
