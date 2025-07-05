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
#include "G4Version.hh"

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
#if G4VERSION_NUMBER >= 1000
#include "G4EmParameters.hh"
#endif
#include "G4OpticalPhysics.hh"
#include "MyStepLimiter.hh"
#include "QGSP_BERT.hh"
#include "FTFP_BERT.hh"
#include "FTF_BIC.hh"
#include "QGSP_BIC_HP.hh"
#include "QGSP_INCLXX.hh"
#include "QGSP_INCLXX_HP.hh"
#include "QGSP_BERT_HP.hh"
//#include "myQGSP_BERT_HP.hh"

#if defined(G4VIS_USE) || defined(G4VIS_USE_OPENGL) || defined(G4VIS_USE_OPENGLX) || defined(G4VIS_USE_OPENGLQT)
#define VIS_USE
#include "G4VisExecutive.hh"
#endif

#if defined(G4UI_USE) || defined(G4UI_USE_QT) || defined(G4UI_USE_TCSH)
#define UI_USE
#include "G4UIExecutive.hh"
#endif

#include "TFile.h"
#include "TTree.h"

#include "MyStackingAction.hh"
#include "MyTrackingAction.hh"
#include "MyRoot.hh"
#include "MyAnalysisSvc.hh"
#include "McTruthSvc.hh"

#include "MaterialDensityScanner.hh"

#include <iostream>
#include <fstream>
#include <sstream>  /* stringstream */
#include <unistd.h> /* getopt */
#include <stdlib.h> /* atoi, atof */

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void print_usage(char* prog_name);
#if G4VERSION_NUMBER < 1000
#include "G4PAIPhotonModel.hh"
#elif G4VERSION_NUMBER < 1040
#include "G4EmConfigurator.hh"
#include "G4LossTableManager.hh"
#include "G4PAIPhotModel.hh"
#include "G4PAIModel.hh"
#include "G4ParticleTable.hh"
void AddPAIModel(const G4String& modname);
void NewPAIModel(const G4ParticleDefinition* part, 
                              const G4String& modname,
                              const G4String& procname);
#endif

G4String PhysicsListName = "";
G4bool   WithPAI = false;
G4bool   WithRadi = false;
G4bool   WithOptical = false;
G4int    UseEmType = 0;
G4double LowEnergyCut = 990*eV;
G4String OutputFileName = "";

int main(int argc,char** argv)
{
    G4String HOME = getenv("MYG4SIMWORKROOT");
    G4String MacroName = "";
    G4String Hstar10Directory = getenv("MYG4SIMHSTAR10ROOT");
    WithPAI = false;
    WithRadi = false;
    PhysicsListName = "QGSP_BERT_HP";
    OutputFileName = HOME+"/output/raw_g4sim.root";
    int nEvents = 0;
    bool setEnergy = false;
    double fPrimaryEnergy = 0;
    int randomSeed = -1;
    int randomSeedIndex = -1;
    std::string scanOutput("");
    float mdScan_xmin = 0;
    float mdScan_xmax = 0;
    float mdScan_dx = 1;
    float mdScan_ymin = 0;
    float mdScan_ymax = 0;
    float mdScan_dy = 1;
    float mdScan_zmin = 0;
    float mdScan_zmax = 0;
    float mdScan_dz = 1;
    double mapEdep_step = 0;
    double mapEdep_xmin = 0;
    double mapEdep_xmax = 0;
    double mapEdep_ymin = 0;
    double mapEdep_ymax = 0;
    double mapEdep_zmin = 0;
    double mapEdep_zmax = 0;
    std::map<float,bool> mdScan_y4ZX;
    std::map<float,bool> mdScan_z4XY;
    std::map<float,bool> mdScan_x4ZY;
    // Load options
    int    opt_result;
    std::stringstream stream;
    while((opt_result=getopt(argc,argv,"e:E:L:N:oO:pP:rR:S:X:Y:Z:x:y:z:M:H:h"))!=-1){
        stream.clear();
        if(optarg) stream.str(optarg);
        switch(opt_result){
            case 'e':
                UseEmType = atoi(optarg);
                if (UseEmType!=0&&UseEmType!=3&&UseEmType!=4&&UseEmType!=-1&&UseEmType!=-2&&UseEmType!=-3){
                    std::cout<<" EmType "<<UseEmType<<" is not supported yet! Will use type 0 (EmStandard) as default"<<std::endl;
                    UseEmType = 0;
                }
                break;
            case 'E':
                setEnergy = true;
                fPrimaryEnergy = atof(optarg);
                break;
            case 'L':
                LowEnergyCut = atof(optarg)*eV;
                break;
            case 'N':
                nEvents = atoi(optarg);
                break;
            case 'o':
                WithOptical = true;
                break;
            case 'O':
                OutputFileName = optarg;
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
            case 'R':
                stream>>randomSeed>>randomSeedIndex;
                std::cerr<<"Random seed and index set to "<<randomSeed<<" "<<randomSeedIndex<<std::endl;
                break;
            case 'S':
                scanOutput = optarg;
                break;
            case 'X':
                stream>>mdScan_dx>>mdScan_xmin>>mdScan_xmax;
                printf("Set x axis : dx %.1f, xmin %.1f, xmax %.1f\n",mdScan_dx,mdScan_xmin,mdScan_xmax);
                break;
            case 'Y':
                stream>>mdScan_dy>>mdScan_ymin>>mdScan_ymax;
                printf("Set y axis : dy %.1f, ymin %.1f, ymax %.1f\n",mdScan_dy,mdScan_ymin,mdScan_ymax);
                break;
            case 'Z':
                stream>>mdScan_dz>>mdScan_zmin>>mdScan_zmax;
                printf("Set z axis : dz %.1f, zmin %.1f, zmax %.1f\n",mdScan_dz,mdScan_zmin,mdScan_zmax);
                break;
            case 'M':
                stream>>mapEdep_step>>mapEdep_xmin>>mapEdep_xmax>>mapEdep_ymin>>mapEdep_ymax>>mapEdep_zmin>>mapEdep_zmax;
                printf("Set mapEdep step size %.1f, x %.1f ~ %.1f, y %.1f ~ %.1f, z %.1f ~ %.1f\n",mapEdep_step,mapEdep_xmin,mapEdep_xmax,mapEdep_ymin,mapEdep_ymax,mapEdep_zmin,mapEdep_zmax);
                break;
            case 'H':
                stream>>Hstar10Directory;
                printf("Will load Hstar10 coefficients from this directory \"%s\"\n",Hstar10Directory.c_str());
                break;
            case 'y':
                float y;
                stream>>y;
                mdScan_y4ZX[y] = true;
                printf("Set y for z-x scan: y = %.1f\n",y);
                break;
            case 'z':
                float z;
                stream>>z;
                mdScan_z4XY[z] = true;
                printf("Set z for x-y scan: z = %.1f\n",z);
                break;
            case 'x':
                float x;
                stream>>x;
                mdScan_x4ZY[x] = true;
                printf("Set x for z-y scan: x = %.1f\n",x);
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
    std::cout<<"  with Radiatiacitve decay? "<<(WithRadi?"yes":"no")<<std::endl;
    std::cout<<"  with Optical Processes? "<<(WithOptical?"yes":"no")<<std::endl;
    std::cout<<"  use EmType "<<UseEmType<<" (ONLY VALID when the \"PhysicsList\" is chosen"<<std::endl;
    std::cout<<"Low energy cut = "<<LowEnergyCut/eV<<" eV"<<std::endl;
    std::cout<<"Run beamOn: "<<nEvents<<" events"<<std::endl;

    if (argc-optind>0){
        MacroName = argv[optind];
    }

    MyRoot* myRoot = new MyRoot();

    // Choose the Random engine
    //
    CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
    if (randomSeed!=-1){
        CLHEP::HepRandom::setTheSeed(randomSeed,randomSeedIndex);
    }

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
    else if (PhysicsListName=="QGSP_BERT"){
        physics = new QGSP_BERT;
    }
    else if (PhysicsListName=="QGSP_INCLXX"){
        physics = new QGSP_INCLXX;
    }
    else if (PhysicsListName=="QGSP_INCLXX_HP"){
        physics = new QGSP_INCLXX_HP;
    }
    else if (PhysicsListName=="FTFP_BERT"){
        physics = new FTFP_BERT;
    }
    else if (PhysicsListName=="FTF_BIC"){
        physics = new FTF_BIC;
    }
    else if (PhysicsListName=="QGSP_BIC_HP"){
        physics = new QGSP_BIC_HP;
    }
    else if (PhysicsListName=="PhysicsList"){
        physics = new PhysicsList(1,UseEmType,0);
    }
    else if (PhysicsListName=="PhysicsListHP"){
        physics = new PhysicsList(1,UseEmType,1);
    }
    else if (PhysicsListName=="PhysicsListINCLXX"){
        physics = new PhysicsList(1,UseEmType,2);
    }
    else if (PhysicsListName=="PhysicsListHPnone"){
        physics = new PhysicsList(1,UseEmType,-1);
    }
    else{
        std::cerr<<"Cannot recognize the PhysicsList name \""<<PhysicsListName<<"\""<<std::endl;
        return 1;
    }

    if (WithRadi){
        physics->RegisterPhysics(new G4RadioactiveDecayPhysics());
    }

    if (WithPAI){
#if G4VERSION_NUMBER < 1000
// to be added
#elif G4VERSION_NUMBER < 1040
        AddPAIModel("pai"); // can choose pai_photon as well
#else
        G4EmParameters::Instance()->AddPAIModel("all","world","pai");
#endif
    }

    // Optical processes
    if (WithOptical){
        G4OpticalPhysics * pG4OpticalPhysics = new G4OpticalPhysics();
#if G4VERSION_NUMBER < 1100 // FIXME: should find the solution for versions since G4.11.00
        pG4OpticalPhysics->SetMaxNumPhotonsPerStep(2000);
        pG4OpticalPhysics->SetMaxBetaChangePerStep(100);
        pG4OpticalPhysics->SetScintillationYieldFactor(1);
        pG4OpticalPhysics->SetTrackSecondariesFirst(kCerenkov,true);
#endif
        physics->RegisterPhysics(pG4OpticalPhysics);
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
    myAnalysisSvc->set_ofile_name(OutputFileName);
    std::cout<<"myAnalysisSvc->set_ofile_name \""<<OutputFileName<<"\""<<std::endl;

    // Set mandatory initialization classes
    //
    runManager->SetUserInitialization(new DetectorConstruction);
    //

    // Initialize G4 kernel
    //
    runManager->Initialize();

    // Perform a grid scan (50x50x1 points, at z=0 plane)
    if (scanOutput!=""){
        MaterialDensityScanner scanner;
        scanner.SetX(mdScan_dx,mdScan_xmin,mdScan_xmax);
        scanner.SetY(mdScan_dy,mdScan_ymin,mdScan_ymax);
        scanner.SetZ(mdScan_dz,mdScan_zmin,mdScan_zmax);
        for (std::map<float,bool>::const_iterator it = mdScan_y4ZX.begin(); it!=mdScan_y4ZX.end(); it++){
            if (it->second) scanner.AddY4ZX(it->first);
        }
        for (std::map<float,bool>::const_iterator it = mdScan_z4XY.begin(); it!=mdScan_z4XY.end(); it++){
            if (it->second) scanner.AddZ4XY(it->first);
        }
        for (std::map<float,bool>::const_iterator it = mdScan_x4ZY.begin(); it!=mdScan_x4ZY.end(); it++){
            if (it->second) scanner.AddX4ZY(it->first);
        }
        std::cout<<"GridScan:"<<std::endl;
        scanner.GridScan(scanOutput);
    }

    McTruthSvc::GetMcTruthSvc()->SetMapEdep(mapEdep_step,mapEdep_xmin,mapEdep_xmax,mapEdep_ymin,mapEdep_ymax,mapEdep_zmin,mapEdep_zmax);
    if (Hstar10Directory!="") McTruthSvc::GetMcTruthSvc()->SetHstar10ConverterDirectory(Hstar10Directory);

    // Get the pointer to the User Interface manager
    G4UImanager* UImanager = G4UImanager::GetUIpointer();

#ifdef VIS_USE
    // Initialize visualization
    // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
    // G4VisManager* visManager = new G4VisExecutive("Quiet");
    G4VisManager* visManager = new G4VisExecutive;
#endif

    if (MacroName!="")   // batch mode
    {
        G4String command = "/control/execute "+MacroName;
        UImanager->ApplyCommand(command);
        if (setEnergy){
            std::ostringstream command;
            command << "/g4sim/gun/defaultEnergy " << fPrimaryEnergy << " MeV";
            UImanager->ApplyCommand(command.str());
            command.str("");
            command.clear();
            command << "/g4sim/gun/initialize";
            UImanager->ApplyCommand(command.str());
        }
        if (nEvents>0){
            std::ostringstream command;
            command << "/run/beamOn " << nEvents;
            UImanager->ApplyCommand(command.str());
        }
    }
    else
    {  // interactive mode : define UI session
#ifdef UI_USE
        G4UIExecutive* ui = new G4UIExecutive(argc, argv);
        G4String macros_dir = getenv("MACROSCOMMONROOT");
#ifdef VIS_USE
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
#ifdef VIS_USE
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

#if G4VERSION_NUMBER < 1000
// to be added
#elif G4VERSION_NUMBER < 1040
void AddPAIModel(const G4String& modname)
{
  G4ParticleTable::G4PTblDicIterator* anotherParticleIterator = G4ParticleTable::GetParticleTable()->GetIterator();
  anotherParticleIterator->reset();
  while ((*anotherParticleIterator)())
  {
    G4ParticleDefinition* particle = anotherParticleIterator->value();
    G4String partname = particle->GetParticleName();
    if(partname == "e-" || partname == "e+") {
      NewPAIModel(particle, modname, "eIoni");

    } else if(partname == "mu-" || partname == "mu+") {
      NewPAIModel(particle, modname, "muIoni");

    } else if(partname == "proton" ||
              partname == "pi+" ||
              partname == "pi-"   
              ) {
      NewPAIModel(particle, modname, "hIoni");
    }
  }
}

void NewPAIModel(const G4ParticleDefinition* part, 
                              const G4String& modname,
                              const G4String& procname)
{
  G4EmConfigurator* config = G4LossTableManager::Instance()->EmConfigurator();
  G4String partname = part->GetParticleName();
  if(modname == "pai") {
    G4PAIModel* pai = new G4PAIModel(part,"PAIModel");
    config->SetExtraEmModel(partname,procname,pai,"GasDetector",
                            0.0,100.*TeV,pai);
  } else if(modname == "pai_photon") {
    G4PAIPhotModel* pai = new G4PAIPhotModel(part,"PAIPhotModel");
    config->SetExtraEmModel(partname,procname,pai,"GasDetector",
                            0.0,100.*TeV,pai);
  }
}

#endif

void print_usage(char * prog_name){
    fprintf(stderr,"Usage %s [options] [macro]\n",prog_name);
    fprintf(stderr,"\t\t In case [macro] is not given, UI mode will be activated\n");
    fprintf(stderr,"[options]\n");
    fprintf(stderr,"\t -e TYPE\n");
    fprintf(stderr,"\t\t Change the EmType. By default it's 0 (EmStandard). Only valid when the \"PhysicsList\" or \"PhysicsListHP\" or \"PhysicsListHPnone\" or \"PhysicsListINCLXX\" is chosen\n");
    fprintf(stderr,"\t\t Available types: -3, no EM at all, -1, EmLivermore; -2, EmCustomised; 0,3,4 EmStandard with the type number as option\n");
    fprintf(stderr,"\t -E energy\n");
    fprintf(stderr,"\t\t Change the default kinetic energy of the primary generator. Unit is MeV.\n");
    fprintf(stderr,"\t -L cut\n");
    fprintf(stderr,"\t\t Change the low energy bound of production cuts to [cut] eV (%.3e eV)\n",LowEnergyCut/eV);
    fprintf(stderr,"\t -N nEvents \n");
    fprintf(stderr,"\t\t Added /run/beamOn nEvents command after the execution of the given macro. Will not be invoked if macro file is not given though.\n",LowEnergyCut/eV);
    fprintf(stderr,"\t -P physics\n");
    fprintf(stderr,"\t\t Change the physics List (%s)\n",PhysicsListName.data());
    fprintf(stderr,"\t\t Available options: QGSP_BERT QGSP_BERT_HP QGSP_INCLXX FTFP_BERT FTF_BIC QGSP_BIC_HP PhysicsList PhysicsListHP\n");
    fprintf(stderr,"\t\t If the provided option is not supported, then QGSP_BERT_HP will be used as default\n");
    fprintf(stderr,"\t -o\n");
    fprintf(stderr,"\t\t Register G4OpticalPhysics\n");
    fprintf(stderr,"\t -p\n");
    fprintf(stderr,"\t\t Apply PAI to world\n");
    fprintf(stderr,"\t -r\n");
    fprintf(stderr,"\t\t Register G4RadioactiveDecayPhysics\n");
    fprintf(stderr,"\t -R \"randomSeed randomSeedIndex\"\n");
    fprintf(stderr,"\t\t Send random seed and index CLHEP::HepRandom by the start of the main function\n");
    fprintf(stderr,"\t -O file\n");
    fprintf(stderr,"\t\t Set output file name [%s]\n",OutputFileName.data());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
