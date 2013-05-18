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
// $Id: MyDecayPhysics.cc,v 1.2 2010-06-03 14:37:24 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//---------------------------------------------------------------------------
//
// ClassName:   MyDecayPhysics
//
// Author: 2002 J.P. Wellisch
//
// Modified:
// 10.11.2005 V.Ivanchenko edit to provide a standard
// 05.12.2005 V.Ivanchenko add controlled verbosity
// 25.04.2006 V.Ivanchenko fix problem of destructor 
//
//----------------------------------------------------------------------------
//

#include "MyDecayPhysics.hh"

#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"

#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BosonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4DecayTable.hh"
#include "G4VDecayChannel.hh"
#include "G4PhaseSpaceDecayChannel.hh"

MyDecayPhysics::MyDecayPhysics(G4int ver)
  :  G4VPhysicsConstructor("Decay"), verbose(ver), wasActivated(false)
{
  fDecayProcess = 0;
}

MyDecayPhysics::MyDecayPhysics(const G4String& name, G4int ver)
  :  G4VPhysicsConstructor(name), verbose(ver), wasActivated(false)
{
  fDecayProcess = 0;
}

MyDecayPhysics::~MyDecayPhysics()
{
  delete fDecayProcess;
}

void MyDecayPhysics::ConstructParticle()
{

// G4cout << "MyDecayPhysics::ConstructParticle" << G4endl;
  G4BosonConstructor  pBosonConstructor;
  pBosonConstructor.ConstructParticle();

  G4LeptonConstructor pLeptonConstructor;
  pLeptonConstructor.ConstructParticle();

  G4MesonConstructor pMesonConstructor;
  pMesonConstructor.ConstructParticle();

  G4BaryonConstructor pBaryonConstructor;
  pBaryonConstructor.ConstructParticle();

  G4IonConstructor pIonConstructor;
  pIonConstructor.ConstructParticle();

  G4ShortLivedConstructor pShortLivedConstructor;
  pShortLivedConstructor.ConstructParticle();  

	G4DecayTable* table;
	G4VDecayChannel* mode;

	table = new G4DecayTable();
	mode = new G4PhaseSpaceDecayChannel("pi-",0.999877,2,"mu-","anti_nu_mu");
	table->Insert(mode);
	//mode = new G4PhaseSpaceDecayChannel("pi-",1,2,"e-","anti_nu_e");
	mode = new G4PhaseSpaceDecayChannel("pi-",1.230e-4,2,"e-","anti_nu_e");
	table->Insert(mode);
	G4PionMinus::PionMinusDefinition()->SetDecayTable(table);

	table = new G4DecayTable();
	mode = new G4PhaseSpaceDecayChannel("pi+",0.999877,2,"mu+","nu_mu");
	table->Insert(mode);
	//mode = new G4PhaseSpaceDecayChannel("pi+",1,2,"e+","nu_e");
	mode = new G4PhaseSpaceDecayChannel("pi+",1.230e-4,2,"e+","nu_e");
	table->Insert(mode);
	G4PionPlus::PionPlusDefinition()->SetDecayTable(table);

}

void MyDecayPhysics::ConstructProcess()
{
  if(wasActivated) { return; }
  wasActivated = true;

  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();

  // Add Decay Process
  fDecayProcess = new G4Decay();
  theParticleIterator->reset();
  G4ParticleDefinition* particle=0;

  while( (*theParticleIterator)() )
  {
    particle = theParticleIterator->value();
    if( fDecayProcess->IsApplicable(*particle) ) 
    { 
      if(verbose > 1) {
        G4cout << "### Decays for " << particle->GetParticleName() << G4endl;
      }
      ph->RegisterProcess(fDecayProcess, particle);
    }
  }

}
