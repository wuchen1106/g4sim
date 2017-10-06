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
// $Id: EventAction.hh,v 1.1 2010-10-18 15:56:17 maire Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "MyTrackingAction.hh"

#include "G4Track.hh"
#include "G4ThreeVector.hh"
#include "G4VProcess.hh"

#include "MyAnalysisSvc.hh"

MyTrackingAction::MyTrackingAction()
{ }

void MyTrackingAction::PreUserTrackingAction(const G4Track* aTrack) {
	MyAnalysisSvc::GetMyAnalysisSvc()->PreUserTrackingAction(aTrack);

//	G4String processName;
//	const G4VProcess* process = aTrack->GetCreatorProcess();
//	if (process) {
//		processName = process->GetProcessName();
//	}
//	else{
//		processName = "NULL";
//	}
//  G4int trackID= aTrack->GetTrackID(); //G4 track ID of current track.
//  G4int ptid = aTrack->GetParentID(); //parent G4 track ID of current track.
//	G4int pid = aTrack->GetParticleDefinition()->GetPDGEncoding();
//  G4double globalT=aTrack->GetGlobalTime();//Time since the event in which the track belongs is created
//	G4ThreeVector mom_3vec = aTrack->GetMomentum();
//	G4double mom = mom_3vec.mag();
//
//	std::cout<<"Begin of track "<<trackID<<std::endl;
//	std::cout<<"    pid: "<<pid<<std::endl;
//	std::cout<<"    ptid: "<<ptid<<std::endl;
//	std::cout<<"    process: "<<processName<<std::endl;
//	std::cout<<"    Global time: "<<globalT/ns<<"ns"<<std::endl;
//	std::cout<<"    momentum: "<<mom/MeV<<"MeV"<<std::endl;

}


void MyTrackingAction::PostUserTrackingAction(const G4Track* aTrack) {
	MyAnalysisSvc::GetMyAnalysisSvc()->PostUserTrackingAction(aTrack);
//  G4int trackID= aTrack->GetTrackID(); //G4 track ID of current track.
//	G4int   nbSteps = aTrack->GetCurrentStepNumber();
//	G4double trackLength = aTrack->GetTrackLength();
//	std::cout<<"End of track "<<trackID<<std::endl;
//	std::cout<<"    number of steps: "<<nbSteps<<std::endl;
//	std::cout<<"    track length: "<<trackLength/mm<<"mm"<<std::endl;
}
