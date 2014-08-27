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
#include "CdcHit.hh"
#include "MyRoot.hh"
#include "MySD.hh"
#include "MyDetectorManager.hh"
#include "MyVGeometrySvc.hh"
#include "MyString2Anything.hh"

#include "CdcSD.hh"

#include "DEBUG.hh"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
typedef HepGeom::Vector3D<double> HepVector3D;
#endif

	CdcSD::CdcSD(G4String name, MyVGeometryParameter* pointer)
:MySD(name, pointer), hitsCollection(0)
{
	m_GeometryParameter= dynamic_cast<CdcGeometryParameter*> (pointer);
	if (!m_GeometryParameter){
		//		std::cout<<"In CdcSD::CdcSD, cannot get CdcGeometryParameter pointer!!!"<<std::endl;
		G4Exception("CdcSD::CdcSD()",
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

CdcSD::~CdcSD(){
}

//-----------------------------------Initialize----------------------------------------------
//Will be called by geant4 automatically at the beginning of each event
void CdcSD::Initialize(G4HCofThisEvent* HCE)
{
	hitsCollection = new CdcHitsCollection
		(SensitiveDetectorName,collectionName[0]);
	std::cout<<"====>In CdcSD::Initialize(), created hitsCollection at ("<<(void*)hitsCollection<<std::endl;
	G4int HCID = -1;
	HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
	if ( HCID < 0 ){
		G4Exception("CdcSD::Initialize()",
				"InvalidSetup", FatalException,
				"Cann not find hitsCollection of this event");
	}
	HCE->AddHitsCollection( HCID, hitsCollection );
	int LayerNo = m_GeometryParameter->get_LayerNo();
	hitPointer.resize(LayerNo);
	for( int  i = 0; i < LayerNo; i++ ){
		int CellNo = m_GeometryParameter->get_layer_HoleNo(i)/2;
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
	m_pOx.clear();
	m_pOy.clear();
	m_pOz.clear();
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
	m_stepL.clear();
	m_nPair.clear();
	m_driftD.clear();
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
	if( flag_pOx ) myRoot->SetBranch(volName+"_pOx", &m_pOx);
	if( flag_pOy ) myRoot->SetBranch(volName+"_pOy", &m_pOy);
	if( flag_pOz ) myRoot->SetBranch(volName+"_pOz", &m_pOz);
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
	if( flag_stepL ) myRoot->SetBranch(volName+"_stepL", &m_stepL);
	if( flag_nPair ) myRoot->SetBranch(volName+"_nPair", &m_nPair);
	if( flag_driftD ) myRoot->SetBranch(volName+"_driftD", &m_driftD);
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
			else if( name == "pOx" ) {flag_pOx = true; buf_card>>unitName_pOx; unit_pOx = MyString2Anything::get_U(unitName_pOx);}
			else if( name == "pOy" ) {flag_pOy = true; buf_card>>unitName_pOy; unit_pOy = MyString2Anything::get_U(unitName_pOy);}
			else if( name == "pOz" ) {flag_pOz = true; buf_card>>unitName_pOz; unit_pOz = MyString2Anything::get_U(unitName_pOz);}
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
			else if( name == "stepL" ) {{flag_stepL = true; buf_card>>unitName_stepL; unit_stepL = MyString2Anything::get_U(unitName_stepL);}}
			else if( name == "nPair" ) {flag_nPair = true;}
			else if( name == "driftD" ) {flag_driftD = true; buf_card>>unitName_driftD; unit_driftD = MyString2Anything::get_U(unitName_driftD);}
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
	flag_pOx = false;
	flag_pOy = false;
	flag_pOz = false;
	flag_t = true;
	flag_tstart = true;
	flag_tstop = true;
	flag_px = false;
	flag_py = false;
	flag_pz = false;
	flag_e = false;
	flag_ekin = false;
	flag_edep = false;
	flag_edepAll = false;
	flag_stepL = false;
	flag_nPair = false;
	flag_driftD = false;
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
	minp = 1.5*MeV;
	maxn = 1000;
	ntracks = 1;
	mint = 0;
	maxt = 0;
	tres = 0;
	minedep = 0;
	//for units
	unitName_pOx = "cm";
	unitName_pOy = "cm";
	unitName_pOz = "cm";
	unitName_x = "cm";
	unitName_y = "cm";
	unitName_z = "cm";
	unitName_t = "ns";
	unitName_tstart = "ns";
	unitName_tstop = "ns";
	unitName_px = "GeV";
	unitName_py = "GeV";
	unitName_pz = "GeV";
	unitName_ekin = "GeV";
	unitName_e = "GeV";
	unitName_edep = "GeV";
	unitName_edepAll = "GeV";
	unitName_stepL = "cm";
	unitName_driftD = "cm";
	unit_pOx = MyString2Anything::get_U(unitName_pOx);
	unit_pOy = MyString2Anything::get_U(unitName_pOy);
	unit_pOz = MyString2Anything::get_U(unitName_pOz);
	unit_x = MyString2Anything::get_U(unitName_x);
	unit_y = MyString2Anything::get_U(unitName_y);
	unit_z = MyString2Anything::get_U(unitName_z);
	unit_t = MyString2Anything::get_U(unitName_t);
	unit_tstart = MyString2Anything::get_U(unitName_tstart);
	unit_tstop = MyString2Anything::get_U(unitName_tstop);
	unit_px = MyString2Anything::get_U(unitName_px);
	unit_py = MyString2Anything::get_U(unitName_py);
	unit_pz = MyString2Anything::get_U(unitName_pz);
	unit_ekin = MyString2Anything::get_U(unitName_ekin);
	unit_e = MyString2Anything::get_U(unitName_e);
	unit_edep = MyString2Anything::get_U(unitName_edep);
	unit_edepAll = MyString2Anything::get_U(unitName_edepAll);
	unit_stepL = MyString2Anything::get_U(unitName_stepL);
	unit_driftD = MyString2Anything::get_U(unitName_driftD);
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
	std::cout<<"output pOx?    "<<(flag_pOx?" yes":" no")<<", unit: "<<unitName_pOx<<std::endl;
	std::cout<<"output pOy?    "<<(flag_pOy?" yes":" no")<<", unit: "<<unitName_pOy<<std::endl;
	std::cout<<"output pOz?    "<<(flag_pOz?" yes":" no")<<", unit: "<<unitName_pOz<<std::endl;
	std::cout<<"output t?      "<<(flag_t?" yes":" no")<<", unit: "<<unitName_t<<std::endl;
	std::cout<<"output tstart? "<<(flag_tstart?" yes":" no")<<", unit: "<<unitName_tstart<<std::endl;
	std::cout<<"output tstop?  "<<(flag_tstop?" yes":" no")<<", unit: "<<unitName_tstop<<std::endl;
	std::cout<<"output px?     "<<(flag_px?" yes":" no")<<", unit: "<<unitName_px<<std::endl;
	std::cout<<"output py?     "<<(flag_py?" yes":" no")<<", unit: "<<unitName_py<<std::endl;
	std::cout<<"output pz?     "<<(flag_pz?" yes":" no")<<", unit: "<<unitName_pz<<std::endl;
	std::cout<<"output ekin?   "<<(flag_ekin?" yes":" no")<<", unit: "<<unitName_ekin<<std::endl;
	std::cout<<"output e?      "<<(flag_e?" yes":" no")<<", unit: "<<unitName_e<<std::endl;
	std::cout<<"output edep?   "<<(flag_edep?" yes":" no")<<", unit: "<<unitName_edep<<std::endl;
	std::cout<<"output edepAll?"<<(flag_edepAll?" yes":" no")<<", unit: "<<unitName_edepAll<<std::endl;
	std::cout<<"output stepL?  "<<(flag_stepL?" yes":" no")<<", unit: "<<unitName_stepL<<std::endl;
	std::cout<<"output nPair?  "<<(flag_nPair?" yes":" no")<<std::endl;
	std::cout<<"output driftD? "<<(flag_driftD?" yes":" no")<<", unit: "<<unitName_driftD<<std::endl;
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

	std::cout<<"In CdcSD::ProcessHits()"<<std::endl;
	//*************************get useful variables***********************

	int status;
	// get track info
	G4Track* gTrack = aStep->GetTrack() ;
	G4int trackID= gTrack->GetTrackID(); //G4 track ID of current track.
	G4double globalT=gTrack->GetGlobalTime();//Time since the event in which the track belongs is created
	G4int charge = gTrack->GetDefinition()->GetPDGCharge();
	G4int pid = gTrack->GetDefinition()->GetPDGEncoding();
	G4double theta = gTrack->GetMomentumDirection().theta();

	// get information at the beginning and at the end of step
	G4StepPoint* prePoint  = aStep->GetPreStepPoint() ;
	G4StepPoint* postPoint = aStep->GetPostStepPoint() ;
	G4double total_e = prePoint->GetTotalEnergy();
	G4double ekin = prePoint->GetKineticEnergy();
	G4ThreeVector pointIn_mom = prePoint->GetMomentum();
	G4double pointIn_pa = pointIn_mom.mag();
	G4ThreeVector pointIn_pos = prePoint->GetPosition();
	G4ThreeVector pointOut_pos = postPoint->GetPosition();
	G4double pointIn_time = prePoint->GetGlobalTime();//Time since the event in which the track belongs is created

	// get step info
	G4double edep = aStep->GetTotalEnergyDeposit();
	G4double edepNonIoni = aStep->GetNonIonizingEnergyDeposit();
	G4double edepIoni = edep - edepNonIoni;
	G4double stepL = aStep->GetStepLength();

	CDCSD_LINEVAR(pointIn_pos.x());
	CDCSD_LINEVAR(pointIn_pos.y());
	CDCSD_LINEVAR(pointIn_pos.z());
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
		G4cout<<"CdcSD:error, globalT is nan "<<G4endl;
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
	if( edepIoni <= minedep ) return false;
	CDCSD_LINEINFO();

	//*************************calculate hitPosition****************************
	G4ThreeVector hitPosition = pointIn_pos;
	G4ThreeVector localHitPosition = prePoint->GetTouchableHandle()->GetHistory()->GetTopTransform().TransformPoint(hitPosition);
	G4double deltaZ = localHitPosition.z();
	G4double driftD = 0;
	G4double driftV = 0.025*mm/ns;
	G4double driftT;
	G4double signalT;
	// to get layerId and cellId
	G4int layerId, cellId;
	G4int holeLayerId, holeId;
	// which layer?
	double Rhit =hitPosition.perp();
	int LayerNo = m_GeometryParameter->get_LayerNo();
	int ilayere = 0;
	int ilayerc = 0;
	for (int ilayer = 0; ilayer<LayerNo; ilayer++){
		if (!ilayere)
			if (m_GeometryParameter->get_layer_Re(ilayer) > Rhit){
				ilayere = ilayer;
			}
		if (!ilayerc)
			if (m_GeometryParameter->get_layer_Rc(ilayer) > Rhit){
				ilayerc = ilayer;
			}
		if (ilayere&&ilayerc) break;
	}
	if (ilayerc==0&&ilayere==0) return false; // not sensitive region
	else if (ilayerc==LayerNo&&ilayere==LayerNo) return false; // not sensitive region
	else if (ilayere==ilayerc+1){ // on the boundary
		double Rc = m_GeometryParameter->get_layer_Rc(ilayerc-1);
		double tanalpha = tan(m_GeometryParameter->get_layer_angle4rotate(ilayerc-1)/2);
		double l = m_GeometryParameter->get_layer_length(ilayerc-1);
		if (Rhit<sqrt(Rc*Rc+deltaZ*deltaZ*Rc*Rc*tanalpha*tanalpha/l/l)){
			holeLayerId = ilayere-1;
		}
		else{
			holeLayerId = ilayerc-1;
		}
	}
	else if (ilayere!=ilayerc){ // what??
		std::cout<<"!!! ilayerc = "<<ilayerc<<", ilayere = "<<ilayere<<std::endl;
		return false;
	}
	else holeLayerId = ilayere-1;
	layerId=m_GeometryParameter->get_layer_ID(holeLayerId);
	G4double phi = localHitPosition.phi();
	G4double Length = m_GeometryParameter->get_layer_length(holeLayerId);
	G4double deltaphi = m_GeometryParameter->get_layer_angle4rotate(holeLayerId)/2*(0.5+deltaZ*2/Length)+m_GeometryParameter->get_layer_SPhi(holeLayerId);
	phi -= deltaphi;
	// which cell?
	int HoleNo = m_GeometryParameter->get_layer_HoleNo(holeLayerId);
	double holeDphi = m_GeometryParameter->get_layer_holeDphi(holeLayerId);
	int ihole = 0;
	for ( ; ihole<HoleNo; ihole++){
		if (ihole*holeDphi>phi) break;
	}
	holeId = ihole - 1;
	if (holeId<0) holeId+=HoleNo;
	// how about corner effect?
	if (m_GeometryParameter->get_layer_type(holeLayerId)==1){ // outer part of a cell
	}
	cellId = holeId/2;
	// position of sense wire at that z plane
	G4ThreeVector localWirePositionAtHitPlane = G4ThreeVector(0,0,0);
	localWirePositionAtHitPlane.setMag(m_GeometryParameter->get_layer_Rc(holeLayerId));
	localWirePositionAtHitPlane.setZ(deltaZ);
	localWirePositionAtHitPlane.setPhi((holeId/2*2+1)*holeDphi+deltaphi);
	driftD = (localHitPosition-localWirePositionAtHitPlane).perp();

	G4double vc = 299792458*m/s; // m/s
	G4double wiredelay = (Length/2-deltaZ)/vc;
	driftT = driftD/driftV;
	signalT = driftT+pointIn_time+wiredelay;

	//*******************generate or modify hit************************
	bool needpush = false;
	if (hitPointer[layerId][cellId] != -1){
		CDCSD_LINEINFO();
		G4int pointer=hitPointer[layerId][cellId];
		G4double pretstop = m_tstop[pointer]*unit_tstop;
		G4double pretstart  = m_tstart[pointer]*unit_tstart;
		if (trackID==m_tid[pointer]&&tres&&signalT-pretstop<tres&&pretstart-signalT<tres) {
			if ( get_VerboseLevel() >= 5 ){
				std::cout<<"    Update Hit!!!"<<std::endl;
			}
			if(flag_edep) m_edep[pointer] += (edepIoni)/unit_edep;
			if(flag_edepAll) m_edepAll[pointer] += (edep)/unit_edepAll;
			//Set for root objects
			if(globalT<m_t[pointer]*unit_t){// is this possible?
				if(flag_x) m_x[pointer] = hitPosition.x()/unit_x;
				if(flag_y) m_y[pointer] = hitPosition.y()/unit_y;
				if(flag_z) m_z[pointer] = hitPosition.z()/unit_z;
				if(flag_t) m_t[pointer] = globalT/unit_t;
				if(flag_e) m_e[pointer] = total_e/unit_e;
				if(flag_ekin) m_ekin[pointer] = ekin/unit_ekin;
				if(flag_px) m_px[pointer] = pointIn_mom.x()/unit_px;
				if(flag_py) m_py[pointer] = pointIn_mom.y()/unit_py;
				if(flag_pz) m_pz[pointer] = pointIn_mom.z()/unit_pz;
			}
			else{
				if(flag_pOx) m_pOx[pointer] = pointOut_pos.x()/unit_pOx;
				if(flag_pOy) m_pOy[pointer] = pointOut_pos.y()/unit_pOy;
				if(flag_pOz) m_pOz[pointer] = pointOut_pos.z()/unit_pOz;
			}
			if (signalT<m_tstart[pointer]*unit_tstart) m_tstart[pointer] = signalT/unit_tstart;
			else if (signalT>m_tstop[pointer]*unit_tstop) m_tstop[pointer] = signalT/unit_tstop;
			if(flag_stepL) m_stepL[pointer] += stepL/unit_stepL;
			if(flag_nPair) m_nPair[pointer]++;
			if(flag_driftD) if(driftD<m_driftD[pointer]*unit_driftD) m_driftD[pointer] = driftD/unit_driftD;
		}
		else needpush = true;
	}
	else needpush = true;
	if (needpush){
		CDCSD_LINEINFO();
		CdcHit* newHit = new CdcHit();
		newHit->SetTrackID(trackID);
		newHit->SetLayerNo(layerId);
		newHit->SetCellNo(cellId);
		newHit->SetEdep(edepIoni);
		newHit->SetPos(hitPosition);
		newHit->SetDriftD(driftD);
		newHit->SetTheta(theta);
		newHit->SetPosFlag(0);
		newHit->SetEnterAngle(0);
		newHit->SetDriftT (signalT);
		newHit->SetGlobalT(globalT);
		hitsCollection->insert(newHit);
		G4int NbHits = hitsCollection->entries();
		hitPointer[layerId][cellId]=NbHits-1;
		//Set for root objects
		if(flag_x) m_x.push_back(hitPosition.x()/unit_x);
		if(flag_y) m_y.push_back(hitPosition.y()/unit_y);
		if(flag_z) m_z.push_back(hitPosition.z()/unit_z);
		if(flag_t) m_t.push_back(globalT/unit_t);
		if(flag_t) m_tstart.push_back(signalT/unit_tstart);
		if(flag_t) m_tstop.push_back(signalT/unit_tstop);
		if(flag_px) m_px.push_back(pointIn_mom.x()/unit_px);
		if(flag_py) m_py.push_back(pointIn_mom.y()/unit_py);
		if(flag_pz) m_pz.push_back(pointIn_mom.z()/unit_pz);
		if(flag_ekin) m_ekin.push_back(ekin/unit_ekin);
		if(flag_e) m_e.push_back(total_e/unit_e);
		if(flag_edep) m_edep.push_back(edepIoni/unit_edep);
		if(flag_edepAll) m_edepAll.push_back(edep/unit_edepAll);
		if(flag_stepL) m_stepL.push_back(stepL/unit_stepL);
		if(flag_nPair) m_nPair.push_back(1);
		if(flag_driftD) m_driftD.push_back(driftD/unit_driftD);
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
