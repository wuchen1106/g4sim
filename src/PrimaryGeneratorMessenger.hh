//---------------------------------------------------------------------------//
//Description: Messenger class
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#ifndef PrimaryGeneratorMessenger_h
#define PrimaryGeneratorMessenger_h 1

#include "G4UImessenger.hh"
#include "myglobals.hh"

class PrimaryGeneratorAction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class G4UIcmdWithoutParameter;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class PrimaryGeneratorMessenger: public G4UImessenger
{
public:
  PrimaryGeneratorMessenger(PrimaryGeneratorAction*);
  virtual ~PrimaryGeneratorMessenger();
    
  void SetNewValue(G4UIcommand*, G4String);
    
private:
  PrimaryGeneratorAction*   Action;
  G4UIdirectory*            gunDir; 
  G4UIcmdWithAString*       ParticleName_cmd;
  G4UIcmdWithAnInteger*     root_index_cmd;
  G4UIcmdWithAString*       RandomMode_cmd;
  G4UIcmdWithAString*       EnergyMode_cmd;
  G4UIcmdWithAString*       DirectionMode_cmd;
  G4UIcmdWithAString*       PositionMode_cmd;
  G4UIcmdWithAString*       pidMode_cmd;
  G4UIcmdWithAString*       EM_hist_filename_cmd;
  G4UIcmdWithAString*       EM_hist_histname_cmd;
  G4UIcmdWithAString*       DM_hist_filename_cmd;
  G4UIcmdWithAString*       DM_hist_histname_cmd;
  G4UIcmdWithAString*       root_filename_cmd;
  G4UIcmdWithAString*       defaultPosition_cmd;
  G4UIcmdWithAString*       defaultEnergy_cmd;
  G4UIcmdWithAString*       root_treename_cmd;
  G4UIcmdWithAString*       ResetGenCmd;
  G4UIcmdWithAString*       ReadCardCmd;
  G4UIcmdWithoutParameter*  InitializeCmd;
  G4UIcmdWithoutParameter*  histo_build_cmd;
  G4UIcmdWithoutParameter*  root_build_cmd;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

