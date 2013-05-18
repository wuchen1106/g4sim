//---------------------------------------------------------------------------//
//Description: Messenger class
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "MyDetectorManagerMessenger.hh"

#include "MyDetectorManager.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"

MyDetectorManagerMessenger::MyDetectorManagerMessenger(MyDetectorManager* pointer)
:m_MyDetectorManager(pointer)
{ 
  g4simDir = new G4UIdirectory("/g4sim/");
  g4simDir->SetGuidance("UI commands of this example");
  
  detDir = new G4UIdirectory("/g4sim/det/");
  detDir->SetGuidance("detector control");
       
  ReadCardCmd = new G4UIcmdWithAString("/g4sim/det/ReadCard",this);
  ReadCardCmd->SetGuidance("Read geometry file.");
  ReadCardCmd->SetGuidance("You have to call update before you start a new run.");
  ReadCardCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

MyDetectorManagerMessenger::~MyDetectorManagerMessenger()
{
	delete g4simDir;
	delete detDir;
  delete ReadCardCmd;
}

void MyDetectorManagerMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{ 
  if( command == ReadCardCmd ) { m_MyDetectorManager->ReadCard(newValue); }
}
