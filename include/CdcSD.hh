//---------------------------------------------------------------------------
//Description: Sensitive detector definition for CDC  
//Author: Yuan Ye(yuany@mail.ihep.ac.cn)
//Created: 22 July, 2012
//Modified:
//Comment: 
//---------------------------------------------------------------------------//

#ifndef CdcSD_h
#define CdcSD_h 1

#include "myglobals.hh"

#include <iostream>
#include <vector>

#include "CdcHit.hh"
#include "MySD.hh"

class G4Step;
class G4HCofThisEvent;
class CdcGeometryParameter;
class MyVGeometryParameter;

class CdcSD : public MySD
{
	public:
		CdcSD(G4String, MyVGeometryParameter*);
		virtual ~CdcSD();

		//=> inherit from G4VSensitiveDetector
		void Initialize(G4HCofThisEvent*);
		G4bool ProcessHits(G4Step*, G4TouchableHistory*);
		void EndOfEvent(G4HCofThisEvent*);

		//=> inherit from MySD
		void SetBranch();
		void ReadOutputCard(G4String filename);

		//=> Access
		CdcGeometryParameter* get_GeometryParameter(){ return m_GeometryParameter; }
		int Get_nHits(){return cdc_nHits;}

		//=> Modify
		void set_GeometryParameter( CdcGeometryParameter* val ){ m_GeometryParameter = val; }

	private:

		void ReSet();
		void ShowOutCard();

		int FindClosestPoint(G4ThreeVector &, double &, G4ThreeVector, G4ThreeVector, G4ThreeVector, G4ThreeVector, int, int, int, G4double&, G4int&);
		int check_type(G4double a[6], G4double phi);
		double NewtonRootFinding(G4double a[6], G4double phi, G4int nTry, G4double torl, G4double &error);
		double calculateDist2(G4double b[4], G4double phi);
		int findSmallest( double a, double b, double c);

	private:

		CdcGeometryParameter* m_GeometryParameter;

		CdcHitsCollection* hitsCollection;
		std::vector<G4ThreeVector>* collection;
		std::vector< std::vector<int> > hitPointer;

		//For root objects
		std::vector<double> m_x;
		std::vector<double> m_y;
		std::vector<double> m_z;
		std::vector<double> m_pIx;
		std::vector<double> m_pIy;
		std::vector<double> m_pIz;
		std::vector<double> m_pOx;
		std::vector<double> m_pOy;
		std::vector<double> m_pOz;
		std::vector<double> m_t;
		std::vector<double> m_px;
		std::vector<double> m_py;
		std::vector<double> m_pz;
		std::vector<double> m_e;
		std::vector<double> m_ekin;
		std::vector<double> m_edep;
		std::vector<double> m_stepL;
		std::vector<int> m_nTry;
		std::vector<int> m_status;
		std::vector<double> m_error;
		std::vector<double> m_driftD;
		std::vector<double> m_driftD_smeared;
		std::vector<int>    m_layerID;
		std::vector<int>    m_cellID;
		std::vector<int>    m_tid;
		std::vector<int>    m_pid;
		std::vector<int>    m_ppid;
		std::vector<int>    m_ptid;
		std::vector<std::string>    m_oprocess;
		std::vector<std::string>    m_ovolName;
		std::vector<std::string>    m_particleName;
		double unit_x;
		double unit_y;
		double unit_z;
		double unit_pIx;
		double unit_pIy;
		double unit_pIz;
		double unit_pOx;
		double unit_pOy;
		double unit_pOz;
		double unit_t;
		double unit_px;
		double unit_py;
		double unit_pz;
		double unit_e;
		double unit_ekin;
		double unit_edep;
		double unit_stepL;
		double unit_driftD;
		double unit_driftD_smeared;
		std::string unitName_x;
		std::string unitName_y;
		std::string unitName_z;
		std::string unitName_pIx;
		std::string unitName_pIy;
		std::string unitName_pIz;
		std::string unitName_pOx;
		std::string unitName_pOy;
		std::string unitName_pOz;
		std::string unitName_t;
		std::string unitName_px;
		std::string unitName_py;
		std::string unitName_pz;
		std::string unitName_e;
		std::string unitName_ekin;
		std::string unitName_edep;
		std::string unitName_stepL;
		std::string unitName_driftD;
		std::string unitName_driftD_smeared;
		//for output setting
		int cdc_nHits;
		bool flag_nHits;
		bool flag_x;
		bool flag_y;
		bool flag_z;
		bool flag_pIx;
		bool flag_pIy;
		bool flag_pIz;
		bool flag_pOx;
		bool flag_pOy;
		bool flag_pOz;
		bool flag_t;
		bool flag_px;
		bool flag_py;
		bool flag_pz;
		bool flag_e;
		bool flag_ekin;
		bool flag_edep;
		bool flag_stepL;
		bool flag_nTry;
		bool flag_status;
		bool flag_error;
		bool flag_driftD;
		bool flag_driftD_smeared;
		bool flag_layerID;
		bool flag_cellID;
		bool flag_tid;
		bool flag_pid;
		bool flag_ppid;
		bool flag_ptid;
		bool flag_oprocess;
		bool flag_ovolName;
		bool flag_particleName;
		//for filter
		bool Switch;
		G4double minp;
		G4int maxn;
		G4int ntracks;
		bool neutralCut;
		G4double mint;
		G4double maxt;
		G4double minedep;
		int nTracks;
		int prevTrackID;
		//fTolerance
		G4double fTolerance;
};

#endif

