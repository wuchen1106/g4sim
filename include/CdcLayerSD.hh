//---------------------------------------------------------------------------
//Description: Sensitive detector definition for CdcLayer
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#ifndef CdcLayerSD_h
#define CdcLayerSD_h 1

#include "globals.hh"

#include <iostream>
#include <vector>

#include "CdcLayerHit.hh"
#include "MySD.hh"

class G4Step;
class G4HCofThisEvent;
class MyVGeometryParameter;
class FormulizedGeometryParameter;

class CdcLayerSD : public MySD
{
  public:
    CdcLayerSD(G4String, MyVGeometryParameter*);
    virtual ~CdcLayerSD();

    void Initialize(G4HCofThisEvent*);
    G4bool ProcessHits(G4Step*, G4TouchableHistory*);
    void EndOfEvent(G4HCofThisEvent*);

		//inherit from MySD
    void SetBranch();

    void ReadOutputCard(G4String filename);

		//=> Access
		int Get_nHits() { return nHits; }
    double Get_m_x( int i ) { return m_x[i]; }
    double Get_m_y( int i ) { return m_y[i]; }
    double Get_m_z( int i ) { return m_z[i]; }
    double Get_m_t( int i ) { return m_t[i]; }
    double Get_m_px( int i ) { return m_px[i]; }
    double Get_m_py( int i ) { return m_py[i]; }
    double Get_m_pz( int i ) { return m_pz[i]; }
    double Get_m_e( int i ) { return m_e[i]; }
    double Get_m_edep( int i ) { return m_edep[i]; }
		int Get_m_volID( int i ) { return m_volID[i]; }
		std::string Get_m_volName( int i ) { return m_volName[i]; }
		int Get_m_tid( int i ) { return m_tid[i]; }
		int Get_m_pid( int i ) { return m_pid[i]; }
		int Get_m_charge( int i ) { return m_charge[i]; }
    std::string Get_unitName_x() { return unitName_x; }
    std::string Get_unitName_y() { return unitName_y; }
    std::string Get_unitName_z() { return unitName_z; }
    std::string Get_unitName_t() { return unitName_t; }
    std::string Get_unitName_px() { return unitName_px; }
    std::string Get_unitName_py() { return unitName_py; }
    std::string Get_unitName_pz() { return unitName_pz; }
    std::string Get_unitName_e() { return unitName_e; }
    std::string Get_unitName_edep() { return unitName_edep; }
    double Get_unit_x() { return unit_x; }
    double Get_unit_y() { return unit_y; }
    double Get_unit_z() { return unit_z; }
    double Get_unit_t() { return unit_t; }
    double Get_unit_px() { return unit_px; }
    double Get_unit_py() { return unit_py; }
    double Get_unit_pz() { return unit_pz; }
    double Get_unit_e() { return unit_e; }
    double Get_unit_edep() { return unit_edep; }

  private:

		void ReSet();
		void ShowOutCard();

  private:

    FormulizedGeometryParameter* m_GeometryParameter;

    CdcLayerHitsCollection* hitsCollection;

    //For root objects
    std::vector<double> m_x;
    std::vector<double> m_y;
    std::vector<double> m_z;
    std::vector<double> m_t;
    std::vector<double> m_px;
    std::vector<double> m_py;
    std::vector<double> m_pz;
    std::vector<double> m_e;
    std::vector<double> m_edep;
		std::vector<int>    m_volID;
		std::vector<std::string>    m_volName;
		std::vector<int>    m_tid;
		std::vector<int>    m_pid;
		std::vector<int>    m_charge;
    std::string unitName_x;
    std::string unitName_y;
    std::string unitName_z;
    std::string unitName_t;
    std::string unitName_px;
    std::string unitName_py;
    std::string unitName_pz;
    std::string unitName_e;
    std::string unitName_edep;
    double unit_x;
    double unit_y;
    double unit_z;
    double unit_t;
    double unit_px;
    double unit_py;
    double unit_pz;
    double unit_e;
    double unit_edep;

		//for output setting
		int nHits;
    bool flag_nHits;
    bool flag_x;
    bool flag_y;
    bool flag_z;
    bool flag_t;
    bool flag_px;
    bool flag_py;
    bool flag_pz;
    bool flag_e;
    bool flag_edep;
    bool flag_volID;
    bool flag_volName;
    bool flag_tid;
    bool flag_pid;
    bool flag_charge;
    //for filter
    bool Switch;
		bool neutralCut;
		bool FirstLoop;
    G4int maxn;
    G4double minp;
		G4double tres;
		G4double mint;
		G4double maxt;
		G4double minedep;
		//for loop control
		std::vector<G4int> m_trackID;
		std::vector<bool>  m_trackLooping;
};

#endif

