//---------------------------------------------------------------------------//
//Description: Messenger class
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "MyVGeometryParameterMessenger.hh"

#include "globals.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAString.hh"

#include "MyVGeometryParameter.hh"

MyVGeometryParameterMessenger::MyVGeometryParameterMessenger(MyVGeometryParameter* fGeometryParameter, G4String name )
{
	set_Name(name);
	set_GeometryParameter(fGeometryParameter);

	m_GeoDirName = "/g4sim/det/" + name + "/";

  g4simDir = new G4UIdirectory("/g4sim/");
  g4simDir->SetGuidance("UI commands of this example");

  detDir = new G4UIdirectory("/g4sim/det/");
  detDir->SetGuidance("detector control");

	geoDir = new G4UIdirectory(m_GeoDirName);
	geoDir->SetGuidance("geometry control");

	G4String CmdName;

	CmdName = m_GeoDirName + "ReadCard";
	ReadCard_cmd = new G4UIcmdWithAString(CmdName, this);
  ReadCard_cmd->SetGuidance("ReadCard.");
  ReadCard_cmd->SetParameterName("filename",false);
  ReadCard_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_GeoDirName + "checkoverlap";
  set_checkoverlap_cmd = new G4UIcmdWithABool(CmdName, this);
  set_checkoverlap_cmd->SetGuidance("set checkoverlap.");
  set_checkoverlap_cmd->SetParameterName("checkoverlap",false);
  set_checkoverlap_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_GeoDirName + "VerboseLevel";
  set_VerboseLevel_cmd = new G4UIcmdWithAnInteger(CmdName, this);
  set_VerboseLevel_cmd->SetGuidance("set VerboseLevel.");
  set_VerboseLevel_cmd->SetParameterName("VerboseLevel",false);
  set_VerboseLevel_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

MyVGeometryParameterMessenger::~MyVGeometryParameterMessenger()
{
	delete g4simDir;
	delete geoDir;
	delete detDir;
  delete ReadCard_cmd;
  delete set_checkoverlap_cmd;
  delete set_VerboseLevel_cmd;
}

int MyVGeometryParameterMessenger::MySetNewValue(G4UIcommand* command,G4String newValue)
{

	int status = 0; //0 means found the value

	MyVGeometryParameter* fMyVGeometryParameter = get_GeometryParameter();

  if( command == ReadCard_cmd ) { m_GeometryParameter->InitFromFile(newValue); }

  else if( command == set_checkoverlap_cmd ) { fMyVGeometryParameter->set_checkoverlap(set_checkoverlap_cmd->GetNewBoolValue(newValue));}

  else if( command == set_VerboseLevel_cmd ) { fMyVGeometryParameter->set_VerboseLevel(set_VerboseLevel_cmd->GetNewIntValue(newValue));}

	else status = 1;

	return status;

}

