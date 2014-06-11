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
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef MyGlobalFieldMessenger_h
#define MyGlobalFieldMessenger_h 1

#include "myglobals.hh"
#include "G4UImessenger.hh"

class MyGlobalField;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithoutParameter;

class MyGlobalFieldMessenger: public G4UImessenger {
    public:
        MyGlobalFieldMessenger(MyGlobalField* );
        ~MyGlobalFieldMessenger();

        void SetNewValue(G4UIcommand*, G4String);
        void SetNewValue(G4UIcommand*, G4int);

    private:

        MyGlobalField*               fGlobalField;

        G4UIdirectory*             detDir;

        G4UIcmdWith3VectorAndUnit* fGetFieldCMD;
        G4UIcmdWithAnInteger*      fVerboseCMD;
        G4int fVerbose;

        G4UIcmdWithAnInteger*      fStepperCMD;
        G4UIcmdWithADoubleAndUnit* fMinStepCMD;
        G4UIcmdWithADoubleAndUnit* fDeltaChordCMD;
        G4UIcmdWithADoubleAndUnit* fDeltaOneStepCMD;
        G4UIcmdWithADoubleAndUnit* fDeltaIntersectionCMD;
        G4UIcmdWithADoubleAndUnit* fEpsMinCMD;
        G4UIcmdWithADoubleAndUnit* fEpsMaxCMD;
        G4UIcmdWithoutParameter*   fUpdateCMD;

};
#endif
