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

#include "G4DecayPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4IonPhysics.hh"
// TODO:
// /home/chen/MyWorkArea/g4sim/src/PhysicsList.cc:57:33: fatal error: G4QStoppingPhysics.hh: No such file or directory
//#include "G4QStoppingPhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4NeutronTrackingCut.hh"
// TODO:
// /home/chen/MyWorkArea/g4sim/src/PhysicsList.cc:62:36: fatal error: G4LHEPStoppingPhysics.hh: No such file or directory
//#include "G4LHEPStoppingPhysics.hh"

#include "G4DataQuestionaire.hh"
// TODO:
// /home/chen/MyWorkArea/g4sim/src/PhysicsList.cc:67:37: fatal error: HadronPhysicsQGSP_BERT.hh: No such file or directory
//#include "HadronPhysicsQGSP_BERT.hh"
//#include "HadronPhysicsQGSP_BERT_HP.hh"

#include "G4MuonicAtomDecayPhysics.hh"
#include "G4HadronHElasticPhysics.hh"
#include "G4StoppingPhysics.hh"


#include "MyDecayPhysics.hh"
#include "MyPionPhysics.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::PhysicsList(int ver):  G4VModularPhysicsList()
{

  G4DataQuestionaire it(photon);
  G4cout << "<<< Geant4 Physics List simulation engine: My Own  physics list based on QGSP_BERT 3.4"<<G4endl;
  G4cout <<G4endl;

  defaultCutValue = 1e-3*m;
  SetVerboseLevel(ver);

  // EM Physics
  RegisterPhysics( new G4EmStandardPhysics(ver) );

  // Synchroton Radiation & GN Physics
  RegisterPhysics( new G4EmExtraPhysics(ver) );

  // Decays
  //RegisterPhysics( new MyDecayPhysics(ver) );
  RegisterPhysics( new G4DecayPhysics(ver) );

  // Hadron Elastic scattering
  RegisterPhysics( new G4HadronElasticPhysics(ver) );

  // Hadron Physics
  //RegisterPhysics( new HadronPhysicsQGSP_BERT_HP(ver));
  //RegisterPhysics( new HadronPhysicsQGSP_BERT(ver));
  RegisterPhysics( new G4HadronHElasticPhysics(ver) );

  // Stopping Physics
  RegisterPhysics( new G4StoppingPhysics(ver) );
  //RegisterPhysics( new G4QStoppingPhysics(ver) );
  //RegisterPhysics( new G4LHEPStoppingPhysics(ver) );

  // Ion Physics
  RegisterPhysics( new G4IonPhysics(ver));
  
  // Neutron tracking cut
  RegisterPhysics( new G4NeutronTrackingCut(ver));

  // My Physics
  //RegisterPhysics( new MyPionPhysics(ver) );

  RegisterPhysics( new G4MuonicAtomDecayPhysics(ver) );
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

  SetCutsWithDefault();   
  
  if (verboseLevel >0)
    G4VModularPhysicsList::DumpCutValuesTable();  
}
