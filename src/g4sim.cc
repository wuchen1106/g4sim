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
// $Id: g4sim.cc,v 1.39 2010-12-01 05:56:17 allison Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "Randomize.hh"

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "SteppingVerbose.hh"
#include "QGSP_BERT.hh"
#include "QGSP_BERT_HP.hh"
//#include "myQGSP_BERT_HP.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include "TFile.h"
#include "TTree.h"

#include "MyStackingAction.hh"
#include "MyTrackingAction.hh"
#include "MyRoot.hh"
#include "MyAnalysisSvc.hh"

#include <iostream>
#include <fstream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{

	MyRoot* myRoot = new MyRoot();

	// Choose the Random engine
	//
	CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);

	// User Verbose output class
	//
	G4VSteppingVerbose::SetInstance(new SteppingVerbose);

	// Construct the default run manager
	//
	G4RunManager * runManager = new G4RunManager;

	// Set mandatory initialization classes
	//
	runManager->SetUserInitialization(new DetectorConstruction);
	//

	G4String PhysicsListName = "QGSP_BERT";
	G4String file_name = getenv("GENFILEROOT");
	size_t sLast = file_name.last('/');
	if(file_name[0] != '/'){ // Relative Dir
		G4String dir_name = getenv("CONFIGUREROOT");
		if (dir_name[dir_name.size()-1] != '/') dir_name.append("/");
		file_name = dir_name + file_name;
	}
	std::ifstream fin_card(file_name);
	if ( !fin_card ){
		std::cout<<"generator file \""<<file_name<<"\" is not available!!!"<<std::endl;
		G4Exception("main","Run0031",
				FatalException, "generator file is not available.");
	}
	std::stringstream buf_card;
	std::string s_card;
	while(getline(fin_card,s_card)){
		buf_card.str("");
		buf_card.clear();
		buf_card<<s_card;
		const char* c = s_card.c_str();
		int length = strlen(c);
		int offset = 0;
		for ( ; offset < length; offset++ ){
			if ( c[offset] != ' ' ) break;
		}
		if ( c[offset] == '#' ) continue;
		else if ( c[offset] == '/' && c[offset+1] == '/' ) continue;
		else if ( length - offset == 0 ) continue;
		std::string keyword;
		buf_card>>keyword;
		if ( keyword == "PhysicsList:" ){
			buf_card>>PhysicsListName;
			break;
		}
		else{
			continue;
		}
	}
	fin_card.close();
	buf_card.str("");
	buf_card.clear();
	std::cout<<"PhysicsList: \""<<PhysicsListName<<"\""<<std::endl;
	if (PhysicsListName=="QGSP_BERT_HP"){
		runManager->SetUserInitialization(new QGSP_BERT_HP);
	}
//	else if (PhysicsListName=="myQGSP_BERT_HP"){
//		runManager->SetUserInitialization(new myQGSP_BERT_HP);
//	}
	else if (PhysicsListName=="PhysicsList"){
		runManager->SetUserInitialization(new PhysicsList);
	}
	else{
		runManager->SetUserInitialization(new QGSP_BERT);
	}

	// Set user action classes
	//
	runManager->SetUserAction(new PrimaryGeneratorAction);
	//
	runManager->SetUserAction(new RunAction());
	//
	runManager->SetUserAction(new EventAction);
	//
	runManager->SetUserAction(new SteppingAction);
	//
	runManager->SetUserAction(new MyTrackingAction);
	//
	runManager->SetUserAction(new MyStackingAction());

	// Initialize G4 kernel
	//
	runManager->Initialize();

	MyAnalysisSvc* myAnalysisSvc = new MyAnalysisSvc();

	// Get the pointer to the User Interface manager
	G4UImanager* UImanager = G4UImanager::GetUIpointer();

#ifdef G4VIS_USE
		// Initialize visualization
		// G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
		// G4VisManager* visManager = new G4VisExecutive("Quiet");
		G4VisManager* visManager = new G4VisExecutive;
#endif

	if (argc!=1)   // batch mode
	{
		G4String command = "/control/execute ";
		G4String fileName = argv[1];
		UImanager->ApplyCommand(command+fileName);
	}
	else
	{  // interactive mode : define UI session
#ifdef G4UI_USE
		G4UIExecutive* ui = new G4UIExecutive(argc, argv);
		G4String macros_dir = getenv("MACROSCOMMONROOT");
#ifdef G4VIS_USE
		// Initialize visualization
		visManager->Initialize();
		UImanager->ApplyCommand("/control/execute "+macros_dir+"/vis.mac"); 
#endif
		if (ui->IsGUI())
			UImanager->ApplyCommand("/control/execute "+macros_dir+"/gui.mac");
		ui->SessionStart();
		delete ui;
#endif
	}

	// Job termination
	// Free the store: user actions, physics_list and detector_description are
	//                 owned and deleted by the run manager, so they should not
	//                 be deleted in the main() program !
#ifdef G4VIS_USE
	delete visManager;
	std::cout<<"visManager deleted!!"<<std::endl;
#endif

	//  delete myRoot;
	std::cout<<"myRoot deleted!!"<<std::endl;
	//delete myAnalysisSvc;
	//std::cout<<"myAnalysisSvc deleted!!"<<std::endl;

	delete runManager;
	std::cout<<"runManager deleted!!"<<std::endl;

	return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
