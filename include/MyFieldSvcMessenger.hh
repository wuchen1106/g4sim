//---------------------------------------------------------------------------//
//Description: Messenger class
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#ifndef MyFieldSvcMessenger_h
#define MyFieldSvcMessenger_h 1

#include "myglobals.hh"
#include "G4UImessenger.hh"

class MyFieldSvc;
class G4UIdirectory;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAString;
class G4UIcmdWithADouble;
class G4UIcmdWithADoubleAndUnit;

class MyFieldSvcMessenger: public G4UImessenger
{
  public:
    MyFieldSvcMessenger(MyFieldSvc* );
   ~MyFieldSvcMessenger();
    
    void SetNewValue(G4UIcommand*, G4String);
    
  private:
    MyFieldSvc*               fMyFieldSvc;
    
    G4UIdirectory*             g4simDir;
    G4UIdirectory*             magDir;
    G4UIdirectory*             eleDir;
    G4UIcmdWithoutParameter*   SetFieldCmd;
    G4UIcmdWithAString*        ReadCardCmd;
    G4UIcmdWithAString*        AddMapCmd;
    G4UIcmdWithoutParameter*   ResetCmd;
    G4UIcmdWithADoubleAndUnit* SetMagIntensityCmd;
    G4UIcmdWithADoubleAndUnit* SetMagThetaCmd;
    G4UIcmdWithADoubleAndUnit* SetMagPhiCmd;
    G4UIcmdWithADoubleAndUnit* SetEleIntensityCmd;
    G4UIcmdWithADoubleAndUnit* SetEleThetaCmd;
    G4UIcmdWithADoubleAndUnit* SetElePhiCmd;
};

#endif

