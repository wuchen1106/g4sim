//---------------------------------------------------------------------------
//Description: Sensitive detector definition for CdcLayer
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "globals.hh"
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

#include "MyString2Anything.hh"
#include "SimpleGeometryParameter.hh"
#include "CdcLayerHit.hh"
#include "MyRoot.hh"
#include "MySD.hh"

#include "CdcLayerSD.hh"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
typedef HepGeom::Vector3D<double> HepVector3D;
#endif

CdcLayerSD::CdcLayerSD(G4String name, MyVGeometryParameter* pointer)
:MySD(name, pointer), hitsCollection(0)
{
  m_GeometryParameter= dynamic_cast<SimpleGeometryParameter*> (pointer);
  if (!m_GeometryParameter){
		std::cout<<"In CdcLayerSD::CdcLayerSD, cannot get SimpleGeometryParameter pointer!!!"<<std::endl;
    G4Exception("CdcLayerSD::CdcLayerSD()",
        "InvalidSetup", FatalException,
        "cannot get SimpleGeometryParameter pointer");
  }
}

CdcLayerSD::~CdcLayerSD(){
}

//-----------------------------------Initialize----------------------------------------------
//Will be called by geant4 automatically at the beginning of each event
void CdcLayerSD::Initialize(G4HCofThisEvent* HCE)
{
	hitsCollection = new CdcLayerHitsCollection
		(SensitiveDetectorName,collectionName[0]);
//	std::cout<<"====>In CdcLayerSD::Initialize(), created hitsCollection at ("<<(void*)hitsCollection<<std::endl;
	G4int HCID = -1;
	HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
	if ( HCID < 0 ){
		G4Exception("CdcLayerSD::Initialize()",
				"InvalidSetup", FatalException,
				"Cann not find hitsCollection of this event");
	}
	HCE->AddHitsCollection( HCID, hitsCollection );
  //initialize for root objects
  nHits = 0;
  m_x.clear();
  m_y.clear();
  m_z.clear();
  m_t.clear();
  m_px.clear();
  m_py.clear();
  m_pz.clear();
  m_e.clear();
  m_edep.clear();
  m_volID.clear();
  m_volName.clear();
  m_tid.clear();
  m_pid.clear();
  m_charge.clear();
  m_trackID.clear();
  m_trackLooping.clear();
}

//-----------------------------------SetBranch----------------------------------------------
//Should be called at the beginning of each run (now in MyAnalysisSvc::BeginOfRunAction())
void CdcLayerSD::SetBranch(){
	MyRoot* myRoot = MyRoot::GetMyRoot();
	G4String volName = get_VolName();
  if( flag_nHits ) myRoot->SetBranch(volName+"_nHits", &nHits);
  if( flag_x ) myRoot->SetBranch(volName+"_x", &m_x);
  if( flag_y ) myRoot->SetBranch(volName+"_y", &m_y);
  if( flag_z ) myRoot->SetBranch(volName+"_z", &m_z);
  if( flag_t ) myRoot->SetBranch(volName+"_t", &m_t);
  if( flag_px ) myRoot->SetBranch(volName+"_px", &m_px);
  if( flag_py ) myRoot->SetBranch(volName+"_py", &m_py);
  if( flag_pz ) myRoot->SetBranch(volName+"_pz", &m_pz);
  if( flag_e ) myRoot->SetBranch(volName+"_e", &m_e);
  if( flag_edep ) myRoot->SetBranch(volName+"_edep", &m_edep);
  if( flag_volID ) myRoot->SetBranch(volName+"_volID", &m_volID);
  if( flag_volName ) myRoot->SetBranch(volName+"_volName", &m_volName);
  if( flag_tid ) myRoot->SetBranch(volName+"_tid", &m_tid);
  if( flag_pid ) myRoot->SetBranch(volName+"_pid", &m_pid);
  if( flag_charge ) myRoot->SetBranch(volName+"_charge", &m_charge);
}

//-----------------------------------ReadOutputCard----------------------------------------------
//Should be called before the begin of each run (now in the constructor of MyAnalysisSvc)
void CdcLayerSD::ReadOutputCard(G4String filename){
	ReSet();
	std::ifstream fin_card(filename);
	if(!fin_card){
		std::cout<<"In CdcLayerSD::ReadOutputCard, cannot open "<<filename<<"!!!"<<std::endl;
    G4Exception("CdcLayerSD::ReadOutputCard()",
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
			else if( name == "x" ) {{flag_x = true; buf_card>>unitName_x; unit_x = MyString2Anything::get_U(unitName_x);}}
			else if( name == "y" ) {{flag_y = true; buf_card>>unitName_y; unit_y = MyString2Anything::get_U(unitName_y);}}
			else if( name == "z" ) {{flag_z = true; buf_card>>unitName_z; unit_z = MyString2Anything::get_U(unitName_z);}}
			else if( name == "t" ) {{flag_t = true; buf_card>>unitName_t; unit_t = MyString2Anything::get_U(unitName_t);}}
			else if( name == "px" ) {{flag_px = true; buf_card>>unitName_px; unit_px = MyString2Anything::get_U(unitName_px);}}
			else if( name == "py" ) {{flag_py = true; buf_card>>unitName_py; unit_py = MyString2Anything::get_U(unitName_py);}}
			else if( name == "pz" ) {{flag_pz = true; buf_card>>unitName_pz; unit_pz = MyString2Anything::get_U(unitName_pz);}}
			else if( name == "e" ) {{flag_e = true; buf_card>>unitName_e; unit_e = MyString2Anything::get_U(unitName_e);}}
			else if( name == "edep" ) {{flag_edep = true; buf_card>>unitName_edep; unit_edep = MyString2Anything::get_U(unitName_edep);}}
			else if( name == "volID" ) {flag_volID = true;}
			else if( name == "volName" ) {flag_volName = true;}
			else if( name == "tid" ) {flag_tid = true;}
			else if( name == "pid" ) {flag_pid = true;}
			else if( name == "charge" ) {flag_charge = true;}
			else{
			  std::cout<<"In CdcLayerSD::ReadOutputCard, unknown name: "<<name<<" in file "<<filename<<std::endl;
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
			else if( name == "FirstLoop" ) FirstLoop = true;
			else if( name == "maxn" ) buf_card>>maxn;
			else{
				G4double para;
				G4String unit;
				buf_card>>para>>unit;
				para *= MyString2Anything::get_U(unit);
				if( name == "minp" ) minp = para;
				else if ( name == "minedep" ) minedep = para;
				else if( name == "mint" ) mint = para;
				else if( name == "maxt" ) maxt = para;
				else if( name == "tres" ) tres = para;
				else{
					std::cout<<"In CdcLayerSD::ReadOutputCard, unknown name: "<<name<<" in file "<<filename<<std::endl;
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
		std::cout<<"In CdcLayerSD::ReadOutputCard, failed to find enough section seperators \""<<BRANCHSECTIONNAME<<"\" for output in file "<<filename<<std::endl;
		std::cout<<"Will use default settings."<<std::endl;
		std::cout<<"********************************************"<<std::endl;
	}
	if ( n_filter_section_symbol<= 1 ){
	  std::cout<<"*****************WARNING********************"<<std::endl;
		std::cout<<"In CdcLayerSD::ReadOutputCard, failed to find enough section seperators \""<<FILTERSECTIONNAME<<"\" for filter in file "<<filename<<std::endl;
		std::cout<<"Will use default settings."<<std::endl;
		std::cout<<"********************************************"<<std::endl;
	}
	fin_card.close();
	ShowOutCard();
}

//-----------------------------------ReSet----------------------------------------------
//Private
void CdcLayerSD::ReSet(){
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
  flag_volID = false;
  flag_volName = false;
  flag_tid = false;
  flag_pid = false;
  flag_charge = false;
  //for fileter
  Switch = false;
  neutralCut = false;
  FirstLoop = false;
  minp = 0;
  maxn = 0;
  mint = 0;
  maxt = 0;
  tres = 0;
  minedep = -1*MeV;
}

//-----------------------------------ShowOutCard----------------------------------------------
//Private
void CdcLayerSD::ShowOutCard(){
	std::cout<<"*************************Output settings for "<<get_VolName()<<"***************************"<<std::endl;
  //default output set
  std::cout<<"output nHits?   "<<(flag_nHits?" yes":" no")<<std::endl;
  std::cout<<"output x?       "<<(flag_x?" yes":" no")<<", unit: "<<unitName_x<<std::endl;
  std::cout<<"output y?       "<<(flag_y?" yes":" no")<<", unit: "<<unitName_y<<std::endl;
  std::cout<<"output z?       "<<(flag_z?" yes":" no")<<", unit: "<<unitName_z<<std::endl;
  std::cout<<"output t?       "<<(flag_t?" yes":" no")<<", unit: "<<unitName_t<<std::endl;
  std::cout<<"output px?      "<<(flag_px?" yes":" no")<<", unit: "<<unitName_px<<std::endl;
  std::cout<<"output py?      "<<(flag_py?" yes":" no")<<", unit: "<<unitName_py<<std::endl;
  std::cout<<"output pz?      "<<(flag_pz?" yes":" no")<<", unit: "<<unitName_pz<<std::endl;
  std::cout<<"output e?       "<<(flag_e?" yes":" no")<<", unit: "<<unitName_e<<std::endl;
  std::cout<<"output edep?    "<<(flag_edep?" yes":" no")<<", unit: "<<unitName_edep<<std::endl;
  std::cout<<"output volID?   "<<(flag_volID?" yes":" no")<<std::endl;
  std::cout<<"output volName? "<<(flag_volName?" yes":" no")<<std::endl;
  std::cout<<"output tid?     "<<(flag_tid?" yes":" no")<<std::endl;
  std::cout<<"output pid?     "<<(flag_pid?" yes":" no")<<std::endl;
  std::cout<<"output charge?  "<<(flag_charge?" yes":" no")<<std::endl;
  //for fileter
  std::cout<<"Switch on?      "<<(Switch?"yes":"no")<<std::endl;
  std::cout<<"neutralCut on?  "<<(neutralCut?"yes":"no")<<std::endl;
  std::cout<<"FirstLoop on?   "<<(FirstLoop?"yes":"no")<<std::endl;
  std::cout<<"minp =          "<<minp/MeV<<"MeV"<<std::endl;
  std::cout<<"maxn =          "<<maxn<<std::endl;
  std::cout<<"mint =          "<<mint/ns<<"ns"<<std::endl;
  std::cout<<"maxt =          "<<maxt/ns<<"ns"<<std::endl;
  std::cout<<"tres =          "<<tres/ns<<"ns"<<std::endl;
  std::cout<<"minedep =       "<<minedep/MeV<<"MeV"<<std::endl;
	std::cout<<"******************************************************************************"<<std::endl;
}

//-----------------------------------ProcessHits----------------------------------------------
//Will be called by geant4 automatically everytime a step in CdcLayerSD generated
G4bool CdcLayerSD::ProcessHits(G4Step* aStep,G4TouchableHistory* touchableHistory)
{

//*************************get useful variables***********************

	// get track info
  G4Track* gTrack = aStep->GetTrack() ;
  G4int trackID= gTrack->GetTrackID(); //G4 track ID of current track.
  G4int charge = gTrack->GetDefinition()->GetPDGCharge();
  G4int pid = gTrack->GetDefinition()->GetPDGEncoding();

  // get information at the beginning and at the end of step
  G4StepPoint* prePoint  = aStep->GetPreStepPoint() ;
  G4StepPoint* postPoint = aStep->GetPostStepPoint() ;
  G4ThreeVector pointIn_pos = prePoint->GetPosition();
  G4ThreeVector pointOut_pos = postPoint->GetPosition();

	// get volume info
  const G4VTouchable *touchable = prePoint->GetTouchable();
  G4int ReplicaNo = touchable->GetVolume(0)->GetCopyNo();
	G4String VolName = touchable->GetVolume(0)->GetName();
	int VolId = m_GeometryParameter->get_VolIndex(VolName);
	int TubsId = m_GeometryParameter->get_SolidIndex(VolId);
	double RMax = m_GeometryParameter->get_Tubs_RMax(TubsId,ReplicaNo);
	double RMin = m_GeometryParameter->get_Tubs_RMin(TubsId,ReplicaNo);
	int nLayers = m_GeometryParameter->get_RepNo(VolId);
	//std::cout<<"ReplicaNo = "<<ReplicaNo<<", RMax = "<<RMax<<", RMin = "<<RMin<<", nLayers = "<<nLayers<<std::endl;

  // get step info
  G4double edep = aStep->GetTotalEnergyDeposit();

	//on surface?
	G4double RIn = pointIn_pos.rho();
	G4double ROut = pointOut_pos.rho();
	G4double torlarance = 1e-5;
	bool prePoint_OnSurf = false;
	if ( prePoint->GetStepStatus() == fGeomBoundary && (RIn - RMin)/mm < torlarance ) prePoint_OnSurf = true;
	bool postPoint_OnSurf = false;
	if ( postPoint->GetStepStatus() == fGeomBoundary && (ROut - RMin)/mm < torlarance ) postPoint_OnSurf = true;
	G4ThreeVector hitPoint_pos, hitPoint2_pos;
	G4ThreeVector hitPoint_mom, hitPoint2_mom;
	G4double hitPoint_time, hitPoint2_time;
	G4double total_e, total_e2;

	//from inside? 
	bool FromInSide = (RIn < ROut);

	//occurred once? get track index
	bool occurredOnce = true;
	G4int track_index = -1;
	for ( int i = 0; i < m_trackID.size(); i++ ){//Has this track left something before?
		if ( m_trackID[i] == trackID ){
			track_index = i;
			break;
		}
	}
	if ( track_index == -1 ) occurredOnce = false;

//*************************filter***********************
	//switch
	if (!Switch) return false;

	//maxn
	if ( maxn && nHits >= maxn ) return false;

	//once traveled outside? if yes, set looping
	if ( ReplicaNo == nLayers - 1 ){
		if ( !FromInSide && prePoint_OnSurf && occurredOnce ){
		//std::cout<<"Traveled outside once! Considered as looping!"<<std::endl;
			m_trackLooping[track_index] = true;
			return false;
		}
	//std::cout<<"Outer most layer! not traveled out yet!"<<std::endl;
	}
	else if( ReplicaNo == 0 ){
		if ( FromInSide && prePoint_OnSurf && occurredOnce ){
		//std::cout<<"Traveled outside once! Considered as looping!"<<std::endl;
			m_trackLooping[track_index] = true;
			return false;
		}
	//std::cout<<"Inner most layer! not traveled out yet!"<<std::endl;
	}

	//at inner boundary? if yes, record. if no, return
	if ( FromInSide ){
		if ( !prePoint_OnSurf ) return false;
		hitPoint_pos = pointIn_pos;
		total_e = prePoint->GetTotalEnergy();
		hitPoint_mom = prePoint->GetMomentum();
		hitPoint_time = prePoint->GetGlobalTime();

		hitPoint2_pos = pointOut_pos;
		total_e2 = postPoint->GetTotalEnergy();
		hitPoint2_mom = postPoint->GetMomentum();
		hitPoint2_time = postPoint->GetGlobalTime();
	}
	else{
		if ( !postPoint_OnSurf ) return false;
		hitPoint_pos = pointOut_pos;
		total_e = postPoint->GetTotalEnergy();
		hitPoint_mom = postPoint->GetMomentum();
		hitPoint_time = postPoint->GetGlobalTime();

		hitPoint2_pos = pointIn_pos;
		total_e2 = prePoint->GetTotalEnergy();
		hitPoint2_mom = prePoint->GetMomentum();
		hitPoint2_time = prePoint->GetGlobalTime();
	}

  //minp
	G4double hitPoint_pa = hitPoint_mom.mag();
	if ( minp && hitPoint_pa < minp ) return false;

	//time_window
  if(isnan(hitPoint_time)){
    G4cout<<"CdcLayerSD:error, hitPoint_time is nan "<<G4endl;
    return false;
  }
  if ( hitPoint_time < mint && mint ) return false;
  if ( hitPoint_time > maxt && maxt ) return false;

  //neutralCut
  if ( charge == 0 && neutralCut ) return false;

	//minedep
  if( edep <= minedep ) return false;

	//looping?
	if ( FirstLoop ){
		//std::cout<<"**************checking looping*****************"<<std::endl;
		//std::cout<<"tid = "<<trackID<<", vol = "<<VolName<<", rep = "<<ReplicaNo<<std::endl;
		if (!occurredOnce){//New track.
			//std::cout<<"New track!"<<std::endl;
			m_trackID.push_back(trackID);
			m_trackLooping.push_back(false);
			track_index = m_trackID.size() - 1;
		}
		else{//old track.
			if (m_trackLooping[track_index]){//looping already
			//std::cout<<"Looping.."<<std::endl;
				return false;
			}
			int count = 1;
			for ( int i = 0; i < nHits; i++ ){
			//std::cout<<"vol["<<i<<"] = "<<m_volName[i]<<", rep["<<i<<"] = "<<m_volID[i]<<", tid["<<i<<"] = "<<m_tid[i]<<std::endl;
				if ( m_volName[i] == VolName && m_volID[i] == ReplicaNo && m_tid[i] == trackID ){
					count++;
				}
				//std::cout<<"occurred "<<count<<" times"<<std::endl;
			}
			if ( count >= 3 ){//3rd time hitting this volume, must be looping
			//std::cout<<"Loop!!!"<<std::endl;
				m_trackLooping[track_index] = true;
				return false;
			}
		}
		//std::cout<<"Not looping"<<std::endl;
	}

//*********************generate new hit*************************************

	CdcLayerHit* newHit = new CdcLayerHit();
	newHit->SetTrackID(trackID);
	newHit->SetVolID(ReplicaNo);
	newHit->SetEdep(edep);
	newHit->SetPos(hitPoint_pos);
	newHit->SetGlobalT(hitPoint_time);
	hitsCollection->insert(newHit);
	//Set for root objects
	if(flag_x) m_x.push_back(hitPoint_pos.x()/unit_x);
	if(flag_y) m_y.push_back(hitPoint_pos.y()/unit_y);
	if(flag_z) m_z.push_back(hitPoint_pos.z()/unit_z);
	m_t.push_back(hitPoint_time/unit_t);
	if(flag_px) m_px.push_back(hitPoint_mom.x()/unit_px);
	if(flag_py) m_py.push_back(hitPoint_mom.y()/unit_py);
	if(flag_pz) m_pz.push_back(hitPoint_mom.z()/unit_pz);
	if(flag_e) m_e.push_back(total_e/unit_e);
	if(flag_edep) m_edep.push_back(edep/unit_edep);
	m_volID.push_back(ReplicaNo);
	m_volName.push_back(VolName);
	if(flag_tid) m_tid.push_back(trackID);
	if(flag_pid) m_pid.push_back(pid);
	if(flag_charge) m_charge.push_back(charge);
	nHits++;

	// prestep point and poststep point on the same layer?
	double gap = RMax - RMin;
	if ( fabs( RIn - ROut ) < gap/2 && prePoint_OnSurf && postPoint_OnSurf ){
		CdcLayerHit* newHit = new CdcLayerHit();
		newHit->SetTrackID(trackID);
		newHit->SetVolID(ReplicaNo);
		newHit->SetEdep(edep);
		newHit->SetPos(hitPoint2_pos);
		newHit->SetGlobalT(hitPoint2_time);
		hitsCollection->insert(newHit);
		//Set for root objects
		if(flag_x) m_x.push_back(hitPoint2_pos.x()/unit_x);
		if(flag_y) m_y.push_back(hitPoint2_pos.y()/unit_y);
		if(flag_z) m_z.push_back(hitPoint2_pos.z()/unit_z);
		if(flag_t) m_t.push_back(hitPoint2_time/unit_t);
		if(flag_px) m_px.push_back(hitPoint2_mom.x()/unit_px);
		if(flag_py) m_py.push_back(hitPoint2_mom.y()/unit_py);
		if(flag_pz) m_pz.push_back(hitPoint2_mom.z()/unit_pz);
		if(flag_e) m_e.push_back(total_e2/unit_e);
		if(flag_edep) m_edep.push_back(edep/unit_edep);
		if(flag_volID) m_volID.push_back(ReplicaNo);
		if(flag_volName) m_volName.push_back(VolName);
		if(flag_tid) m_tid.push_back(trackID);
		if(flag_pid) m_pid.push_back(pid);
		if(flag_charge) m_charge.push_back(charge);
		nHits++;
	}

  return true;
}

//-----------------------------------EndOfEvent----------------------------------------------
//Will be called by geant4 automatically at the end of each event
void CdcLayerSD::EndOfEvent(G4HCofThisEvent*){
  if (verboseLevel>0) {
//    hitsCollection->PrintAllHits();
    /*
       G4int NbHits = hitsCollection->entries();
       G4cout << "\n-------->Hits Collection: in this event they are " << NbHits
       << " hits in the CDCLAYER chambers: " << G4endl;
       for (G4int i=0;i<NbHits;i++) (*hitsCollection)[i]->Print();
     */
  }
}
