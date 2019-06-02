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

#include "myglobals.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"

#include "G4EnergyLossTables.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4ProductionCutsTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4LossTableManager.hh"
#include "G4RegionStore.hh"

#include "G4EmCalculator.hh"
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

//#################################################################################333	
	//Print range/energy table
	const G4double cutmin=1*um, cutmax=1*m;
	const G4int nbin=1000;
	G4double cut[nbin];
	const G4double dp = std::log10(cutmax/cutmin)/nbin;
	const G4double dt = std::pow(10.,dp);
	cut[0] = cutmin;
	for (G4int i=1; i<nbin; ++i) cut[i] = cut[i-1]*dt;

    // get the region of the world and its cuts
	G4Region* region = G4RegionStore::GetInstance()->GetRegion("DefaultRegionForTheWorld", false);
	G4ProductionCuts* pcuts = region->GetProductionCuts();

	//set precisions
	std::ios::fmtflags mode = G4cout.flags();
	G4cout.setf(std::ios::fixed,std::ios::floatfield);
	G4int  prec = G4cout.precision(3);
	G4cout.setf(std::ios::scientific,std::ios::floatfield);

    std::vector<G4ParticleDefinition*> particles;
	particles.push_back(G4ParticleTable::GetParticleTable()->FindParticle("gamma"));
	particles.push_back(G4ParticleTable::GetParticleTable()->FindParticle("e-"));
	particles.push_back(G4ParticleTable::GetParticleTable()->FindParticle("e+"));
	particles.push_back(G4ParticleTable::GetParticleTable()->FindParticle("proton"));

	G4ProductionCutsTable* theCoupleTable =
		G4ProductionCutsTable::GetProductionCutsTable();
	size_t numOfCouples = theCoupleTable->GetTableSize();
	const G4MaterialCutsCouple* couple = 0;

    G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(1*eV,100*GeV);
    for (size_t i=0; i<numOfCouples; i++) {
        couple = theCoupleTable->GetMaterialCutsCouple(i);

        const G4Material* mat = couple->GetMaterial();
        G4cout << "Range (mm)/Energy (MeV) in " << mat ->GetName() << G4endl;
        G4cout.precision(6);
        G4cout << "Range";
        for (size_t ipart=0; ipart<particles.size(); ipart++){
            G4ParticleDefinition* part = particles.at(ipart);
            G4cout << " E_" << part->GetParticleName();
        }
        G4cout << G4endl;
        for (G4int l=0;l<nbin; ++l)
        {
            for (size_t ipart=0; ipart<particles.size(); ipart++){
                G4ParticleDefinition* part = particles.at(ipart);
                pcuts->SetProductionCut(cut[l],part->GetParticleName());
            }

            G4ProductionCutsTable::GetProductionCutsTable()->UpdateCoupleTable(G4PhysicalVolumeStore::GetInstance()->GetVolume("World"));

            G4cout << " " << cut[l];
            for (size_t ipart=0; ipart<particles.size(); ipart++){
                G4double* energies = G4ProductionCutsTable::GetProductionCutsTable()->GetEnergyCutsDoubleVector(ipart);
                G4cout << " " << energies[i]/(MeV);
            }
            G4cout << G4endl;
        }
    }

	G4cout.precision(prec);
	G4cout.setf(mode,std::ios::floatfield);
//#################################################################################333	

    /*
//#################################################################################333	
	//Print dE/dx tables with binning identical to the Geant3 JMATE bank.
	//The printout is readable as Geant3 ffread data cards (by the program g4mat).
	//
	const G4double tkmin=1*keV, tkmax=10000*GeV;
	const G4int nbin=1000;
	G4double tk[nbin];

	const G4int ncolumn = 1;

	//compute the kinetic energies
	//
	const G4double dp = std::log10(tkmax/tkmin)/nbin;
	const G4double dt = std::pow(10.,dp);
	tk[0] = tkmin;
	for (G4int i=1; i<nbin; ++i) tk[i] = tk[i-1]*dt;

	//print the kinetic energies
	//
	std::ios::fmtflags mode = G4cout.flags();
	G4cout.setf(std::ios::fixed,std::ios::floatfield);
	G4int  prec = G4cout.precision(3);

	//print the dE/dx tables
	//
	G4cout.setf(std::ios::scientific,std::ios::floatfield);

    std::vector<G4ParticleDefinition*> particles;
	particles.push_back(G4ParticleTable::GetParticleTable()->FindParticle("mu-"));
	particles.push_back(G4ParticleTable::GetParticleTable()->FindParticle("mu+"));
	particles.push_back(G4ParticleTable::GetParticleTable()->FindParticle("e-"));
	particles.push_back(G4ParticleTable::GetParticleTable()->FindParticle("e+"));
	particles.push_back(G4ParticleTable::GetParticleTable()->FindParticle("proton"));
	particles.push_back(G4ParticleTable::GetParticleTable()->FindParticle("anti_proton"));

	G4ProductionCutsTable* theCoupleTable =
		G4ProductionCutsTable::GetProductionCutsTable();
	size_t numOfCouples = theCoupleTable->GetTableSize();
	const G4MaterialCutsCouple* couple = 0;

    for (size_t ipart=0; ipart<particles.size(); ipart++){
        for (size_t i=0; i<numOfCouples; i++) {
            G4ParticleDefinition* part = particles.at(ipart);
            couple = theCoupleTable->GetMaterialCutsCouple(i);
            const G4Material* mat = couple->GetMaterial();
            G4cout << "\nLIST";
            G4cout << "\nC \nC  dE/dx (MeV/cm) for " << part->GetParticleName()
                << " in " << mat ->GetName() << "\nC";
            G4cout.precision(6);
            G4cout << "\nEnergy/MeV   dE/dx (MeV/cm)\n ";
            for (G4int l=0;l<nbin; ++l)
            {
                G4cout << tk[l]/MeV<< "\t";
                G4double dedx = G4LossTableManager::Instance()
                    ->GetDEDX(part,tk[l],couple);
                G4cout << dedx/(MeV/cm) << "\n";
            }
            G4cout << G4endl;
        }
    }

	G4cout.precision(prec);
	G4cout.setf(mode,std::ios::floatfield);
//#################################################################################333	
    */


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
