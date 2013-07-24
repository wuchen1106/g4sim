//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
//

#include "MyGlobalFieldMessenger.hh"

#include "MyGlobalField.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"

//////////////////////////////////////////////////////////////////////////////

MyGlobalFieldMessenger::MyGlobalFieldMessenger(MyGlobalField* pEMfield)
: fGlobalField(pEMfield) {
    detDir = new G4UIdirectory("/field/");
    detDir->SetGuidance(" Field tracking control ");

    fVerboseCMD = new G4UIcmdWithAnInteger("/field/verbose",this);
    fVerboseCMD->SetGuidance("Verbosity of getField command");
    fVerboseCMD->SetParameterName("verbose",true);
    fVerboseCMD->SetDefaultValue(1);
    fVerboseCMD->AvailableForStates(G4State_PreInit,G4State_Idle);
    fVerbose=1;

    fGetFieldCMD =
        new G4UIcmdWith3VectorAndUnit("/field/getField",this);
    fGetFieldCMD->
        SetGuidance("Get the B field at the position.");
    fGetFieldCMD->SetParameterName("GlobalPosX","GlobalPosY","GlobalPosZ",false);
    fGetFieldCMD->SetUnitCategory("Length");
    fGetFieldCMD->AvailableForStates(G4State_Idle);

    fStepperCMD = new G4UIcmdWithAnInteger("/field/setStepperType",this);
    fStepperCMD->SetGuidance("Select stepper type for field");
    fStepperCMD->SetParameterName("choice",true);
    fStepperCMD->SetDefaultValue(4);
    fStepperCMD->AvailableForStates(G4State_PreInit,G4State_Idle);

    fUpdateCMD = new G4UIcmdWithoutParameter("/field/update",this);
    fUpdateCMD->SetGuidance("Update Field");
    fUpdateCMD->SetGuidance("This command MUST be applied before \"beamOn\" ");
    fUpdateCMD->SetGuidance("if you changed field settings.");
    fUpdateCMD->AvailableForStates(G4State_PreInit,G4State_Idle);

    fMinStepCMD = new G4UIcmdWithADoubleAndUnit("/field/setMinStep",this);
    fMinStepCMD->SetGuidance("Define minimal step");
    fMinStepCMD->SetParameterName("min step",false,false);
    fMinStepCMD->SetDefaultUnit("mm");
    fMinStepCMD->AvailableForStates(G4State_PreInit,G4State_Idle);

    fDeltaChordCMD = new G4UIcmdWithADoubleAndUnit("/field/setDeltaChord",this);
    fDeltaChordCMD->SetGuidance("Define delta chord");
    fDeltaChordCMD->SetParameterName("delta chord",false,false);
    fDeltaChordCMD->SetDefaultUnit("mm");
    fDeltaChordCMD->AvailableForStates(G4State_PreInit,G4State_Idle);

    fDeltaOneStepCMD =
        new G4UIcmdWithADoubleAndUnit("/field/setDeltaOneStep",this);
    fDeltaOneStepCMD->SetGuidance("Define delta one step");
    fDeltaOneStepCMD->SetParameterName("delta one step",false,false);
    fDeltaOneStepCMD->SetDefaultUnit("mm");
    fDeltaOneStepCMD->AvailableForStates(G4State_PreInit,G4State_Idle);

    fDeltaIntersectionCMD =
        new G4UIcmdWithADoubleAndUnit("/field/setDeltaIntersection",this);
    fDeltaIntersectionCMD->SetGuidance("Define delta intersection");
    fDeltaIntersectionCMD->SetParameterName("delta intersection",false,false);
    fDeltaIntersectionCMD->SetDefaultUnit("mm");
    fDeltaIntersectionCMD->AvailableForStates(G4State_PreInit,G4State_Idle);

    fEpsMinCMD = new G4UIcmdWithADoubleAndUnit("/field/setEpsMin",this);
    fEpsMinCMD->SetGuidance("Define eps min");
    fEpsMinCMD->SetParameterName("eps min",false,false);
    fEpsMinCMD->SetDefaultUnit("mm");
    fEpsMinCMD->AvailableForStates(G4State_PreInit,G4State_Idle);

    fEpsMaxCMD = new G4UIcmdWithADoubleAndUnit("/field/setEpsMax",this);
    fEpsMaxCMD->SetGuidance("Define eps max");
    fEpsMaxCMD->SetParameterName("eps max",false,false);
    fEpsMaxCMD->SetDefaultUnit("mm");
    fEpsMaxCMD->AvailableForStates(G4State_PreInit,G4State_Idle);
}


MyGlobalFieldMessenger::~MyGlobalFieldMessenger() {
    delete detDir;

    delete fGetFieldCMD;
    delete fVerboseCMD;

    delete fStepperCMD;
    delete fMinStepCMD;
    delete fDeltaChordCMD;
    delete fDeltaOneStepCMD;
    delete fDeltaIntersectionCMD;
    delete fEpsMinCMD;
    delete fEpsMaxCMD;
    delete fUpdateCMD;
}


void MyGlobalFieldMessenger::SetNewValue( G4UIcommand* command, G4String newValue) {
    if( command == fGetFieldCMD ) {
        G4ThreeVector aPos = fGetFieldCMD->GetNew3VectorValue(newValue);
        G4double aPosVec[3];
        aPosVec[0] = aPos.x();
        aPosVec[1] = aPos.y();
        aPosVec[2] = aPos.z();

        G4double theField[6];
        fGlobalField->GetFieldValue(aPosVec, theField);
        if(fVerbose>0) {
            G4cout << "Point = [" << aPosVec[0]/mm << ", "
                << aPosVec[1]/mm << ", " << aPosVec[2]/mm << "] mm" << G4endl;
            G4cout << "  Field = ["
                << theField[0]/tesla << ", " << theField[1]/tesla
                << ", " << theField[2]/tesla << "] T "
                << " [" << theField[3] << ", " << theField[4]
                << ", " << theField[5] << "] V/m" << G4endl;
        }
        else {
            G4cout << aPosVec[0]/mm << " " << aPosVec[1]/mm << " "
                << aPosVec[2]/mm << " " << theField[0]/tesla << " "
                << theField[1]/tesla << " " << theField[2]/tesla << " "
                << theField[3] << " " << theField[4] << " " << theField[5]
                << G4endl;
        }
    }

    if( command == fVerboseCMD ) {
        fVerbose=fStepperCMD->GetNewIntValue(newValue);
    }

    if( command == fStepperCMD ) {
        fGlobalField->SetStepperType(fStepperCMD->GetNewIntValue(newValue));
    }
    if( command == fUpdateCMD ) {
        fGlobalField->updateField();
    }
    if( command == fMinStepCMD ) {
        fGlobalField->SetMinStep(fMinStepCMD->GetNewDoubleValue(newValue));
    }
    if( command == fDeltaChordCMD ) {
        fGlobalField->SetDeltaChord(fDeltaChordCMD->GetNewDoubleValue(newValue));
    }
    if( command == fDeltaOneStepCMD ) {
        fGlobalField->
            SetDeltaOneStep(fDeltaOneStepCMD->GetNewDoubleValue(newValue));
    }
    if( command == fDeltaIntersectionCMD ) {
        fGlobalField->
            SetDeltaIntersection(fDeltaIntersectionCMD->GetNewDoubleValue(newValue));
    }
    if( command == fEpsMinCMD ) {
        fGlobalField->SetEpsMin(fEpsMinCMD->GetNewDoubleValue(newValue));
    }
    if( command == fEpsMaxCMD ) {
        fGlobalField->SetEpsMax(fEpsMaxCMD->GetNewDoubleValue(newValue));
    }
}
