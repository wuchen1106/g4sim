//-*-C++-*-
// $Id: MyStackingActionMessenger.cc 80 2007-12-09 10:01:09Z comet $

static const char* MyStackingActionMessenger_cc =
"@(#) $Id: MyStackingActionMessenger.cc 80 2007-12-09 10:01:09Z comet $";

#include "MyStackingActionMessenger.hh"

#include "MyStackingAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "globals.hh"

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
}


MyStackingActionMessenger::~MyStackingActionMessenger() {
    delete fDir;
    delete fEleCmd;
    delete fPosCmd;
    delete fGamCmd;
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

}
