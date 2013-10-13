//---------------------------------------------------------------------------
//Description: Sensitive detector definition for Monitor
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#ifndef MonitorSD_h
#define MonitorSD_h 1

#include "globals.hh"

#include <iostream>
#include <vector>

#include "MonitorHit.hh"
#include "MySD.hh"

class G4Step;
class G4HCofThisEvent;
class MyVGeometryParameter;

class MonitorSD : public MySD
{
	public:
		MonitorSD(G4String, MyVGeometryParameter*);
		virtual ~MonitorSD();

		void Initialize(G4HCofThisEvent*);
		G4bool ProcessHits(G4Step*, G4TouchableHistory*);
		void EndOfEvent(G4HCofThisEvent*);

		//inherit from MySD
		void SetBranch();

		void ReadOutputCard(G4String filename);

		//=> Access
		double Get_m_x( int i ) { return m_x[i]; }
		double Get_m_y( int i ) { return m_y[i]; }
		double Get_m_z( int i ) { return m_z[i]; }
		double Get_m_t( int i ) { return m_t[i]; }
		double Get_m_px( int i ) { return m_px[i]; }
		double Get_m_py( int i ) { return m_py[i]; }
		double Get_m_pz( int i ) { return m_pz[i]; }
		double Get_m_ox( int i ) { return m_ox[i]; }
		double Get_m_oy( int i ) { return m_oy[i]; }
		double Get_m_oz( int i ) { return m_oz[i]; }
		double Get_m_ot( int i ) { return m_ot[i]; }
		double Get_m_opx( int i ) { return m_opx[i]; }
		double Get_m_opy( int i ) { return m_opy[i]; }
		double Get_m_opz( int i ) { return m_opz[i]; }
		double Get_m_e( int i ) { return m_e[i]; }
		double Get_m_edep( int i ) { return m_edep[i]; }
		double Get_m_stepL( int i ) { return m_stepL[i]; }
		int Get_m_volID( int i ) { return m_volID[i]; }
		std::string Get_m_volName( int i ) { return m_volName[i]; }
		int Get_m_ppid( int i ) { return m_ppid[i]; }
		int Get_m_ptid( int i ) { return m_ptid[i]; }
		std::string Get_m_oprocess( int i ) { return m_oprocess[i]; }
		std::string Get_m_ovolName( int i ) { return m_ovolName[i]; }
		int Get_m_tid( int i ) { return m_tid[i]; }
		int Get_m_pid( int i ) { return m_pid[i]; }
		int Get_m_charge( int i ) { return m_charge[i]; }
		int Get_m_stopped( int i ) { return m_stopped[i]; }
		double Get_m_stop_time( int i ) { return m_stop_time[i]; }
		int Get_m_killed( int i ) { return m_killed[i]; }
		double Get_m_kill_time( int i ) { return m_kill_time[i]; }
		std::string Get_unitName_x() { return unitName_x; }
		std::string Get_unitName_y() { return unitName_y; }
		std::string Get_unitName_z() { return unitName_z; }
		std::string Get_unitName_t() { return unitName_t; }
		std::string Get_unitName_px() { return unitName_px; }
		std::string Get_unitName_py() { return unitName_py; }
		std::string Get_unitName_pz() { return unitName_pz; }
		std::string Get_unitName_ox() { return unitName_ox; }
		std::string Get_unitName_oy() { return unitName_oy; }
		std::string Get_unitName_oz() { return unitName_oz; }
		std::string Get_unitName_ot() { return unitName_ot; }
		std::string Get_unitName_opx() { return unitName_opx; }
		std::string Get_unitName_opy() { return unitName_opy; }
		std::string Get_unitName_opz() { return unitName_opz; }
		std::string Get_unitName_e() { return unitName_e; }
		std::string Get_unitName_edep() { return unitName_edep; }
		std::string Get_unitName_stepL() { return unitName_stepL; }
		std::string Get_unitName_stop_time() { return unitName_stop_time; }
		std::string Get_unitName_kill_time() { return unitName_kill_time; }
		double Get_unit_x() { return unit_x; }
		double Get_unit_y() { return unit_y; }
		double Get_unit_z() { return unit_z; }
		double Get_unit_t() { return unit_t; }
		double Get_unit_px() { return unit_px; }
		double Get_unit_py() { return unit_py; }
		double Get_unit_pz() { return unit_pz; }
		double Get_unit_ox() { return unit_ox; }
		double Get_unit_oy() { return unit_oy; }
		double Get_unit_oz() { return unit_oz; }
		double Get_unit_ot() { return unit_ot; }
		double Get_unit_opx() { return unit_opx; }
		double Get_unit_opy() { return unit_opy; }
		double Get_unit_opz() { return unit_opz; }
		double Get_unit_e() { return unit_e; }
		double Get_unit_edep() { return unit_edep; }
		double Get_unit_stepL() { return unit_stepL; }
		double Get_unit_stop_time() { return unit_stop_time; }
		double Get_unit_kill_time() { return unit_kill_time; }
		int Get_nHits() { return nHits; }

	private:

		void ReSet();
		void ShowOutCard();

		MonitorHitsCollection* hitsCollection;

		//For root objects
		std::vector<double> m_x;
		std::vector<double> m_y;
		std::vector<double> m_z;
		std::vector<double> m_t;
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
		std::vector<double> m_edep;
		std::vector<double> m_stepL;
		std::vector<int>    m_volID;
		std::vector<std::string>    m_volName;
		std::vector<int>    m_ppid;
		std::vector<int>    m_ptid;
		std::vector<std::string>    m_oprocess;
		std::vector<std::string>    m_ovolName;
		std::vector<int>    m_tid;
		std::vector<int>    m_pid;
		std::vector<std::string>    m_particleName;
		std::vector<int>    m_charge;
		std::vector<int>    m_stopped;
		std::vector<double> m_stop_time;
		std::vector<int>    m_killed;
		std::vector<double> m_kill_time;
		std::string unitName_x;
		std::string unitName_y;
		std::string unitName_z;
		std::string unitName_t;
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
		std::string unitName_edep;
		std::string unitName_stepL;
		std::string unitName_stop_time;
		std::string unitName_kill_time;
		double unit_x;
		double unit_y;
		double unit_z;
		double unit_t;
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
		double unit_edep;
		double unit_stepL;
		double unit_stop_time;
		double unit_kill_time;

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
		bool flag_ox;
		bool flag_oy;
		bool flag_oz;
		bool flag_ot;
		bool flag_opx;
		bool flag_opy;
		bool flag_opz;
		bool flag_e;
		bool flag_edep;
		bool flag_stepL;
		bool flag_volID;
		bool flag_volName;
		bool flag_ppid;
		bool flag_ptid;
		bool flag_oprocess;
		bool flag_ovolName;
		bool flag_tid;
		bool flag_pid;
		bool flag_particleName;
		bool flag_charge;
		bool flag_stopped;
		bool flag_stop_time;
		bool flag_killed;
		bool flag_kill_time;
		//for filter
		bool Switch;
		bool neutralCut;
		G4int maxn;
		G4double minp;
		G4double mine;
		G4double tres;
		G4double mint;
		G4double maxt;
		G4double minedep;
		std::vector<int> white_list;
		std::vector<int> black_list;
};

#endif

