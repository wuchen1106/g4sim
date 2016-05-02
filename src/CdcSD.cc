//---------------------------------------------------------------------------
//Description: Sensitive detector definition for CDC
//Author: Yuan Ye(yuany@mail.ihep.ac.cn)
//Created: 22 July, 2012
//Modified: 29 Sep, 2012 by Wu Chen(wuchen@mail.ihep.ac.cn)
//Modified: 8 Oct, 2012 by Wu Chen(wuchen@mail.ihep.ac.cn)
//Modified: 21 Oct, 2012 by Wu Chen(wuchen@mail.ihep.ac.cn)
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
#include "G4VPhysicalVolume.hh"
#include "G4RotationMatrix.hh"

#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Point3D.h"

#include "TVector3.h"
#include "TRandom.h"
#include "TH1D.h"
#include "TF1.h"
#include "TFile.h"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "CdcGeometryParameter.hh"
//#include "CdcDigitizer.hh"
#include "CdcHit.hh"
#include "MyRoot.hh"
#include "MySD.hh"
#include "McTruthSvc.hh"
#include "MyDetectorManager.hh"
#include "MyVGeometrySvc.hh"
#include "MyString2Anything.hh"
#include "PrimaryGeneratorAction.hh"
#include "MyAnalysisSvc.hh"

#include "CdcSD.hh"

#include "DEBUG.hh"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
typedef HepGeom::Vector3D<double> HepVector3D;
#endif

	CdcSD::CdcSD(G4String name, MyVGeometryParameter* pointer)
:MySD(name, pointer), hitsCollection(0), m_xt_hist(0), m_xt_func(0), m_xt_file(0)
{
	m_GeometryParameter= dynamic_cast<CdcGeometryParameter*> (pointer);
	if (!m_GeometryParameter){
		//		std::cout<<"In CdcSD::CdcSD, cannot get CdcGeometryParameter pointer!!!"<<std::endl;
		G4Exception("CdcSD::CdcSD()",
				"InvalidSetup", FatalException,
				"cannot get CdcGeometryParameter pointer");
	}
	fMyAnalysisSvc = MyAnalysisSvc::GetMyAnalysisSvc();
	min_evt_num = 0;
}

CdcSD::~CdcSD(){
}

//-----------------------------------Initialize----------------------------------------------
//Will be called by geant4 automatically at the beginning of each event
void CdcSD::Initialize(G4HCofThisEvent* HCE)
{
	min_evt_num = atoi(getenv("MINEVTNUM")); 
	pPrimaryGeneratorAction = PrimaryGeneratorAction::GetPrimaryGeneratorAction();
	hitsCollection = new CdcHitsCollection
		(SensitiveDetectorName,collectionName[0]);
//	std::cout<<"====>In CdcSD::Initialize(), created hitsCollection at ("<<(void*)hitsCollection<<std::endl;
	G4int HCID = -1;
	HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
	if ( HCID < 0 ){
		G4Exception("CdcSD::Initialize()",
				"InvalidSetup", FatalException,
				"Cann not find hitsCollection of this event");
	}
	HCE->AddHitsCollection( HCID, hitsCollection );
	int LayerNo = m_GeometryParameter->get_LayerNo();
	CDCSD_LINEVAR(LayerNo);
//	hitPointer.resize(m_GeometryParameter->get_layer_ID(LayerNo-3)+1);
	hitPointer.resize(m_GeometryParameter->get_layer_ID(LayerNo-3)+1);
	CDCSD_LINEVAR(hitPointer.size());
	for( int  i = 0; i < LayerNo; i++ ){
		if (m_GeometryParameter->get_layer_type(i)!=1) continue;
		int icelllayer = m_GeometryParameter->get_layer_ID(i);
		int CellNo = m_GeometryParameter->get_layer_HoleNo(i)/2;
		hitPointer[icelllayer].resize(CellNo);
//		std::cout<<"hitPointer["<<icelllayer<<"].resize("<<CellNo<<")"<<std::endl;
		for( int j = 0; j < CellNo; j++ ){
			hitPointer[icelllayer][j] = -1;
		}
	}
	//initialize for root objects
	cdc_nHits = 0;
	m_x.clear();
	m_y.clear();
	m_z.clear();
	m_wx.clear();
	m_wy.clear();
	m_wz.clear();
	m_t.clear();
	m_tstart.clear();
	m_tstop.clear();
	m_px.clear();
	m_py.clear();
	m_pz.clear();
	m_e.clear();
	m_ekin.clear();
	m_edep.clear();
	m_edepAll.clear();
	m_edepDelta.clear();
	m_stepL.clear();
	m_nPair.clear();
	m_driftD.clear();
	m_driftDtrue.clear();
	m_posflag.clear();
	m_layerID.clear();
	m_cellID.clear();
	m_tid.clear();
	m_pid.clear();
	m_particleName.clear();
	m_ppid.clear();
	m_ptid.clear();
	m_oprocess.clear();
	m_ovolName.clear();
	//initialize for filter
	nTracks = 0;
	prevTrackID = -100;
}

//-----------------------------------SetBranch----------------------------------------------
//Should be called at the beginning of each run (now in MyAnalysisSvc::BeginOfRunAction())
void CdcSD::SetBranch(){
	MyRoot* myRoot = MyRoot::GetMyRoot();
	G4String volName = get_VolName();
	if( flag_nHits ) myRoot->SetBranch(volName+"_nHits", &cdc_nHits);
	if( flag_x ) myRoot->SetBranch(volName+"_x", &m_x);
	if( flag_y ) myRoot->SetBranch(volName+"_y", &m_y);
	if( flag_z ) myRoot->SetBranch(volName+"_z", &m_z);
	if( flag_wx ) myRoot->SetBranch(volName+"_wx", &m_wx);
	if( flag_wy ) myRoot->SetBranch(volName+"_wy", &m_wy);
	if( flag_wz ) myRoot->SetBranch(volName+"_wz", &m_wz);
	if( flag_t ) myRoot->SetBranch(volName+"_t", &m_t);
	if( flag_tstart ) myRoot->SetBranch(volName+"_tstart", &m_tstart);
	if( flag_tstop ) myRoot->SetBranch(volName+"_tstop", &m_tstop);
	if( flag_px ) myRoot->SetBranch(volName+"_px", &m_px);
	if( flag_py ) myRoot->SetBranch(volName+"_py", &m_py);
	if( flag_pz ) myRoot->SetBranch(volName+"_pz", &m_pz);
	if( flag_e ) myRoot->SetBranch(volName+"_e", &m_e);
	if( flag_ekin ) myRoot->SetBranch(volName+"_ekin", &m_ekin);
	if( flag_edep ) myRoot->SetBranch(volName+"_edep", &m_edep);
	if( flag_edepAll ) myRoot->SetBranch(volName+"_edepAll", &m_edepAll);
	if( flag_edepDelta ) myRoot->SetBranch(volName+"_edepDelta", &m_edepDelta);
	if( flag_stepL ) myRoot->SetBranch(volName+"_stepL", &m_stepL);
	if( flag_nPair ) myRoot->SetBranch(volName+"_nPair", &m_nPair);
	if( flag_driftD ) myRoot->SetBranch(volName+"_driftD", &m_driftD);
	if( flag_driftDtrue ) myRoot->SetBranch(volName+"_driftDtrue", &m_driftDtrue);
	if( flag_posflag ) myRoot->SetBranch(volName+"_posflag", &m_posflag);
	if( flag_layerID ) myRoot->SetBranch(volName+"_layerID", &m_layerID);
	if( flag_cellID ) myRoot->SetBranch(volName+"_cellID", &m_cellID);
	if( flag_tid ) myRoot->SetBranch(volName+"_tid", &m_tid);
	if( flag_pid ) myRoot->SetBranch(volName+"_pid", &m_pid);
	if( flag_particleName ) myRoot->SetBranch(volName+"_particleName", &m_particleName);
	if( flag_ppid) myRoot->SetBranch(volName+"_ppid", &m_ppid);
	if( flag_ptid) myRoot->SetBranch(volName+"_ptid", &m_ptid);
	if( flag_oprocess) myRoot->SetBranch(volName+"_oprocess", &m_oprocess);
	if( flag_ovolName) myRoot->SetBranch(volName+"_ovolName", &m_ovolName);
}

//-----------------------------------ReadOutputCard----------------------------------------------
//Should be called before the begin of each run (now in the constructor of MyAnalysisSvc)
void CdcSD::ReadOutputCard(G4String filename){
	ReSet();
	G4String volName = get_VolName();
	std::ifstream fin_card(filename);
	if(!fin_card){
		std::cout<<"In CdcSD::ReadOutputCard, cannot open "<<filename<<"!!!"<<std::endl;
		G4Exception("CdcSD::ReadOutputCard()",
				"InvalidSetup", FatalException,
				"cannot find output card");
	}
	std::stringstream buf_card;
	std::string s_card;
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

		std::string name;
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
			else if( name == "nHits" ) flag_nHits = true;
			else if( name == "x" ) {flag_x = true; buf_card>>unitName_x; unit_x = MyString2Anything::get_U(unitName_x);}
			else if( name == "y" ) {flag_y = true; buf_card>>unitName_y; unit_y = MyString2Anything::get_U(unitName_y);}
			else if( name == "z" ) {flag_z = true; buf_card>>unitName_z; unit_z = MyString2Anything::get_U(unitName_z);}
			else if( name == "wx" ) {flag_wx = true; buf_card>>unitName_wx; unit_wx = MyString2Anything::get_U(unitName_wx);}
			else if( name == "wy" ) {flag_wy = true; buf_card>>unitName_wy; unit_wy = MyString2Anything::get_U(unitName_wy);}
			else if( name == "wz" ) {flag_wz = true; buf_card>>unitName_wz; unit_wz = MyString2Anything::get_U(unitName_wz);}
			else if( name == "t" ) {flag_t = true; buf_card>>unitName_t; unit_t = MyString2Anything::get_U(unitName_t);}
			else if( name == "tstart" ) {flag_tstart = true; buf_card>>unitName_tstart; unit_tstart = MyString2Anything::get_U(unitName_tstart);}
			else if( name == "tstop" ) {flag_tstop = true; buf_card>>unitName_tstop; unit_tstop = MyString2Anything::get_U(unitName_tstop);}
			else if( name == "px" ) {flag_px = true; buf_card>>unitName_px; unit_px = MyString2Anything::get_U(unitName_px);}
			else if( name == "py" ) {flag_py = true; buf_card>>unitName_py; unit_py = MyString2Anything::get_U(unitName_py);}
			else if( name == "pz" ) {flag_pz = true; buf_card>>unitName_pz; unit_pz = MyString2Anything::get_U(unitName_pz);}
			else if( name == "ekin" ) {{flag_ekin = true; buf_card>>unitName_ekin; unit_ekin = MyString2Anything::get_U(unitName_ekin);}}
			else if( name == "e" ) {flag_e = true; buf_card>>unitName_e; unit_e = MyString2Anything::get_U(unitName_e);}
			else if( name == "edep" ) {flag_edep = true; buf_card>>unitName_edep; unit_edep = MyString2Anything::get_U(unitName_edep);}
			else if( name == "edepAll" ) {flag_edepAll = true; buf_card>>unitName_edepAll; unit_edepAll = MyString2Anything::get_U(unitName_edepAll);}
			else if( name == "edepDelta" ) {flag_edepDelta = true; buf_card>>unitName_edepDelta; unit_edepDelta = MyString2Anything::get_U(unitName_edepDelta);}
			else if( name == "stepL" ) {{flag_stepL = true; buf_card>>unitName_stepL; unit_stepL = MyString2Anything::get_U(unitName_stepL);}}
			else if( name == "nPair" ) {flag_nPair = true;}
			else if( name == "driftD" ) {flag_driftD = true; buf_card>>unitName_driftD; unit_driftD = MyString2Anything::get_U(unitName_driftD);}
			else if( name == "driftDtrue" ) {flag_driftDtrue = true; buf_card>>unitName_driftDtrue; unit_driftDtrue = MyString2Anything::get_U(unitName_driftDtrue);}
			else if( name == "posflag" ) {flag_posflag = true;}
			else if( name == "cellID" ) flag_cellID = true;
			else if( name == "layerID" ) flag_layerID = true;
			else if( name == "tid" ) flag_tid = true;
			else if( name == "pid" ) flag_pid = true;
			else if( name == "ppid" ) {flag_ppid = true;}
			else if( name == "ptid" ) {flag_ptid = true;}
			else if( name == "oprocess" ) {flag_oprocess = true;}
			else if( name == "ovolName" ) {flag_ovolName = true;}
			else if( name == "particleName" ) flag_particleName = true;
			else{
				std::cout<<"In CdcSD::ReadOutputCard, unknown name: "<<name<<" in file "<<filename<<std::endl;
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
			else if( name == "neutralCut" ) neutralCut = true;
			else if( name == "maxn" ) buf_card>>maxn;
			else if( name == "ntracks" ) buf_card>>ntracks;
			else if( name == "xt" ){
				G4String type,tname,filename;
				buf_card>>type>>filename>>tname;
				G4String dir_name = getenv("CONFIGUREDATAROOT");
				if (dir_name[dir_name.size()-1] != '/') dir_name.append("/");
				std::string TFile_name = dir_name + filename;
				if (filename[0] == '/')
					TFile_name = filename;
				m_xt_file = new TFile(TFile_name.c_str());
				if (m_xt_file){
					if (type=="func")
						m_xt_func= (TF1*) m_xt_file->Get(tname.c_str());
					else if (type=="hist")
						m_xt_hist = (TH1D*) m_xt_file->Get(tname.c_str());
					else{
						std::cout<<"In CdcSD::ReadOutputCard, unknown xt type name: "<<type<<" in file "<<filename<<std::endl;
						std::cout<<"Will ignore this line!"<<std::endl;
					}
				}
			}
			else{
				G4double para;
				std::string unit;
				buf_card>>para>>unit;
				para *= MyString2Anything::get_U(unit);
				if( name == "minp" ) minp = para;
				else if ( name == "minedep" ) minedep = para;
				else if( name == "mint" ) mint = para;
				else if( name == "maxt" ) maxt = para;
				else if( name == "tres" ) tres = para;
				else{
					std::cout<<"In CdcSD::ReadOutputCard, unknown name: "<<name<<" in file "<<filename<<std::endl;
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
		std::cout<<"In CdcSD::ReadOutputCard, failed to find enough section seperators for output in file "<<filename<<std::endl;
		std::cout<<"Seperator: "<<BRANCHSECTIONNAME<<std::endl;
		std::cout<<"Will use default settings."<<std::endl;
		std::cout<<"********************************************"<<std::endl;
	}
	if ( n_filter_section_symbol<= 1 ){
		std::cout<<"*****************WARNING********************"<<std::endl;
		std::cout<<"In CdcSD::ReadOutputCard, failed to find enough section seperators for filter in file "<<filename<<std::endl;
		std::cout<<"Seperator: "<<FILTERSECTIONNAME<<std::endl;
		std::cout<<"Will use default settings."<<std::endl;
		std::cout<<"********************************************"<<std::endl;
	}
	fin_card.close();
	ShowOutCard();
}

//-----------------------------------ReSet----------------------------------------------
//Private
void CdcSD::ReSet(){
	//default output set
	flag_nHits = false;
	flag_x = false;
	flag_y = false;
	flag_z = false;
	flag_wx = false;
	flag_wy = false;
	flag_wz = false;
	flag_t = true;
	flag_tstart = true;
	flag_tstop = true;
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
	flag_e = false;
	flag_ekin = false;
	flag_edep = false;
	flag_edepAll = false;
	flag_edepDelta = false;
	flag_stepL = false;
	flag_nPair = false;
	flag_driftD = false;
	flag_driftDtrue = false;
	flag_posflag = false;
	flag_cellID = false;
	flag_layerID = false;
	flag_tid = true;
	flag_pid = false;
	flag_particleName = false;
	flag_ppid = false;
	flag_ptid = false;
	flag_oprocess = false;
	flag_ovolName = false;
	//for fileter
	Switch = false;
	neutralCut = false;
	minp = 0;
	maxn = 0;
	ntracks = 0;
	mint = 0;
	maxt = 0;
	tres = 0;
	minedep = 0;
	//for units
	unitName_wx = "cm";
	unitName_wy = "cm";
	unitName_wz = "cm";
	unitName_x = "cm";
	unitName_y = "cm";
	unitName_z = "cm";
	unitName_t = "ns";
	unitName_tstart = "ns";
	unitName_tstop = "ns";
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
	unitName_stepL = "cm";
	unitName_driftD = "cm";
	unitName_driftDtrue = "cm";
	unit_wx = MyString2Anything::get_U(unitName_wx);
	unit_wy = MyString2Anything::get_U(unitName_wy);
	unit_wz = MyString2Anything::get_U(unitName_wz);
	unit_x = MyString2Anything::get_U(unitName_x);
	unit_y = MyString2Anything::get_U(unitName_y);
	unit_z = MyString2Anything::get_U(unitName_z);
	unit_t = MyString2Anything::get_U(unitName_t);
	unit_tstart = MyString2Anything::get_U(unitName_tstart);
	unit_tstop = MyString2Anything::get_U(unitName_tstop);
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
	unit_stepL = MyString2Anything::get_U(unitName_stepL);
	unit_driftD = MyString2Anything::get_U(unitName_driftD);
	unit_driftDtrue = MyString2Anything::get_U(unitName_driftDtrue);
	// xt
	if (m_xt_file) m_xt_file->Close(); m_xt_file = 0;
	if (m_xt_hist) delete m_xt_hist; m_xt_hist = 0;
	if (m_xt_func) delete m_xt_func; m_xt_func = 0;
	// else
	prelayerId = -1;
	precellId = -1;
	preTid = -1;
	edeptemp = 0;
	stepLtemp = 0;
}

//-----------------------------------ShowOutCard----------------------------------------------
//Private
void CdcSD::ShowOutCard(){
	std::cout<<"*************************Output settings for "<<get_VolName()<<"***************************"<<std::endl;
	//default output set
	std::cout<<"output nHits?  "<<(flag_nHits?" yes":" no")<<std::endl;
	std::cout<<"output x?      "<<(flag_x?" yes":" no")<<", unit: "<<unitName_x<<std::endl;
	std::cout<<"output y?      "<<(flag_y?" yes":" no")<<", unit: "<<unitName_y<<std::endl;
	std::cout<<"output z?      "<<(flag_z?" yes":" no")<<", unit: "<<unitName_z<<std::endl;
	std::cout<<"output wx?     "<<(flag_wx?" yes":" no")<<", unit: "<<unitName_wx<<std::endl;
	std::cout<<"output wy?     "<<(flag_wy?" yes":" no")<<", unit: "<<unitName_wy<<std::endl;
	std::cout<<"output wz?     "<<(flag_wz?" yes":" no")<<", unit: "<<unitName_wz<<std::endl;
	std::cout<<"output t?      "<<(flag_t?" yes":" no")<<", unit: "<<unitName_t<<std::endl;
	std::cout<<"output tstart? "<<(flag_tstart?" yes":" no")<<", unit: "<<unitName_tstart<<std::endl;
	std::cout<<"output tstop?  "<<(flag_tstop?" yes":" no")<<", unit: "<<unitName_tstop<<std::endl;
	std::cout<<"output px?     "<<(flag_px?" yes":" no")<<", unit: "<<unitName_px<<std::endl;
	std::cout<<"output py?     "<<(flag_py?" yes":" no")<<", unit: "<<unitName_py<<std::endl;
	std::cout<<"output pz?     "<<(flag_pz?" yes":" no")<<", unit: "<<unitName_pz<<std::endl;
	std::cout<<"output ox?      "<<(flag_ox?" yes":" no")<<", unit: "<<unitName_ox<<std::endl;
	std::cout<<"output oy?      "<<(flag_oy?" yes":" no")<<", unit: "<<unitName_oy<<std::endl;
	std::cout<<"output oz?      "<<(flag_oz?" yes":" no")<<", unit: "<<unitName_oz<<std::endl;
	std::cout<<"output ot?      "<<(flag_ot?" yes":" no")<<", unit: "<<unitName_ot<<std::endl;
	std::cout<<"output opx?     "<<(flag_opx?" yes":" no")<<", unit: "<<unitName_opx<<std::endl;
	std::cout<<"output opy?     "<<(flag_opy?" yes":" no")<<", unit: "<<unitName_opy<<std::endl;
	std::cout<<"output opz?     "<<(flag_opz?" yes":" no")<<", unit: "<<unitName_opz<<std::endl;
	std::cout<<"output ekin?   "<<(flag_ekin?" yes":" no")<<", unit: "<<unitName_ekin<<std::endl;
	std::cout<<"output e?      "<<(flag_e?" yes":" no")<<", unit: "<<unitName_e<<std::endl;
	std::cout<<"output edep?   "<<(flag_edep?" yes":" no")<<", unit: "<<unitName_edep<<std::endl;
	std::cout<<"output edepAll?"<<(flag_edepAll?" yes":" no")<<", unit: "<<unitName_edepAll<<std::endl;
	std::cout<<"output edepDelta?"<<(flag_edepDelta?" yes":" no")<<", unit: "<<unitName_edepDelta<<std::endl;
	std::cout<<"output stepL?  "<<(flag_stepL?" yes":" no")<<", unit: "<<unitName_stepL<<std::endl;
	std::cout<<"output nPair?  "<<(flag_nPair?" yes":" no")<<std::endl;
	std::cout<<"output driftD? "<<(flag_driftD?" yes":" no")<<", unit: "<<unitName_driftD<<std::endl;
	std::cout<<"output driftDtrue? "<<(flag_driftDtrue?" yes":" no")<<", unit: "<<unitName_driftDtrue<<std::endl;
	std::cout<<"output posflag? "<<(flag_posflag?" yes":" no")<<std::endl;
	std::cout<<"output layerID?"<<(flag_layerID?" yes":" no")<<std::endl;
	std::cout<<"output cellID? "<<(flag_cellID?" yes":" no")<<std::endl;
	std::cout<<"output tid?    "<<(flag_tid?" yes":" no")<<std::endl;
	std::cout<<"output pid?    "<<(flag_pid?" yes":" no")<<std::endl;
	//for fileter
	std::cout<<"Switch on?     "<<(Switch?" yes":" no")<<std::endl;
	std::cout<<"neutralCut on? "<<(neutralCut?"yes":"no")<<std::endl;
	std::cout<<"minp =         "<<minp/MeV<<"MeV"<<std::endl;
	std::cout<<"maxn =         "<<maxn<<std::endl;
	std::cout<<"ntracks =      "<<ntracks<<std::endl;
	std::cout<<"mint =         "<<mint/ns<<"ns"<<std::endl;
	std::cout<<"maxt =         "<<maxt/ns<<"ns"<<std::endl;
	std::cout<<"tres =         "<<tres/ns<<"ns"<<std::endl;
	std::cout<<"minedep =      "<<minedep/MeV<<"MeV"<<std::endl;
	std::cout<<"VerboseLevel = "<<get_VerboseLevel()<<std::endl;
	std::cout<<"******************************************************************************"<<std::endl;
}

//-----------------------------------ProcessHits----------------------------------------------
//Will be called by geant4 automatically everytime a step in CdcSD generated
G4bool CdcSD::ProcessHits(G4Step* aStep,G4TouchableHistory* touchableHistory)
{
	int evt_num = fMyAnalysisSvc->get_evt_num();
	CDCSDPH_LINEINFO();

	//*************************get useful variables***********************

	int status;
	// get track info
	G4Track* aTrack = aStep->GetTrack() ;
	G4int trackID= aTrack->GetTrackID(); //G4 track ID of current track.
	G4double globalT=aTrack->GetGlobalTime();//Time since the event in which the track belongs is created
	G4int charge = aTrack->GetDefinition()->GetPDGCharge();
	G4int pid = aTrack->GetDefinition()->GetPDGEncoding();
	G4double theta = aTrack->GetMomentumDirection().theta();

	// get information at the beginning and at the end of step
	G4StepPoint* prePoint  = aStep->GetPreStepPoint() ;
	G4StepPoint* postPoint = aStep->GetPostStepPoint() ;
	G4double total_e = prePoint->GetTotalEnergy();
	G4double ekin = prePoint->GetKineticEnergy();
	G4ThreeVector pointIn_mom = prePoint->GetMomentum();
	G4double pointIn_pa = pointIn_mom.mag();
	G4ThreeVector pointIn_pos = prePoint->GetPosition();
	G4ThreeVector pointOut_pos = postPoint->GetPosition();
	G4ThreeVector pointIn_pos_local = prePoint->GetTouchableHandle()->GetHistory()->GetTopTransform().TransformPoint(pointIn_pos); // to local coordinates
	G4ThreeVector pointOut_pos_local = prePoint->GetTouchableHandle()->GetHistory()->GetTopTransform().TransformPoint(pointOut_pos); // to local coordinates
	G4double pointIn_time = prePoint->GetGlobalTime();//Time since the event in which the track belongs is created

	// get step info
	G4double edep = aStep->GetTotalEnergyDeposit();
	G4double edepNonIoni = aStep->GetNonIonizingEnergyDeposit();
	G4double edepIoni = edep - edepNonIoni;
	G4double edepDelta = aStep->GetDeltaEnergy();
	G4double stepL = aStep->GetStepLength();
	G4ThreeVector stepVec = pointOut_pos-pointIn_pos;

	//*************************filter***********************
	//switch
	if (!Switch) return false;

	//neutralCut
	if ( charge == 0 && neutralCut ) return false;

	CDCSDPH_LINEVAR(pointIn_pos.x());
	CDCSDPH_LINEVAR(pointIn_pos.y());
	CDCSDPH_LINEVAR(pointIn_pos.z());

	//*************************Locate the nearest sense wire****************************
	G4ThreeVector hitPosition = pointIn_pos_local; // first, assume that the hit position is step in point
	G4ThreeVector hitPositionOnWire = pointIn_pos_local; // first, assume that the hit position on the wire is step in point
	G4double deltaZ = hitPosition.z();
	int posflag = 0; // left or right
	G4double driftD = 0;
	G4double driftDtrue = 0; // minimum distance regardless of finite ionization effect
	G4double driftV = 0.025*mm/ns;
	G4double driftT;
	G4double signalT;
	// to get layerId and cellId
	G4int layerId, cellId;
	G4int holeLayerId, holeId, senseLayerId;
	// which layer?
	const G4VTouchable *touchable = prePoint->GetTouchable();
	holeLayerId= touchable->GetReplicaNumber(0);
	layerId=m_GeometryParameter->get_layer_ID(holeLayerId);
	if (m_GeometryParameter->get_layer_type(holeLayerId)==1){ // outer part of a cell
		senseLayerId = holeLayerId;
	}
	else{
		senseLayerId = holeLayerId+1;
	}
	// get layer info
	G4double phi = hitPosition.phi();
	if(phi<0) phi = 2*pi+phi; // (-pi,pi) => (0,2*pi)
	G4double Length = m_GeometryParameter->get_layer_length(senseLayerId);
	G4double phi0z = m_GeometryParameter->get_layer_phi0z(senseLayerId,deltaZ);
	G4double phi0zU = m_GeometryParameter->get_layer_phi0z(senseLayerId+1,deltaZ);
	// which cell?
	int HoleNo = m_GeometryParameter->get_layer_HoleNo(senseLayerId);
	int HoleNoU = m_GeometryParameter->get_layer_HoleNo(senseLayerId+1);
	double holeDphi = m_GeometryParameter->get_layer_holeDphi(senseLayerId);
	double holeDphiU = m_GeometryParameter->get_layer_holeDphi(senseLayerId+1);
	holeId = angdiff2(phi,phi0z)/holeDphi;
//	std::cout<<"____________________________________________________"<<std::endl;
//	std::cout<<"holeLayerId = "<<holeLayerId<<", holeId = "<<phi/deg<<"-"<<phi0z/deg<<"="<<angdiff2(phi,phi0z)/deg<<"/"<<holeDphi/deg<<" = "<<holeId<<std::endl;
	// how about corner effect?
	double Rz = m_GeometryParameter->get_layer_Rz(senseLayerId,deltaZ);
	double RzU = m_GeometryParameter->get_layer_Rz(senseLayerId+1,deltaZ);
	if (m_GeometryParameter->get_layer_type(holeLayerId)==1){ // outer part of a cell
		int holeIdU = angdiff2(phi ,phi0zU)/holeDphiU;
//		std::cout<<"holeIdU = "<<phi/deg<<"-"<<phi0zU/deg<<"="<<(phi-phi0zU)/deg<<"/"<<holeDphiU/deg<<" = "<<holeIdU<<std::endl;
		if (holeId%2==0){ // right part
			//   o u1 o u0
			//         .
			//     x m1 o m0
			//
			//     o    o
			holeIdU++; // for right side we choose the wire (u0) beyond current point to compare with m0
			double phi_u0 = holeIdU*holeDphiU+phi0zU;
			double phi_m0 = holeId*holeDphi+phi0z;
//			std::cout<<phi_u0/deg<<"<"<<0.5*holeDphi/deg<<"+"<<phi_m0/deg<<"="<<0.5*holeDphi/deg+phi_m0/deg<<"?"<<std::endl;
			if (angdiff(phi_u0,0.5*holeDphiU+phi_m0)<0){
				double delta_r = hitPosition.perp() - Rz;
				double delta_r2 = RzU - Rz;
				double delta_phi = angdiff(phi,phi_m0);
				double delta_phi2 = angdiff(phi_u0 , phi_m0);
//				std::cout<<"right part:"<<delta_r<<"*"<<delta_phi2/deg<<">"<<delta_r2<<"*"<<delta_phi/deg<<"?"<<std::endl;
				if (delta_r*delta_phi2>delta_r2*delta_phi){
					holeId--;
//					std::cout<<"  Yes"<<std::endl;
				}
			}
		}
		else{ // left part
			//       o u0 o u1
			//      .
			//     o m0 x m1
			//
			//     o    o
			double phi_u0 = holeIdU*holeDphiU+phi0zU;
			double phi_m0 = (holeId+1)*holeDphi+phi0z;
//			std::cout<<phi_u0/deg<<">"<<-0.5*holeDphi/deg<<"+"<<phi_m0/deg<<"="<<-0.5*holeDphi/deg+phi_m0/deg<<"?"<<std::endl;
			if (angdiff(phi_u0,-0.5*holeDphiU+phi_m0)>0){
				double delta_r = hitPosition.perp() - Rz;
				double delta_r2 = RzU - Rz;
				double delta_phi =  angdiff(phi_m0,phi);
				double delta_phi2 =  angdiff(phi_m0,phi_u0 );
//				std::cout<<"left part:"<<delta_r<<"*"<<delta_phi2/deg<<">"<<delta_r2<<"*"<<delta_phi/deg<<"?"<<std::endl;
				if (delta_r*delta_phi2>delta_r2*delta_phi){
					holeId++;
//					std::cout<<"  Yes"<<std::endl;
				}
			}
		}
	}
	if (holeId<0) holeId+=HoleNo;
	else if (holeId>=HoleNo) holeId-=HoleNo;
	cellId = holeId/2;
	CDCSDPH_LINEVAR(holeId);
	CDCSDPH_LINEVAR(layerId);

	//*************************get POCA and posflag****************************
	// direction of the momentum;
	G4ThreeVector mom_ori_dir = aTrack->GetVertexMomentumDirection();
	// direction of sense wire
	G4double phi0U = m_GeometryParameter->get_layer_phi0z(senseLayerId,-Length/2);
	G4double phi0D = m_GeometryParameter->get_layer_phi0z(senseLayerId,Length/2);
	double RU = m_GeometryParameter->get_layer_Rz(senseLayerId,-Length/2);
	double RD = m_GeometryParameter->get_layer_Rz(senseLayerId,Length/2);
	G4ThreeVector localWirePositionAtUpEndPlate = G4ThreeVector(1,1,1);
	G4ThreeVector localWirePositionAtDownEndPlate = G4ThreeVector(1,1,1);
	localWirePositionAtUpEndPlate.setZ(-Length/2);
	localWirePositionAtUpEndPlate.setPerp(RU);
	localWirePositionAtUpEndPlate.setPhi((holeId/2*2+1)*holeDphi+phi0U);
	localWirePositionAtDownEndPlate.setZ(Length/2);
	localWirePositionAtDownEndPlate.setPerp(RD);
	localWirePositionAtDownEndPlate.setPhi((holeId/2*2+1)*holeDphi+phi0D);
	G4ThreeVector wire_dir = localWirePositionAtDownEndPlate-localWirePositionAtUpEndPlate;
	// get POCA
	G4ThreeVector wireUE2pointIn = -localWirePositionAtUpEndPlate+pointIn_pos_local;
	G4ThreeVector vAxis = wire_dir.cross(stepVec);
	CDCSDPH_LINEVAR(wireUE2pointIn);
	CDCSDPH_LINEVAR(vAxis.unit());
	CDCSDPH_LINEVAR(wire_dir.unit());
	G4ThreeVector wireUE2pointInVertical = wireUE2pointIn-wire_dir.unit()*(wireUE2pointIn*(wire_dir.unit()));
	wireUE2pointInVertical = wireUE2pointInVertical-vAxis.unit()*(wireUE2pointInVertical*(vAxis.unit()));
	CDCSDPH_LINEVAR(wireUE2pointInVertical);
	CDCSDPH_LINEVAR(wireUE2pointInVertical.mag());
	CDCSDPH_LINEVAR(stepVec.mag());
	double hitPositionK = wireUE2pointInVertical.mag()/stepVec.mag();
	if (hitPositionK>1) hitPosition = pointOut_pos_local;
	else if (hitPositionK<0) hitPosition = pointIn_pos_local;
	else hitPosition = (1-hitPositionK)*pointIn_pos_local+hitPositionK*pointOut_pos_local;
	CDCSDPH_LINEVAR(hitPositionK);
	CDCSDPH_LINEVAR(pointIn_pos_local);
	CDCSDPH_LINEVAR(pointOut_pos_local);
	CDCSDPH_LINEVAR(hitPosition);
	double hitZ = hitPosition.z();
	hitPositionOnWire.setZ(hitZ);
	hitPositionOnWire.setPerp(m_GeometryParameter->get_layer_Rz(senseLayerId,hitZ));
	hitPositionOnWire.setPhi((holeId/2*2+1)*holeDphi+m_GeometryParameter->get_layer_phi0z(senseLayerId,hitZ));
	G4ThreeVector hit2wire = hitPositionOnWire-hitPosition;
	driftD=hit2wire.mag();
	// from hit point to sense wire
	double posflagD = (pointIn_mom.cross(wire_dir)).dot(hit2wire);
	posflag = posflagD/fabs(posflagD);
	CDCSDPH_LINEVAR(driftD/cm);

	//*************************accumulate step info********************************
	bool stillIn = false;
	if (layerId==prelayerId&&cellId==precellId&&trackID==preTid){
		edeptemp+=edepIoni;
		stepLtemp+=stepL;
		stillIn = true;
		// FIXME:
//		std::cout<<"  Same cell! "<<edeptemp<<","<<stepLtemp<<std::endl;
	}
	else{
		edeptemp = 0;
		stepLtemp = 0;
		precellId = cellId;
		prelayerId = layerId;
		preTid = trackID;
		// FIXME:
//		std::cout<<"  new cell!"<<std::endl;
	}

	G4double vc = 299792458*m/s; // m/s
	G4double wiredelay = (Length/2-deltaZ)/vc;
	if (m_xt_func)
		driftT = m_xt_func->Eval(driftD/cm);
	else if (m_xt_hist)
		driftT = m_xt_hist->GetBinContent(m_xt_hist->FindBin(driftD/cm));
	else
		driftT = driftD/driftV;
	signalT = driftT+pointIn_time+wiredelay;

	//*************************determine the action****************************
	int action = 0; // 0: pass; 1: new hit; 2: update hit;
	bool isPrimaryIon =  false;
	G4int pointer=-1;
	G4double pretstop = -1;
	G4double pretstart  = -1;
	// FIXME minedep should not be smaller than one ionization 
//	double minedeptemp = G4UniformRand()*minedep;
	double minedeptemp = minedep;
//	std::cout<<"minedeptemp = "<<minedeptemp/keV<<std::endl;
//	std::cout<<"@ ["<<layerId<<"]["<<cellId<<"]"<<std::endl;
	CDCSDPH_LINEVAR(stillIn);
	CDCSDPH_LINEVAR(layerId);
	CDCSDPH_LINEVAR(cellId);
	CDCSDPH_LINEVAR(hitPointer[layerId].size());
	if (stillIn){// same track in same cell
		action = 2;
		pointer = m_driftD.size()-1;
		CDCSDPH_LINEVAR(pointer);
	}
	else if (hitPointer[layerId][cellId] != -1){ // There is a pulse in this same cell
		CDCSDPH_LINEVAR(layerId);
		pointer=hitPointer[layerId][cellId];
		CDCSDPH_LINEVAR(pointer);
//		std::cout<<"hitPointer["<<layerId<<"]["<<cellId<<"] = "<<pointer<<std::endl;
		pretstop = m_tstop[pointer]*unit_tstop;
		pretstart  = m_tstart[pointer]*unit_tstart;
//		if (trackID==m_tid[pointer]&&tres&&signalT-pretstop<tres&&pretstart-signalT<tres) {
		if (tres<0||(signalT<pretstart-tres||signalT>pretstop+tres)){ // this can be a new hit
			action = 1;
		}
		else{
			action = 2;
		}
	}
	else{
		CDCSDPH_LINEINFO();
		action = 1;
	}
	// further cut
	if (action == 0) return false; // pass
	else if (action == 1){
		CDCSDPH_LINEINFO();
		//maxn
		if ( maxn && cdc_nHits >= maxn ) return false;
		//ntracks
		if ( trackID != prevTrackID ){
			prevTrackID = trackID;
			nTracks++;
		}
		if ( nTracks > ntracks && ntracks) return false;
		//minp
		if ( minp && pointIn_pa < minp ) return false;
		//time_window
		if(isnan(globalT)){
			G4cout<<"CdcSD:error, globalT is nan "<<G4endl;
			return false;
		}
		if ( globalT < mint && mint ) return false;
		if ( globalT > maxt && maxt ) return false;
		//edep
//		std::cout<<"edepIoni = "<<edepIoni<<std::endl;
		if (edeptemp<=minedeptemp) return false;
//		std::cout<<"Passed!"<<std::endl;
		CDCSDPH_LINEINFO();
	}
	else if (action == 2){
		CDCSDPH_LINEVAR(edepIoni);
		if (edepIoni>minedeptemp){isPrimaryIon=true;}
	}
	// FIXME:
	CDCSDPH_LINEVAR(action);

	//*******************generate or modify hit************************
	if (action==1){ // this is a new hit
//		std::cout<<"=>New Hit!"<<std::endl;
		CDCSDPH_LINEINFO();
		CdcHit* newHit = new CdcHit();
		newHit->SetTrackID(trackID);
		newHit->SetLayerNo(layerId);
		newHit->SetCellNo(cellId);
		newHit->SetEdep(edeptemp);
		newHit->SetPos(pointIn_pos);
		newHit->SetDriftD(driftD);
		newHit->SetTheta(theta);
		newHit->SetPosFlag(0);
		newHit->SetEnterAngle(0);
		newHit->SetDriftT (driftT);
		newHit->SetGlobalT(globalT);
		hitsCollection->insert(newHit);
		G4int NbHits = hitsCollection->entries();
		hitPointer[layerId][cellId]=NbHits-1;
		//Set for root objects
		if(flag_x) m_x.push_back(hitPosition.x()/unit_x);
		if(flag_y) m_y.push_back(hitPosition.y()/unit_y);
		if(flag_z) m_z.push_back(hitPosition.z()/unit_z);
		if(flag_wx) m_wx.push_back(hitPositionOnWire.x()/unit_wx);
		if(flag_wy) m_wy.push_back(hitPositionOnWire.y()/unit_wy);
		if(flag_wz) m_wz.push_back(hitPositionOnWire.z()/unit_wz);
		if(flag_t) m_t.push_back(globalT/unit_t);
		if(flag_t) m_tstart.push_back(signalT/unit_tstart);
		if(flag_t) m_tstop.push_back(signalT/unit_tstop);
		if(flag_px) m_px.push_back(pointIn_mom.x()/unit_px);
		if(flag_py) m_py.push_back(pointIn_mom.y()/unit_py);
		if(flag_pz) m_pz.push_back(pointIn_mom.z()/unit_pz);
		if(flag_ekin) m_ekin.push_back(ekin/unit_ekin);
		if(flag_e) m_e.push_back(total_e/unit_e);
		if(flag_edep) m_edep.push_back(edeptemp/unit_edep);
		if(flag_edepAll) m_edepAll.push_back(edeptemp/unit_edepAll);
		if(flag_edepDelta) m_edepDelta.push_back(edeptemp/unit_edepDelta);
		if(flag_stepL) m_stepL.push_back(stepLtemp/unit_stepL);
		if(flag_nPair) m_nPair.push_back(1);
		if(flag_driftD) m_driftD.push_back(driftD/unit_driftD);
		if(flag_driftDtrue) m_driftDtrue.push_back(driftD/unit_driftDtrue);
		if(flag_posflag) m_posflag.push_back(posflag);
		if(flag_layerID) m_layerID.push_back(layerId);
		if(flag_cellID) m_cellID.push_back(cellId);
		if(flag_tid) m_tid.push_back(trackID);
		if(flag_pid) m_pid.push_back(pid);
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
				void *result = pPrimaryGeneratorAction->get_extra("ox");
				if (result) ot = *((double*)result)*ns/unit_ot;
			}
			m_ot.push_back(ot);
		}
		if (flag_opx||flag_opy||flag_opz){
			G4double Ekin = aTrack->GetVertexKineticEnergy();
			G4double mass = aTrack->GetDynamicParticle()->GetMass();
			G4double mom = sqrt((mass+Ekin)*(mass+Ekin)-mass*mass);
			double opx = mom*mom_ori_dir.x()/unit_opx;
			double opy = mom*mom_ori_dir.y()/unit_opy;
			double opz = mom*mom_ori_dir.z()/unit_opz;
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
		cdc_nHits++;
	CDCSDPH_LINEINFO();
	}
	else { // update the track
//		std::cout<<"hitPointer["<<layerId<<"]["<<cellId<<"] = "<<pointer<<std::endl;
//		std::cout<<"=>Update Hit!"<<std::endl;
		CDCSDPH_LINEVAR(pointer);
		if(flag_edep) m_edep[pointer] += (edepIoni)/unit_edep;
		if(flag_edepAll) m_edepAll[pointer] += (edep)/unit_edepAll;
		if(flag_edepAll) m_edepDelta[pointer] += (edepDelta)/unit_edepAll;
		if(flag_stepL) m_stepL[pointer] += stepL/unit_stepL;
		if(flag_driftDtrue) if(driftD<m_driftDtrue[pointer]*unit_driftDtrue) m_driftDtrue[pointer] = driftD/unit_driftDtrue;
		if (isPrimaryIon){
			if (signalT<m_tstart[pointer]*unit_tstart) m_tstart[pointer] = signalT/unit_tstart;
			else if (signalT>m_tstop[pointer]*unit_tstop) m_tstop[pointer] = signalT/unit_tstop;
			if(flag_nPair) m_nPair[pointer]++;
			if(flag_driftD) if(driftD<m_driftD[pointer]*unit_driftD){
				if ( trackID != prevTrackID ){
					prevTrackID = trackID;
					nTracks++;
				}
				m_driftD[pointer] = driftD/unit_driftD;
				if(flag_wx) m_wx[pointer] = hitPositionOnWire.x()/unit_x;
				if(flag_wy) m_wy[pointer] = hitPositionOnWire.y()/unit_y;
				if(flag_wz) m_wz[pointer] = hitPositionOnWire.z()/unit_z;
				if(flag_x) m_x[pointer] = hitPosition.x()/unit_x;
				if(flag_y) m_y[pointer] = hitPosition.y()/unit_y;
				if(flag_z) m_z[pointer] = hitPosition.z()/unit_z;
				if(flag_t) m_t[pointer] = globalT/unit_t;
				if(flag_e) m_e[pointer] = total_e/unit_e;
				if(flag_ekin) m_ekin[pointer] = ekin/unit_ekin;
				if(flag_px) m_px[pointer] = pointIn_mom.x()/unit_px;
				if(flag_py) m_py[pointer] = pointIn_mom.y()/unit_py;
				if(flag_pz) m_pz[pointer] = pointIn_mom.z()/unit_pz;
				if(flag_pid) m_pid[pointer] = pid;
				if(flag_tid) m_tid[pointer] = trackID;
				if(flag_tid) m_posflag[pointer] = posflag;
				(*hitsCollection)[pointer]->SetPos(pointIn_pos);
				if(flag_ptid){
					int ptid = aTrack->GetParentID();
					if (trackID==1){
						void *result = pPrimaryGeneratorAction->get_extra("ptid");
						if (result) ptid = *((int*)result);
					}
					m_ptid[pointer] = ptid;
				}
				if(flag_ppid){
					int ptid = aTrack->GetParentID();
					int ppid = McTruthSvc::GetMcTruthSvc()->tid2pid(ptid);
					if (trackID==1){
						void *result = pPrimaryGeneratorAction->get_extra("ppid");
						if (result) ppid = *((int*)result);
					}
					m_ppid[pointer] = ppid;
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
					m_oprocess[pointer] = processName;
				}
				if(flag_ovolName){
					G4String volume = aTrack->GetLogicalVolumeAtVertex()->GetName();
					if (trackID==1){
						void *result = pPrimaryGeneratorAction->get_extra("volume");
						if (result) volume = *((std::string*)result);
					}
					m_ovolName[pointer] = volume;
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
					if(flag_ox) m_ox[pointer] = ox;
					if(flag_oy) m_oy[pointer] = oy;
					if(flag_oz) m_oz[pointer] = oz;
				}
				if(flag_ot){
					double ot;
					ot = McTruthSvc::GetMcTruthSvc()->tid2time(trackID)/unit_ot;
					if (trackID==1){
						void *result = pPrimaryGeneratorAction->get_extra("ox");
						if (result) ot = *((double*)result)*ns/unit_ot;
					}
					m_ot[pointer] = ot;
				}
				if (flag_opx||flag_opy||flag_opz){
					G4double Ekin = aTrack->GetVertexKineticEnergy();
					G4double mass = aTrack->GetDynamicParticle()->GetMass();
					G4double mom = sqrt((mass+Ekin)*(mass+Ekin)-mass*mass);
					double opx = mom*mom_ori_dir.x()/unit_opx;
					double opy = mom*mom_ori_dir.y()/unit_opy;
					double opz = mom*mom_ori_dir.z()/unit_opz;
					if (trackID==1){
						void *result = pPrimaryGeneratorAction->get_extra("opx");
						if (result) opx = *((double*)result)*mm/unit_opx;
						result = pPrimaryGeneratorAction->get_extra("opy");
						if (result) opy = *((double*)result)*mm/unit_opy;
						result = pPrimaryGeneratorAction->get_extra("opz");
						if (result) opz = *((double*)result)*mm/unit_opz;
					}
					if(flag_opx) m_opx[pointer] = opx;
					if(flag_opy) m_opy[pointer] = opy;
					if(flag_opz) m_opz[pointer] = opz;
				}
			}
		}
	CDCSDPH_LINEINFO();
	}
	//	std::cout<<"  "<<hitPosition.x()<<", "<<globalT<<", ("<<layerId<<","<<cellId<<"), "<<edepIoni/eV<<", "<<stepL<<std::endl;
	// FIXME:
	if (pointer<0) pointer=m_driftD.size()-1;
//	std::cout<<"  "<<pointer<<":"<<m_driftD[pointer]<<", "<<m_driftDtrue[pointer]<<", "<<m_stepL[pointer]<<", "<<m_edep[pointer]<<std::endl;
	CDCSDPH_LINEINFO();
	return true;
}

//-----------------------------------EndOfEvent----------------------------------------------
//Will be called by geant4 automatically at the end of each event
void CdcSD::EndOfEvent(G4HCofThisEvent*){
	if (verboseLevel>0) {
		//    hitsCollection->PrintAllHits();
		/*
		   G4int NbHits = hitsCollection->entries();
		   G4cout << "\n-------->Hits Collection: in this event they are " << NbHits
		   << " hits in the CDC chambers: " << G4endl;
		   for (G4int i=0;i<NbHits;i++) (*hitsCollection)[i]->Print();
		 */
	}
}

//-----------------------------------EndOfEvent----------------------------------------------
double CdcSD::angdiff(double a1, double a2){
	double da = a1-a2;
	if (da>2*pi) da-=2*pi;
	if (da<-2*pi) da+=2*pi;
	if (da<-pi) da+=2*pi;
	if (da>pi) da-=2*pi;
	return da;
}
double CdcSD::angdiff2(double a1, double a2){
	double da = a1-a2;
	if (da>2*pi) da-=2*pi;
	if (da<0) da+=2*pi;
	return da;
}
