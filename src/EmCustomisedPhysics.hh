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
// $Id: EmCustomisedPhysics.hh 105735 2017-08-16 12:59:43Z gcosmo $
//
//---------------------------------------------------------------------------
//
// ClassName:   EmCustomisedPhysics
//
// Author:      V.Ivanchenko 09.11.2005
//
// Modified:
// 05.12.2005 V.Ivanchenko add controlled verbosity
// 23.11.2006 V.Ivanchenko remove mscStepLimit option and improve cout
//
//----------------------------------------------------------------------------
//
// This class provides construction of default EM standard physics
//

#ifndef EmCustomisedPhysics_h
#define EmCustomisedPhysics_h 1

#include "G4Version.hh"
#include "G4VPhysicsConstructor.hh"
#if G4VERSION_NUMBER >= 1040
#include "G4EmParticleList.hh"
#endif
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EmCustomisedPhysics : public G4VPhysicsConstructor
{
public:

  explicit EmCustomisedPhysics(G4int ver=0, const G4String& name="");

  virtual ~EmCustomisedPhysics();

  virtual void ConstructParticle();
  virtual void ConstructProcess();

private:
  G4int  verbose;
#if G4VERSION_NUMBER >= 1040
  G4EmParticleList partList;
#endif
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif






