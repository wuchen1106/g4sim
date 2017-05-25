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

#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Point3D.h"

#include "TVector3.h"

#include <iostream>
#include <vector>

#include "CdcCellGeometryParameter.hh"
//#include "CdcDigitizer.hh"
#include "CdcIonHit.hh"
#include "MyRoot.hh"
#include "MySD.hh"
#include "MyDetectorManager.hh"
#include "MyVGeometrySvc.hh"
#include "MyString2Anything.hh"

#include "CdcIonSD.hh"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
typedef HepGeom::Vector3D<double> HepVector3D;
#endif

CdcIonSD::CdcIonSD(G4String name, MyVGeometryParameter* pointer)
:MySD(name, pointer), hitsCollection(0)
{
  m_GeometryParameter= dynamic_cast<CdcCellGeometryParameter*> (pointer);
  if (!m_GeometryParameter){
//		std::cout<<"In CdcIonSD::CdcIonSD, cannot get CdcCellGeometryParameter pointer!!!"<<std::endl;
    G4Exception("CdcIonSD::CdcIonSD()",
        "InvalidSetup", FatalException,
        "cannot get CdcCellGeometryParameter pointer");
  }

	//name of related Digiteizer module
//  CdcDigitizer* cdcDM = new CdcDigitizer("CdcDigitizer");
//	MyDetectorManager::GetMyDetectorManager()->RegistorDM("CdcDigitizer");
//  G4DigiManager::GetDMpointer()->AddNewModule(cdcDM);

	//define fTolerance
	fTolerance = 2.2E-13;
}

CdcIonSD::~CdcIonSD(){
}

//-----------------------------------Initialize----------------------------------------------
//Will be called by geant4 automatically at the beginning of each event
void CdcIonSD::Initialize(G4HCofThisEvent* HCE)
{
	hitsCollection = new CdcIonHitsCollection
		(SensitiveDetectorName,collectionName[0]);
//	std::cout<<"====>In CdcIonSD::Initialize(), created hitsCollection at ("<<(void*)hitsCollection<<std::endl;
	G4int HCID = -1;
	HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
	if ( HCID < 0 ){
		G4Exception("CdcIonSD::Initialize()",
				"InvalidSetup", FatalException,
				"Cann not find hitsCollection of this event");
	}
	HCE->AddHitsCollection( HCID, hitsCollection );
	int LayerNo = m_GeometryParameter->get_LayerNo();
	hitPointer.resize(LayerNo);
	hitTime.resize(LayerNo);
	for( int  i = 0; i < LayerNo; i++ ){
		int CellNo = m_GeometryParameter->get_layer_cell_num(i);
		hitPointer[i].resize(CellNo);
		hitTime[i].resize(CellNo);
		for( int j = 0; j < CellNo; j++ ){
			hitPointer[i][j] = -1;
			hitTime[i][j] = -tres*2;
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
  m_pa.clear();
  m_e.clear();
  m_ccol.clear();
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
void CdcIonSD::SetBranch(){
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
  if( flag_pa ) myRoot->SetBranch(volName+"_pa", &m_pa);
  if( flag_e ) myRoot->SetBranch(volName+"_e", &m_e);
  if( flag_ccol ) myRoot->SetBranch(volName+"_ccol", &m_ccol);
  if( flag_edep ) myRoot->SetBranch(volName+"_edep", &m_edep);
  if( flag_layerID ) myRoot->SetBranch(volName+"_layerID", &m_layerID);
  if( flag_cellID ) myRoot->SetBranch(volName+"_cellID", &m_cellID);
  if( flag_tid ) myRoot->SetBranch(volName+"_tid", &m_tid);
  if( flag_pid ) myRoot->SetBranch(volName+"_pid", &m_pid);
}

//-----------------------------------ReadOutputCard----------------------------------------------
//Should be called before the begin of each run (now in the constructor of MyAnalysisSvc)
void CdcIonSD::ReadOutputCard(G4String filename){
	ReSet();
	G4String volName = get_VolName();
	std::ifstream fin_card(filename);
	if(!fin_card){
		std::cout<<"In CdcIonSD::ReadOutputCard, cannot open "<<filename<<"!!!"<<std::endl;
    G4Exception("CdcIonSD::ReadOutputCard()",
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
			else if( name == "pa" ) {flag_pa = true; buf_card>>unitName_pa; unit_pa = MyString2Anything::get_U(unitName_pa);}
			else if( name == "e" ) {flag_e = true; buf_card>>unitName_e; unit_e = MyString2Anything::get_U(unitName_e);}
			else if( name == "ccol" ) {flag_ccol = true; buf_card>>unitName_ccol; unit_ccol = MyString2Anything::get_U(unitName_ccol);}
			else if( name == "edep" ) {flag_edep = true; buf_card>>unitName_edep; unit_edep = MyString2Anything::get_U(unitName_edep);}
			else if( name == "cellID" ) flag_cellID = true;
			else if( name == "layerID" ) flag_layerID = true;
			else if( name == "tid" ) flag_tid = true;
			else if( name == "pid" ) flag_pid = true;
			else{
			  std::cout<<"In CdcIonSD::ReadOutputCard, unknown name: "<<name<<" in file "<<filename<<std::endl;
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
			else if( name == "maxn" ) buf_card>>maxn;
			else if( name == "ntracks" ) buf_card>>ntracks;
			else{
				G4double para;
				std::string unit;
				buf_card>>para>>unit;
				para *= MyString2Anything::get_U(unit);
				if( name == "minC" ) minC = para;
				else if( name == "mint" ) mint = para;
				else if( name == "maxt" ) maxt = para;
				else if( name == "tres" ) tres = para;
				else{
					std::cout<<"In CdcIonSD::ReadOutputCard, unknown name: "<<name<<" in file "<<filename<<std::endl;
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
		std::cout<<"In CdcIonSD::ReadOutputCard, failed to find enough section seperators for output in file "<<filename<<std::endl;
		std::cout<<"Will use default settings."<<std::endl;
		std::cout<<"********************************************"<<std::endl;
	}
	if ( n_filter_section_symbol<= 1 ){
	  std::cout<<"*****************WARNING********************"<<std::endl;
		std::cout<<"In CdcIonSD::ReadOutputCard, failed to find enough section seperators for filter in file "<<filename<<std::endl;
		std::cout<<"Will use default settings."<<std::endl;
		std::cout<<"********************************************"<<std::endl;
	}
	fin_card.close();
	ShowOutCard();
}

//-----------------------------------ReSet----------------------------------------------
//Private
void CdcIonSD::ReSet(){
  //default output set
  flag_nHits = false;
  flag_x = false;
  flag_y = false;
  flag_z = false;
  flag_t = false;
  flag_px = false;
  flag_py = false;
  flag_pz = false;
  flag_pa = false;
  flag_e = false;
  flag_ccol = false;
  flag_edep = false;
  flag_cellID = false;
  flag_layerID = false;
  flag_tid = false;
  flag_pid = false;
  //for fileter
  Switch = false;
  minC = 1;
  maxn = 1000;
  ntracks = 1;
  mint = 0;
  maxt = 0;
  tres = 0;
}

//-----------------------------------ShowOutCard----------------------------------------------
//Private
void CdcIonSD::ShowOutCard(){
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
  std::cout<<"output pa?     "<<(flag_pa?" yes":" no")<<", unit: "<<unitName_pa<<std::endl;
  std::cout<<"output e?      "<<(flag_e?" yes":" no")<<", unit: "<<unitName_e<<std::endl;
  std::cout<<"output ccol?   "<<(flag_ccol?" yes":" no")<<", unit: "<<unitName_ccol<<std::endl;
  std::cout<<"output edep?   "<<(flag_edep?" yes":" no")<<", unit: "<<unitName_edep<<std::endl;
  std::cout<<"output layerID?"<<(flag_layerID?" yes":" no")<<std::endl;
  std::cout<<"output cellID? "<<(flag_cellID?" yes":" no")<<std::endl;
  std::cout<<"output tid?    "<<(flag_tid?" yes":" no")<<std::endl;
  std::cout<<"output pid?    "<<(flag_pid?" yes":" no")<<std::endl;
  //for fileter
  std::cout<<"Switch on?     "<<(Switch?" yes":" no")<<std::endl;
  std::cout<<"minC =         "<<minC/MeV<<"MeV"<<std::endl;
  std::cout<<"maxn =         "<<maxn<<std::endl;
  std::cout<<"ntracks =      "<<ntracks<<std::endl;
  std::cout<<"mint =         "<<mint/ns<<"ns"<<std::endl;
  std::cout<<"maxt =         "<<maxt/ns<<"ns"<<std::endl;
	std::cout<<"******************************************************************************"<<std::endl;
}

//-----------------------------------ProcessHits----------------------------------------------
//Will be called by geant4 automatically everytime a step in CdcIonSD generated
G4bool CdcIonSD::ProcessHits(G4Step* aStep,G4TouchableHistory* touchableHistory)
{

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
  G4int ReplicaNo = touchable->GetVolume(0)->GetCopyNo();
  G4int layerId, cellId;
  status = m_GeometryParameter->get_layerIdcellId(ReplicaNo, &layerId, &cellId);
  if(status){
		std::cout<<"In CdcIonSD::ProcessHits(), unknown ReplicaNo = "<<ReplicaNo<<"!!! Will not generate hit"<<std::endl;
		return false;
  }

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

//*************************filter***********************
	//switch
	if (!Switch) return false;

  //ntracks
  if ( trackID != prevTrackID ){
    prevTrackID = trackID;
    nTracks++;
  }
	if ( nTracks > ntracks && ntracks) return false;

	//maxn
	if ( maxn && cdc_nHits >= maxn ) return false;

  //minC
	if ( minC && pointIn_pa < minC ) return false;

	//time_window
  if(std::isnan(globalT)){
    G4cout<<"CdcIonSD:error, globalT is nan "<<G4endl;
    return false;
  }
  if ( globalT < mint && mint ) return false;
  if ( globalT > maxt && maxt ) return false;

	//secondaries
	// suppose there would be 10 electrons in this step
	int nSec = 100;

//*************************calculate drift time and colected charge****************************
	G4ThreeVector hitPosition(0.,0.,0.);
	G4double driftD = 0;
	CalculateHitInfo(hitPosition,driftD,pointIn_pos,pointOut_pos,pointIn_mom,Bfield,layerId,cellId,charge,nSec);
	G4double driftT = 0;//should be calculated in future
	G4double ccol = 0;//should be calculated in future
	G4double hit_time = pointIn_time;

	//*******************generate or modify hit************************
	//tres
	//look for last hit in the same volume.
	G4int index = -1;
	G4double dt;
	if ( hitPointer[layerId][cellId] == -1 ){
	dt = tres*2.;
	}
	else{
		dt = hit_time - hitTime[layerId][cellId];
	}

	if ( get_VerboseLevel() >= 5 ){
		std::cout<<"in layer["<<layerId<<"], track ["<<trackID<<"] hit cell["<<cellId<<"]"<<std::endl;
	}
	if (dt >= tres){
		CdcIonHit* newHit = new CdcIonHit();
		newHit->SetTrackID(trackID);
		newHit->SetLayerNo(layerId);
		newHit->SetCellNo(cellId);
		newHit->SetEdep(edep);
		newHit->SetCcol(ccol);
		newHit->SetPos(hitPosition);
		newHit->SetTheta(theta);
		newHit->SetPosFlag(0);
		newHit->SetEnterAngle(0);
		newHit->SetDriftT (driftT);
		newHit->SetGlobalT(globalT);
		hitsCollection->insert(newHit);
		G4int NbHits = hitsCollection->entries();
		hitPointer[layerId][cellId]=NbHits-1;
		hitTime[layerId][cellId]=hit_time;
		//Set for root objects
		if(flag_x) m_x.push_back(hitPosition.x()/unit_x);
		if(flag_y) m_y.push_back(hitPosition.y()/unit_y);
		if(flag_z) m_z.push_back(hitPosition.z()/unit_z);
		if(flag_t) m_t.push_back(globalT/unit_t);
		if(flag_px) m_px.push_back(pointIn_mom.x()/unit_px);
		if(flag_py) m_py.push_back(pointIn_mom.y()/unit_py);
		if(flag_pz) m_pz.push_back(pointIn_mom.z()/unit_pz);
		if(flag_e) m_e.push_back(total_e/unit_e);
		if(flag_ccol) m_ccol.push_back(ccol/unit_ccol);
		if(flag_edep) m_edep.push_back(edep/unit_edep);
		if(flag_layerID) m_layerID.push_back(layerId);
		if(flag_cellID) m_cellID.push_back(cellId);
		if(flag_tid) m_tid.push_back(trackID);
		if(flag_pid) m_pid.push_back(pid);
		cdc_nHits++;
	}
	else{
		G4int pointer=hitPointer[layerId][cellId];
		G4double edepTemp = (*hitsCollection)[pointer]->GetEdep();
		G4double ccolTemp = (*hitsCollection)[pointer]->GetCcol();
		(*hitsCollection)[pointer]->SetEdep(edepTemp  + edep);
		(*hitsCollection)[pointer]->SetCcol(ccolTemp  + ccol);
		if(flag_edep) m_edep[pointer] = (edepTemp + edep)/unit_edep;
		if(flag_ccol) m_ccol[pointer] = (ccolTemp + ccol)/unit_ccol;
		G4double preDriftT = (*hitsCollection)[pointer]->GetDriftT();
		if (driftT < preDriftT) {
			(*hitsCollection)[pointer]->SetTrackID(trackID);
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
			if(flag_t) m_t[pointer] = globalT/unit_t;
			if(flag_px) m_px[pointer] = pointIn_mom.x()/unit_px;
			if(flag_py) m_py[pointer] = pointIn_mom.y()/unit_py;
			if(flag_pz) m_pz[pointer] = pointIn_mom.z()/unit_pz;
			if(flag_e) m_e[pointer] = total_e/unit_e;
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
void CdcIonSD::EndOfEvent(G4HCofThisEvent*){
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

//-----------------------------------CalculateHitInfo----------------------------------------------
//private
int CdcIonSD::CalculateHitInfo(G4ThreeVector &hitPosition, double &driftD, G4ThreeVector pointIn_pos, G4ThreeVector pointOut_pos, G4ThreeVector pointIn_mom, G4ThreeVector Bfield, int layerId, int cellId, int charge, int nSec){

	//Get position of signal wire
	G4double wire_pos_R, wire_pos_phi;
	G4double alpha = m_GeometryParameter->get_layer_angle4rotate(layerId)/2;
	wire_pos_R = m_GeometryParameter->get_layer_RMid(layerId)*cos(alpha/2);
	wire_pos_phi = m_GeometryParameter->get_layer_cell_phi(layerId, cellId);
	G4ThreeVector wire_pos(1.,1.,1.);
	wire_pos.setMag(wire_pos_R);
	wire_pos.setTheta(pi/2);
	wire_pos.setPhi(wire_pos_phi-alpha);//careful with the sign
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

	if ( get_VerboseLevel() >= 5 ){
		std::cout<<"  position of pointIn(rho,phi,z):"<<pointIn_pos.rho()/cm<<"cm, "<<pointIn_pos.phi()/deg<<"deg, "<<pointIn_pos.z()/cm<<"cm"<<std::endl;
		std::cout<<"  position of pointOut(rho,phi,z):"<<pointOut_pos.rho()/cm<<"cm, "<<pointOut_pos.phi()/deg<<"deg, "<<pointOut_pos.z()/cm<<"cm"<<std::endl;
		std::cout<<"  B(phi, theta, B): "<<B_direction.phi()/deg<<"deg, "<<B_direction.theta()/deg<<"deg, "<<B_amplitude/tesla<<"T"<<std::endl;
		std::cout<<"  momentum of pointIn(r,theta,phi):"<<pointIn_mom.mag()/MeV<<"MeV, "<<pointIn_mom.theta()/deg<<"deg, "<<pointIn_mom.phi()/deg<<"deg, pT = "<<pT/MeV<<"MeV"<<std::endl;
		std::cout<<"  position of wire(rho,phi,z):"<<wire_pos.rho()/cm<<"cm, "<<wire_pos.phi()/deg<<"deg, "<<wire_pos.z()/cm<<"cm"<<std::endl;
		std::cout<<"  direction of wire(theta, phi):"<<wire_direction.theta()/deg<<"deg, "<<wire_direction.phi()/deg<<"deg"<<std::endl;
	}

	//************************************1, r = 0******************************
	// the track is a line
	//calculate the distance between two straight lines
	if ( B_amplitude == 0 || pT == 0 || charge == 0 ){
		if ( get_VerboseLevel() >= 5 ){
			std::cout<<"  => r_helix = 0"<<std::endl;
		}
		G4ThreeVector mom_direction = pointIn_mom.unit();
		if (mom_direction.mag()==0){
			std::cout<<"WARNING!!! In CdcSD::FindClosestPoint(), momentum is zero!"<<std::endl;
			return 1;
		}
		G4double theta = mom_direction.theta(wire_direction);
		G4ThreeVector w2PI = pointIn_pos - wire_pos;
		G4double DpI = (w2PI - w2PI.dot(wire_direction)*wire_direction ).mag();
		if (theta == 0){
			//Special case!
			//-----------PI--------PO------
			//           |
			//-----C-----D------------------
			hitPosition = pointIn_pos + ( pointOut_pos - pointIn_pos )*G4UniformRand();
			driftD = DpI;
			return 0;
		}
		//Normal case!
		//----------------PI--------PO------
		//-          | Theta != 0          +
		//-----C-----D----------------------
		G4double d = (mom_direction.cross(wire_direction)).dot(w2PI);
		G4double lI = sqrt(DpI*DpI-d*d)/sin(theta);
		G4ThreeVector w2PO = pointOut_pos - wire_pos;
		G4double DpO = (w2PO - w2PO.dot(wire_direction)*wire_direction ).mag();
		G4double lO = sqrt(DpO*DpO-d*d)/sin(theta);
		G4ThreeVector point1 = pointIn_pos + lI*mom_direction;
		G4ThreeVector point2 = pointIn_pos - lI*mom_direction;
		G4ThreeVector w2P1 = point1 - wire_pos;
		G4ThreeVector w2P2 = point2 - wire_pos;
		G4double D1 = (w2P1 - w2P1.dot(wire_direction)*wire_direction ).mag();
		G4double D2 = (w2P2 - w2P2.dot(wire_direction)*wire_direction ).mag();
		bool Moving_Toward_Center = (D1 < D2);
		if (Moving_Toward_Center) lI *= -1;
		point1 = pointOut_pos - lO*mom_direction;
		point2 = pointOut_pos + lO*mom_direction;
		w2P1 = point1 - wire_pos;
		w2P2 = point2 - wire_pos;
		D1 = (w2P1 - w2P1.dot(wire_direction)*wire_direction).mag();
		D2 = (w2P2 - w2P2.dot(wire_direction)*wire_direction).mag();
		bool Moved_Passed_Center = (D1 < D2);
		if (!Moved_Passed_Center) lO *= -1;
		double lmin = 1e10; 
		for ( int i = 0; i < nSec; i++ ){
			double li = lI + (lO-lI)*G4UniformRand();
			if ( fabs(li) < fabs(lmin) ) lmin = li;
		}
		double lsin = lmin*sin(theta);
		driftD = sqrt(d*d+lsin*lsin);
		hitPosition = pointIn_pos + ( lmin - lI )*mom_direction;
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

		//pIC_D means the vector from pointIn to the centre of the curvature
		//pOC_D means the vector from pointOut to the centre of the curvature
		G4ThreeVector pTxBu_vec = pointIn_mom.cross(B_direction);
		pTxBu_vec.setMag(pT);
		G4ThreeVector pIC_D = (pTxBu_vec/eV) / (B_amplitude/tesla) / charge / (c_light/(m/s)) * m;
		G4ThreeVector pOC_D = ( pointIn_pos + pIC_D ) - pointOut_pos;

		//wC_D means the vector from wire_pos to centre of the curvature
		G4ThreeVector wC_D = ( pointIn_pos + pIC_D ) - wire_pos;

		std::cout<<"pIC_D @ phi = "<<pIC_D.phi()/deg<<"deg"<<std::endl;
		G4double phi_in = pIC_D.phi();
		if (phi_in<0) phi_in += 360.*deg;
		std::cout<<"phi_in = "<<phi_in/deg<<std::endl;
		G4double phi_out = pOC_D.phi();
		if (phi_out<0) phi_out += 360.*deg;
		std::cout<<"phi_out = "<<phi_out/deg<<std::endl;
		G4double phi_w = wC_D.phi();
		if (phi_w<0) phi_w += 360.*deg;
		std::cout<<"phi_w = "<<phi_w/deg<<std::endl;
		double phi_hit = 180.*deg + phi_w - 1e-10;
		for ( int i = 0; i < nSec; i++ ){
			double phi = phi_in + ( phi_out - phi_in )*G4UniformRand();
			double delta_phi = StandardDeltaAngle(phi-phi_w);
			double delta_phi_min = StandardDeltaAngle(phi_hit-phi_w);
			std::cout<<"    Sec["<<i<<"]: phi = "<<phi/deg<<"deg, delta_phi = "<<delta_phi/deg<<"deg"<<std::endl;
			if ( fabs(phi - phi_w) < fabs(phi_hit - phi_w) ){
				std::cout<<"      smaller than "<<phi_hit/deg<<" - "<<phi_w/deg<<" = "<<delta_phi_min/deg<<"deg"<<std::endl;
				phi_hit = phi;
			}
		}
		std::cout<<"    => phi_hit = "<<phi_hit/deg<<"deg"<<std::endl;
		G4ThreeVector pHC_D = pIC_D;
		pHC_D.setPhi(phi_hit);
		hitPosition = pointIn_pos + pIC_D - pHC_D;
		G4ThreeVector pHw_D = wire_pos - hitPosition;
		driftD = pHw_D.mag();
		G4int toTravel_derection = 0;
		if ( phi_out - phi_in > 0 ) toTravel_derection = 1;
		else toTravel_derection = -1;
		G4double toTravel_dz = (phi_hit - phi_in)*toTravel_derection/rad * (pointIn_mom.z()/eV) / (B_amplitude/tesla) / fabs(charge) / (c_light/(m/s)) * m;
		hitPosition.setZ( pointIn_pos.z() + toTravel_dz );

		//rotate back
		hitPosition.rotate( axis4rotate, -theta4rotate );

		if ( get_VerboseLevel() >= 5 ){
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
		G4double l_helix = - 2*pi*(pointIn_mom_B.z()/eV) / (c_light/(m/s)) / charge / (B_amplitude/tesla) * m;
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

		G4double B_direction_phi = B_direction.phi();
		if (B_direction_phi<0) B_direction_phi += 360.*deg;
		G4double theta4rotate_y = 90.*deg - B_direction.phi();
		B_direction.rotate(z_axis, theta4rotate_y);
		wire_pos.rotate(z_axis, theta4rotate_y);
		pointIn_pos.rotate(z_axis, theta4rotate_y);
		pointIn_mom.rotate(z_axis, theta4rotate_y);

		G4ThreeVector vec4move(wire_pos.x(),wire_pos.y(),0.);
		pointIn_pos -= vec4move;

		//=>Step 3, get parameters of initial state and final state
		G4double theta_helix = B_direction.theta();
		G4ThreeVector pTxBu_vec = pointIn_mom.cross(B_direction);
		pTxBu_vec.setMag(pT);
		G4ThreeVector cPI_D = - (pTxBu_vec/eV) / (B_amplitude/tesla) / charge / (c_light/(m/s)) * m;
		G4double phi_in = ( cPI_D.rotate(x_axis, theta_helix) ).phi();
		if (phi_in<0) phi_in += 360.*deg;
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

		//=>Step 5, generate secondaries and find the closest point
		G4double phi_hit = phi_in;
		G4double driftD_temp = 1e10;
		for ( int i = 0; i < nSec; i++ ){
			G4double phi =phi_in + traveled_dphi*G4UniformRand();
			G4double X = r_helix * ( cos(phi) + A );
			G4double Y = r_helix * ( sin(phi)*cosTheta + k*phi*sinTheta + B );
			G4double Z = r_helix * ( k*phi*cosTheta - sin(phi)*sinTheta - k*phi_in*cosTheta + z0 + sinPhi*sinTheta );
			G4ThreeVector pointSec(1,1,1);
			pointSec.setX(X);
			pointSec.setY(Y);
			pointSec.setZ(Z);
			G4double driftD_temp = pointSec.rho();
			if ( driftD_temp < driftD ){
				driftD = driftD_temp;
				hitPosition = pointSec;
			}
		}

		//=>Step 6, transform it back to lab coordinate system
		driftD = hitPosition.rho();
		hitPosition += vec4move;
		hitPosition.rotate(z_axis, -theta4rotate_y);
		hitPosition.rotate(axis4rotate_z, -theta4rotate_z);

		if ( get_VerboseLevel() >= 5 ){
			std::cout<<"    traveled_dphi = "<<traveled_dphi/deg
				       <<"deg, phi_in = "<<phi_in/deg
				       <<"deg, phi_out = "<<phi_out/deg
				       <<"deg"<<std::endl;
		}
	}

	if ( get_VerboseLevel() >= 5 ){
		std::cout<<"  hitPosition(rho,phi,z):"<<hitPosition.rho()/cm<<"cm, "<<hitPosition.phi()/deg<<"deg, "<<hitPosition.z()/cm<<"cm, driftD = "<<driftD/cm<<"cm"<<std::endl;
	}

	return 0;
}

G4double CdcIonSD::StandardDeltaAngle(G4double val){
	while ( val >= 180 || val < 0 ){
		if ( val>=180 ) val -= 360;
		if ( val < 0 ) val *= -1;
	}
	return val;
}
