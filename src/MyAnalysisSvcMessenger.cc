//---------------------------------------------------------------------------//
//Description: Messenger class
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "MyAnalysisSvcMessenger.hh"

#include "MyAnalysisSvc.hh"
#include "globals.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

	MyAnalysisSvcMessenger::MyAnalysisSvcMessenger(MyAnalysisSvc* myAnalysisSvc)
:m_myAnalysisSvc(myAnalysisSvc)
{

	myAnalysisSvcDir = new G4UIdirectory("/g4sim/myAnalysisSvc/");
	myAnalysisSvcDir->SetGuidance("myAnalysisSvc control");

	//about output
	out_card_cmd = new G4UIcmdWithAString("/g4sim/myAnalysisSvc/set_out_card",this);
	out_card_cmd->SetGuidance("set name for MyAnalysisSvc file");
	out_card_cmd->SetParameterName("out_card",false);
	out_card_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	//about root file
	ofile_name_cmd = new G4UIcmdWithAString("/g4sim/myAnalysisSvc/set_ofile_name",this);
	ofile_name_cmd->SetGuidance("set name for MyAnalysisSvc file");
	ofile_name_cmd->SetParameterName("ofile_name",true);
	ofile_name_cmd->SetDefaultValue("raw_g4sim.root");
	ofile_name_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	tree_name_cmd = new G4UIcmdWithAString("/g4sim/myAnalysisSvc/set_tree_name",this);
	tree_name_cmd->SetGuidance("set name for MyAnalysisSvc tree");
	tree_name_cmd->SetParameterName("tree_name",true);
	tree_name_cmd->SetDefaultValue("t");
	tree_name_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	//about cut
	set_minT_cmd = new G4UIcmdWithADoubleAndUnit("/g4sim/myAnalysisSvc/set_minT",this);
	set_minT_cmd->SetGuidance("Define minimal time");
	set_minT_cmd->SetParameterName("minT",false,false);
	set_minT_cmd->SetDefaultUnit("ns");
	set_minT_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	set_maxT_cmd = new G4UIcmdWithADoubleAndUnit("/g4sim/myAnalysisSvc/set_maxT",this);
	set_maxT_cmd->SetGuidance("Define minimal time");
	set_maxT_cmd->SetParameterName("maxT",false,false);
	set_maxT_cmd->SetDefaultUnit("ns");
	set_maxT_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	//about log
	run_name_cmd = new G4UIcmdWithAString("/g4sim/myAnalysisSvc/set_run_name",this);
	run_name_cmd->SetGuidance("set run_name");
	run_name_cmd->SetParameterName("run_name",true);
	run_name_cmd->SetDefaultValue("test");
	run_name_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	//about PrintModulo
	PrintCmd = new G4UIcmdWithAnInteger("/g4sim/myAnalysisSvc/printModulo",this);
	PrintCmd->SetGuidance("Print events modulo n");
	PrintCmd->SetParameterName("EventNb",false);
	PrintCmd->SetRange("EventNb>0");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MyAnalysisSvcMessenger::~MyAnalysisSvcMessenger()
{
	delete myAnalysisSvcDir;
	//about output
	delete out_card_cmd;

	//about root file
	delete ofile_name_cmd;
	delete tree_name_cmd;

	//about cut
	delete set_minT_cmd;
	delete set_maxT_cmd;

	//about log
	delete run_name_cmd;

	//about PrintModulo
	delete PrintCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyAnalysisSvcMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{      
	//about output
	if(command==out_card_cmd){m_myAnalysisSvc->set_out_card(newValue);}

	//about root file
	if(command==ofile_name_cmd){m_myAnalysisSvc->set_ofile_name(newValue);}
	if(command==tree_name_cmd){m_myAnalysisSvc->set_tree_name(newValue);}

	//about cut
    if(command==set_minT_cmd) m_myAnalysisSvc->set_minT(set_minT_cmd->GetNewDoubleValue(newValue));
    if(command==set_maxT_cmd) m_myAnalysisSvc->set_maxT(set_maxT_cmd->GetNewDoubleValue(newValue));

	//about log
	if(command==run_name_cmd){m_myAnalysisSvc->set_run_name(newValue);}

	//about PrintModulo
	if(command == PrintCmd){m_myAnalysisSvc->set_PrintModulo(PrintCmd->GetNewIntValue(newValue));}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
