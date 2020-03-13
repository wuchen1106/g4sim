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

#include "G4VModularPhysicsList.hh"
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "SteppingVerbose.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4EmParameters.hh"
#include "MyStepLimiter.hh"
#include "QGSP_BERT.hh"
#include "QGSP_INCLXX.hh"
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
#include <unistd.h> /* getopt */
#include <stdlib.h> /* atoi, atof */

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void print_usage(char* prog_name);
G4String PhysicsListName = "";
G4bool   WithPAI = false;
G4bool   WithRadi = false;
G4double LowEnergyCut = 990*eV;

int main(int argc,char** argv)
{
    G4String MacroName = "";
    WithPAI = false;
    WithRadi = false;
    PhysicsListName = "QGSP_BERT_HP";
    // Load options
    int    opt_result;
    while((opt_result=getopt(argc,argv,"L:pP:rh"))!=-1){
        switch(opt_result){
            case 'L':
                LowEnergyCut = atof(optarg)*eV;
                break;
            case 'p':
                WithPAI = true;
                break;
            case 'P':
                PhysicsListName = optarg;
                break;
            case 'r':
                WithRadi = true;
                break;
            case '?':
                printf("Wrong option! optopt=%c, optarg=%s\n", optopt, optarg);
            case 'h':
            default:
                print_usage(argv[0]);
                return 1;
        }
    }
    std::cout<<"PhysicsList: \""<<PhysicsListName<<"\""<<std::endl;
    std::cout<<"  with PAI? "<<(WithPAI?"yes":"no")<<std::endl;
    std::cout<<"  with Radiation? "<<(WithRadi?"yes":"no")<<std::endl;
    std::cout<<"Low energy cut = "<<LowEnergyCut/eV<<" eV"<<std::endl;

    if (argc-optind>0){
        MacroName = argv[optind];
    }

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

    // get physicslist
    G4VModularPhysicsList* physics;
    if (PhysicsListName=="QGSP_BERT_HP"){
        physics= new QGSP_BERT_HP;
    }
    else if (PhysicsListName=="QGSP_INCLXX"){
        physics = new QGSP_INCLXX;
    }
    else if (PhysicsListName=="PhysicsList"){
        physics = new PhysicsList;
    }
    else{
        physics = new QGSP_BERT;
    }

    if (WithRadi){
        physics->RegisterPhysics(new G4RadioactiveDecayPhysics());
    }

    if (WithPAI){
        G4EmParameters::Instance()->AddPAIModel("all","world","pai");
    }

    physics->RegisterPhysics(new MyStepLimiter());

    G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(LowEnergyCut,10000*GeV);

    runManager->SetUserInitialization(physics);

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

    // Get analysis service
    MyAnalysisSvc* myAnalysisSvc = new MyAnalysisSvc();

    // Set mandatory initialization classes
    //
    runManager->SetUserInitialization(new DetectorConstruction);
    //

    // Initialize G4 kernel
    //
    runManager->Initialize();

    // Get the pointer to the User Interface manager
    G4UImanager* UImanager = G4UImanager::GetUIpointer();

#ifdef G4VIS_USE
    // Initialize visualization
    // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
    // G4VisManager* visManager = new G4VisExecutive("Quiet");
    G4VisManager* visManager = new G4VisExecutive;
#endif

    if (MacroName!="")   // batch mode
    {
        G4String command = "/control/execute "+MacroName;
        UImanager->ApplyCommand(command);
    }
    else
    {  // interactive mode : define UI session
#ifdef G4UI_USE
        G4UIExecutive* ui = new G4UIExecutive(argc, argv);
        G4String macros_dir = getenv("MACROSCOMMONROOT");
#ifdef G4VIS_USE
        // Initialize visualization
        visManager->Initialize();
        UImanager->ApplyCommand("/control/execute "+macros_dir+"/vis.macro"); 
#endif
        if (ui->IsGUI())
            UImanager->ApplyCommand("/control/execute "+macros_dir+"/gui.macro");
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

void print_usage(char * prog_name){
    fprintf(stderr,"Usage %s [options] [macro]\n",prog_name);
    fprintf(stderr,"\t\t In case [macro] is not given, UI mode will be activated\n");
    fprintf(stderr,"[options]\n");
    fprintf(stderr,"\t -L cut\n");
    fprintf(stderr,"\t\t Change the low energy bound of production cuts to [cut] eV (%.3e eV)\n",LowEnergyCut/eV);
    fprintf(stderr,"\t -P physics\n");
    fprintf(stderr,"\t\t Change the physics List (%s)\n",PhysicsListName.data());
    fprintf(stderr,"\t\t Available options: QGSP_BERT QGSP_BERT_HP QGSP_INCLXX PhysicsList\n");
    fprintf(stderr,"\t\t If the provided option is not supported, then QGSP_BERT_HP will be used as default\n");
    fprintf(stderr,"\t -p\n");
    fprintf(stderr,"\t\t Apply PAI to world\n");
    fprintf(stderr,"\t -r\n");
    fprintf(stderr,"\t\t Register G4RadioactiveDecayPhysics\n");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
