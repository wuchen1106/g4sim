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

class TF1;
class TH1D;
class TFile;
class G4Step;
class G4HCofThisEvent;
class CdcGeometryParameter;
class MyVGeometryParameter;
class PrimaryGeneratorAction;

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

		PrimaryGeneratorAction* pPrimaryGeneratorAction;

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
		std::vector<double> m_wx;
		std::vector<double> m_wy;
		std::vector<double> m_wz;
		std::vector<double> m_t;
		std::vector<double> m_tstart;
		std::vector<double> m_tstop;
		std::vector<double> m_px;
		std::vector<double> m_py;
		std::vector<double> m_pz;
		std::vector<double> m_ox;
		std::vector<double> m_oy;
		std::vector<double> m_oz;
		std::vector<double> m_ot;
		std::vector<double> m_opx;
		std::vector<double> m_opy;
		std::vector<double> m_opz;
		std::vector<double> m_e;
		std::vector<double> m_ekin;
		std::vector<double> m_edep;
		std::vector<double> m_edepAll;
		std::vector<double> m_edepDelta;
		std::vector<double> m_stepL;
		std::vector<int> m_nPair;
		std::vector<double> m_driftD;
		std::vector<double> m_driftDtrue;
		std::vector<int> m_posflag;
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
		double unit_wx;
		double unit_wy;
		double unit_wz;
		double unit_t;
		double unit_tstart;
		double unit_tstop;
		double unit_px;
		double unit_py;
		double unit_pz;
		double unit_ox;
		double unit_oy;
		double unit_oz;
		double unit_ot;
		double unit_opx;
		double unit_opy;
		double unit_opz;
		double unit_e;
		double unit_ekin;
		double unit_edep;
		double unit_edepAll;
		double unit_edepDelta;
		double unit_stepL;
		double unit_driftD;
		double unit_driftDtrue;
		std::string unitName_x;
		std::string unitName_y;
		std::string unitName_z;
		std::string unitName_wx;
		std::string unitName_wy;
		std::string unitName_wz;
		std::string unitName_t;
		std::string unitName_tstart;
		std::string unitName_tstop;
		std::string unitName_px;
		std::string unitName_py;
		std::string unitName_pz;
		std::string unitName_ox;
		std::string unitName_oy;
		std::string unitName_oz;
		std::string unitName_ot;
		std::string unitName_opx;
		std::string unitName_opy;
		std::string unitName_opz;
		std::string unitName_e;
		std::string unitName_ekin;
		std::string unitName_edep;
		std::string unitName_edepAll;
		std::string unitName_edepDelta;
		std::string unitName_stepL;
		std::string unitName_driftD;
		std::string unitName_driftDtrue;
		//for output setting
		int cdc_nHits;
		bool flag_nHits;
		bool flag_x;
		bool flag_y;
		bool flag_z;
		bool flag_wx;
		bool flag_wy;
		bool flag_wz;
		bool flag_t;
		bool flag_tstart;
		bool flag_tstop;
		bool flag_px;
		bool flag_py;
		bool flag_pz;
		bool flag_ox;
		bool flag_oy;
		bool flag_oz;
		bool flag_ot;
		bool flag_opx;
		bool flag_opy;
		bool flag_opz;
		bool flag_e;
		bool flag_ekin;
		bool flag_edep;
		bool flag_edepAll;
		bool flag_edepDelta;
		bool flag_stepL;
		bool flag_nPair;
		bool flag_error;
		bool flag_driftD;
		bool flag_driftDtrue;
		bool flag_posflag;
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
		G4double tres;
		int nTracks;
		int prevTrackID;
		int precellID;
		int prelayerID;
		// xt
		TFile * m_xt_file;
		TH1D* m_xt_hist;
		TF1* m_xt_func;
};

#endif

