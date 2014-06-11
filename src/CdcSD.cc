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
	m_pIx.clear();
	m_pIy.clear();
	m_pIz.clear();
	m_pOx.clear();
	m_pOy.clear();
	m_pOz.clear();
	m_t.clear();
	m_px.clear();
	m_py.clear();
	m_pz.clear();
	m_e.clear();
	m_edep.clear();
	m_nTry.clear();
	m_status.clear();
	m_error.clear();
	m_driftD_smeared.clear();
	m_driftD.clear();
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
void CdcSD::SetBranch(){
	MyRoot* myRoot = MyRoot::GetMyRoot();
	G4String volName = get_VolName();
	if( flag_nHits ) myRoot->SetBranch(volName+"_nHits", &cdc_nHits);
	if( flag_x ) myRoot->SetBranch(volName+"_x", &m_x);
	if( flag_y ) myRoot->SetBranch(volName+"_y", &m_y);
	if( flag_z ) myRoot->SetBranch(volName+"_z", &m_z);
	if( flag_pIx ) myRoot->SetBranch(volName+"_pIx", &m_pIx);
	if( flag_pIy ) myRoot->SetBranch(volName+"_pIy", &m_pIy);
	if( flag_pIz ) myRoot->SetBranch(volName+"_pIz", &m_pIz);
	if( flag_pOx ) myRoot->SetBranch(volName+"_pOx", &m_pOx);
	if( flag_pOy ) myRoot->SetBranch(volName+"_pOy", &m_pOy);
	if( flag_pOz ) myRoot->SetBranch(volName+"_pOz", &m_pOz);
	if( flag_t ) myRoot->SetBranch(volName+"_t", &m_t);
	if( flag_px ) myRoot->SetBranch(volName+"_px", &m_px);
	if( flag_py ) myRoot->SetBranch(volName+"_py", &m_py);
	if( flag_pz ) myRoot->SetBranch(volName+"_pz", &m_pz);
	if( flag_e ) myRoot->SetBranch(volName+"_e", &m_e);
	if( flag_edep ) myRoot->SetBranch(volName+"_edep", &m_edep);
	if( flag_nTry ) myRoot->SetBranch(volName+"_nTry", &m_nTry);
	if( flag_status ) myRoot->SetBranch(volName+"_status", &m_status);
	if( flag_error ) myRoot->SetBranch(volName+"_error", &m_error);
	if( flag_driftD_smeared ) myRoot->SetBranch(volName+"_driftD_smeared", &m_driftD_smeared);
	if( flag_driftD ) myRoot->SetBranch(volName+"_driftD", &m_driftD);
	if( flag_layerID ) myRoot->SetBranch(volName+"_layerID", &m_layerID);
	if( flag_cellID ) myRoot->SetBranch(volName+"_cellID", &m_cellID);
	if( flag_tid ) myRoot->SetBranch(volName+"_tid", &m_tid);
	if( flag_pid ) myRoot->SetBranch(volName+"_pid", &m_pid);
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
			else if( name == "pIx" ) {flag_pIx = true; buf_card>>unitName_pIx; unit_pIx = MyString2Anything::get_U(unitName_pIx);}
			else if( name == "pIy" ) {flag_pIy = true; buf_card>>unitName_pIy; unit_pIy = MyString2Anything::get_U(unitName_pIy);}
			else if( name == "pIz" ) {flag_pIz = true; buf_card>>unitName_pIz; unit_pIz = MyString2Anything::get_U(unitName_pIz);}
			else if( name == "pOx" ) {flag_pOx = true; buf_card>>unitName_pOx; unit_pOx = MyString2Anything::get_U(unitName_pOx);}
			else if( name == "pOy" ) {flag_pOy = true; buf_card>>unitName_pOy; unit_pOy = MyString2Anything::get_U(unitName_pOy);}
			else if( name == "pOz" ) {flag_pOz = true; buf_card>>unitName_pOz; unit_pOz = MyString2Anything::get_U(unitName_pOz);}
			else if( name == "t" ) {flag_t = true; buf_card>>unitName_t; unit_t = MyString2Anything::get_U(unitName_t);}
			else if( name == "px" ) {flag_px = true; buf_card>>unitName_px; unit_px = MyString2Anything::get_U(unitName_px);}
			else if( name == "py" ) {flag_py = true; buf_card>>unitName_py; unit_py = MyString2Anything::get_U(unitName_py);}
			else if( name == "pz" ) {flag_pz = true; buf_card>>unitName_pz; unit_pz = MyString2Anything::get_U(unitName_pz);}
			else if( name == "e" ) {flag_e = true; buf_card>>unitName_e; unit_e = MyString2Anything::get_U(unitName_e);}
			else if( name == "edep" ) {flag_edep = true; buf_card>>unitName_edep; unit_edep = MyString2Anything::get_U(unitName_edep);}
			else if( name == "nTry" ) {flag_nTry = true;}
			else if( name == "status" ) {flag_status = true;}
			else if( name == "error" ) {flag_error = true;}
			else if( name == "driftD_smeared" ) {flag_driftD_smeared = true; buf_card>>unitName_driftD_smeared; unit_driftD_smeared = MyString2Anything::get_U(unitName_driftD_smeared);}
			else if( name == "driftD" ) {flag_driftD = true; buf_card>>unitName_driftD; unit_driftD = MyString2Anything::get_U(unitName_driftD);}
			else if( name == "cellID" ) flag_cellID = true;
			else if( name == "layerID" ) flag_layerID = true;
			else if( name == "tid" ) flag_tid = true;
			else if( name == "pid" ) flag_pid = true;
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
	flag_pIx = false;
	flag_pIy = false;
	flag_pIz = false;
	flag_pOx = false;
	flag_pOy = false;
	flag_pOz = false;
	flag_t = false;
	flag_px = false;
	flag_py = false;
	flag_pz = false;
	flag_e = false;
	flag_edep = false;
	flag_nTry = false;
	flag_status = false;
	flag_error = false;
	flag_driftD = false;
	flag_driftD_smeared = false;
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
void CdcSD::ShowOutCard(){
	std::cout<<"*************************Output settings for "<<get_VolName()<<"***************************"<<std::endl;
	//default output set
	std::cout<<"output nHits?  "<<(flag_nHits?" yes":" no")<<std::endl;
	std::cout<<"output x?      "<<(flag_x?" yes":" no")<<", unit: "<<unitName_x<<std::endl;
	std::cout<<"output y?      "<<(flag_y?" yes":" no")<<", unit: "<<unitName_y<<std::endl;
	std::cout<<"output z?      "<<(flag_z?" yes":" no")<<", unit: "<<unitName_z<<std::endl;
	std::cout<<"output x?      "<<(flag_pIx?" yes":" no")<<", unit: "<<unitName_pIx<<std::endl;
	std::cout<<"output y?      "<<(flag_pIy?" yes":" no")<<", unit: "<<unitName_pIy<<std::endl;
	std::cout<<"output z?      "<<(flag_pIz?" yes":" no")<<", unit: "<<unitName_pIz<<std::endl;
	std::cout<<"output x?      "<<(flag_pOx?" yes":" no")<<", unit: "<<unitName_pOx<<std::endl;
	std::cout<<"output y?      "<<(flag_pOy?" yes":" no")<<", unit: "<<unitName_pOy<<std::endl;
	std::cout<<"output z?      "<<(flag_pOz?" yes":" no")<<", unit: "<<unitName_pOz<<std::endl;
	std::cout<<"output t?      "<<(flag_t?" yes":" no")<<", unit: "<<unitName_t<<std::endl;
	std::cout<<"output px?     "<<(flag_px?" yes":" no")<<", unit: "<<unitName_px<<std::endl;
	std::cout<<"output py?     "<<(flag_py?" yes":" no")<<", unit: "<<unitName_py<<std::endl;
	std::cout<<"output pz?     "<<(flag_pz?" yes":" no")<<", unit: "<<unitName_pz<<std::endl;
	std::cout<<"output e?      "<<(flag_e?" yes":" no")<<", unit: "<<unitName_e<<std::endl;
	std::cout<<"output edep?   "<<(flag_edep?" yes":" no")<<", unit: "<<unitName_edep<<std::endl;
	std::cout<<"output nTry?   "<<(flag_nTry?" yes":" no")<<std::endl;
	std::cout<<"output status?   "<<(flag_status?" yes":" no")<<std::endl;
	std::cout<<"output error?   "<<(flag_error?" yes":" no")<<std::endl;
	std::cout<<"output driftD?   "<<(flag_driftD?" yes":" no")<<", unit: "<<unitName_driftD<<std::endl;
	std::cout<<"output driftD_smeared?   "<<(flag_driftD_smeared?" yes":" no")<<", unit: "<<unitName_driftD_smeared<<std::endl;
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
	G4ThreeVector pointIn_mom = prePoint->GetMomentum();
	G4double pointIn_pa = pointIn_mom.mag();
	G4ThreeVector pointIn_pos = prePoint->GetPosition();
	G4ThreeVector pointOut_pos = postPoint->GetPosition();
	G4double pointIn_time = prePoint->GetGlobalTime();//Time since the event in which the track belongs is created

	// get step info
	G4double edep = aStep->GetTotalEnergyDeposit();

	// get layerId and cellId
	const G4VTouchable *touchable = prePoint->GetTouchable();
	G4int ReplicaNo = touchable->GetReplicaNumber(0);
	G4int layerId, cellId;
	status = m_GeometryParameter->get_layerIdcellId(ReplicaNo, &layerId, &cellId);
	if(status){
		std::cout<<"In CdcSD::ProcessHits(), unknown ReplicaNo = "<<ReplicaNo<<"!!! Will not generate hit"<<std::endl;
		return false;
	}
	G4VPhysicalVolume *phy_vol = touchable->GetVolume(0);
	//G4RotationMatrix  rot_mat = phy_vol->GetObjectRotationValue();
	//double phiX_touchable = rot_mat.phiX();
	//double phiY_touchable = rot_mat.phiY();
	//double phiZ_touchable = rot_mat.phiZ();
	//std::cout<<"### ("<<layerId
	//	     <<","<<cellId
	//	     <<") phiX_touchable = "<<phiX_touchable/deg
	//	     <<") phiY_touchable = "<<phiY_touchable/deg
	//	     <<") phiZ_touchable = "<<phiZ_touchable/deg
	//	     <<"deg, phi = "<<m_GeometryParameter->get_layer_cell_phi(layerId,cellId)/deg
	//	     <<"deg!"
	//	     <<std::endl;

	//fieldVal means the field value at pointIn_pos
	G4double fieldVal[3];
	G4double point[4];
	point[0] = pointIn_pos.x();
	point[1] = pointIn_pos.y();
	point[2] = pointIn_pos.z();
	point[3] = pointIn_time;
	const G4Field* field = G4TransportationManager::GetTransportationManager()->GetFieldManager()->GetDetectorField();
	field->GetFieldValue(point, fieldVal);
	G4ThreeVector Bfield(fieldVal[0], fieldVal[1], fieldVal[2]);

	CDCSD_LINEVAR(layerId);
	CDCSD_LINEVAR(cellId);
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
	if( edep <= minedep ) return false;
	CDCSD_LINEINFO();

	if ( get_VerboseLevel() >= 5 ){
		std::cout<<"in layer["<<layerId<<"], the track hit cell["<<cellId<<"]"<<std::endl;
	}

	//*************************calculate hitPosition****************************
	G4ThreeVector hitPosition(0.,0.,0.);
	G4double driftD = 0;
	G4double error = 0;
	G4int nTry = 0;
	status = FindClosestPoint(hitPosition, driftD, pointIn_pos, pointOut_pos, pointIn_mom, Bfield, layerId, cellId, charge, error, nTry);
	if (status){
		std::cout<<"In CdcSD::ProcessHits(), cannot find closest point!!! will not generate hit!"<<std::endl;
		return false;
	}
	double driftD_err = 0.2*mm;
	double driftD_smeared;
//	while (1) {
//		driftD_smeared = gRandom->Gaus(driftD, driftD_err);
//		if (driftD_smeared>0) break;
//	}
	// TODO: Should move this part to Digitizer
	//hitPosition = pointIn_pos;
	G4double driftV = 0.025*mm/ns;
	G4double driftT = driftD_smeared/driftV+pointIn_time;
	CDCSD_LINEINFO();

	//*******************generate or modify hit************************
	if (hitPointer[layerId][cellId] == -1){
		CDCSD_LINEINFO();
		CdcHit* newHit = new CdcHit();
		newHit->SetTrackID(trackID);
		newHit->SetLayerNo(layerId);
		newHit->SetCellNo(cellId);
		newHit->SetEdep(edep);
		newHit->SetPos(hitPosition);
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
		if(flag_pIx) m_pIx.push_back(pointIn_pos.x()/unit_pIx);
		if(flag_pIy) m_pIy.push_back(pointIn_pos.y()/unit_pIy);
		if(flag_pIz) m_pIz.push_back(pointIn_pos.z()/unit_pIz);
		if(flag_pOx) m_pOx.push_back(pointOut_pos.x()/unit_pOx);
		if(flag_pOy) m_pOy.push_back(pointOut_pos.y()/unit_pOy);
		if(flag_pOz) m_pOz.push_back(pointOut_pos.z()/unit_pOz);
		if(flag_t) m_t.push_back(globalT/unit_t);
		if(flag_px) m_px.push_back(pointIn_mom.x()/unit_px);
		if(flag_py) m_py.push_back(pointIn_mom.y()/unit_py);
		if(flag_pz) m_pz.push_back(pointIn_mom.z()/unit_pz);
		if(flag_e) m_e.push_back(total_e/unit_e);
		if(flag_edep) m_edep.push_back(edep/unit_edep);
		if(flag_error) m_error.push_back(error);
		if(flag_status) m_status.push_back(status);
		if(flag_nTry) m_nTry.push_back(nTry);
		if(flag_driftD) m_driftD.push_back(driftD/unit_driftD);
		if(flag_driftD_smeared) m_driftD_smeared.push_back(driftD_smeared/unit_driftD_smeared);
		if(flag_layerID) m_layerID.push_back(layerId);
		if(flag_cellID) m_cellID.push_back(cellId);
		if(flag_tid) m_tid.push_back(trackID);
		if(flag_pid) m_pid.push_back(pid);
		cdc_nHits++;
	}
	else{
		CDCSD_LINEINFO();
		G4int pointer=hitPointer[layerId][cellId];
		G4double edepTemp = (*hitsCollection)[pointer]->GetEdep();
		(*hitsCollection)[pointer]->SetEdep(edepTemp  + edep);
		if(flag_edep) m_edep[pointer] = (edepTemp + edep)/unit_edep;
		G4double preDriftT = (*hitsCollection)[pointer]->GetDriftT();
		G4double preDriftD = (*hitsCollection)[pointer]->GetDriftD();
		if ( get_VerboseLevel() >= 5 ){
			std::cout<<"  preDriftD = "<<preDriftD/cm
				     <<"cm, curDriftT = "<<preDriftT/ns
				     <<"ns, curDriftD = "<<driftD/cm
				     <<"cm, curDriftT = "<<driftT/ns
				     <<"ns"
				     <<std::endl;
		}
//		if (driftD < preDriftD) {
		if (driftT < preDriftT) {
			if ( get_VerboseLevel() >= 5 ){
				std::cout<<"    Update Hit!!!"<<std::endl;
			}
			(*hitsCollection)[pointer]->SetTrackID(trackID);
			(*hitsCollection)[pointer]->SetDriftD(driftD);
			(*hitsCollection)[pointer]->SetDriftT(driftT);
			(*hitsCollection)[pointer]->SetPos(hitPosition);
			(*hitsCollection)[pointer]->SetGlobalT(globalT);
			(*hitsCollection)[pointer]->SetTheta(theta);
			(*hitsCollection)[pointer]->SetPosFlag(0);
			(*hitsCollection)[pointer]->SetEnterAngle(0);
			//Set for root objects
			if(flag_x) m_x[pointer] = hitPosition.x()/unit_x;
			if(flag_y) m_y[pointer] = hitPosition.y()/unit_y;
			if(flag_z) m_z[pointer] = hitPosition.z()/unit_z;
			if(flag_pIx) m_pIx[pointer] = pointIn_pos.x()/unit_pIx;
			if(flag_pIy) m_pIy[pointer] = pointIn_pos.y()/unit_pIy;
			if(flag_pIz) m_pIz[pointer] = pointIn_pos.z()/unit_pIz;
			if(flag_pOx) m_pOx[pointer] = pointOut_pos.x()/unit_pOx;
			if(flag_pOy) m_pOy[pointer] = pointOut_pos.y()/unit_pOy;
			if(flag_pOz) m_pOz[pointer] = pointOut_pos.z()/unit_pOz;
			if(flag_t) m_t[pointer] = globalT/unit_t;
			if(flag_px) m_px[pointer] = pointIn_mom.x()/unit_px;
			if(flag_py) m_py[pointer] = pointIn_mom.y()/unit_py;
			if(flag_pz) m_pz[pointer] = pointIn_mom.z()/unit_pz;
			if(flag_e) m_e[pointer] = total_e/unit_e;
			if(flag_nTry) m_nTry[pointer] = nTry;
			if(flag_status) m_status[pointer] = status;
			if(flag_error) m_error[pointer] = error;
			if(flag_driftD_smeared) m_driftD_smeared[pointer] = driftD_smeared/unit_driftD_smeared;
			if(flag_driftD) m_driftD[pointer] = driftD/unit_driftD;
			if(flag_layerID) m_layerID[pointer] = layerId;
			if(flag_cellID) m_cellID[pointer] = cellId;
			if(flag_tid) m_tid[pointer] = trackID;
			if(flag_pid) m_pid[pointer] = pid;
		}
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

//-----------------------------------FindClosestPoint----------------------------------------------
//private
int CdcSD::FindClosestPoint(G4ThreeVector &closestPoint_pos, double &driftD,
		G4ThreeVector pointIn_pos, G4ThreeVector pointOut_pos, G4ThreeVector pointIn_mom, G4ThreeVector Bfield, int layerId, int cellId, int charge,
		G4double &error_max, G4int &nTry){

	nTry = 0;
	error_max = 0;
	//Get position of signal wire
	G4double wire_pos_R, wire_pos_phi;
	G4double alpha = m_GeometryParameter->get_layer_angle4rotate(layerId)/2;
	wire_pos_R = m_GeometryParameter->get_layer_RMid(layerId)*cos(alpha/2);
	wire_pos_phi = m_GeometryParameter->get_layer_cell_phi(layerId, cellId);
	G4ThreeVector wire_pos(1.,1.,1.);
	wire_pos.setMag(wire_pos_R);
	wire_pos.setTheta(pi/2);
	wire_pos.setPhi(wire_pos_phi);//careful with the sign
	G4double layer_halfzlen = m_GeometryParameter->get_layer_length(layerId)/2.; //In Geant4, soild has central-symmetry 
	G4double alpha_RMid = alpha/2;
	G4double wire_theta = atan(wire_pos_R*sin(alpha_RMid)/layer_halfzlen);
	G4ThreeVector wire_direction(1.,1.,1.);
	wire_direction.setMag(1.);
	wire_direction.setPhi(wire_pos_phi+ (alpha>0?pi/2:-pi/2));
	wire_direction.setTheta(wire_theta);
	wire_direction = wire_direction.unit();//r=0? self : unit 

	G4ThreeVector B_direction = Bfield.unit();
	G4double B_amplitude = Bfield.mag();
	G4double pT = (pointIn_mom.cross(B_direction)).mag();

	G4ThreeVector w2PI = pointIn_pos - wire_pos; // vector from wire(centre) to pointIn
	G4double DpI = (w2PI - w2PI.dot(wire_direction)*wire_direction ).mag(); // distance form pointIn to wire
	G4ThreeVector w2PO = pointOut_pos - wire_pos; // vector from wire(centre) to pointOut
	G4double DpO = (w2PO - w2PO.dot(wire_direction)*wire_direction ).mag(); // distance form pointOut to wire

	if ( get_VerboseLevel() >= 5 ){
		std::cout<<"  position of pointIn(rho,phi,z):"<<pointIn_pos.rho()/cm<<"cm, "<<pointIn_pos.phi()/deg<<"deg, "<<pointIn_pos.z()/cm<<"cm"<<std::endl;
		std::cout<<"  position of pointOut(rho,phi,z):"<<pointOut_pos.rho()/cm<<"cm, "<<pointOut_pos.phi()/deg<<"deg, "<<pointOut_pos.z()/cm<<"cm"<<std::endl;
		std::cout<<"  B(phi, theta, B): "<<B_direction.phi()/deg<<"deg, "<<B_direction.theta()/deg<<"deg, "<<B_amplitude/tesla<<"T"<<std::endl;
		std::cout<<"  momentum of pointIn(r,theta,phi):"<<pointIn_mom.mag()/MeV<<"MeV, "<<pointIn_mom.theta()/deg<<"deg, "<<pointIn_mom.phi()/deg<<"deg, pT = "<<pT/MeV<<"MeV"<<std::endl;
		std::cout<<"  position of wire(rho,phi,z):"<<wire_pos.rho()/cm<<"cm, "<<wire_pos.phi()/deg<<"deg, "<<wire_pos.z()/cm<<"cm"<<std::endl;
		std::cout<<"  direction of wire(theta, phi):"<<wire_direction.theta()/deg<<"deg, "<<wire_direction.phi()/deg<<"deg"<<std::endl;
		std::cout<<"  DpI = "<<DpI/cm<<"cm, DpO = "<<DpO/cm<<std::endl;
	}

	//************************************1, r = 0******************************
	//calculate the distance between two straight lines
	if ( B_amplitude == 0 || pT == 0 ){
		if ( get_VerboseLevel() >= 5 ){
			std::cout<<"  => r_helix = 0"<<std::endl;
		}
		G4ThreeVector mom_direction = pointIn_mom.unit();
		if (mom_direction.mag()==0){
			std::cout<<"WARNING!!! In CdcSD::FindClosestPoint(), momentum is zero!"<<std::endl;
			return 1;
		}
		G4double theta = mom_direction.theta(wire_direction); // angle between momentum and wire
		if (theta == 0){ // momentum parallels with wire, then no need to go on
			closestPoint_pos = pointIn_pos;
			driftD = DpI;
			return 0;
		}
		G4double d = (mom_direction.cross(wire_direction)).dot(w2PI); // distance from wire to (whole) track
		G4double l = sqrt(DpI*DpI-d*d)/cos(theta);
		G4ThreeVector point1 = pointIn_pos + l*mom_direction;
		G4ThreeVector point2 = pointIn_pos - l*mom_direction;
		G4ThreeVector w2P1 = point1 - wire_pos;
		G4ThreeVector w2P2 = point2 - wire_pos;
		G4double D1 = (w2P1 - w2P1.dot(wire_direction)*wire_direction ).mag();
		G4double D2 = (w2P2 - w2P2.dot(wire_direction)*wire_direction ).mag();
		driftD = d;
		closestPoint_pos = (D1 < D2 ? point1 : point2);
		G4double h2PI = ( pointIn_pos - closestPoint_pos ).mag();
		G4double h2PO = ( pointOut_pos - closestPoint_pos ).mag();
		G4double pI2PO = ( pointOut_pos - pointIn_pos ).mag();
		if ( h2PO > pI2PO || h2PI > pI2PO ){
			if ( h2PO > h2PI ){
				closestPoint_pos = pointIn_pos;
				driftD = DpI;
			}
			else{
				closestPoint_pos = pointOut_pos;
				driftD = DpO;
			}
		}
	}
	//***********************2, r = !0 && B_direction parallels wire******************************
	else if ( B_direction.isParallel(wire_direction, fTolerance) ){
		if ( get_VerboseLevel() >= 5 ){
			std::cout<<"  => B_direction parallels wire"<<std::endl;
		}
		//rotate z to wire direction
		G4double theta4rotate = wire_direction.theta();
		G4ThreeVector axis4rotate = (wire_direction.cross(G4ThreeVector(0,0,1))).unit();
		if (axis4rotate.mag() == 0){
			axis4rotate = G4ThreeVector(0., 0., 1.);
		}
		B_direction.rotate(axis4rotate, theta4rotate);
		wire_pos.rotate(axis4rotate, theta4rotate);
		pointIn_pos.rotate(axis4rotate, theta4rotate);
		pointOut_pos.rotate(axis4rotate, theta4rotate);
		pointIn_mom.rotate(axis4rotate, theta4rotate);
		//now wire_pos is the position of the projection of pointIn on the wire
		wire_pos.setZ(pointIn_pos.z());

		//pIC_D means the vector from pointIn to the centre of the curvature; z=0!!!
		//pOC_D means the vector from pointOut to the centre of the curvature; z!=0!!!
		G4ThreeVector pT_vec = pointIn_mom.cross(B_direction);
		pT_vec.setMag(pT);
		G4ThreeVector pIC_D = (pT_vec/eV) / (B_amplitude/tesla) / charge / (c_light/(m/s)) * m;
		G4ThreeVector pOC_D = ( pointIn_pos + pIC_D ) - pointOut_pos;

		//wC_D means the vector from wire_pos to centre of the curvature
		G4ThreeVector wC_D = ( pointIn_pos + pIC_D ) - wire_pos;
		if ( get_VerboseLevel() >= 5 ){
			std::cout<<"    [X(rho/cm,phi/deg,z/cm)]: pIC_D("<<pIC_D.rho()/cm
				<<","<<pIC_D.phi()/deg
				<<","<<pIC_D.z()/cm
				<<"), pOC_D("<<pOC_D.rho()/cm
				<<","<<pOC_D.phi()/deg
				<<","<<pOC_D.z()/cm
				<<"), wC_D("<<wC_D.rho()/cm
				<<","<<wC_D.phi()/deg
				<<","<<wC_D.z()/cm
				<<")"
				<<std::endl;
		}

		//how much phi has the partical traveled within this step.
		G4double traveled_dphi; 
		if ( pointIn_mom.z() != 0 ){
			traveled_dphi = fabs(((pOC_D.z() - pIC_D.z())/m) * (B_amplitude/tesla) * charge * (c_light/(m/s)) / (pointIn_mom.z()/eV) * rad);
		}
		else{
			traveled_dphi = 720 * deg;
		}

		//toTravel_dphi indicates how much phi distance(in intrinsic cylindrical coordinate system) does the partical have to travel to reach the closest point w.r.t wire.
		//toTravel_derection indicates whether the partical has to travel more than 180 deg or not
		G4double toTravel_dphi;
		G4double theta_pIHit = wC_D.theta(pIC_D);// 0~180 degree
		G4double theta_pOHit = wC_D.theta(pOC_D.perpPart());// 0~180 degree
		G4double toTravel_derection = (pointIn_mom.cross(pIC_D)).z() * (pIC_D.cross(wC_D)).z();//+: 0~180; -: 180~360
		if ( toTravel_derection < 0 ) toTravel_dphi = 360 * deg - theta_pIHit;
		else toTravel_dphi = theta_pIHit;
		G4double toTravel_dz = toTravel_dphi/rad * (pointIn_mom.z()/eV) / (B_amplitude/tesla) / fabs(charge) / (c_light/(m/s)) * m;

		//if toTravel_dphi >= traveled_dphi, then the pointOut or pointIn will be considered as the closest point, ie hit point.
		int hit_type = 0;
		if ( toTravel_dphi > traveled_dphi ){
			if ( theta_pIHit < theta_pOHit ){
				closestPoint_pos = pointIn_pos;
				hit_type = 1;
			}
			else{
				closestPoint_pos = pointOut_pos;
				hit_type = 2;
			}
		}
		else{
			closestPoint_pos = pointIn_pos;
			G4ThreeVector pHC_D = pIC_D;
			pHC_D.setPhi(wC_D.phi());
			closestPoint_pos += ( pIC_D - pHC_D );
			closestPoint_pos.setZ( pointIn_pos.z() + toTravel_dz );
		}
		driftD = (closestPoint_pos - wire_pos).perp();
		// rotate back to lab coordinates
		closestPoint_pos.rotate( axis4rotate, -theta4rotate );
		//driftD = pIC_D.mag() - wC_D.mag();

		if ( get_VerboseLevel() >= 5 ){
			std::cout<<"    traveled_dphi = "<<traveled_dphi/deg
				<<"deg, toTravel_dphi = "<<toTravel_dphi/deg
				<<"deg, theta_pIHit = "<<theta_pIHit/deg
				<<"deg, theta_pOHit = "<<theta_pOHit/deg
				<<"deg, type: "<<hit_type
				<<std::endl;
		}
	}
	//***********************3, r = !0 && B_direction does not parallel wire******************************
	else{
		if ( get_VerboseLevel() >= 5 ){
			std::cout<<"  => B_direction does not parallel wire"<<std::endl;
		}
		G4ThreeVector x_axis(1.,0.,0.),y_axis(0.,1.,0.),z_axis(0.,0.,1.);

		//=>Step 1, Calculate helix parameters
		//rotate z to B direction
		G4double theta4rotate_B = B_direction.theta();
		G4ThreeVector axis4rotate_B = (B_direction.cross(G4ThreeVector(0,0,1))).unit();
		if (axis4rotate_B.mag() == 0){
			axis4rotate_B = G4ThreeVector(0., 0., 1.);
		}
		G4ThreeVector pointIn_pos_B = pointIn_pos;
		G4ThreeVector pointOut_pos_B = pointOut_pos;
		G4ThreeVector pointIn_mom_B = pointIn_mom;
		pointIn_pos_B.rotate(axis4rotate_B, theta4rotate_B);
		pointOut_pos_B.rotate(axis4rotate_B, theta4rotate_B);
		pointIn_mom_B.rotate(axis4rotate_B, theta4rotate_B);
		//calculate l_helix
		//  l_helix>0: right hand helix; l_helix<0: left hand helix;
		G4double l_helix = - (pointIn_mom_B.z()/eV) / (c_light/(m/s)) / charge / (B_amplitude/tesla) * m;
		//calculate traveled_dphi( with sign )
		G4double traveled_dphi;
		if ( l_helix != 0 ){
			traveled_dphi = ( pointOut_pos_B.z() - pointIn_pos_B.z() ) / l_helix;
		}
		else {
			traveled_dphi = 720 * deg;
		}
		//calculate r_helix( positive )
		G4double r_helix = fabs( (pT/eV) / (c_light/(m/s)) / charge / (B_amplitude/tesla) ) * m;

		//=>Step 2, rotate z to wire direction, y to Bxy direction and move (x=0,y=0) to wire
		G4double theta4rotate_z = wire_direction.theta();
		G4ThreeVector axis4rotate_z = (wire_direction.cross(G4ThreeVector(0,0,1))).unit();
		if (axis4rotate_z.mag() == 0){
			axis4rotate_z = G4ThreeVector(0., 0., 1.);
		}
		B_direction.rotate(axis4rotate_z, theta4rotate_z);
		wire_pos.rotate(axis4rotate_z, theta4rotate_z);
		pointIn_pos.rotate(axis4rotate_z, theta4rotate_z);
		pointIn_mom.rotate(axis4rotate_z, theta4rotate_z);

		G4double theta4rotate_y = 90.*deg - B_direction.phi();
		B_direction.rotate(z_axis, theta4rotate_y);
		wire_pos.rotate(z_axis, theta4rotate_y);
		pointIn_pos.rotate(z_axis, theta4rotate_y);
		pointIn_mom.rotate(z_axis, theta4rotate_y);

		G4ThreeVector vec4move(wire_pos.x(),wire_pos.y(),0.);
		pointIn_pos -= vec4move;

		//=>Step 3, get parameters of initial state and final state
		G4double theta_helix = B_direction.theta();
		G4ThreeVector pT_vec = pointIn_mom.cross(B_direction);
		pT_vec.setMag(pT);
		G4ThreeVector cPI_D = - (pT_vec/eV) / (B_amplitude/tesla) / charge / (c_light/(m/s)) * m;
		G4double phi_in = ( cPI_D.rotate(x_axis, theta_helix) ).phi();
		G4double phi_out = phi_in + traveled_dphi;

		//=>Step 4, get parameters of the function for distance
		//d(D^2/r^2)/d(phi)) = a[0] + a[1]*sin(2*phi) + a[2]*sin(phi) + a[3]*cos(phi) + a[4]*phi*cos(phi) + a[5]*phi
		G4double x0 = pointIn_pos.x()/r_helix;
		G4double y0 = pointIn_pos.y()/r_helix;
		G4double z0 = pointIn_pos.z()/r_helix;
		G4double k = l_helix/r_helix;
		G4double cosPhi = cos(phi_in);
		G4double sinPhi = sin(phi_in);
		G4double sinTheta = sin(theta_helix);
		G4double cosTheta = cos(theta_helix);
		G4double A = x0- cosPhi;
		G4double B = y0- sinPhi*cosTheta - k*phi_in*sinTheta;
		G4double a[6];
		a[0] = B*k*sinTheta;
		a[1] = -sinTheta*sinTheta/2;
		a[2] = k*sinTheta*cosTheta-A;
		a[3] = B;
		a[4] = k*sinTheta*cosTheta;
		a[5] = k*k*sinTheta*sinTheta;
		G4double phi_left, phi_right;
		if ( traveled_dphi > 0 ){
			phi_left = phi_in;
			phi_right = phi_out;
		}
		else{
			phi_left = phi_out;
			phi_right = phi_in;
		}

		//=>Step 5, find phi
		//About type:
		//0: f'>=0 && f''<0;
		//1: f'<0 && f''<=0;
		//2: f'<0 && f''>=0;
		//3: f'>=0 && f''>0;
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//ATTENTION!!! This method will fail if there are more than 4 segments(devided according to types) in this step
		int type_left = check_type(a,phi_left);
		int type_right = check_type(a,phi_right);
		if ( get_VerboseLevel() >= 5 ){
			std::cout<<"    type_left = "<<type_left<<", type_right == "<<type_right<<std::endl;
		}
		G4int nTry_max = 100;
		double error = 0;
		G4double root_left = NewtonRootFinding(a,phi_left,nTry_max,fTolerance,error);
		if (error_max<fabs(error)) error_max = fabs(error);
		G4double root_right = NewtonRootFinding(a,phi_right,nTry_max,fTolerance,error);
		if (error_max<fabs(error)) error_max = fabs(error);
		G4double b[4];
		b[0] = theta_helix;
		b[1] = k;
		b[2] = A;
		b[3] = B;
		G4double dist2_left = calculateDist2(b,phi_left);
		G4double dist2_right = calculateDist2(b,phi_right);
		G4double phi;
		if ( (type_left == type_right && type_right <= 1 && root_left != root_right) || (type_left == 1 && type_right == 0) ){
			if ( get_VerboseLevel() >= 5 ){
				std::cout<<"    =>Need to find minimum value"<<std::endl;
			}
			//half finding
			G4double left = root_left;
			G4double right = root_right;
			G4double mid;
			int iTry = 0;
			while (1){
				mid = ( left + right )/2;
				int type = check_type(a,mid);
				if ( type == 1 || type == 2 ) break;
				else if (type == 0) right = mid;
				else left = mid;
				if ( iTry++ >= nTry_max ){
					//std::cout<<"WARNING: in CdcSD::FindClosestPoint(), iTry reached maximum limit "<<iTry<<std::endl;
					break;
				}
			}
			if (nTry<iTry) nTry = iTry;
			G4double root_mid = NewtonRootFinding(a,mid,nTry_max,fTolerance,error);
			if (error_max<fabs(error)) error_max = fabs(error);
			G4double dist2_root = calculateDist2(b,root_mid);
			int index = findSmallest(dist2_left, dist2_right, dist2_root);
			if ( index == 0 ) phi = phi_left;
			else if ( index == 1 ) phi = phi_right;
			else phi = root_mid;
		}
		else if( (type_left <= 2 && type_right == 3) ||
				(type_left == type_right && type_right == 2 && root_left != root_right) ||
				(type_left == type_right && type_right == 3 && root_left != root_right) ||
				(type_left == 2 && type_right <= 1) ){
			if ( get_VerboseLevel() >= 5 ){
				std::cout<<"    =>Got minimum value"<<std::endl;
			}
			G4double dist2_root = calculateDist2(b,root_right);
			int index = findSmallest(dist2_left, dist2_right, dist2_root);
			if ( index == 0 ) phi = phi_left;
			else if ( index == 1 ) phi = phi_right;
			else phi = root_right;
		}
		else {
			if ( get_VerboseLevel() >= 5 ){
				std::cout<<"    =>Don't need minimum value"<<std::endl;
			}
			phi = (dist2_left < dist2_right ? phi_left : phi_right);
		}

		//=>Step 6, get the point
		G4double X = r_helix * ( cos(phi) + A );
		G4double Y = r_helix * ( sin(phi)*cosTheta + k*phi*sinTheta + B );
		G4double Z = r_helix * ( k*phi*cosTheta - sin(phi)*sinTheta - k*phi_in*cosTheta + z0 + sinPhi*sinTheta );
		closestPoint_pos.setX(X);
		closestPoint_pos.setY(Y);
		closestPoint_pos.setZ(Z);

		//=>Step 7, transform it back to lab coordinate system
		driftD = closestPoint_pos.rho();
		closestPoint_pos += vec4move;
		closestPoint_pos.rotate(z_axis, -theta4rotate_y);
		closestPoint_pos.rotate(axis4rotate_z, -theta4rotate_z);

		if ( get_VerboseLevel() >= 5 ){
			std::cout<<"    traveled_dphi = "<<traveled_dphi/deg
				<<"deg, phi_in = "<<phi_in/deg
				<<"deg, phi_out = "<<phi_out/deg
				<<"deg, phi = "<<phi/deg
				<<"deg"<<std::endl;
		}
	}
	if ( get_VerboseLevel() >= 5 ){
		std::cout<<"  closestPoint_pos(rho,phi,z):"<<closestPoint_pos.rho()/cm<<"cm, "<<closestPoint_pos.phi()/deg<<"deg, "<<closestPoint_pos.z()/cm<<"cm, driftD = "<<driftD/cm<<"cm"<<std::endl;
	}
	return 0;
}
int CdcSD::check_type(G4double a[6], G4double phi){
	//0: f'>=0 && f''<0;
	//1: f'<0 && f''<=0;
	//2: f'<0 && f''>=0;
	//3: f'>=0 && f''>0;
	//d(D^2/r^2)/d(phi)) = a[0] + a[1]*sin(2*phi) + a[2]*sin(phi) + a[3]*cos(phi) + a[4]*phi*cos(phi) + a[5]*phi
	//dd(D^2/r^2)/dd(phi)) = 2*a[1]*cos(2*phi) + (a[2]+a[4])*cos(phi) - a[3]*sin(phi) - a[4]*phi*sin(phi) + a[5]
	G4double f1 = a[0] + a[1]*sin(2*phi) + a[2]*sin(phi) + a[3]*cos(phi) + a[4]*phi*cos(phi) + a[5]*phi;
	G4double f2 = 2*a[1]*cos(2*phi) + (a[2]+a[4])*cos(phi) - a[3]*sin(phi) - a[4]*phi*sin(phi) + a[5];
	int type;
	if (f1>=0 && f2<0) type = 0;
	else if (f1<0 && f2<=0) type = 1;
	else if (f1<0 && f2>=0) type = 2;
	else if (f1>=0 && f2>0) type = 3;
	return type;
}
double CdcSD::NewtonRootFinding(G4double a[6], G4double phi, G4int nTry_max, G4double torl, G4double &error){
	G4double delta = 10e10;
	G4int iTry = 0;
	while( fabs(delta) > torl ){
		iTry++;
		if( iTry > nTry_max ){
			//std::cout<<"WARNING: in CdcSD::NewtonRootFinding(), iTry reached maximum limit "<<iTry<<std::endl;
			break;
		}
		//d(D^2/r^2)/d(phi)) = a[0] + a[1]*sin(2*phi) + a[2]*sin(phi) + a[3]*cos(phi) + a[4]*phi*cos(phi) + a[5]*phi
		//dd(D^2/r^2)/dd(phi)) = 2*a[1]*cos(2*phi) + (a[2]+a[4])*cos(phi) - a[3]*sin(phi) - a[4]*phi*sin(phi) + a[5]
		delta = a[0] + a[1]*sin(2*phi) + a[2]*sin(phi) + a[3]*cos(phi) + a[4]*phi*cos(phi) + a[5]*phi;
		G4double k = 2*a[1]*cos(2*phi) + (a[2]+a[4])*cos(phi) - a[3]*sin(phi) - a[4]*phi*sin(phi) + a[5];
		if ( k == 0 ){
			//std::cout<<"WARNING: in CdcSD::NewtonRootFinding(), k = 0!!!"<<std::endl;
			phi += pi/50;
		}
		else phi -= delta/k;
	}
	error = delta;
	return phi;
}
double CdcSD::calculateDist2(G4double b[4], G4double phi){
	//b[0] = theta_helix;
	//b[1] = k;
	//b[2] = A;
	//b[3] = B;
	G4double dx = cos(phi)+b[2];
	G4double dy = sin(phi)*cos(b[0])+b[1]*phi*sin(b[0])+b[3];
	G4double dist2 = dx*dx + dy*dy;
	return dist2;
}
int CdcSD::findSmallest( double a, double b, double c){
	if ( a < b ){
		if ( a < c ) return 0;
		else return 2;
	}
	else if ( b<c ) return 1;
	else return 2;
}
