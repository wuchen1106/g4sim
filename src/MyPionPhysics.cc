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
// $Id: MyPionPhysics.cc,v 1.2 2010-06-03 14:37:24 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//---------------------------------------------------------------------------
//
// ClassName:   MyPionPhysics
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

#include "MyPionPhysics.hh"

#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"

#include "G4PiMinusAbsorptionAtRest.hh"

#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"

MyPionPhysics::MyPionPhysics(G4int ver)
  :  G4VPhysicsConstructor("MyPionPhysics"), verbose(ver), wasActivated(false)
{
  fPiMinusAbsorptionAtRest = 0;
}

MyPionPhysics::MyPionPhysics(const G4String& name, G4int ver)
  :  G4VPhysicsConstructor(name), verbose(ver), wasActivated(false)
{
  fPiMinusAbsorptionAtRest = 0;
}

MyPionPhysics::~MyPionPhysics()
{
  delete fPiMinusAbsorptionAtRest;
}

void MyPionPhysics::ConstructParticle()
{

// G4cout << "MyPionPhysics::ConstructParticle" << G4endl;

}

void MyPionPhysics::ConstructProcess()
{
  if(wasActivated) { return; }
  wasActivated = true;

  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();

  // Add Decay Process
  fPiMinusAbsorptionAtRest = new G4PiMinusAbsorptionAtRest();
  theParticleIterator->reset();
  G4ParticleDefinition* particle=0;

  while( (*theParticleIterator)() )
  {
    particle = theParticleIterator->value();
		if ( particle->GetParticleName() == "pi-" ){
			// Add the pion capture process
			G4ProcessManager* pmanager = particle->GetProcessManager();
			pmanager->AddRestProcess(fPiMinusAbsorptionAtRest);
		}
  }

}
