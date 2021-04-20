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
// $Id: RunAction.hh,v 1.1 2010-10-18 15:56:17 maire Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "myglobals.hh"

#include <map>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class G4VPhysicalVolume;

class G4Run;

class RunAction : public G4UserRunAction
{
public:
//  RunAction( );
  RunAction();
  virtual ~RunAction();

  void BeginOfRunAction(const G4Run*);
  void   EndOfRunAction(const G4Run*);

  G4int find_volume(G4VPhysicalVolume* volume){
    std::map<G4VPhysicalVolume*,G4int>::const_iterator cit = m_volume.find(volume);
    if ( cit != m_volume.end() )
      return cit->second;
    else
      return 0;
  };

  G4int find_process(G4String process){
    std::map<G4String,G4int>::const_iterator cit = m_process.find(process);
    if ( cit != m_process.end() )
      return cit->second;
    else
      return -2;
  };
    
private:
  std::map<G4VPhysicalVolume*,G4int> m_volume;
  std::map<G4String,int> m_process;
  double t_begin;
  double t_end;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

