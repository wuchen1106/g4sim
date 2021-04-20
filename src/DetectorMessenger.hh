//---------------------------------------------------------------------------//
//Description: Messenger class
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#ifndef DetectorMessenger_h
#define DetectorMessenger_h 1

#include "myglobals.hh"
#include "G4UImessenger.hh"

class DetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAString;

class DetectorMessenger: public G4UImessenger
{
  public:
    DetectorMessenger(DetectorConstruction* );
   ~DetectorMessenger();
    
    void SetNewValue(G4UIcommand*, G4String);
    
  private:
    DetectorConstruction* Detector;
    
    G4UIdirectory*             g4simDir;
    G4UIdirectory*             detDir;
    G4UIcmdWithAString*        ResetMagCmd;
    G4UIcmdWithAString*        ReloadGeoCmd;
    G4UIcmdWithoutParameter*   UpdateCmd;
};

#endif

