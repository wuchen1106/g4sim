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
// $Id: RunAction.cc,v 1.1 2010-10-18 15:56:17 maire Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "RunAction.hh"

#include "globals.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"

#include "G4EnergyLossTables.hh"
#include "G4ProductionCutsTable.hh"
#include "G4ParticleDefinition.hh"


#include "G4EmCalculator.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4VPhysicalVolume.hh"

#include "DetectorConstruction.hh"
#include "MyGlobalField.hh"

#include "MyAnalysisSvc.hh"

#include <time.h>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdlib.h>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run* aRun)
{ 
  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;

  //inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(true);

	//deal with analysis
	MyAnalysisSvc::GetMyAnalysisSvc()->BeginOfRunAction();

	//t_begin = (double)clock();

    FieldList* fields = MyGlobalField::getObject()->getFields();
    if (fields) {
        if (fields->size()>0) {
            FieldList::iterator i;
            for (i=fields->begin(); i!=fields->end(); ++i) {
                (*i)->construct();
                //G4cout << "Constructed field " << G4endl;
            }
        }
    }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* aRun)
{

	//deal with analysis
	MyAnalysisSvc::GetMyAnalysisSvc()->EndOfRunAction(aRun);

  G4int NbOfEvents = aRun->GetNumberOfEvent();
  if (NbOfEvents == 0) return;

	//t_end = clock();
	//double duration = (t_end-t_begin)/CLOCKS_PER_SEC;
	//std::cout<<"TOTAL TIME COST FOR THIS RUN:     "<<duration<<"s"<<std::endl;
	//std::cout<<"TIME COST PER EVENT FOR THIS RUN: "<<duration/NbOfEvents<<"s"<<std::endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
