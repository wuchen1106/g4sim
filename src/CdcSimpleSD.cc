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

#include <iostream>

#include "CdcGeometryParameter.hh"
//#include "CdcDigitizer.hh"
#include "MyRoot.hh"
#include "MySD.hh"
#include "MyDetectorManager.hh"
#include "MyVGeometrySvc.hh"
#include "MyString2Anything.hh"

#include "CdcSimpleSD.hh"

#include "DEBUG.hh"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
typedef HepGeom::Vector3D<double> HepVector3D;
#endif

	CdcSimpleSD::CdcSimpleSD(G4String name, MyVGeometryParameter* pointer)
:MySD(name, pointer)
{
	m_GeometryParameter= dynamic_cast<CdcGeometryParameter*> (pointer);
	if (!m_GeometryParameter){
		//		std::cout<<"In CdcSimpleSD::CdcSimpleSD, cannot get CdcGeometryParameter pointer!!!"<<std::endl;
		G4Exception("CdcSimpleSD::CdcSimpleSD()",
				"InvalidSetup", FatalException,
				"cannot get CdcGeometryParameter pointer");
	}

	//name of related Digiteizer module
	//  CdcDigitizer* cdcDM = new CdcDigitizer("CdcDigitizer");
	//	MyDetectorManager::GetMyDetectorManager()->RegistorDM("CdcDigitizer");
	//  G4DigiManager::GetDMpointer()->AddNewModule(cdcDM);

	//define fTolerance
	fTolerance = 2.2E-6;
}

CdcSimpleSD::~CdcSimpleSD(){
}

//-----------------------------------Initialize----------------------------------------------
//Will be called by geant4 automatically at the beginning of each event
void CdcSimpleSD::Initialize(G4HCofThisEvent* HCE)
{
	int LayerNo = m_GeometryParameter->get_LayerNo();
	hitPointer.resize(LayerNo);
	for( int  i = 0; i < LayerNo; i++ ){
		int CellNo = m_GeometryParameter->get_layer_cell_num(i);
		hitPointer[i].resize(CellNo);
		for( int j = 0; j < CellNo; j++ ){
			hitPointer[i][j] = -1;
		}
	}
	//initialize for root objects
	cdc_nHits = 0;
	m_x.clear();
	m_y.clear();
	m_z.clear();
	m_t.clear();
	m_px.clear();
	m_py.clear();
	m_pz.clear();
	m_e.clear();
	m_edep.clear();
	m_layerID.clear();
	m_cellID.clear();
	m_tid.clear();
	m_pid.clear();
	//initialize for filter
	nTracks = 0;
	prevTrackID = -100;
}

//-----------------------------------SetBranch----------------------------------------------
//Should be called at the beginning of each run (now in MyAnalysisSvc::BeginOfRunAction())
void CdcSimpleSD::SetBranch(){
	MyRoot* myRoot = MyRoot::GetMyRoot();
	G4String volName = get_VolName();
	if( flag_nHits ) myRoot->SetBranch(volName+"_nHits", &cdc_nHits);
	if( flag_x ) myRoot->SetBranch(volName+"_x", &m_x);
	if( flag_y ) myRoot->SetBranch(volName+"_y", &m_y);
	if( flag_z ) myRoot->SetBranch(volName+"_z", &m_z);
	if( flag_t ) myRoot->SetBranch(volName+"_t", &m_t);
	if( flag_px ) myRoot->SetBranch(volName+"_px", &m_px);
	if( flag_py ) myRoot->SetBranch(volName+"_py", &m_py);
	if( flag_pz ) myRoot->SetBranch(volName+"_pz", &m_pz);
	if( flag_e ) myRoot->SetBranch(volName+"_e", &m_e);
	if( flag_edep ) myRoot->SetBranch(volName+"_edep", &m_edep);
	if( flag_layerID ) myRoot->SetBranch(volName+"_layerID", &m_layerID);
	if( flag_cellID ) myRoot->SetBranch(volName+"_cellID", &m_cellID);
	if( flag_tid ) myRoot->SetBranch(volName+"_tid", &m_tid);
	if( flag_pid ) myRoot->SetBranch(volName+"_pid", &m_pid);
}

//-----------------------------------ReadOutputCard----------------------------------------------
//Should be called before the begin of each run (now in the constructor of MyAnalysisSvc)
void CdcSimpleSD::ReadOutputCard(G4String filename){
	ReSet();
	G4String volName = get_VolName();
	std::ifstream fin_card(filename);
	if(!fin_card){
		std::cout<<"In CdcSimpleSD::ReadOutputCard, cannot open "<<filename<<"!!!"<<std::endl;
		G4Exception("CdcSimpleSD::ReadOutputCard()",
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
			else if( name == "t" ) {flag_t = true; buf_card>>unitName_t; unit_t = MyString2Anything::get_U(unitName_t);}
			else if( name == "px" ) {flag_px = true; buf_card>>unitName_px; unit_px = MyString2Anything::get_U(unitName_px);}
			else if( name == "py" ) {flag_py = true; buf_card>>unitName_py; unit_py = MyString2Anything::get_U(unitName_py);}
			else if( name == "pz" ) {flag_pz = true; buf_card>>unitName_pz; unit_pz = MyString2Anything::get_U(unitName_pz);}
			else if( name == "e" ) {flag_e = true; buf_card>>unitName_e; unit_e = MyString2Anything::get_U(unitName_e);}
			else if( name == "edep" ) {flag_edep = true; buf_card>>unitName_edep; unit_edep = MyString2Anything::get_U(unitName_edep);}
			else if( name == "cellID" ) flag_cellID = true;
			else if( name == "layerID" ) flag_layerID = true;
			else if( name == "tid" ) flag_tid = true;
			else if( name == "pid" ) flag_pid = true;
			else{
				std::cout<<"In CdcSimpleSD::ReadOutputCard, unknown name: "<<name<<" in file "<<filename<<std::endl;
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
			else{
				G4double para;
				std::string unit;
				buf_card>>para>>unit;
				para *= MyString2Anything::get_U(unit);
				if( name == "minp" ) minp = para;
				else if ( name == "minedep" ) minedep = para;
				else if( name == "mint" ) mint = para;
				else if( name == "maxt" ) maxt = para;
				else{
					std::cout<<"In CdcSimpleSD::ReadOutputCard, unknown name: "<<name<<" in file "<<filename<<std::endl;
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
		std::cout<<"In CdcSimpleSD::ReadOutputCard, failed to find enough section seperators for output in file "<<filename<<std::endl;
		std::cout<<"Will use default settings."<<std::endl;
		std::cout<<"********************************************"<<std::endl;
	}
	if ( n_filter_section_symbol<= 1 ){
		std::cout<<"*****************WARNING********************"<<std::endl;
		std::cout<<"In CdcSimpleSD::ReadOutputCard, failed to find enough section seperators for filter in file "<<filename<<std::endl;
		std::cout<<"Will use default settings."<<std::endl;
		std::cout<<"********************************************"<<std::endl;
	}
	fin_card.close();
	ShowOutCard();
}

//-----------------------------------ReSet----------------------------------------------
//Private
void CdcSimpleSD::ReSet(){
	//default output set
	flag_nHits = false;
	flag_x = false;
	flag_y = false;
	flag_z = false;
	flag_t = false;
	flag_px = false;
	flag_py = false;
	flag_pz = false;
	flag_e = false;
	flag_edep = false;
	flag_cellID = false;
	flag_layerID = false;
	flag_tid = false;
	flag_pid = false;
	//for fileter
	Switch = false;
	neutralCut = false;
	minp = 1.5*MeV;
	maxn = 1000;
	ntracks = 1;
	mint = 0;
	maxt = 0;
	minedep = 0;
}

//-----------------------------------ShowOutCard----------------------------------------------
//Private
void CdcSimpleSD::ShowOutCard(){
	std::cout<<"*************************Output settings for "<<get_VolName()<<"***************************"<<std::endl;
	//default output set
	std::cout<<"output nHits?  "<<(flag_nHits?" yes":" no")<<std::endl;
	std::cout<<"output x?      "<<(flag_x?" yes":" no")<<", unit: "<<unitName_x<<std::endl;
	std::cout<<"output y?      "<<(flag_y?" yes":" no")<<", unit: "<<unitName_y<<std::endl;
	std::cout<<"output z?      "<<(flag_z?" yes":" no")<<", unit: "<<unitName_z<<std::endl;
	std::cout<<"output t?      "<<(flag_t?" yes":" no")<<", unit: "<<unitName_t<<std::endl;
	std::cout<<"output px?     "<<(flag_px?" yes":" no")<<", unit: "<<unitName_px<<std::endl;
	std::cout<<"output py?     "<<(flag_py?" yes":" no")<<", unit: "<<unitName_py<<std::endl;
	std::cout<<"output pz?     "<<(flag_pz?" yes":" no")<<", unit: "<<unitName_pz<<std::endl;
	std::cout<<"output e?      "<<(flag_e?" yes":" no")<<", unit: "<<unitName_e<<std::endl;
	std::cout<<"output edep?   "<<(flag_edep?" yes":" no")<<", unit: "<<unitName_edep<<std::endl;
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
	std::cout<<"minedep =      "<<minedep/MeV<<"MeV"<<std::endl;
	std::cout<<"VerboseLevel = "<<get_VerboseLevel()<<std::endl;
	std::cout<<"******************************************************************************"<<std::endl;
}

//-----------------------------------ProcessHits----------------------------------------------
//Will be called by geant4 automatically everytime a step in CdcSimpleSD generated
G4bool CdcSimpleSD::ProcessHits(G4Step* aStep,G4TouchableHistory* touchableHistory)
{

	//*************************get useful variables***********************

	int status;
	// get track info
	G4Track* gTrack = aStep->GetTrack() ;
	G4int trackID= gTrack->GetTrackID(); //G4 track ID of current track.
	G4double globalT=gTrack->GetGlobalTime();//Time since the event in which the track belongs is created
	G4int charge = gTrack->GetDefinition()->GetPDGCharge();
	G4int pid = gTrack->GetDefinition()->GetPDGEncoding();

	// get information at the beginning and at the end of step
	G4StepPoint* prePoint  = aStep->GetPreStepPoint() ;
	G4double total_e = prePoint->GetTotalEnergy();
	G4ThreeVector pointIn_mom = prePoint->GetMomentum();
	G4double pointIn_pa = pointIn_mom.mag();
	G4ThreeVector pointIn_pos = prePoint->GetPosition();
	G4double pointIn_time = prePoint->GetGlobalTime();//Time since the event in which the track belongs is created

	// get step info
	G4double edep = aStep->GetTotalEnergyDeposit();

	// get layerId and cellId
	const G4VTouchable *touchable = prePoint->GetTouchable();
	G4int ReplicaNo = touchable->GetReplicaNumber(0);
	G4int layerId, cellId;
	status = m_GeometryParameter->get_layerIdcellId(ReplicaNo, &layerId, &cellId);
	if(status){
		std::cout<<"In CdcSimpleSD::ProcessHits(), unknown ReplicaNo = "<<ReplicaNo<<"!!! Will not generate hit"<<std::endl;
		return false;
	}

	//*************************filter***********************
	//switch
	if (!Switch) return false;
	CDCSD_LINEINFO();

	//ntracks
	if ( trackID != prevTrackID ){
		prevTrackID = trackID;
		nTracks++;
	}
	if ( nTracks > ntracks && ntracks) return false;
	CDCSD_LINEINFO();

	//maxn
	if ( maxn && cdc_nHits >= maxn ) return false;
	CDCSD_LINEINFO();

	//minp
	if ( minp && pointIn_pa < minp ) return false;
	CDCSD_LINEINFO();

	//time_window
	if(isnan(globalT)){
		G4cout<<"CdcSimpleSD:error, globalT is nan "<<G4endl;
		return false;
	}
	CDCSD_LINEINFO();
	if ( globalT < mint && mint ) return false;
	CDCSD_LINEINFO();
	if ( globalT > maxt && maxt ) return false;
	CDCSD_LINEINFO();

	//neutralCut
	if ( charge == 0 && neutralCut ) return false;
	CDCSD_LINEINFO();

	//edep
	if( edep <= minedep ) return false;
	CDCSD_LINEINFO();

	if ( get_VerboseLevel() >= 5 ){
		std::cout<<"in layer["<<layerId<<"], the track hit cell["<<cellId<<"]"<<std::endl;
	}

	//*************************calculate hitPosition****************************
	G4ThreeVector hitPosition(0.,0.,0.);
	hitPosition = pointIn_pos;

	//*******************generate or modify hit************************
	if (hitPointer[layerId][cellId] == -1){
		CDCSD_LINEINFO();
		hitPointer[layerId][cellId]=cdc_nHits;
		//Set for root objects
		if(flag_x) m_x.push_back(hitPosition.x()/unit_x);
		if(flag_y) m_y.push_back(hitPosition.y()/unit_y);
		if(flag_z) m_z.push_back(hitPosition.z()/unit_z);
		if(flag_t) m_t.push_back(globalT/unit_t);
		if(flag_px) m_px.push_back(pointIn_mom.x()/unit_px);
		if(flag_py) m_py.push_back(pointIn_mom.y()/unit_py);
		if(flag_pz) m_pz.push_back(pointIn_mom.z()/unit_pz);
		if(flag_e) m_e.push_back(total_e/unit_e);
		if(flag_edep) m_edep.push_back(edep/unit_edep);
		if(flag_layerID) m_layerID.push_back(layerId);
		if(flag_cellID) m_cellID.push_back(cellId);
		if(flag_tid) m_tid.push_back(trackID);
		if(flag_pid) m_pid.push_back(pid);
		cdc_nHits++;
	}
	return true;
}

//-----------------------------------EndOfEvent----------------------------------------------
//Will be called by geant4 automatically at the end of each event
void CdcSimpleSD::EndOfEvent(G4HCofThisEvent*){
}
