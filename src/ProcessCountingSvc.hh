//---------------------------------------------------------------------------//
//Description: In charge of ProcessCounting
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#ifndef ProcessCountingSvc_h
#define ProcessCountingSvc_h 1

#include "myglobals.hh"

#include <string>
#include <vector>

class G4Event;
class G4Step;

class ProcessCountingSvc
{
  public:
    ProcessCountingSvc();
     ~ProcessCountingSvc();

    static ProcessCountingSvc* GetProcessCountingSvc();

    void SetBranch();

		void ReadOutputCard(G4String filename);

    void SetValue(const G4Step*);

		void Initialize();

		void InitialStep(const G4Step*);

		void AddASDI(G4String);

		void AddPSDI(G4String);

		//=> Access
		G4String GetVolName(){return VolName;}

  private:

		void ReSet();
		void ShowOutCard();
		void CheckFilter(const G4Step*);
		void CheckTrack(const G4Step*);

    static ProcessCountingSvc* fProcessCountingSvc;

		//for output setting
    bool flag_nSteps;
    bool flag_pid;
    bool flag_tid;
    bool flag_nSec;
    bool flag_time;
    bool flag_stepL;
    bool flag_prePx;
    bool flag_prePy;
    bool flag_prePz;
    bool flag_postPx;
    bool flag_postPy;
    bool flag_postPz;
    bool flag_dTheta;
    bool flag_dE;
    bool flag_edepT;
    bool flag_edepN;
    bool flag_edepI;
    bool flag_preEkin;
    bool flag_preX;
    bool flag_preY;
    bool flag_preZ;
    bool flag_postX;
    bool flag_postY;
    bool flag_postZ;
    bool flag_charge;
    bool flag_particleName;
    bool flag_process;
    bool flag_ASDI_msc;
    bool flag_ASDI_hPairProd;
    bool flag_ASDI_ProtonInelastic;
    bool flag_ASDI_hBrems;
    bool flag_ASDI_hIoni;
    bool flag_ASDI_hadElastic;
    bool flag_ASDI_ionIoni;
    bool flag_ASDI_Decay;
    bool flag_ASDI_Transportation;
    bool flag_ASDI_eBrem;
    bool flag_ASDI_eIoni;
    bool flag_PSDI_eBrem;
    bool flag_PSDI_msc;
    bool flag_PSDI_hPairProd;
    bool flag_PSDI_ProtonInelastic;
    bool flag_PSDI_hBrems;
    bool flag_PSDI_hIoni;
    bool flag_PSDI_hadElastic;
    bool flag_PSDI_ionIoni;
    bool flag_PSDI_Decay;
    bool flag_PSDI_Transportation;
    bool flag_PSDI_eIoni;
    bool flag_volume;
    //for filter
    G4String VolName;
    bool PASSEDFILTER;
    bool m_Switch;
    double m_minp;
    double m_mine;
    int m_maxnSteps;
		double m_mint;
		double m_maxt;
		std::vector<G4String> m_Volumes;
    int m_maxnTracks;
		std::vector<int> m_TrackIDs;
    //For root objects
    int m_nSteps;
    std::vector<int> m_pid;
    std::vector<int> m_tid;
    std::vector<int> m_nSec;
    std::vector<double> m_time;
    std::vector<double> m_stepL;
    std::vector<double> m_prePx;
    std::vector<double> m_prePy;
    std::vector<double> m_prePz;
    std::vector<double> m_postPx;
    std::vector<double> m_postPy;
    std::vector<double> m_postPz;
    std::vector<double> m_dTheta;
    std::vector<double> m_dE;
    std::vector<double> m_edepT;
    std::vector<double> m_edepN;
    std::vector<double> m_edepI;
    std::vector<double> m_preEkin;
    std::vector<double> m_preX;
    std::vector<double> m_preY;
    std::vector<double> m_preZ;
    std::vector<double> m_postX;
    std::vector<double> m_postY;
    std::vector<double> m_postZ;
    std::vector<std::string> m_particleName;
    std::vector<int> m_charge;
    std::vector<std::string> m_process;
    std::vector<int> m_ASDI_msc;
    std::vector<int> m_ASDI_hPairProd;
    std::vector<int> m_ASDI_ProtonInelastic;
    std::vector<int> m_ASDI_hBrems;
    std::vector<int> m_ASDI_hIoni;
    std::vector<int> m_ASDI_hadElastic;
    std::vector<int> m_ASDI_ionIoni;
    std::vector<int> m_ASDI_Decay;
    std::vector<int> m_ASDI_Transportation;
    std::vector<int> m_ASDI_eBrem;
    std::vector<int> m_ASDI_eIoni;
    std::vector<int> m_PSDI_eBrem;
    std::vector<int> m_PSDI_eIoni;
    std::vector<int> m_PSDI_msc;
    std::vector<int> m_PSDI_hPairProd;
    std::vector<int> m_PSDI_ProtonInelastic;
    std::vector<int> m_PSDI_hBrems;
    std::vector<int> m_PSDI_hIoni;
    std::vector<int> m_PSDI_hadElastic;
    std::vector<int> m_PSDI_ionIoni;
    std::vector<int> m_PSDI_Decay;
    std::vector<int> m_PSDI_Transportation;
    std::vector<std::string> m_volume;
    std::string unitName_time;
    std::string unitName_stepL;
    std::string unitName_prePx;
    std::string unitName_prePy;
    std::string unitName_prePz;
    std::string unitName_postPx;
    std::string unitName_postPy;
    std::string unitName_postPz;
    std::string unitName_dTheta;
    std::string unitName_dE;
    std::string unitName_edepT;
    std::string unitName_edepN;
    std::string unitName_edepI;
    std::string unitName_preEkin;
    std::string unitName_preX;
    std::string unitName_preY;
    std::string unitName_preZ;
    std::string unitName_postX;
    std::string unitName_postY;
    std::string unitName_postZ;
    double unit_time;
    double unit_stepL;
    double unit_prePx;
    double unit_prePy;
    double unit_prePz;
    double unit_postPx;
    double unit_postPy;
    double unit_postPz;
    double unit_dTheta;
    double unit_dE;
    double unit_edepT;
    double unit_edepN;
    double unit_edepI;
    double unit_preEkin;
    double unit_preX;
    double unit_preY;
    double unit_preZ;
    double unit_postX;
    double unit_postY;
    double unit_postZ;
};

#endif

