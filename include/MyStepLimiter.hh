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
// $Id: MyStepLimiter.hh,v 1.4 2010-06-03 15:03:53 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//---------------------------------------------------------------------------
//
// ClassName:   MyStepLimiter
//
// Author: 2002 J.P. Wellisch
//
// Modified:
// 10.11.2005 V.Ivanchenko edit to provide a standard
// 05.12.2005 V.Ivanchenko add controlled verbosity
//
//----------------------------------------------------------------------------
//

#ifndef MyStepLimiter_h
#define MyStepLimiter_h 1

#include "myglobals.hh"
#include "G4VPhysicsConstructor.hh"

class G4StepLimiter;

class MyStepLimiter : public G4VPhysicsConstructor
{
  public: 
    MyStepLimiter(G4int ver = 1);
    MyStepLimiter(const G4String& name, G4int ver = 1);
    virtual ~MyStepLimiter();

  public: 
    // This method will be invoked in the Construct() method. 
    // each particle type will be instantiated
  virtual void ConstructParticle();
 
    // This method will be invoked in the Construct() method.
    // each physics process will be instantiated and
    // registered to the process manager of each particle type 
  virtual void ConstructProcess();

private:
  G4StepLimiter* fStepLimiter;
  G4int    verbose;
  G4bool   wasActivated;
};

#endif
