//---------------------------------------------------------------------------//
//Description: Messenger class
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "MyFieldSvcMessenger.hh"

#include "MyFieldSvc.hh"
#include "MyString2Anything.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithoutParameter.hh"

MyFieldSvcMessenger::MyFieldSvcMessenger(MyFieldSvc* magFieldSvc)
:fMyFieldSvc(magFieldSvc)
{
  g4simDir = new G4UIdirectory("/g4sim/");
  g4simDir->SetGuidance("UI commands of this example");

  magDir = new G4UIdirectory("/g4sim/mag/");
  magDir->SetGuidance("magnet field control");

  ResetCmd = new G4UIcmdWithoutParameter("/g4sim/mag/Reset",this);
  ResetCmd->SetGuidance("Reset field to none.");
  ResetCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  ReadCardCmd = new G4UIcmdWithAString("/g4sim/mag/ReadCard",this);
  ReadCardCmd->SetGuidance("Read MagField settings from assigned file.");
  ReadCardCmd->SetParameterName("filename",false);
  ReadCardCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  AddMapCmd = new G4UIcmdWithAString("/g4sim/mag/AddMap",this);
  AddMapCmd->SetGuidance("Add a map with scale factor.");
  AddMapCmd->SetParameterName("filename",false);
  AddMapCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  SetFieldCmd = new G4UIcmdWithoutParameter("/g4sim/mag/update",this);
  SetFieldCmd->SetGuidance("SetField.");
  SetFieldCmd->SetGuidance("This command MUST be applied before \"beamOn\" and cannot be used when you chose fieldMap mode!!!");
  SetFieldCmd->SetGuidance("if you changed MagField value(s).");
  SetFieldCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  SetMagIntensityCmd = new G4UIcmdWithADoubleAndUnit("/g4sim/mag/setIntensity",this);
  SetMagIntensityCmd->SetGuidance("SetMagIntensity.");
  SetMagIntensityCmd->SetParameterName("intensity",false);
  SetMagIntensityCmd->SetRange("intensity>=0.");
  SetMagIntensityCmd->SetUnitCategory("Magnetic flux density");
  SetMagIntensityCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  SetMagThetaCmd = new G4UIcmdWithADoubleAndUnit("/g4sim/mag/setTheta",this);
  SetMagThetaCmd->SetGuidance("SetMagTheta.");
  SetMagThetaCmd->SetParameterName("Theta",false);
  SetMagThetaCmd->SetUnitCategory("Angle");
  SetMagThetaCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  SetMagPhiCmd = new G4UIcmdWithADoubleAndUnit("/g4sim/mag/setPhi",this);
  SetMagPhiCmd->SetGuidance("SetMagPhi.");
  SetMagPhiCmd->SetParameterName("Phi",false);
  SetMagPhiCmd->SetUnitCategory("Angle");
  SetMagPhiCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  eleDir = new G4UIdirectory("/g4sim/ele/");
  eleDir->SetGuidance("electric field control");

  SetEleIntensityCmd = new G4UIcmdWithADoubleAndUnit("/g4sim/ele/setIntensity",this);
  SetEleIntensityCmd->SetGuidance("SetEleIntensity.");
  SetEleIntensityCmd->SetParameterName("intensity",false);
  SetEleIntensityCmd->SetRange("intensity>=0.");
  SetEleIntensityCmd->SetUnitCategory("Electric flux density");
  SetEleIntensityCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  SetEleThetaCmd = new G4UIcmdWithADoubleAndUnit("/g4sim/ele/setTheta",this);
  SetEleThetaCmd->SetGuidance("SetEleTheta.");
  SetEleThetaCmd->SetParameterName("Theta",false);
  SetEleThetaCmd->SetUnitCategory("Angle");
  SetEleThetaCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  SetElePhiCmd = new G4UIcmdWithADoubleAndUnit("/g4sim/ele/setPhi",this);
  SetElePhiCmd->SetGuidance("SetElePhi.");
  SetElePhiCmd->SetParameterName("Phi",false);
  SetElePhiCmd->SetUnitCategory("Angle");
  SetElePhiCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

MyFieldSvcMessenger::~MyFieldSvcMessenger()
{
	delete g4simDir;
	delete magDir;
  delete SetFieldCmd;
  delete ReadCardCmd;
  delete ResetCmd;
  delete AddMapCmd;
  delete SetMagIntensityCmd;
  delete SetMagThetaCmd;
  delete SetMagPhiCmd;
  delete SetEleIntensityCmd;
  delete SetEleThetaCmd;
  delete SetElePhiCmd;
}

void MyFieldSvcMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
  if( command == SetFieldCmd ) { fMyFieldSvc->SetField(); }
  if( command == ReadCardCmd ) { fMyFieldSvc->ReadCard(newValue); }
  if( command == ResetCmd ) { fMyFieldSvc->Reset(); }
  if( command == AddMapCmd ) {
	  G4String name; G4double val, x, y, z;
	  MyString2Anything::get_SDDDD(newValue,name,val,x,y,z);
	  fMyFieldSvc->AddMap(name,val,x,y,z);
  }
  if( command == SetMagIntensityCmd ) { fMyFieldSvc->SetMagIntensity(SetMagIntensityCmd->GetNewDoubleValue(newValue)); }
  if( command == SetMagThetaCmd ) { fMyFieldSvc->SetMagTheta(SetMagThetaCmd->GetNewDoubleValue(newValue)); }
  if( command == SetMagPhiCmd ) { fMyFieldSvc->SetMagPhi(SetMagPhiCmd->GetNewDoubleValue(newValue)); }
  if( command == SetEleIntensityCmd ) { fMyFieldSvc->SetEleIntensity(SetEleIntensityCmd->GetNewDoubleValue(newValue)); }
  if( command == SetEleThetaCmd ) { fMyFieldSvc->SetEleTheta(SetEleThetaCmd->GetNewDoubleValue(newValue)); }
  if( command == SetElePhiCmd ) { fMyFieldSvc->SetElePhi(SetElePhiCmd->GetNewDoubleValue(newValue)); }
}
