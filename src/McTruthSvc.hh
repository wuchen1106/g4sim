//---------------------------------------------------------------------------//
//Description: In charge of McTruth
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#ifndef McTruthSvc_h
#define McTruthSvc_h 1

#include "myglobals.hh"
#include "G4String.hh"

#include <string>
#include <vector>
#include <map>

class G4Event;
class G4Track;
class G4Step;
class Hstar10Converter;
class SurfacePlane;
class ThinSlab;

class McTruthSvc
{
	public:
		McTruthSvc();
		~McTruthSvc();

		static McTruthSvc* GetMcTruthSvc();

		void SetBranch();

		void ReadOutputCard(G4String filename);

		void SetValuePre(const G4Track*);
		void SetValuePost(const G4Track*,double dt = 0);
		void AddStep(const G4Step*);
		void SetMapEdep(double step, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax){
		    m_map_step = step;
		    m_map_xmin = xmin;
		    m_map_xmax = xmax;
		    m_map_ymin = ymin;
		    m_map_ymax = ymax;
		    m_map_zmin = zmin;
		    m_map_zmax = zmax;
		}
		void SetHstar10ConverterDirectory(G4String dir){m_fluence2H10_directory = dir;}

		void Initialize();
		void InitializeRun();

                void EndOfRunAction();

                void AddEdep2Map(double edep, double x, double y, double z);

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
		bool flag_tcost;
		bool flag_px;
		bool flag_py;
		bool flag_pz;
		bool flag_e;
		bool flag_ekin;
		bool flag_x;
		bool flag_y;
		bool flag_z;
		bool flag_nSteps;
		bool flag_step_l;
		bool flag_step_edep;
		bool flag_step_t;
		bool flag_step_x;
		bool flag_step_y;
		bool flag_step_z;
		bool flag_step_px;
		bool flag_step_py;
		bool flag_step_pz;
		bool flag_step_ekin;
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
		std::vector<double> m_tcost;
		std::vector<double> m_px;
		std::vector<double> m_py;
		std::vector<double> m_pz;
		std::vector<double> m_e;
		std::vector<double> m_ekin;
		std::vector<double> m_x;
		std::vector<double> m_y;
		std::vector<double> m_z;
		bool m_recordCurrentTrack;
                std::vector<int> m_nSteps;
                std::vector<std::vector<double> > m_step_l;
                std::vector<std::vector<double> > m_step_edep;
                std::vector<std::vector<double> > m_step_t;
                std::vector<std::vector<double> > m_step_x;
		std::vector<std::vector<double> > m_step_y;
		std::vector<std::vector<double> > m_step_z;
                std::vector<std::vector<double> > m_step_px;
		std::vector<std::vector<double> > m_step_py;
		std::vector<std::vector<double> > m_step_pz;
		std::vector<std::vector<double> > m_step_ekin;
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
		std::string unitName_step_l;
		std::string unitName_step_edep;
		std::string unitName_step_t;
		std::string unitName_step_x;
		std::string unitName_step_y;
		std::string unitName_step_z;
		std::string unitName_step_px;
		std::string unitName_step_py;
		std::string unitName_step_pz;
		std::string unitName_step_ekin;
		double unit_time;
		double unit_px;
		double unit_py;
		double unit_pz;
		double unit_e;
		double unit_ekin;
		double unit_x;
		double unit_y;
		double unit_z;
		double unit_step_l;
		double unit_step_edep;
		double unit_step_t;
		double unit_step_x;
		double unit_step_y;
		double unit_step_z;
		double unit_step_px;
		double unit_step_py;
		double unit_step_pz;
		double unit_step_ekin;

                std::map<short int, std::map<short int, std::map<short int, float> > > m_xyz2edep;
                double m_map_step;
                double m_map_xmin;
                double m_map_xmax;
                double m_map_ymin;
                double m_map_ymax;
                double m_map_zmin;
                double m_map_zmax;

                Hstar10Converter * m_fluence2H10_neutron;
                Hstar10Converter * m_fluence2H10_photon;
                Hstar10Converter * m_fluence2H10_proton;
                Hstar10Converter * m_fluence2H10_electron;
                Hstar10Converter * m_fluence2H10_positron;
                Hstar10Converter * m_fluence2H10_muonp;
                Hstar10Converter * m_fluence2H10_muonm;
                Hstar10Converter * m_fluence2H10_pionp;
                Hstar10Converter * m_fluence2H10_pionm;
                Hstar10Converter * m_fluence2H10_kaonp;
                Hstar10Converter * m_fluence2H10_kaonm;

                G4String m_fluence2H10_directory;

                std::vector<SurfacePlane*> m_flux_surfaces;
                std::vector<ThinSlab*> m_flux_slabs;
};

#endif

