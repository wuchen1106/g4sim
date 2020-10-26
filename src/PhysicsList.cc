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
// $Id: PhysicsList.cc,v 1.1 2010-10-18 15:56:17 maire Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PhysicsList.hh"

#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"

#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4ios.hh"
#include <iomanip>   

#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"

// TODO: add version case
// These are files for older versions
//#include "G4QStoppingPhysics.hh"
//#include "G4LHEPStoppingPhysics.hh"
//#include "HadronPhysicsQGSP_BERT.hh"
//#include "HadronPhysicsQGSP_BERT_HP.hh"

#include "G4DecayPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmLivermorePhysics.hh"
#include "EmCustomisedPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4IonPhysics.hh"
#include "G4StoppingPhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4NeutronTrackingCut.hh"
#include "G4HadronPhysicsQGSP_BERT.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh"

#include "MyDecayPhysics.hh"
#include "MyPionPhysics.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::PhysicsList(int ver, int EmType):  G4VModularPhysicsList()
{

  G4cout << "<<< Geant4 Physics List simulation engine: My Own  physics list based on QGSP_BERT 3.4"<<G4endl;
  G4cout <<G4endl;

  this->defaultCutValue = 0.7*CLHEP::mm;  
  this->SetVerboseLevel(ver);

  // EM Physics
  if (EmType==-1){
      this->RegisterPhysics( new G4EmLivermorePhysics(ver) );
  }
  else if (EmType==-2){
      this->RegisterPhysics( new EmCustomisedPhysics(ver) );
  }
  else{
      if (EmType == 0){
          this->RegisterPhysics( new G4EmStandardPhysics(ver) );
      }
      else if (EmType == 3){
          this->RegisterPhysics( new G4EmStandardPhysics_option3(ver) );
      }
      else if (EmType == 4){
          this->RegisterPhysics( new G4EmStandardPhysics_option4(ver) );
      }
  }

  // Synchroton Radiation & GN Physics
  this->RegisterPhysics( new G4EmExtraPhysics(ver) );

  // Decays
  this->RegisterPhysics( new G4DecayPhysics(ver) );
  //this->RegisterPhysics( new MyDecayPhysics(ver) );

   // Hadron Elastic scattering
  this->RegisterPhysics( new G4HadronElasticPhysics(ver) );

  // Hadron Physics
  this->RegisterPhysics( new G4HadronPhysicsQGSP_BERT(ver));
  //this->RegisterPhysics( new G4HadronPhysicsQGSP_BERT_HP(ver));

  // Stopping Physics
  //this->RegisterPhysics( new G4QStoppingPhysics(ver) );
  //this->RegisterPhysics( new G4LHEPStoppingPhysics(ver) );
  this->RegisterPhysics( new G4StoppingPhysics(ver) );

  // Ion Physics
  this->RegisterPhysics( new G4IonPhysics(ver));
  
  // Neutron tracking cut
  this->RegisterPhysics( new G4NeutronTrackingCut(ver));

  // My Physics
  //RegisterPhysics( new MyPionPhysics(ver) );
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::~PhysicsList()
{}

void PhysicsList::SetCuts()
{
  if (verboseLevel >1){
    G4cout << "PhysicsList::SetCuts:";
  }  
  //  " G4VModularPhysicsList::SetCutsWithDefault" method sets 
  //   the default cut value for all particle types 

  this->SetCutsWithDefault();   

  if (verboseLevel >0)
    G4VModularPhysicsList::DumpCutValuesTable();  
}
