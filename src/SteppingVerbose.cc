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
// $Id: SteppingVerbose.cc,v 1.1 2010-09-16 16:26:13 gcosmo Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...... 

#include "SteppingVerbose.hh"

#include "G4SteppingManager.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingVerbose::SteppingVerbose()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingVerbose::~SteppingVerbose()
{} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingVerbose::StepInfo()
{
	CopyState();

	G4int prec = std::cout.precision(3);

	std::cout<<"***********************************************************"<<std::endl;
//	std::cout<<"verboseLevel = "<<verboseLevel<<std::endl;
//	std::cout<<"***********************************************************"<<std::endl;
	if( verboseLevel >= 1 ){
		if( verboseLevel >= 4 ) VerboseTrack();
		if( verboseLevel >= 3 ){
			std::cout << std::endl;    
			std::cout << std::setw( 6) << "#Step#" << " "
				<< std::setw( 9) << "X"         << " "
				<< std::setw( 9) << "Y"         << " "  
				<< std::setw( 9) << "Z"         << " "
				<< std::setw( 9) << "t"         << " "
				<< std::setw( 9) << "pX"        << " "
				<< std::setw( 9) << "pY"        << " "  
				<< std::setw( 9) << "pZ"        << " "
				<< std::setw( 9) << "KineE"     << " "
				<< std::setw( 9) << "dEtot"     << " "
				<< std::setw( 9) << "dEIon"     << " "
				<< std::setw( 9) << "dEdel"     << " "
				<< std::setw(10) << "StepLeng"  << " "
				<< std::setw(10) << "TrakLeng"  << " "
				<< std::setw(10) << "Volume"    << " "
				<< std::setw(10) << "Process"   << std::endl;	          
		}

		std::cout << std::setw(6) << fTrack->GetCurrentStepNumber() << " "
			<< std::setw(5) << G4BestUnit(fTrack->GetPosition().x(),"Length")<< " "
			<< std::setw(5) << G4BestUnit(fTrack->GetPosition().y(),"Length")<< " "
			<< std::setw(5) << G4BestUnit(fTrack->GetPosition().z(),"Length")<< " "
			<< std::setw(5) << G4BestUnit(fTrack->GetGlobalTime(),"Time")<< " "
			<< std::setw(5) << G4BestUnit(fTrack->GetMomentum().x(),"Energy")<< " "
			<< std::setw(5) << G4BestUnit(fTrack->GetMomentum().y(),"Energy")<< " "
			<< std::setw(5) << G4BestUnit(fTrack->GetMomentum().z(),"Energy")<< " "
			<< std::setw(5) << G4BestUnit(fTrack->GetKineticEnergy(),"Energy")<< " "
			<< std::setw(5) << G4BestUnit(fStep->GetTotalEnergyDeposit(),"Energy")<< " "
			<< std::setw(5) << G4BestUnit(fStep->GetTotalEnergyDeposit()-fStep->GetNonIonizingEnergyDeposit(),"Energy")<< " "
			<< std::setw(5) << G4BestUnit(fStep->GetDeltaEnergy(),"Energy")<< " "
			<< std::setw(6) << G4BestUnit(fStep->GetStepLength(),"Length")<< " "
			<< std::setw(6) << G4BestUnit(fTrack->GetTrackLength(),"Length")<< " "
			<< std::setw(6) << fTrack->GetVolume()->GetName();

		const G4VProcess* process 
			= fStep->GetPreStepPoint()->GetProcessDefinedStep();
		G4String procName = " UserLimit";
		if (process) procName = process->GetProcessName();
		if (fStepStatus == fWorldBoundary) procName = "OutOfWorld";
		std::cout << "   " << std::setw(10) << procName;
		std::cout << std::endl;

		if( verboseLevel == 2 ){
			G4int tN2ndariesTot = fN2ndariesAtRestDoIt +
				fN2ndariesAlongStepDoIt +
				fN2ndariesPostStepDoIt;
			if(tN2ndariesTot>0){
				std::cout << "\n    :----- List of secondaries ----------------"
					<< std::endl;
				std::cout.precision(4);
				for(size_t lp1=(*fSecondary).size()-tN2ndariesTot; 
						lp1<(*fSecondary).size(); lp1++){
					std::cout << "   "
						<< std::setw(13)		 
						<< (*fSecondary)[lp1]->GetDefinition()->GetParticleName()	  
						<< ":  energy ="
						<< std::setw(6)
						<< G4BestUnit((*fSecondary)[lp1]->GetKineticEnergy(),"Energy")	 
						<< "  time ="
						<< std::setw(6)
						<< G4BestUnit((*fSecondary)[lp1]->GetGlobalTime(),"Time");	 			 
					std::cout << std::endl;
				}

				std::cout << "    :------------------------------------------\n"
					<< std::endl;
			}
		}

	}
	std::cout.precision(prec);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingVerbose::TrackingStarted()
{

	CopyState();
	G4int prec = std::cout.precision(3);
	if( verboseLevel > 0 ){

		std::cout << std::setw( 6) << "#Step#" << " "
			<< std::setw( 9) << "X"         << " "
			<< std::setw( 9) << "Y"         << " "  
			<< std::setw( 9) << "Z"         << " "
			<< std::setw( 9) << "t"         << " "
			<< std::setw( 9) << "pX"        << " "
			<< std::setw( 9) << "pY"        << " "  
			<< std::setw( 9) << "pZ"        << " "
			<< std::setw( 9) << "KineE"     << " "
			<< std::setw( 9) << "dEtot"     << " "
			<< std::setw( 9) << "dEIon"     << " "
			<< std::setw( 9) << "dEdel"     << " "
			<< std::setw(10) << "StepLeng"  << " "
			<< std::setw(10) << "TrakLeng"  << " "
			<< std::setw(10) << "Volume"    << " "
			<< std::setw(10) << "Process"   << std::endl;	          

		std::cout << std::setw(6) << fTrack->GetCurrentStepNumber() << " "
			<< std::setw(5) << G4BestUnit(fTrack->GetPosition().x(),"Length")<< " "
			<< std::setw(5) << G4BestUnit(fTrack->GetPosition().y(),"Length")<< " "
			<< std::setw(5) << G4BestUnit(fTrack->GetPosition().z(),"Length")<< " "
			<< std::setw(5) << G4BestUnit(fTrack->GetGlobalTime(),"Time")<< " "
			<< std::setw(5) << G4BestUnit(fTrack->GetMomentum().x(),"Energy")<< " "
			<< std::setw(5) << G4BestUnit(fTrack->GetMomentum().y(),"Energy")<< " "
			<< std::setw(5) << G4BestUnit(fTrack->GetMomentum().z(),"Energy")<< " "
			<< std::setw(5) << G4BestUnit(fTrack->GetKineticEnergy(),"Energy")<< " "
			<< std::setw(5) << G4BestUnit(fStep->GetTotalEnergyDeposit(),"Energy")<< " "
			<< std::setw(5) << G4BestUnit(fStep->GetTotalEnergyDeposit()-fStep->GetNonIonizingEnergyDeposit(),"Energy")<< " "
			<< std::setw(5) << G4BestUnit(fStep->GetDeltaEnergy(),"Energy")<< " "
			<< std::setw(6) << G4BestUnit(fStep->GetStepLength(),"Length")<< " "
			<< std::setw(6) << G4BestUnit(fTrack->GetTrackLength(),"Length")<< " "
			<< std::setw(6) << fTrack->GetVolume()->GetName()<< " "
			<< std::setw(6) << "   initStep" << std::endl;	
	}
	std::cout.precision(prec);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
