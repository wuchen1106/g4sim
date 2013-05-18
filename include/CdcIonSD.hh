//---------------------------------------------------------------------------
//Description: Sensitive detector definition for CDC  
//Author: Yuan Ye(yuany@mail.ihep.ac.cn)
//Created: 22 July, 2012
//Modified:
//Comment: 
//---------------------------------------------------------------------------//

#ifndef CdcIonSD_h
#define CdcIonSD_h 1

#include "globals.hh"

#include <iostream>
#include <vector>

#include "CdcIonHit.hh"
#include "MySD.hh"

class G4Step;
class G4HCofThisEvent;
class CdcGeometryParameter;
class MyVGeometryParameter;

class CdcIonSD : public MySD
{
  public:
    CdcIonSD(G4String, MyVGeometryParameter*);
    virtual ~CdcIonSD();

		//=> inherit from G4VSensitiveDetector
    void Initialize(G4HCofThisEvent*);
    G4bool ProcessHits(G4Step*, G4TouchableHistory*);
    void EndOfEvent(G4HCofThisEvent*);

		//=> inherit from MySD
    void SetBranch();
    void ReadOutputCard(G4String filename);

		//=> Access
		CdcGeometryParameter* get_GeometryParameter(){ return m_GeometryParameter; }

		//=> Modify
		void set_GeometryParameter( CdcGeometryParameter* val ){ m_GeometryParameter = val; }

  private:

		void ReSet();
		void ShowOutCard();
		int CalculateHitInfo(G4ThreeVector &, double &, G4ThreeVector, G4ThreeVector, G4ThreeVector, G4ThreeVector, int, int, int, int);
		G4double StandardDeltaAngle(G4double);

	private:

    CdcGeometryParameter* m_GeometryParameter;

    CdcIonHitsCollection* hitsCollection;
		std::vector<G4ThreeVector>* collection;
		std::vector< std::vector<int> > hitPointer;
		std::vector< std::vector<double> > hitTime;

    //For root objects
    std::vector<double> m_x;
    std::vector<double> m_y;
    std::vector<double> m_z;
    std::vector<double> m_t;
    std::vector<double> m_px;
    std::vector<double> m_py;
    std::vector<double> m_pz;
    std::vector<double> m_pa;
    std::vector<double> m_e;
    std::vector<double> m_ccol;
    std::vector<double> m_edep;
		std::vector<int>    m_layerID;
		std::vector<int>    m_cellID;
		std::vector<int>    m_tid;
		std::vector<int>    m_pid;
    double unit_x;
    double unit_y;
    double unit_z;
    double unit_t;
    double unit_px;
    double unit_py;
    double unit_pz;
    double unit_pa;
    double unit_e;
    double unit_ccol;
    double unit_edep;
    std::string unitName_x;
    std::string unitName_y;
    std::string unitName_z;
    std::string unitName_t;
    std::string unitName_px;
    std::string unitName_py;
    std::string unitName_pz;
    std::string unitName_pa;
    std::string unitName_e;
    std::string unitName_ccol;
    std::string unitName_edep;
		//for output setting
		int cdc_nHits;
    bool flag_nHits;
    bool flag_x;
    bool flag_y;
    bool flag_z;
    bool flag_t;
    bool flag_px;
    bool flag_py;
    bool flag_pz;
    bool flag_pa;
    bool flag_e;
    bool flag_ccol;
    bool flag_edep;
    bool flag_layerID;
    bool flag_cellID;
    bool flag_tid;
    bool flag_pid;
    //for filter
    bool Switch;
    G4double minC;
    G4int maxn;
    G4int ntracks;
		G4double mint;
		G4double maxt;
		G4double tres;
		int nTracks;
		int prevTrackID;
		//fTolerance
		G4double fTolerance;
};

#endif

