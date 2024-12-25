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
#include "SimpleGeometryParameter.hh"
#include "MonitorHit.hh"
#include "MyRoot.hh"
#include "MySD.hh"
#include "McTruthSvc.hh"
#include "PrimaryGeneratorAction.hh"

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
	pPrimaryGeneratorAction = PrimaryGeneratorAction::GetPrimaryGeneratorAction();
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
	m_Ox.clear();
	m_Oy.clear();
	m_Oz.clear();
	m_x.clear();
	m_y.clear();
	m_z.clear();
	m_t.clear();
	m_Ot.clear();
	m_Opx.clear();
	m_Opy.clear();
	m_Opz.clear();
	m_Oekin.clear();
	m_px.clear();
	m_py.clear();
	m_pz.clear();
	m_ox.clear();
	m_oy.clear();
	m_oz.clear();
	m_ot.clear();
	m_opx.clear();
	m_opy.clear();
	m_opz.clear();
	m_ekin.clear();
	m_e.clear();
	m_nph.clear();
	m_edep.clear();
	m_edepAll.clear();
	m_edepDelta.clear();
	m_edepSec.clear();
	m_stepL.clear();
	m_volID.clear();
	m_volName.clear();
	m_ppid.clear();
	m_ptid.clear();
	m_oprocess.clear();
	m_ovolName.clear();
	m_tid.clear();
	m_pid.clear();
	m_particleName.clear();
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
	if( flag_Ox ) myRoot->SetBranch(volName+"_Ox", &m_Ox);
	if( flag_Oy ) myRoot->SetBranch(volName+"_Oy", &m_Oy);
	if( flag_Oz ) myRoot->SetBranch(volName+"_Oz", &m_Oz);
	if( flag_Ot ) myRoot->SetBranch(volName+"_Ot", &m_Ot);
	if( flag_x ) myRoot->SetBranch(volName+"_x", &m_x);
	if( flag_y ) myRoot->SetBranch(volName+"_y", &m_y);
	if( flag_z ) myRoot->SetBranch(volName+"_z", &m_z);
	if( flag_t ) myRoot->SetBranch(volName+"_t", &m_t);
	if( flag_Opx ) myRoot->SetBranch(volName+"_Opx", &m_Opx);
	if( flag_Opy ) myRoot->SetBranch(volName+"_Opy", &m_Opy);
	if( flag_Opz ) myRoot->SetBranch(volName+"_Opz", &m_Opz);
	if( flag_Oekin ) myRoot->SetBranch(volName+"_Oekin", &m_Oekin);
	if( flag_px ) myRoot->SetBranch(volName+"_px", &m_px);
	if( flag_py ) myRoot->SetBranch(volName+"_py", &m_py);
	if( flag_pz ) myRoot->SetBranch(volName+"_pz", &m_pz);
	if( flag_ox ) myRoot->SetBranch(volName+"_ox", &m_ox);
	if( flag_oy ) myRoot->SetBranch(volName+"_oy", &m_oy);
	if( flag_oz ) myRoot->SetBranch(volName+"_oz", &m_oz);
	if( flag_ot ) myRoot->SetBranch(volName+"_ot", &m_ot);
	if( flag_opx ) myRoot->SetBranch(volName+"_opx", &m_opx);
	if( flag_opy ) myRoot->SetBranch(volName+"_opy", &m_opy);
	if( flag_opz ) myRoot->SetBranch(volName+"_opz", &m_opz);
	if( flag_ekin ) myRoot->SetBranch(volName+"_ekin", &m_ekin);
	if( flag_e ) myRoot->SetBranch(volName+"_e", &m_e);
	if( flag_nph) myRoot->SetBranch(volName+"_nph", &m_nph);
	if( flag_edep ) myRoot->SetBranch(volName+"_edep", &m_edep);
	if( flag_edepAll ) myRoot->SetBranch(volName+"_edepAll", &m_edepAll);
	if( flag_edepDelta ) myRoot->SetBranch(volName+"_edepDelta", &m_edepDelta);
	if( flag_edepSec ) myRoot->SetBranch(volName+"_edepSec", &m_edepSec);
	if( flag_stepL ) myRoot->SetBranch(volName+"_stepL", &m_stepL);
	if( flag_volID ) myRoot->SetBranch(volName+"_volID", &m_volID);
	if( flag_volName ) myRoot->SetBranch(volName+"_volName", &m_volName);
	if( flag_ppid) myRoot->SetBranch(volName+"_ppid", &m_ppid);
	if( flag_ptid) myRoot->SetBranch(volName+"_ptid", &m_ptid);
	if( flag_oprocess) myRoot->SetBranch(volName+"_oprocess", &m_oprocess);
	if( flag_ovolName) myRoot->SetBranch(volName+"_ovolName", &m_ovolName);
	if( flag_tid ) myRoot->SetBranch(volName+"_tid", &m_tid);
	if( flag_pid ) myRoot->SetBranch(volName+"_pid", &m_pid);
	if( flag_particleName ) myRoot->SetBranch(volName+"_particleName", &m_particleName);
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
			else if( name == "Ox" ) {{flag_Ox = true; buf_card>>unitName_Ox; unit_Ox = MyString2Anything::get_U(unitName_Ox);}}
			else if( name == "Oy" ) {{flag_Oy = true; buf_card>>unitName_Oy; unit_Oy = MyString2Anything::get_U(unitName_Oy);}}
			else if( name == "Oz" ) {{flag_Oz = true; buf_card>>unitName_Oz; unit_Oz = MyString2Anything::get_U(unitName_Oz);}}
			else if( name == "Ot" ) {{flag_Ot = true; buf_card>>unitName_Ot; unit_Ot = MyString2Anything::get_U(unitName_Ot);}}
			else if( name == "x" ) {{flag_x = true; buf_card>>unitName_x; unit_x = MyString2Anything::get_U(unitName_x);}}
			else if( name == "y" ) {{flag_y = true; buf_card>>unitName_y; unit_y = MyString2Anything::get_U(unitName_y);}}
			else if( name == "z" ) {{flag_z = true; buf_card>>unitName_z; unit_z = MyString2Anything::get_U(unitName_z);}}
			else if( name == "t" ) {{flag_t = true; buf_card>>unitName_t; unit_t = MyString2Anything::get_U(unitName_t);}}
			else if( name == "Opx" ) {{flag_Opx = true; buf_card>>unitName_Opx; unit_Opx = MyString2Anything::get_U(unitName_Opx);}}
			else if( name == "Opy" ) {{flag_Opy = true; buf_card>>unitName_Opy; unit_Opy = MyString2Anything::get_U(unitName_Opy);}}
			else if( name == "Opz" ) {{flag_Opz = true; buf_card>>unitName_Opz; unit_Opz = MyString2Anything::get_U(unitName_Opz);}}
			else if( name == "Oekin" ) {{flag_Oekin = true; buf_card>>unitName_Oekin; unit_Oekin = MyString2Anything::get_U(unitName_Oekin);}}
			else if( name == "px" ) {{flag_px = true; buf_card>>unitName_px; unit_px = MyString2Anything::get_U(unitName_px);}}
			else if( name == "py" ) {{flag_py = true; buf_card>>unitName_py; unit_py = MyString2Anything::get_U(unitName_py);}}
			else if( name == "pz" ) {{flag_pz = true; buf_card>>unitName_pz; unit_pz = MyString2Anything::get_U(unitName_pz);}}
			else if( name == "ox" ) {{flag_ox = true; buf_card>>unitName_ox; unit_ox = MyString2Anything::get_U(unitName_ox);}}
			else if( name == "oy" ) {{flag_oy = true; buf_card>>unitName_oy; unit_oy = MyString2Anything::get_U(unitName_oy);}}
			else if( name == "oz" ) {{flag_oz = true; buf_card>>unitName_oz; unit_oz = MyString2Anything::get_U(unitName_oz);}}
			else if( name == "ot" ) {{flag_ot = true; buf_card>>unitName_ot; unit_ot = MyString2Anything::get_U(unitName_ot);}}
			else if( name == "opx" ) {{flag_opx = true; buf_card>>unitName_opx; unit_opx = MyString2Anything::get_U(unitName_opx);}}
			else if( name == "opy" ) {{flag_opy = true; buf_card>>unitName_opy; unit_opy = MyString2Anything::get_U(unitName_opy);}}
			else if( name == "opz" ) {{flag_opz = true; buf_card>>unitName_opz; unit_opz = MyString2Anything::get_U(unitName_opz);}}
			else if( name == "ekin" ) {{flag_ekin = true; buf_card>>unitName_ekin; unit_ekin = MyString2Anything::get_U(unitName_ekin);}}
			else if( name == "e" ) {{flag_e = true; buf_card>>unitName_e; unit_e = MyString2Anything::get_U(unitName_e);}}
			else if( name == "nph" ) {flag_nph= true;}
			else if( name == "edep" ) {{flag_edep = true; buf_card>>unitName_edep; unit_edep = MyString2Anything::get_U(unitName_edep);}}
			else if( name == "edepAll" ) {{flag_edepAll = true; buf_card>>unitName_edepAll; unit_edepAll = MyString2Anything::get_U(unitName_edepAll);}}
			else if( name == "edepDelta" ) {{flag_edepDelta = true; buf_card>>unitName_edepDelta; unit_edepDelta = MyString2Anything::get_U(unitName_edepDelta);}}
			else if( name == "edepSec" ) {{flag_edepSec = true; buf_card>>unitName_edepSec; unit_edepSec = MyString2Anything::get_U(unitName_edepSec);}}
			else if( name == "stepL" ) {{flag_stepL = true; buf_card>>unitName_stepL; unit_stepL = MyString2Anything::get_U(unitName_stepL);}}
			else if( name == "volID" ) {flag_volID = true;}
			else if( name == "volName" ) {flag_volName = true;}
			else if( name == "ppid" ) {flag_ppid = true;}
			else if( name == "ptid" ) {flag_ptid = true;}
			else if( name == "oprocess" ) {flag_oprocess = true;}
			else if( name == "ovolName" ) {flag_ovolName = true;}
			else if( name == "tid" ) {flag_tid = true;}
			else if( name == "pid" ) {flag_pid = true;}
			else if( name == "particleName" ) flag_particleName = true;
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
			else if( name == "stopped" ) needstopped = true;
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
				if( name == "maxp" ) maxp = para;
				else if( name == "mine" ) mine = para;
				else if ( name == "minnph" ) minnph = para;
				else if ( name == "minedep" ) minedep = para;
				else if ( name == "maxOekin" ) maxOekin = para;
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
	flag_Ox = false;
	flag_Oy = false;
	flag_Oz = false;
	flag_Ot = false;
	flag_x = false;
	flag_y = false;
	flag_z = false;
	flag_t = false;
	flag_Opx = false;
	flag_Opy = false;
	flag_Opz = false;
	flag_Oekin = false;
	flag_px = false;
	flag_py = false;
	flag_pz = false;
	flag_ox = false;
	flag_oy = false;
	flag_oz = false;
	flag_ot = false;
	flag_opx = false;
	flag_opy = false;
	flag_opz = false;
	flag_ekin = false;
	flag_e = false;
	flag_nph = false;
	flag_edep = false;
	flag_edepAll = false;
	flag_edepDelta = false;
	flag_edepSec = false;
	flag_stepL = false;
	flag_volID = false;
	flag_volName = false;
	flag_ppid = false;
	flag_ptid = false;
	flag_oprocess = false;
	flag_ovolName = false;
	flag_tid = false;
	flag_pid = false;
	flag_particleName = false;
	flag_charge = false;
	flag_stopped = false;
	flag_stop_time = false;
	flag_killed = false;
	flag_kill_time = false;
	//for fileter
	Switch = false;
	neutralCut = false;
	needstopped = false;
	minp = 0;
	maxp = 0;
	mine = 0;
	maxn = 0;
	mint = 0;
	maxt = 0;
	tres = 0;
	minnph = 0;
	minedep = -1*MeV;
	maxOekin = -1*MeV;
	white_list.clear();
	black_list.clear();
	//for units
	unitName_Ox = "cm";
	unitName_Oy = "cm";
	unitName_Oz = "cm";
	unitName_Ot = "ns";
	unitName_x = "cm";
	unitName_y = "cm";
	unitName_z = "cm";
	unitName_t = "ns";
	unitName_Opx = "GeV";
	unitName_Opy = "GeV";
	unitName_Opz = "GeV";
	unitName_Oekin = "GeV";
	unitName_px = "GeV";
	unitName_py = "GeV";
	unitName_pz = "GeV";
	unitName_ox = "cm";
	unitName_oy = "cm";
	unitName_oz = "cm";
	unitName_ot = "ns";
	unitName_opx = "GeV";
	unitName_opy = "GeV";
	unitName_opz = "GeV";
	unitName_ekin = "GeV";
	unitName_e = "GeV";
	unitName_edep = "GeV";
	unitName_edepAll = "GeV";
	unitName_edepDelta = "GeV";
	unitName_edepSec = "GeV";
	unitName_stepL = "cm";
	unitName_stop_time = "ns";
	unitName_kill_time = "ns";
	unit_Ox = MyString2Anything::get_U(unitName_Ox);
	unit_Oy = MyString2Anything::get_U(unitName_Oy);
	unit_Oz = MyString2Anything::get_U(unitName_Oz);
	unit_Ot = MyString2Anything::get_U(unitName_Ot);
	unit_x = MyString2Anything::get_U(unitName_x);
	unit_y = MyString2Anything::get_U(unitName_y);
	unit_z = MyString2Anything::get_U(unitName_z);
	unit_t = MyString2Anything::get_U(unitName_t);
	unit_Opx = MyString2Anything::get_U(unitName_Opx);
	unit_Opy = MyString2Anything::get_U(unitName_Opy);
	unit_Opz = MyString2Anything::get_U(unitName_Opz);
	unit_Oekin = MyString2Anything::get_U(unitName_Oekin);
	unit_px = MyString2Anything::get_U(unitName_px);
	unit_py = MyString2Anything::get_U(unitName_py);
	unit_pz = MyString2Anything::get_U(unitName_pz);
	unit_ox = MyString2Anything::get_U(unitName_ox);
	unit_oy = MyString2Anything::get_U(unitName_oy);
	unit_oz = MyString2Anything::get_U(unitName_oz);
	unit_ot = MyString2Anything::get_U(unitName_ot);
	unit_opx = MyString2Anything::get_U(unitName_opx);
	unit_opy = MyString2Anything::get_U(unitName_opy);
	unit_opz = MyString2Anything::get_U(unitName_opz);
	unit_ekin = MyString2Anything::get_U(unitName_ekin);
	unit_e = MyString2Anything::get_U(unitName_e);
	unit_edep = MyString2Anything::get_U(unitName_edep);
	unit_edepAll = MyString2Anything::get_U(unitName_edepAll);
	unit_edepDelta = MyString2Anything::get_U(unitName_edepDelta);
	unit_edepSec = MyString2Anything::get_U(unitName_edepSec);
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
	std::cout<<"output Ox?       "<<(flag_Ox?" yes":" no")<<", unit: "<<unitName_Ox<<std::endl;
	std::cout<<"output Oy?       "<<(flag_Oy?" yes":" no")<<", unit: "<<unitName_Oy<<std::endl;
	std::cout<<"output Oz?       "<<(flag_Oz?" yes":" no")<<", unit: "<<unitName_Oz<<std::endl;
	std::cout<<"output Ot?       "<<(flag_Ot?" yes":" no")<<", unit: "<<unitName_Ot<<std::endl;
	std::cout<<"output Opx?      "<<(flag_Opx?" yes":" no")<<", unit: "<<unitName_Opx<<std::endl;
	std::cout<<"output Opy?      "<<(flag_Opy?" yes":" no")<<", unit: "<<unitName_Opy<<std::endl;
	std::cout<<"output Opz?      "<<(flag_Opz?" yes":" no")<<", unit: "<<unitName_Opz<<std::endl;
	std::cout<<"output Oekin?    "<<(flag_Oekin?" yes":" no")<<", unit: "<<unitName_Oekin<<std::endl;
	std::cout<<"output x?       "<<(flag_x?" yes":" no")<<", unit: "<<unitName_x<<std::endl;
	std::cout<<"output y?       "<<(flag_y?" yes":" no")<<", unit: "<<unitName_y<<std::endl;
	std::cout<<"output z?       "<<(flag_z?" yes":" no")<<", unit: "<<unitName_z<<std::endl;
	std::cout<<"output t?       "<<(flag_t?" yes":" no")<<", unit: "<<unitName_t<<std::endl;
	std::cout<<"output px?      "<<(flag_px?" yes":" no")<<", unit: "<<unitName_px<<std::endl;
	std::cout<<"output py?      "<<(flag_py?" yes":" no")<<", unit: "<<unitName_py<<std::endl;
	std::cout<<"output pz?      "<<(flag_pz?" yes":" no")<<", unit: "<<unitName_pz<<std::endl;
	std::cout<<"output ox?      "<<(flag_ox?" yes":" no")<<", unit: "<<unitName_ox<<std::endl;
	std::cout<<"output oy?      "<<(flag_oy?" yes":" no")<<", unit: "<<unitName_oy<<std::endl;
	std::cout<<"output oz?      "<<(flag_oz?" yes":" no")<<", unit: "<<unitName_oz<<std::endl;
	std::cout<<"output ot?      "<<(flag_ot?" yes":" no")<<", unit: "<<unitName_ot<<std::endl;
	std::cout<<"output opx?     "<<(flag_opx?" yes":" no")<<", unit: "<<unitName_opx<<std::endl;
	std::cout<<"output opy?     "<<(flag_opy?" yes":" no")<<", unit: "<<unitName_opy<<std::endl;
	std::cout<<"output opz?     "<<(flag_opz?" yes":" no")<<", unit: "<<unitName_opz<<std::endl;
	std::cout<<"output ekin?     "<<(flag_ekin?" yes":" no")<<", unit: "<<unitName_ekin<<std::endl;
	std::cout<<"output e?       "<<(flag_e?" yes":" no")<<", unit: "<<unitName_e<<std::endl;
	std::cout<<"output nph?    "<<(flag_nph?" yes":" no")<<std::endl;
	std::cout<<"output edep?    "<<(flag_edep?" yes":" no")<<", unit: "<<unitName_edep<<std::endl;
	std::cout<<"output edepAll? "<<(flag_edepAll?" yes":" no")<<", unit: "<<unitName_edepAll<<std::endl;
	std::cout<<"output edepDelta? "<<(flag_edepDelta?" yes":" no")<<", unit: "<<unitName_edepDelta<<std::endl;
	std::cout<<"output edepSec? "<<(flag_edepSec?" yes":" no")<<", unit: "<<unitName_edepSec<<std::endl;
	std::cout<<"output stepL?   "<<(flag_stepL?" yes":" no")<<", unit: "<<unitName_stepL<<std::endl;
	std::cout<<"output volID?   "<<(flag_volID?" yes":" no")<<std::endl;
	std::cout<<"output volName? "<<(flag_volName?" yes":" no")<<std::endl;
	std::cout<<"output ppid?    "<<(flag_ppid?" yes":" no")<<std::endl;
	std::cout<<"output ptid?    "<<(flag_ptid?" yes":" no")<<std::endl;
	std::cout<<"output oprocess?"<<(flag_oprocess?" yes":" no")<<std::endl;
	std::cout<<"output ovolName?"<<(flag_ovolName?" yes":" no")<<std::endl;
	std::cout<<"output tid?     "<<(flag_tid?" yes":" no")<<std::endl;
	std::cout<<"output pid?     "<<(flag_pid?" yes":" no")<<std::endl;
	std::cout<<"output particleName?  "<<(flag_particleName?" yes":" no")<<std::endl;
	std::cout<<"output charge?  "<<(flag_charge?" yes":" no")<<std::endl;
	std::cout<<"output stopped? "<<(flag_stopped?" yes":" no")<<std::endl;
	std::cout<<"output stop_time?"<<(flag_stop_time?" yes":" no")<<", unit: "<<unitName_stop_time<<std::endl;
	std::cout<<"output killed?  "<<(flag_killed?" yes":" no")<<std::endl;
	std::cout<<"output kill_time?"<<(flag_kill_time?" yes":" no")<<", unit: "<<unitName_kill_time<<std::endl;
	//for fileter
	std::cout<<"Switch on?      "<<(Switch?"yes":"no")<<std::endl;
	std::cout<<"neutralCut on?  "<<(neutralCut?"yes":"no")<<std::endl;
	std::cout<<"Need stopped?   "<<(needstopped?"yes":"no")<<std::endl;
	std::cout<<"minp =          "<<minp/MeV<<"MeV"<<std::endl;
	std::cout<<"maxp =          "<<maxp/MeV<<"MeV"<<std::endl;
	std::cout<<"mine =          "<<mine/MeV<<"MeV"<<std::endl;
	std::cout<<"maxn =          "<<maxn<<std::endl;
	std::cout<<"mint =          "<<mint/ns<<"ns"<<std::endl;
	std::cout<<"maxt =          "<<maxt/ns<<"ns"<<std::endl;
	std::cout<<"tres =          "<<tres/ns<<"ns"<<std::endl;
	std::cout<<"minnph =        "<<minnph<<std::endl;
	std::cout<<"minedep =       "<<minedep/MeV<<"MeV"<<std::endl;
	std::cout<<"maxOekin =       "<<maxOekin/MeV<<"MeV"<<std::endl;
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
	std::cout<<"******************************************************************************"<<std::endl;
}

//-----------------------------------ProcessHits----------------------------------------------
//Will be called by geant4 automatically everytime a step in MonitorSD generated
G4bool MonitorSD::ProcessHits(G4Step* aStep,G4TouchableHistory* touchableHistory)
{

	//*************************get useful variables***********************

	// get track info
	G4Track* aTrack = aStep->GetTrack() ;
	G4int trackID= aTrack->GetTrackID(); //G4 track ID of current track.
	G4int charge = aTrack->GetDefinition()->GetPDGCharge();
	G4int pid = aTrack->GetDefinition()->GetPDGEncoding();
	std::string particleName = aTrack->GetParticleDefinition()->GetParticleName();

	// get information at the beginning and at the end of step
	G4StepPoint* prePoint  = aStep->GetPreStepPoint() ;
	G4double total_e = prePoint->GetTotalEnergy();
	G4double ekin = prePoint->GetKineticEnergy();
	G4ThreeVector pointIn_mom = prePoint->GetMomentum();
	G4double pointIn_pa = pointIn_mom.mag();
	G4ThreeVector pointIn_pos = prePoint->GetPosition();
	G4double pointIn_time = prePoint->GetGlobalTime();//Time since the event in which the track belongs is created
	G4StepPoint* postPoint  = aStep->GetPostStepPoint() ;
	G4double pointOut_time = postPoint->GetGlobalTime();//Time since the event in which the track belongs is created
	G4ThreeVector pointOut_mom = postPoint->GetMomentum();
	G4double pointOut_pa = pointOut_mom.mag();
	G4ThreeVector pointOut_pos = postPoint->GetPosition();
	G4double Oekin = postPoint->GetKineticEnergy();

	// get volume info
	const G4VTouchable *touchable = prePoint->GetTouchable();
	G4int ReplicaNo = touchable->GetVolume(0)->GetCopyNo();
	G4String VolName = touchable->GetVolume(0)->GetName();

	// get step info
	G4double edep = aStep->GetTotalEnergyDeposit();
	G4double edepNonIoni = aStep->GetNonIonizingEnergyDeposit();
	G4double edepIoni = edep - edepNonIoni;
	G4double edepDelta = aStep->GetDeltaEnergy();
	G4double edepSec = 0;
	G4double stepL = aStep->GetStepLength();
	G4int nph = 0;
	G4int nsec = aStep->GetSecondaryInCurrentStep()->size();
	for (int isec = 0; isec<nsec; isec++){
		if ((*(aStep->GetSecondaryInCurrentStep()))[isec]->GetParticleDefinition()->GetPDGEncoding()==0
			&&(*(aStep->GetSecondaryInCurrentStep()))[isec]->GetCreatorProcess()
			&&(*(aStep->GetSecondaryInCurrentStep()))[isec]->GetCreatorProcess()->GetProcessName()=="Cerenkov")
			nph++;
		edepSec += (*(aStep->GetSecondaryInCurrentStep()))[isec]->GetKineticEnergy();
	}

	McTruthSvc::GetMcTruthSvc()->AddEdep2Map(
	        edep
	        ,(pointIn_pos.x()+pointOut_pos.x())/2
	        ,(pointIn_pos.y()+pointOut_pos.y())/2
	        ,(pointIn_pos.z()+pointOut_pos.z())/2
	        );

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
	if ( minp && pointIn_pa < minp ) return false;
	//maxp
	if ( maxp && pointIn_pa > maxp ) return false;
	//energy
	if (mine&&aTrack->GetTotalEnergy()<mine) return false;

	//time_window
	if(std::isnan(pointIn_time)){
		G4cout<<"MonitorSD:error, pointIn_time is nan "<<G4endl;
		return false;
	}
	if ( pointIn_time < mint && mint ) return false;
	if ( pointIn_time > maxt && maxt ) return false;

	//minedep
	if( edepIoni <= minedep) return false;

	//maxOekin
        if ( maxOekin>=0&&maxOekin < Oekin ) return false;

	//minnph
	if( minnph>0&&nph <= 0 ) return false;

	//****************************Stopped or not****************************************
//	std::cout<<"tid("<<trackID
//	         <<"), pid("<<pid
//	         <<"), e = "<<total_e/MeV
//	         <<"MeV, edep = "<<edep/MeV
//	         <<"MeV, t_in = "<<pointIn_time/ns
//	         <<"ns, pos_in = "<<pointIn_pos/mm
//	         <<"mm, On Boundary? "<<(prePoint->GetStepStatus() == fGeomBoundary)
//	         <<", t_out = "<<pointOut_time/ns
//	         <<"ns, pos_out = "<<pointOut_pos/mm
//	         <<"mm, On Boundary? "<<(postPoint->GetStepStatus() == fGeomBoundary)
//	         <<std::endl;
	G4TrackStatus fTrackStatus = aTrack->GetTrackStatus();
//	if (fTrackStatus == fAlive) std::cout<<"fAlive"<<std::endl;
//	else if (fTrackStatus == fStopButAlive) std::cout<<"fStopButAlive"<<std::endl;
//	else if (fTrackStatus == fStopAndKill) std::cout<<"fStopAndKill"<<std::endl;
//	else if (fTrackStatus == fKillTrackAndSecondaries) std::cout<<"fKillTrackAndSecondaries"<<std::endl;
//	else if (fTrackStatus == fSuspend) std::cout<<"fSuspend"<<std::endl;
//	else if (fTrackStatus == fPostponeToNextEvent) std::cout<<"fPostponeToNextEvent"<<std::endl;
//	else std::cout<<"What?!"<<std::endl;

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

	if (needstopped&&(!killed&&!stopped)) return false;

	//****************************generate new hit or modify old one********************************
	bool willPush = true;
	//look for last hit in the same volume.
	G4int index = -1;
	G4double dt = 1e10*s;
	//std::cout<<"At first, dt = "<<dt/ns<<"ns"<<std::endl;
	//std::cout<<"pointOut_time = "<<pointOut_time/ns<<"ns"<<std::endl;
	if (nHits>=1&&m_tid[nHits-1] == trackID){
	    dt = 0;
	    index = nHits-1;
	}
        else{
	for ( int i = 0; i < nHits; i++ ){
		if ( m_volName[i] == VolName && m_volID[i] == ReplicaNo ){
			//std::cout<<"m_t["<<i<<"] = "<<m_t[i]*unit_t/ns<<"ns"<<std::endl;
			G4double dtime = pointOut_time - m_t[i]*unit_t;
			if ( fabs(dtime) < fabs(dt) ){
				index = i;
				dt = dtime;
				//std::cout<<i<<": tid "<<m_tid[i]<<" dtime = "<<pointOut_time<<" - "<<m_t[i]<<" = "<<dtime/ns<<"ns < dt = "<<dt/ns<<"ns"<<std::endl;
			}
		}
	}
        }
//	std::cout<<"At last, dt = "<<dt/ns<<"ns"<<std::endl;
	if ( index != -1 ){// found a previous hit in the same volume
//		std::cout<<"found a previous hit in the same volume!"<<std::endl;
		if ( fabs(dt) < tres ){// dt too small, will not push
//			std::cout<<"dt too small, will not push"<<std::endl;
			willPush = false;
		}
		if ( tres>=0 && m_tid[index] == trackID&& prePoint->GetStepStatus() != fGeomBoundary){ // If this particle was in this volume in last step, don't generate a new hit
//			std::cout<<"Was here last step"<<std::endl;
			willPush = false;
		}
	}

	if (willPush){
		MonitorHit* newHit = new MonitorHit();
		newHit->SetTrackID(trackID);
		newHit->SetVolID(ReplicaNo);
		newHit->SetEdep(edepIoni);
		newHit->SetPos(pointIn_pos);
		newHit->SetGlobalT(pointIn_time);
		hitsCollection->insert(newHit);
		//Set for root objects
		if(flag_Ox) m_Ox.push_back(pointOut_pos.x()/unit_Ox);
		if(flag_Oy) m_Oy.push_back(pointOut_pos.y()/unit_Oy);
		if(flag_Oz) m_Oz.push_back(pointOut_pos.z()/unit_Oz);
		if(flag_Ot) m_Ot.push_back(pointOut_time/unit_Ot);
		if(flag_Opx) m_Opx.push_back(pointOut_mom.x()/unit_Opx);
		if(flag_Opy) m_Opy.push_back(pointOut_mom.y()/unit_Opy);
		if(flag_Opz) m_Opz.push_back(pointOut_mom.z()/unit_Opz);
		if(flag_Oekin) m_Oekin.push_back(Oekin/unit_Oekin);
		if(flag_x) m_x.push_back(pointIn_pos.x()/unit_x);
		if(flag_y) m_y.push_back(pointIn_pos.y()/unit_y);
		if(flag_z) m_z.push_back(pointIn_pos.z()/unit_z);
		m_t.push_back(pointIn_time/unit_t);
		//std::cout<<"m_t = pointOut_time/"<<unitName_t<<" = pointOut_time/"<<unit_t/ns<<"ns = "<<pointOut_time/unit_t<<unitName_t<<std::endl;
		if(flag_px) m_px.push_back(pointIn_mom.x()/unit_px);
		if(flag_py) m_py.push_back(pointIn_mom.y()/unit_py);
		if(flag_pz) m_pz.push_back(pointIn_mom.z()/unit_pz);
		if(flag_ekin) m_ekin.push_back(ekin/unit_ekin);
		if(flag_e) m_e.push_back(total_e/unit_e);
		if(flag_edep) m_edep.push_back(edepIoni/unit_edep);
		if(flag_nph) m_nph.push_back(nph);
		if(flag_edepAll) m_edepAll.push_back(edep/unit_edepAll);
		if(flag_edepDelta) m_edepDelta.push_back(edepDelta/unit_edepDelta);
		if(flag_edepSec) m_edepSec.push_back(edepSec/unit_edepSec);
		if(flag_stepL) m_stepL.push_back(stepL/unit_stepL);
		m_volID.push_back(ReplicaNo);
		m_volName.push_back(VolName);
		m_tid.push_back(trackID);
		if(flag_pid) m_pid.push_back(pid);
		if(flag_particleName) m_particleName.push_back(particleName);
		if(flag_charge) m_charge.push_back(charge);
		if(flag_stopped) m_stopped.push_back(stopped); // always false unless stopped at the first step
		if(flag_killed) m_killed.push_back(killed); // always false unless killed at the first step
		if(flag_stop_time) m_stop_time.push_back(stop_time/unit_stop_time); // always 0 unless stopped at the first step
		if(flag_kill_time) m_kill_time.push_back(kill_time/unit_kill_time); // always 0 unless killed at the first step
		if(flag_ptid){
			int ptid = aTrack->GetParentID();
			if (trackID==1){
				void *result = pPrimaryGeneratorAction->get_extra("ptid");
				if (result) ptid = *((int*)result);
			}
			m_ptid.push_back(ptid);
		}
		if(flag_ppid){
			int ptid = aTrack->GetParentID();
			int ppid = McTruthSvc::GetMcTruthSvc()->tid2pid(ptid);
			if (trackID==1){
				void *result = pPrimaryGeneratorAction->get_extra("ppid");
				if (result) ppid = *((int*)result);
			}
			m_ppid.push_back(ppid);
		}
		if(flag_oprocess){
			G4String processName;
			const G4VProcess* process = aTrack->GetCreatorProcess();
			if (process) {
				processName = process->GetProcessName();
			}
			else{
				processName = "NULL";
			}
			if (trackID==1){
				void *result = pPrimaryGeneratorAction->get_extra("process");
				if (result) processName = *((std::string*)result);
			}
			m_oprocess.push_back(processName);
		}
		if(flag_ovolName){
			G4String volume = aTrack->GetLogicalVolumeAtVertex()->GetName();
			if (trackID==1){
				void *result = pPrimaryGeneratorAction->get_extra("volume");
				if (result) volume = *((std::string*)result);
			}
			m_ovolName.push_back(volume);
		}
		if (flag_ox||flag_oy||flag_oz){
			G4ThreeVector pos_3vec = aTrack->GetVertexPosition();
			double ox = pos_3vec.x()/unit_ox;
			double oy = pos_3vec.y()/unit_oy;
			double oz = pos_3vec.z()/unit_oz;
			if (trackID==1){
				void *result = pPrimaryGeneratorAction->get_extra("ox");
				if (result) ox = *((double*)result)*mm/unit_ox;
				result = pPrimaryGeneratorAction->get_extra("oy");
				if (result) oy = *((double*)result)*mm/unit_oy;
				result = pPrimaryGeneratorAction->get_extra("oz");
				if (result) oz = *((double*)result)*mm/unit_oz;
			}
			if(flag_ox) m_ox.push_back(ox);
			if(flag_oy) m_oy.push_back(oy);
			if(flag_oz) m_oz.push_back(oz);
		}
		if(flag_ot){
			double ot;
			ot = McTruthSvc::GetMcTruthSvc()->tid2time(trackID)/unit_ot;
			if (trackID==1){
				void *result = pPrimaryGeneratorAction->get_extra("ot");
				if (result) ot = *((double*)result)*ns/unit_ot;
			}
			m_ot.push_back(ot);
		}
		if (flag_opx||flag_opy||flag_opz){
			G4ThreeVector mom_dir = aTrack->GetVertexMomentumDirection();
			G4double Ekin = aTrack->GetVertexKineticEnergy();
			G4double mass = aTrack->GetDynamicParticle()->GetMass();
			G4double mom = sqrt((mass+Ekin)*(mass+Ekin)-mass*mass);
			double opx = mom*mom_dir.x()/unit_opx;
			double opy = mom*mom_dir.y()/unit_opy;
			double opz = mom*mom_dir.z()/unit_opz;
			if (trackID==1){
				void *result = pPrimaryGeneratorAction->get_extra("opx");
				if (result) opx = *((double*)result)*mm/unit_opx;
				result = pPrimaryGeneratorAction->get_extra("opy");
				if (result) opy = *((double*)result)*mm/unit_opy;
				result = pPrimaryGeneratorAction->get_extra("opz");
				if (result) opz = *((double*)result)*mm/unit_opz;
			}
			if(flag_opx) m_opx.push_back(opx);
			if(flag_opy) m_opy.push_back(opy);
			if(flag_opz) m_opz.push_back(opz);
		}
		nHits++;
	}
	else {
		G4double edepTemp = (*hitsCollection)[index]->GetEdep();
		(*hitsCollection)[index]->SetEdep(edepTemp  + edepIoni);
		(*hitsCollection)[index]->SetPos(pointOut_pos);
		(*hitsCollection)[index]->SetGlobalT(pointOut_time);
		if (m_tid[index] == trackID){
			if(flag_Ox) m_Ox[index] = pointOut_pos.x()/unit_Ox;
			if(flag_Oy) m_Oy[index] = pointOut_pos.y()/unit_Oy;
			if(flag_Oz) m_Oz[index] = pointOut_pos.z()/unit_Oz;
			if(flag_Ot) m_Ot[index] = pointOut_time/unit_Ot;
			if(flag_Opx) m_Opx[index] = pointOut_mom.x()/unit_Opx;
			if(flag_Opy) m_Opy[index] = pointOut_mom.y()/unit_Opy;
			if(flag_Opz) m_Opz[index] = pointOut_mom.z()/unit_Opz;
			if(flag_Oekin) m_Oekin[index] = Oekin/unit_Oekin;
			if(flag_stepL) m_stepL[index] += stepL/unit_stepL;
			if(flag_stop_time&&stopped) m_stop_time[index] = stop_time/unit_stop_time; // modify only if it got stopped at this step
			if(flag_kill_time&&killed) m_kill_time[index] = kill_time/unit_kill_time;// modify only if it got killed at this step
			if(flag_stopped&&stopped) m_stopped[index] = stopped;// modify only if it got stopped at this step
			if(flag_killed&&killed) m_killed[index] = killed;// modify only if it got killed at this step
		}
		if(flag_edep) m_edep[index] += edepIoni/unit_edep;
		if(flag_nph) m_nph[index] += nph;
		if(flag_edepAll) m_edepAll[index] += edep/unit_edepAll;
		if(flag_edepDelta) m_edepDelta[index] += edepDelta/unit_edepDelta;
		if(flag_edepSec) m_edepSec[index] += edepSec/unit_edepSec;
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
