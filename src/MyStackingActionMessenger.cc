//-*-C++-*-
// $Id: MyStackingActionMessenger.cc 80 2007-12-09 10:01:09Z comet $

static const char* MyStackingActionMessenger_cc =
"@(#) $Id: MyStackingActionMessenger.cc 80 2007-12-09 10:01:09Z comet $";

#include "MyStackingActionMessenger.hh"

#include "MyStackingAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "myglobals.hh"

MyStackingActionMessenger::MyStackingActionMessenger(MyStackingAction* aStack)
: fStackingAction(aStack) {
    fDir = new G4UIdirectory("/stacking/");
    fDir->SetGuidance("My stacking action control.");

    fEleCmd = new G4UIcmdWithADoubleAndUnit("/stacking/setEleCut",this);
    fEleCmd->SetGuidance("Set the minimum energy of electron to be stacked.");
    fEleCmd->SetParameterName("eleCut",false);
    fEleCmd->SetUnitCategory("Energy");
    fEleCmd->AvailableForStates(G4State_Idle);

    fPosCmd = new G4UIcmdWithADoubleAndUnit("/stacking/setPosCut",this);
    fPosCmd->SetGuidance("Set the minimum energy of positron to be stacked.");
    fPosCmd->SetParameterName("posCut",false);
    fPosCmd->SetUnitCategory("Energy");
    fPosCmd->AvailableForStates(G4State_Idle);

    fGamCmd = new G4UIcmdWithADoubleAndUnit("/stacking/setGamCut",this);
    fGamCmd->SetGuidance("Set the minimum energy of gamma to be stacked.");
    fGamCmd->SetParameterName("gamCut",false);
    fGamCmd->SetUnitCategory("Energy");
    fGamCmd->AvailableForStates(G4State_Idle);

	fadd_whiteCmd = new G4UIcmdWithAnInteger("/stacking/add_white_list", this);
	fadd_whiteCmd->SetGuidance("Add PDGEncoding to white list.");
	fadd_whiteCmd->SetParameterName("PDGEncoding",false);
	fadd_whiteCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	fadd_blackCmd = new G4UIcmdWithAnInteger("/stacking/add_black_list", this);
	fadd_blackCmd->SetGuidance("Add PDGEncoding to black list.");
	fadd_blackCmd->SetParameterName("PDGEncoding",false);
	fadd_blackCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	fclear_whiteCmd = new G4UIcmdWithoutParameter("/stacking/clear_white_list",this);
	fclear_whiteCmd->SetGuidance("Clear white list.");
	fclear_whiteCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	fclear_blackCmd = new G4UIcmdWithoutParameter("/stacking/clear_black_list",this);
	fclear_blackCmd->SetGuidance("Clear black list.");
	fclear_blackCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	fset_no_secCmd = new G4UIcmdWithAnInteger("/stacking/set_no_sec",this);
	fset_no_secCmd->SetGuidance("Set Muon Capture");
	fset_no_secCmd->SetParameterName("val",false);
	fset_no_secCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	fset_no_MCCmd = new G4UIcmdWithAnInteger("/stacking/set_no_MC",this);
	fset_no_MCCmd->SetGuidance("Set Muon Capture");
	fset_no_MCCmd->SetParameterName("val",false);
	fset_no_MCCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	fset_no_PCCmd = new G4UIcmdWithAnInteger("/stacking/set_no_PC",this);
	fset_no_PCCmd->SetGuidance("Set Pion Capture");
	fset_no_PCCmd->SetParameterName("val",false);
	fset_no_PCCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}


MyStackingActionMessenger::~MyStackingActionMessenger() {
    delete fDir;
    delete fEleCmd;
    delete fPosCmd;
    delete fGamCmd;
    delete fadd_whiteCmd;
    delete fadd_blackCmd;
    delete fclear_blackCmd;
    delete fclear_whiteCmd;
    delete fset_no_PCCmd;
    delete fset_no_secCmd;
    delete fset_no_MCCmd;
}


void MyStackingActionMessenger::SetNewValue(G4UIcommand * command,
G4String newValue) {
    if( command == fEleCmd ) {
        fStackingAction->
            SetEleCut(fEleCmd->GetNewDoubleValue(newValue));
    }

    if( command == fPosCmd ) {
        fStackingAction->
            SetPosCut(fPosCmd->GetNewDoubleValue(newValue));
    }

    if( command == fGamCmd ) {
        fStackingAction->
            SetGamCut(fGamCmd->GetNewDoubleValue(newValue));
    }

    if( command == fadd_whiteCmd ) {
        fStackingAction->
            add_white_list(fadd_whiteCmd->GetNewIntValue(newValue));
    }

    if( command == fadd_blackCmd ) {
        fStackingAction->
            add_black_list(fadd_blackCmd->GetNewIntValue(newValue));
    }

    if( command == fclear_whiteCmd ) {
        fStackingAction->
            clear_white_list();
    }

    if( command == fclear_blackCmd ) {
        fStackingAction->
            clear_black_list();
    }

    if( command == fset_no_secCmd ) {
        fStackingAction->
            set_no_sec(fset_no_secCmd->GetNewIntValue(newValue));
    }

    if( command == fset_no_MCCmd ) {
        fStackingAction->
            set_no_MC(fset_no_MCCmd->GetNewIntValue(newValue));
    }

    if( command == fset_no_PCCmd ) {
        fStackingAction->
            set_no_PC(fset_no_PCCmd->GetNewIntValue(newValue));
    }

}
