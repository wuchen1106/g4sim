//---------------------------------------------------------------------------//
//Description: Setup PrimaryGenerator
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include <map>
#include <vector>

#include "G4VUserPrimaryGeneratorAction.hh"
#include "myglobals.hh"

#include "MyConfigure.hh"
#include "TF1.h"

class G4ParticleGun;
class G4Event;
class PrimaryGeneratorMessenger;
class TChain;
class TFile;
class TH1F;
class G4ParticleDefinition;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction,
							   public MyConfigure
{
	public:
		PrimaryGeneratorAction();    
		virtual ~PrimaryGeneratorAction();

		void GeneratePrimaries(G4Event*);

		void Initialize();

		void ResetGen(G4String);

		void ReadCard(G4String);

		void* get_extra(G4String);

		static PrimaryGeneratorAction* GetPrimaryGeneratorAction();

		void Dump();

		void Reset();

		void set_EnergyMode(G4String val) { EnergyMode= val; }
		void set_DirectionMode(G4String val) { DirectionMode= val; }
		void set_PositionMode(G4String val) { PositionMode= val; }
		void set_TimeMode(G4String val) { TimeMode= val; }
		void set_pidMode(G4String val) { pidMode= val; }
		void set_EM_hist_filename(G4String val) { EM_hist_filename= val; }
		void set_EM_hist_histname(G4String val) { EM_hist_histname= val; }
		void set_DM_hist_filename(G4String val) { DM_hist_filename= val; }
		void set_DM_hist_histname(G4String val) { DM_hist_histname= val; }
		void set_root_filename(G4String val) { root_filename= val; }
		void set_root_treename(G4String val) { root_treename= val; }
		void set_root_index(int val){root_index=val;}


		void SetUniformDirection();
		void SetRandomDirection();
		void SetRandomPosition();
		void SetRandomEnergy();
		void SetUniformPosition();
		void BuildHistoFromFile();
		void root_get_para();
		void root_build();
		void root_set_Position();
		void root_set_Energy();
		void root_set_Time();
		void root_set_pid();
		void root_set_Rand();
		void root_set_extra();

		//  typedef std::map<G4int, class G4ParticleDefinition *> ParticleCache;

		//protected:
		//  ParticleCache targetMap;

	private:
		//	G4double get_mom_from_histo();

		G4ParticleGun*             particleGun;	 //pointer a to G4  class
		PrimaryGeneratorMessenger* gunMessenger;   //messenger of this class
		G4ParticleDefinition*      fParticle;

		static PrimaryGeneratorAction* fPrimaryGeneratorAction;

		//general options
		G4String                   fType;
		G4String                   ParticleName;
		G4int                      Z,A,C;
		G4double                   E;
		G4double                   Pa;
		G4double                   Ekin;
		G4double                   mass;
		G4int                      EnergyType;
		G4double                   Px,Py,Pz;
		G4double                   Theta,Phi;
		G4double                   x,y,z,t;
		G4double                   xSpread,ySpread,zSpread;
		G4double                   PosLimit2;
		G4double                   MomSpread;
		G4double                   EkinSpread;
		G4String                   RandMode;
		G4String                   EnergyMode;
		G4String                   PositionMode;
		G4String                   TimeMode;
		G4String                   pidMode;
		G4String                   DirectionMode;
		G4String                   PhiMode;
		G4String                   ThetaMode;
		G4double                   ThetaSpread,PhiSpread;
		G4double                   Ephi;
		G4double                   Etheta;
		G4double                   Epsi;

		//For EnergyMode
		TH1F*                      EM_hist;
		G4String                   EM_hist_filename;
		G4String                   EM_hist_histname;
		TH1F*                      DM_hist;
		G4String                   DM_hist_filename;
		G4String                   DM_hist_histname;

		//For PositionMode
		G4String                   UP_SubDet;
		G4String                   UP_Volume;
		G4String                   UP_Type;
		//For DirectionMode

		//For root file
		G4bool                     UseRoot;
		TChain*                    m_TChain;
		G4int                      root_num;
		G4int                      root_index;
		G4String                   root_filename;
		G4String                   root_treename;
		double                     root_double[20];
		int                        root_int[5];
		std::string                *root_str[2];
		TFile                      *fp;

		//For extra
		bool flag_weight;
		bool flag_ox;
		bool flag_oy;
		bool flag_oz;
		bool flag_opx;
		bool flag_opy;
		bool flag_opz;
		bool flag_ipx;
		bool flag_ipy;
		bool flag_ipz;
		bool flag_ot;
		bool flag_process;
		bool flag_volume;
		bool flag_R0;
		bool flag_R1;
		bool flag_ppid;
		bool flag_ptid;

  // For TURTLE data
  // No Fit result for Y-angle because it was uniform between -150 and 150 mrad
  TF1* fXPositionTurtleFit;
  TF1* fXAngleTurtleFit;
  TF1* fYPositionTurtleFit;
  static const double fYAngleTurtle_Lower = -150;
  static const double fYAngleTurtle_Upper = 150;
};

#endif

