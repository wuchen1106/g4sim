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
#include "MyString2Anything.hh"
#include "G4UIcmdWithoutParameter.hh"

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

	AddGeoCmd = new G4UIcmdWithAString("/g4sim/det/AddGeo",this);
	AddGeoCmd->SetGuidance("Read geometry file.");
	AddGeoCmd->SetGuidance("You have to call update before you start a new run.");
	AddGeoCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	SetSDCmd = new G4UIcmdWithAString("/g4sim/det/SetSD",this);
	SetSDCmd->SetGuidance("Assign a SD to a volume.");
	SetSDCmd->SetGuidance("Svc Volume SD.");
	SetSDCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	ClearGeoCmd = new G4UIcmdWithoutParameter("/g4sim/det/ClearGeo",this);
	ClearGeoCmd->SetGuidance("Clear geometry.");
	ClearGeoCmd->AvailableForStates(G4State_Idle);
}

MyDetectorManagerMessenger::~MyDetectorManagerMessenger()
{
	delete g4simDir;
	delete detDir;
	delete ReadCardCmd;
	delete AddGeoCmd;
	delete SetSDCmd;
	delete ClearGeoCmd;
}

void MyDetectorManagerMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{ 
	if( command == ReadCardCmd ) { m_MyDetectorManager->ReadCard(newValue); }
	if( command == AddGeoCmd ) {
		G4String file, type, name;
		MyString2Anything::get_SSS(newValue, name, file, type);
		m_MyDetectorManager->AddGeo(name, file, type);
	}
	if( command == SetSDCmd ) {
		G4String Svc, volume, SD;
		MyString2Anything::get_SSS(newValue, Svc, volume, SD);
		m_MyDetectorManager->SetSD(volume, SD, Svc);
	}
	if (command == ClearGeoCmd){ m_MyDetectorManager->ClearGeo(); }
}
