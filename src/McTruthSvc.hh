//---------------------------------------------------------------------------//
//Description: In charge of McTruth
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#ifndef McTruthSvc_h
#define McTruthSvc_h 1

#include "myglobals.hh"

#include <string>
#include <vector>

class G4Event;
class G4Track;

class McTruthSvc
{
	public:
		McTruthSvc();
		~McTruthSvc();

		static McTruthSvc* GetMcTruthSvc();

		void SetBranch();

		void ReadOutputCard(G4String filename);

		void SetValuePre(const G4Track*);
		void SetValuePost(const G4Track*);

		void Initialize();

		int get_nTracks(){return m_nTracks;}
		int get_nTracksAll(){return m_nTracksAll;}
		int get_pid( int i ){ if(i<m_nTracks) return m_pid[i]; else return 0; }
		double get_px( int i ){ if(i<m_nTracks) return m_px[i]; else return 0; }
		double get_py( int i ){ if(i<m_nTracks) return m_py[i]; else return 0; }
		double get_pz( int i ){ if(i<m_nTracks) return m_pz[i]; else return 0; }

		int tid2pid(int tid){
			for (int i = 0; i< m_dicttid.size(); i++){
				if (tid==m_dicttid[i])
					return m_dictpid[i];
			}
			return 0;
		}

		double tid2time(int tid){
			for (int i = 0; i< m_dicttid.size(); i++){
				if (tid==m_dicttid[i])
					return m_dicttime[i];
			}
			return 0;
		}

	private:

		void ReSet();
		void ShowOutCard();

		static McTruthSvc* fMcTruthSvc;

		//for output setting
		bool flag_tid2pid;
		bool flag_tid2time;
		bool flag_nTracks;
		bool flag_pid;
		bool flag_tid;
		bool flag_ptid;
		bool flag_ppid;
		bool flag_time;
		bool flag_px;
		bool flag_py;
		bool flag_pz;
		bool flag_e;
		bool flag_ekin;
		bool flag_x;
		bool flag_y;
		bool flag_z;
		bool flag_charge;
		bool flag_particleName;
		bool flag_process;
		bool flag_volume;
		//for filter
		bool m_Switch;
		double m_minp;
		double m_mine;
		int m_maxnTracks;
		double m_mint;
		double m_maxt;
		std::vector<int> white_list;
		std::vector<int> black_list;
		//For root objects
		int m_nTracksAll;
		int m_nTracks;
		std::vector<double> m_dicttime;
		std::vector<int> m_dictpid;
		std::vector<int> m_dicttid;
		std::vector<int> m_pid;
		std::vector<int> m_tid;
		std::vector<int> m_ptid;
		std::vector<int> m_ppid;
		std::vector<double> m_time;
		std::vector<double> m_px;
		std::vector<double> m_py;
		std::vector<double> m_pz;
		std::vector<double> m_e;
		std::vector<double> m_ekin;
		std::vector<double> m_x;
		std::vector<double> m_y;
		std::vector<double> m_z;
		std::vector<std::string> m_particleName;
		std::vector<int> m_charge;
		std::vector<std::string> m_process;
		std::vector<std::string> m_volume;
		std::string unitName_time;
		std::string unitName_px;
		std::string unitName_py;
		std::string unitName_pz;
		std::string unitName_e;
		std::string unitName_ekin;
		std::string unitName_x;
		std::string unitName_y;
		std::string unitName_z;
		double unit_time;
		double unit_px;
		double unit_py;
		double unit_pz;
		double unit_e;
		double unit_ekin;
		double unit_x;
		double unit_y;
		double unit_z;
};

#endif

