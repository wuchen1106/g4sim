//---------------------------------------------------------------------------//
//Description: Messenger class
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "DetectorMessenger.hh"

#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithoutParameter.hh"

DetectorMessenger::DetectorMessenger(
                                           DetectorConstruction* Det)
:Detector(Det)
{ 
  g4simDir = new G4UIdirectory("/g4sim/");
  g4simDir->SetGuidance("UI commands of this example");
  
  detDir = new G4UIdirectory("/g4sim/det/");
  detDir->SetGuidance("detector control");
       
  UpdateCmd = new G4UIcmdWithoutParameter("/g4sim/det/update",this);
  UpdateCmd->SetGuidance("Update calorimeter geometry.");
  UpdateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
  UpdateCmd->SetGuidance("if you changed geometrical value(s).");
  UpdateCmd->AvailableForStates(G4State_Idle);
}

DetectorMessenger::~DetectorMessenger()
{
  delete g4simDir;
  delete detDir;
  delete UpdateCmd;
}

void DetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{ 
  if( command == UpdateCmd ) { Detector->UpdateGeometry(); }
}
