//---------------------------------------------------------------------------//
//Description: Setup PrimaryGenerator
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "PrimaryGeneratorAction.hh"

#include "PrimaryGeneratorMessenger.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleMomentum.hh"
#include "G4TransportationManager.hh"
#include "Randomize.hh"

//supported geometry
#include "MyDetectorManager.hh"
#include "MyVGeometrySvc.hh"
#include "MyVGeometryParameter.hh"
#include "SimpleGeometryParameter.hh"

#include "EventHeaderSvc.hh"

#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TFitResult.h"

#include "CLHEP/Vector/EulerAngles.h"

#include <stdlib.h>
#include <iostream>
#include <fstream>

PrimaryGeneratorAction* PrimaryGeneratorAction::fPrimaryGeneratorAction = 0;

PrimaryGeneratorAction::PrimaryGeneratorAction()
	: MyConfigure(),
	root_index(0),
	m_TChain(0),
	particleGun(0),	 //pointer a to G4  class
	gunMessenger(0),   //messenger of this class
	fParticle(0),
	fp(0)
{
	if (fPrimaryGeneratorAction){
		G4Exception("PrimaryGeneratorAction::PrimaryGeneratorAction()","Run0031",
				FatalException, "PrimaryGeneratorAction constructed twice.");
	}
	fPrimaryGeneratorAction = this;
	//create a messenger for this class
	gunMessenger = new PrimaryGeneratorMessenger(this);

	// default particle kinematic
	G4String file_name = getenv("GENFILEROOT");
	ReadCard(file_name);
	Initialize();

	// Initialise all the TURTLE/data fits
	fXPositionFinalFocusFit = NULL;
	fYPositionFinalFocusFit = NULL;

	fXPositionFinalFocus_Lower = -1.5;
	fXPositionFinalFocus_Upper = 1.5;

	fYPositionFinalFocus_Lower = -1.5;
	fYPositionFinalFocus_Upper = 1.5;

	fMuPCBeamDistHist = NULL;
	
	// Set the "collimated" input hists to NULL
	fCollimatedInputHist_XYPz = NULL;
	fCollimatedInputHist_XPxPz = NULL;
	fCollimatedInputHist_YPyPz = NULL;

	//	fMuPCBeamDistRandom = new TH2F("muPC_random", "muPC_random", 100,-24,24, 100,-24,24);
	//	fFFBeamDistRandom = new TH2F("FF_random", "FF_random", 100,-24,24, 100,-24,24);;
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  /*  TFile* output = new TFile("test.root", "RECREATE");
  fMuPCBeamDistRandom->Write();
  fFFBeamDistRandom->Write();
  output->Close();
  */
	delete particleGun;
	delete gunMessenger;
	if(EM_hist) delete EM_hist;
	if(DM_hist) delete DM_hist;
	if(PM_hist) delete PM_hist;
	if(m_TChain) delete m_TChain;
}

PrimaryGeneratorAction* PrimaryGeneratorAction::GetPrimaryGeneratorAction(){
	if ( !fPrimaryGeneratorAction ){
		new PrimaryGeneratorAction();
	}
	return fPrimaryGeneratorAction;
}

void* PrimaryGeneratorAction::get_extra(G4String name){
	if (name=="weight") {if(!flag_weight)  return &root_double[9];}
	else if (name=="ox") {if(!flag_ox)  return &root_double[10];}
	else if (name=="oy") {if(!flag_oy)  return &root_double[11];}
	else if (name=="oz") {if(!flag_oz)  return &root_double[12];}
	else if (name=="opx") {if(!flag_opx)  return &root_double[13];}
	else if (name=="opy") {if(!flag_opy)  return &root_double[14];}
	else if (name=="opz") {if(!flag_opz)  return &root_double[15];}
	else if (name=="ot") {if(!flag_ot)  return &root_double[16];}
	else if (name=="ppid") {if(!flag_ppid)  return &root_int[1];}
	else if (name=="ptid") {if(!flag_ptid)  return &root_int[2];}
	else if (name=="process") {if(!flag_process)  return root_str[0];}
	else if (name=="volume") {if(!flag_volume)  return root_str[1];}
//	else if (name=="R0") {if(flag_R0)  return &root_double[7];}
//	else if (name=="R1") {if(flag_R1)  return &root_double[8];}
	else if (name=="R0") {if(!flag_R0)  return &root_int[3];}
	else if (name=="R1") {if(!flag_R1)  return &root_int[4];}
	return NULL;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	//this function is called at the begining of event
	// 
	if (UseRoot){
		root_get_para();
	}
	if (RandMode=="root"){
		long seeds[3];
//		seeds[0] = root_double[7];
//		seeds[1] = root_double[8];
		seeds[0] = root_int[3];
		seeds[1] = root_int[4];
		seeds[2] = 0;
//		std::cout<<"setTheSeeds("<<(int)seeds[0]<<","<<(int)seeds[1]<<")"<<std::endl;
		CLHEP::HepRandom::setTheSeeds(seeds);
	}

	// Show Status:
//	std::cout<<"==>Event "<<root_index<<std::endl;
//    CLHEP::HepRandom::showEngineStatus();

	if (fType=="ion"){
		if (!fParticle){
			G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
			fParticle = particleTable->GetIon(Z,A,E*keV);
			if (!fParticle){
				std::cout<<"ERROR: In PrimaryGeneratorAction::PrimaryGeneratorAction() Cannot find particle "
						 <<"Z = "<<Z
						 <<", A = "<<A
						 <<", E = "<<E<<" keV"
						 <<"!!!"<<std::endl;
				G4Exception("PrimaryGeneratorAction::PrimaryGeneratorAction()","Run0031",
						FatalException, "Cannot find particle.");
			}
		}
		if (fType == "stable"){
			fParticle->SetPDGStable(true);
		}
		particleGun->SetParticleDefinition(fParticle);
		mass = particleGun->GetParticleDefinition()->GetPDGMass();
		particleGun->SetParticleCharge(C*eplus);
	}

	if ( pidMode == "root"){
		G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
		G4ParticleDefinition* particle = particleTable->FindParticle(root_int[0]);
		if (!particle){
			std::cout<<"ERROR: In PrimaryGeneratorAction::PrimaryGeneratorAction() Cannot find particle "<<root_int[0]<<"!!! Will use geantino"<<std::endl;
//			G4Exception("PrimaryGeneratorAction::PrimaryGeneratorAction()","Run0031",
//					FatalException, "Cannot find particle.");
			particle = particleTable->FindParticle("geantino");
		}
		if (fType == "stable"){
			particle->SetPDGStable(true);
		}
		particleGun->SetParticleDefinition(particle);
		mass = particleGun->GetParticleDefinition()->GetPDGMass();
	}

	if ( EnergyMode == "histo"){
		G4double ekin = EM_hist->GetRandom() / 1e3 * MeV;
		//		G4double mom = sqrt(ekin*ekin + 2*ekin*mass);
		particleGun->SetParticleEnergy(ekin);
	}
	else if ( EnergyMode == "root" ){
		//std::cout<<"PGA EM = root!"
		//	     <<", ("<<root_double[3]
		//	     <<","<<root_double[4]
		//	     <<","<<root_double[5]
		//	     <<") MeV"
		//	     <<std::endl;
		G4double ekin = sqrt(root_double[3]*root_double[3]*MeV*MeV+root_double[4]*root_double[4]*MeV*MeV+root_double[5]*root_double[5]*MeV*MeV+mass*mass)-mass;
		particleGun->SetParticleMomentumDirection(G4ThreeVector(root_double[3] * MeV, root_double[4] * MeV, root_double[5] * MeV).unit());
		particleGun->SetParticleEnergy(ekin);
	}
	else if ( EnergyMode == "gRand" || EnergyMode == "uRand" ){
		SetRandomEnergy();
	}
	else if ( EnergyMode == "collimated") {
	  if (!fCollimatedInputHist_XYPz && !fCollimatedInputHist_XPxPz && !fCollimatedInputHist_YPyPz) {
	    TDirectory* prev_dir = gDirectory;
	    // Get the relevant functions/histograms
	    std::string dir_name = getenv("CONFIGUREDATAROOT");
	    dir_name += "CollimatedInput.root";
	    TFile* collimated_file = new TFile(dir_name.c_str(), "READ");
	    
	    fCollimatedInputHist_XYPz = (TH3F*) (collimated_file->Get("hXYPz"))->Clone();
	    fCollimatedInputHist_XYPz->SetDirectory(0);

	    fCollimatedInputHist_XPxPz = (TH3F*) (collimated_file->Get("hXPxPz"))->Clone();
	    fCollimatedInputHist_XPxPz->SetDirectory(0);

	    fCollimatedInputHist_YPyPz = (TH3F*) (collimated_file->Get("hYPyPz"))->Clone();
	    fCollimatedInputHist_YPyPz->SetDirectory(0);

	    collimated_file->Close();
	    gDirectory = prev_dir; // need to go back to where we were so that we can get the tree written to the output file
	  }

	  // The idea here is to make sure the correlations are maintained
	  // The plan is:
	  //  1. Generate a random set of XYPz
	  //  2. Select a random x position based on the values of Px and Pz
	  //  3. Select a random y position based on the values of Py and Pz
	  //  4. Track this (x, y) pair back to make sure that it passed the ellipse
	  double px, py, pz; // the variables we want to get
	  double x, y, z;
	  z = -7.06*cm; // already set by location of ColMon

	  TH2F* hXPx = NULL; // we will use these to store the projections of the various plots
	  TH1D* hPx = NULL;
	  TH2F* hYPy = NULL;
	  TH1D* hPy = NULL;

	  double monitor_location = 2*cm; // the monitor we read out from to get collimated information was 20mm in front of the collimator
	  double hole_ellipse_half_x = 1.6*cm; // the two radii of the hole in the collimator
	  double hole_ellipse_half_y = 2.5*cm;

	  bool found = false;
	  while (!found) {
	    fCollimatedInputHist_XYPz->GetRandom3(x, y, pz);
	    x *= cm; y *= cm; pz *= MeV;
	    //	    std::cout << "Drawn (x, y, pz): (" << x/cm << " cm, " << y/cm << " cm, " << pz/MeV << " MeV)" << std::endl;

	    // First get the slice in the X-Z plane
	    int bin = fCollimatedInputHist_XPxPz->GetZaxis()->FindBin(pz);
	    fCollimatedInputHist_XPxPz->GetZaxis()->SetRange(bin, bin);
	    hXPx = (TH2F*) fCollimatedInputHist_XPxPz->Project3D("xy");

	    if (hXPx->GetEntries() == 0) {
	      continue;
	    }

	    // Now get the slice in the y plane so that we can randomly select a value for x
	    bin = hXPx->GetYaxis()->FindBin(x);
	    hXPx->GetYaxis()->SetRange(bin, bin);
	    hPx = hXPx->ProjectionX();

	    if (hPx->GetEntries() == 0) {
	      continue;
	    }

	    // Randomly select a value for x
	    px = hPx->GetRandom()*MeV;

	    // First get the slice in the Y-Z plane
	    bin = fCollimatedInputHist_YPyPz->GetZaxis()->FindBin(pz);
	    fCollimatedInputHist_YPyPz->GetZaxis()->SetRange(bin, bin);
	    hYPy = (TH2F*) fCollimatedInputHist_YPyPz->Project3D("xy");

	    if (hYPy->GetEntries() == 0) {
	      continue;
	    }


	    // Now get the slice in the y plane so that we can randomly select a value for y
	    bin = hYPy->GetYaxis()->FindBin(y);
	    hYPy->GetYaxis()->SetRange(bin, bin);
	    hPy = hYPy->ProjectionX();
	    
	    if (hPy->GetEntries() == 0) {
	      continue;
	    }

	    // Randomly select a value for y
	    py = hPy->GetRandom()*MeV;
	    //	    std::cout << "Drawn (px, py) = (" << px/MeV << " MeV, " << py/MeV << " MeV)" << std::endl;

	    // Track back to make sure we did pass ellipse
	    double p_total = std::sqrt(px*px + py*py + pz*pz);

	    double unit_px = px / p_total;
	    double trackback_x = x - unit_px*monitor_location;
	    double unit_py = py / p_total;
	    double trackback_y = y - unit_py*monitor_location;

	    double ellipse = (trackback_x*trackback_x)/(hole_ellipse_half_x*hole_ellipse_half_x) + (trackback_y*trackback_y)/(hole_ellipse_half_y*hole_ellipse_half_y);
	    if (ellipse <= 1) {
	      //	      std::cout << ellipse << " OK" << std::endl;
	      found = true;
	    }
	  }
	  G4ParticleMomentum particleMomentum(px, py, pz);

	  G4double mom = particleMomentum.mag();
	  G4double mass = particleGun->GetParticleDefinition()->GetPDGMass();
	  G4double ekin = sqrt(mom*mom+mass*mass)-mass;
	  particleGun->SetParticleEnergy(ekin);
	  particleGun->SetParticleMomentumDirection(particleMomentum.unit());

	  G4ThreeVector start_pos(x, y, z);
	  particleGun->SetParticlePosition(start_pos);

	  //	  std::cout << particleMomentum/MeV << " MeV, " << mom/MeV << " MeV, " << mass << ", " << ekin << ", " << start_pos/cm << " cm" << std::endl;
	}
	else if ( EnergyMode != "none" ){
		std::cout<<"ERROR: unknown EnergyMode: "<<EnergyMode<<"!!!"<<std::endl;
		G4Exception("PrimaryGeneratorAction::GeneratePrimaries()",
				"InvalidSetup", FatalException,
				"unknown EnergyMode");
	}

	if ( DirectionMode == "uniform" ){
		SetUniformDirection();
	}
	else if ( DirectionMode == "histo" ){
		G4double theta = DM_hist->GetRandom() * rad;
		G4double phi = G4UniformRand() * 360. *deg;
		G4ThreeVector dir_3Vec(1, 1, 1);
		dir_3Vec.setTheta(theta);
		dir_3Vec.setPhi(phi);
		particleGun->SetParticleMomentumDirection(dir_3Vec);
	}
	else if ( DirectionMode == "turtle" || PositionMode == "turtle") {

	  // Plan
	  // -- Get an X-Y position at muPC
	  // -- Get an X-Y position at final focus 
	  // -- Get the direction vector between the two
	  // -- Track the direction forward to a start point

	  if (!fXPositionFinalFocusFit && !fYPositionFinalFocusFit && !fMuPCBeamDistHist) {
	    TDirectory* prev_dir = gDirectory;
	    // Get the relevant functions/histograms
	    std::string dir_name = getenv("CONFIGUREDATAROOT");
	    dir_name += "TURTLE_fits.root";
	    TFile* turtle_file = new TFile(dir_name.c_str(), "READ");
	    
	    // Get the functions that describe the x and y positions of the beam at the final focus
	    fXPositionFinalFocusFit = (TF1*) turtle_file->Get("final_focus_horizontal");
	    fYPositionFinalFocusFit = (TF1*) turtle_file->Get("final_focus_vertical");

	    // Get the histogram of the beam distribution at the muPC
	    fMuPCBeamDistHist = (TH2F*) turtle_file->Get("hmuPC_XYWires")->Clone(); // need to clone because the file will be closing soon
	    fMuPCBeamDistHist->SetDirectory(0); // need to set directory to 0 so that we can use this histogram after the file is closed
	    turtle_file->Close();
	    gDirectory = prev_dir; // need to go back to where we were so that we can get the tree written to the output file
	  }

	  double x_muPC = 0;
	  double y_muPC = 0;
	  fMuPCBeamDistHist->GetRandom2(x_muPC, y_muPC);
	  //	  fMuPCBeamDistRandom->Fill(x_muPC, y_muPC);
	  double z_muPC = 52.5; // approx 10cm downstream of the end of the beampipe (from previous g4sim geometry)
	  
	  double x_ff = fXPositionFinalFocusFit->GetRandom()*10; // convert from cm to mm
	  double y_ff = fYPositionFinalFocusFit->GetRandom()*10; // convert from cm to mm
	  //	  fFFBeamDistRandom->Fill(x_ff, y_ff);
	  double z_ff = 120; // 12cm downstream of the beam pipe (according to MuSun report)
	  
	  // Translate to having the target at the origin
	  double z_pos_beam_pipe = -285.58 - 60; // relative to target
	  double translation = z_pos_beam_pipe; // from g4sim geometry
	  //	  std::cout << "translation = " << translation << std::endl;
	  z_muPC += translation; // from the g4sim geometry
	  z_ff += translation; 
	  
	  G4ThreeVector muPCPos(x_muPC*mm, y_muPC*mm, z_muPC*mm);
	  G4ThreeVector ffPos(x_ff*mm, y_ff*mm, z_ff*mm);
	  //	  std::cout << "muPC: (" << muPCPos.x()/mm << ", " << muPCPos.y()/mm << ", " << muPCPos.z()/mm << ") mm" << std::endl;	
	  //	  std::cout << "FF: (" << ffPos.x()/mm << ", " << ffPos.y()/mm << ", " << ffPos.z()/mm << ") mm" << std::endl;
	  
	  G4ThreeVector direction = (ffPos - muPCPos).unit();
	  //	  std::cout << "dir: (" << direction.x()/mm << ", " << direction.y()/mm << ", " << direction.z()/mm << ") mm" << std::endl;	
	  
	  // Track back to exit of beam pipe (z = -304*mm)
	  double n_steps = (z_pos_beam_pipe - muPCPos.z()/mm) / (direction.z()/mm);
	  //	  std::cout << "n_steps to start of beam pipe: " << n_steps << std::endl;
	  G4ThreeVector start_pos = muPCPos + n_steps*direction;
	  //	  std::cout << "start: (" << start_pos.x()/mm << ", " << start_pos.y()/mm << ", " << start_pos.z()/mm << ") mm" << std::endl;

	  particleGun->SetParticlePosition(start_pos);
	  particleGun->SetParticleMomentumDirection(direction);
	}
	else if ( DirectionMode == "muPC" || PositionMode == "muPC" || PositionMode == "collimator" || DirectionMode == "collimator") {
	  if (!fMuPCBeamDistHist) {
	    TDirectory* prev_dir = gDirectory;
	    // Get the relevant functions/histograms
	    std::string dir_name = getenv("CONFIGUREDATAROOT");
	    dir_name += "mupc_profile_run2808_Al100.root";
	    TFile* mupc_profile_file = new TFile(dir_name.c_str(), "READ");
	    
	    // Get the histogram of the beam distribution at the muPC
	    fMuPCBeamDistHist = (TH2F*) mupc_profile_file->Get("muPC/hmuPC_XYWires")->Clone(); // need to clone because the file will be closing soon
	    fMuPCBeamDistHist->SetDirectory(0); // need to set directory to 0 so that we can use this histogram after the file is closed
	    mupc_profile_file->Close();
	    gDirectory = prev_dir; // need to go back to where we were so that we can get the tree written to the output file
	  }

	  bool found = false;
	  
	  while (!found) {

	    if (DirectionMode == "muPC" || PositionMode == "muPC") {
	      found = true; // we take anything if we want to start from the beginning
	    }

	    double x, y, z;
	    z = -(285.58+7.5)*mm;
	    fMuPCBeamDistHist->GetRandom2(x, y);
	    x *= mm; y *= mm;
	    G4ThreeVector muPCPos(x, y, z);
	    //	  std::cout << "(x, y, z) = (" << x << ", " << y << ", " << z << ")" << std::endl;


	    double dMom=G4RandGauss::shoot(0,MomSpread);
	    double pz = Pa + dMom;
	    //	  std::cout << "Pa = " << Pa << ", dMom = " << dMom << std::endl;
	    double px = G4RandGauss::shoot(0, 0.052*pz);
	    double py = G4RandGauss::shoot(0, 0.054*pz);
	    double p_tot = std::sqrt(px*px + py*py + pz*pz);
	    //	    std::cout << "Before: (px, py, pz) = (" << px << ", " << py << ", " << pz << ")" << std::endl;
	    //	    std::cout << "p_tot = " << p_tot << std::endl;
	    
	    // Rescale components so that we get the correct momentum again
	    double scale_factor = pz / p_tot;
	    px *= scale_factor;
	    py *= scale_factor;
	    pz *= scale_factor;
	    p_tot = std::sqrt(px*px + py*py + pz*pz);
	    //	    std::cout << "After: (px, py, pz) = (" << px << ", " << py << ", " << pz << ")" << std::endl;
	    //	    std::cout << "p_tot = " << p_tot << std::endl;
	    G4ThreeVector particleMomentum(px, py, pz);
	    G4ThreeVector direction = particleMomentum.unit();

	    G4double mom = particleMomentum.mag();
	    G4double mass = particleGun->GetParticleDefinition()->GetPDGMass();
	    G4double ekin = sqrt(mom*mom+mass*mass)-mass;
	    particleGun->SetParticleEnergy(ekin);
	    particleGun->SetParticleMomentumDirection(direction);
	    	    
	    if (DirectionMode == "collimator" || PositionMode == "collimator") {
	      double z_pos_collimator = -90.5;
	      double n_steps = (z_pos_collimator - muPCPos.z()/mm) / (direction.z()/mm);
	      G4ThreeVector start_pos = muPCPos + n_steps*direction;

	      double hole_ellipse_half_x = 16*mm; // the two radii of the hole in the collimator
	      double hole_ellipse_half_y = 25*mm;
	      double ellipse = (start_pos.x()*start_pos.x())/(hole_ellipse_half_x*hole_ellipse_half_x) + (start_pos.y()*start_pos.y())/(hole_ellipse_half_y*hole_ellipse_half_y);
	      if ( ellipse < 1) {
		particleGun->SetParticlePosition(start_pos);

		if (!fEnergyLoss) {
		  fEnergyLoss = new TF1("energy_loss", "[0]*TMath::Landau(x, [1], [2]) + [3]*TMath::Exp([4]*x^[5] + [6]) + [7]*TMath::Gaus(x, [8], [9])", 0, 0.03);
		  // Set the parameters (hard-coded from the fit I did separately (2014-11-04)
		  fEnergyLoss->SetParameter(0, 14051.2);
		  fEnergyLoss->SetParameter(1, 0.0141187);
		  fEnergyLoss->SetParameter(2, 0.000738656);
		  fEnergyLoss->SetParameter(3, -0.0127883);
		  fEnergyLoss->SetParameter(4, 0.0599257);
		  fEnergyLoss->SetParameter(5, 11.5222);
		  fEnergyLoss->SetParameter(6, -1.30946);
		  fEnergyLoss->SetParameter(7, 2510.06);
		  fEnergyLoss->SetParameter(8, 0.0155356);
		  fEnergyLoss->SetParameter(9, 0.00170543);
		}
		// Add some energy loss
		double e_loss = fEnergyLoss->GetRandom()*GeV;
		G4double new_mom = mom - e_loss;
		mass = particleGun->GetParticleDefinition()->GetPDGMass();
		ekin = sqrt(new_mom*new_mom+mass*mass)-mass;
		particleGun->SetParticleEnergy(ekin);
		//		std::cout << "Old Mom: " << mom << ", e_loss: " << e_loss << ", new_mom: " << new_mom << std::endl;
		found = true;
	      }
	    }
	    if (DirectionMode == "muPC" || PositionMode == "muPC") {
	      // Track forward to the FF
	      double z_pos_beam_pipe = -285.58 + 60;
	      double n_steps = (z_pos_beam_pipe - muPCPos.z()/mm) / (direction.z()/mm);
	      //	  std::cout << "n_steps to start of beam pipe: " << n_steps << std::endl;
	      G4ThreeVector start_pos = muPCPos + n_steps*direction;
	      //	      std::cout << "AE: PosSpread: " << xSpread << ", " << ySpread << ", " << zSpread << std::endl;
	      G4ThreeVector move(xSpread, ySpread, zSpread); // to translate the whole beam
	      start_pos += move;
	      particleGun->SetParticlePosition(start_pos);
	    }
	  }
	}
	else if ( DirectionMode != "none" ){
		std::cout<<"ERROR: unknown DirectionMode: "<<DirectionMode<<"!!!"<<std::endl;
		G4Exception("PrimaryGeneratorAction::GeneratePrimaries()",
				"InvalidSetup", FatalException,
				"unknown DirectionMode");
	}


	if ( PhiMode== "gRand" || PhiMode=="uRand" || ThetaMode== "gRand" || ThetaMode=="uRand" ){
		SetRandomDirection();
	}

	if ( PositionMode == "uniform" ){
		SetUniformPosition();
	}
	else if ( PositionMode == "root" ){
		//std::cout<<"PGA PM = root!"
		//	     <<", ("<<root_double[0]
		//	     <<","<<root_double[1]
		//	     <<","<<root_double[2]
		//	     <<") mm"
		//	     <<std::endl;
		particleGun->SetParticlePosition(G4ThreeVector(root_double[0] * mm, root_double[1] * mm, (root_double[2])*mm));
	}
	else if ( PositionMode == "gRand" || PositionMode == "sRand" || PositionMode == "bRand" ){
		SetRandomPosition();
	}
	else if ( PositionMode == "target") {
	  SetRandomPosition();  
	}
	else if ( PositionMode == "source") {
	  SetRandomPosition();  
	}
	else if ( PositionMode == "histo") {
	  if (PM_hist) { // if we have a TH3F
		double x=0,y=0,z=0;
		PM_hist->GetRandom3(x,y,z);
		G4ThreeVector pos_3Vec(x, y, z);
		particleGun->SetParticlePosition(pos_3Vec);
	  }
	  else if (PM_hist_sparse) {
	    double random_pos[3];
	    PM_hist_sparse->GetRandom(random_pos);
	    G4ThreeVector pos_3Vec(random_pos[0], random_pos[1], random_pos[2]);
	    particleGun->SetParticlePosition(pos_3Vec); // should probably try and avoid having same line twice
	  }
	}
	else if ( PositionMode == "turtle" || PositionMode == "muPC" || PositionMode == "collimator") {
	  // Already handled in the DirectionMode if block
	}
	else if ( PositionMode != "none" ){
		std::cout<<"ERROR: unknown PositionMode: "<<PositionMode<<"!!!"<<std::endl;
		G4Exception("PrimaryGeneratorAction::GeneratePrimaries()",
				"InvalidSetup", FatalException,
				"unknown PositionMode");
	}

	if ( TimeMode == "root" ){
		//std::cout<<"PGA TM = root!"
		//	     <<", ("<<root_double[6]
		//	     <<") ns"
		//	     <<std::endl;
		particleGun->SetParticleTime(root_double[6]*ns);
	}
	else if ( TimeMode != "none" ){
		std::cout<<"ERROR: unknown TimeMode: "<<TimeMode<<"!!!"<<std::endl;
		G4Exception("PrimaryGeneratorAction::GeneratePrimaries()",
				"InvalidSetup", FatalException,
				"unknown TimeMode");
	}

	//	std::cout << "Particle Momentum: " << particleGun->GetParticleEnergy() << std::endl << std::endl;
	particleGun->GeneratePrimaryVertex(anEvent);

//	std::cout.precision(17);
//	std::cout<<"\tDirection: "<<particleGun->GetParticleMomentumDirection()<<std::endl;
//	std::cout<<"\tEnergy: "<<particleGun->GetParticleEnergy()/MeV<<" MeV"<<std::endl;
//	std::cout.precision(3);
//    CLHEP::HepRandom::showEngineStatus();

        /// Inform the event header of the primary particle so we store info in the output root tree
        InformEventHeaderHeader();
	if (!UseRoot) root_index++;
}

void PrimaryGeneratorAction::InformEventHeaderHeader(){
        // Set Random number seeds in the event header (R0 and R1) taken from elsewhere
	EventHeaderSvc::GetEventHeaderSvc()->SetSeedsValue();

        // Set the primary particle's momentum in the event header 
	G4ParticleMomentum mom=particleGun->GetParticleMomentumDirection();
        double E_kinetic=particleGun->GetParticleEnergy();
        double M=particleGun->GetParticleDefinition()->GetPDGMass();
        double P=sqrt(E_kinetic*E_kinetic + 2*M*E_kinetic);
        mom=P*mom;
	EventHeaderSvc::GetEventHeaderSvc()->SetInitialMomentum(mom.x(),mom.y(),mom.z());

        // Set the primary particle's position in the event header 
	G4ThreeVector pos=particleGun->GetParticlePosition();
	EventHeaderSvc::GetEventHeaderSvc()->SetInitialPosition(pos.x(),pos.y(),pos.z());

        // Set the primary particle's position in the event header 
	EventHeaderSvc::GetEventHeaderSvc()->SetInitialParticle(particleGun->GetParticleDefinition()->GetParticleName());
}

void PrimaryGeneratorAction::SetUniformDirection(){
	G4double dir_x = 0., dir_y = 0., dir_z = 0.;
	G4bool dir_OK = false;
	while( !dir_OK ){
		dir_x=G4UniformRand()-0.5;
		dir_y=G4UniformRand()-0.5;
		dir_z=G4UniformRand()-0.5;
		if ( dir_x*dir_x + dir_y*dir_y + dir_z*dir_z <= 0.025 && dir_x*dir_x + dir_y*dir_y + dir_z*dir_z != 0) dir_OK = true;
	}
	G4ThreeVector dir_3Vec(dir_x, dir_y, dir_z);
	particleGun->SetParticleMomentumDirection(dir_3Vec);
}

void PrimaryGeneratorAction::SetRandomEnergy(){
	if (EnergyType == 1){
		G4double dE=0;
		if(EnergyMode=="gRand"){
			dE=G4RandGauss::shoot(0,EkinSpread);
		}
		else if(EnergyMode=="uRand"){
			dE=(G4UniformRand()-0.5)*EkinSpread;
		}
		particleGun->SetParticleEnergy(Ekin+dE);
	}
	else if (EnergyType == 0 ){
		G4double dMom=0;
		if(EnergyMode=="gRand"){
			dMom=G4RandGauss::shoot(0,MomSpread);
		}
		else if(EnergyMode=="uRand"){
			dMom=(G4UniformRand()-0.5)*MomSpread;
		}
		G4double ekin = sqrt((Pa+dMom)*(Pa+dMom)+mass*mass)-mass;
		particleGun->SetParticleEnergy(ekin);
	}
}

void PrimaryGeneratorAction::SetRandomDirection(){
	G4double dPhi=0;
	G4double dTheta=0;
	if(PhiMode=="gRand"){
		if (PhiSpread) dPhi=G4RandGauss::shoot(Phi,PhiSpread);
	}
	else if (PhiMode=="uRand"){
		if (PhiSpread) {dPhi=(G4UniformRand()-0.5)*PhiSpread;} 
	}
	if(ThetaMode=="gRand"){
		if (ThetaSpread) dTheta=G4RandGauss::shoot(Theta,ThetaSpread);
	}
	else if (ThetaMode=="uRand"){
		if (ThetaSpread) {dTheta=(G4UniformRand()-0.5)*ThetaSpread;} 
	}
	G4ThreeVector dir(1,1,1);
	dir.setTheta(Theta+dTheta);
	dir.setPhi(Phi+dPhi);
	G4RotationMatrix rot(Ephi,Etheta,Epsi);
	dir = rot*dir;
	particleGun->SetParticleMomentumDirection(dir.unit());
}

void PrimaryGeneratorAction::SetRandomPosition(){
	G4double dx=0;
	G4double dy=0;
	G4double dz=0;
	G4double dx2=0;
	G4double dy2=0;
	G4double dz2=0;
	bool gotit=false;
	if(PositionMode=="gRand"){
		do {
			dx=G4RandGauss::shoot(0,xSpread);
			dy=G4RandGauss::shoot(0,ySpread);
			dz=G4RandGauss::shoot(0,zSpread);
			if (dx2+dy2+dz2<=PosLimit2) gotit = true;
		} while (!gotit);
	}
	else if (PositionMode=="sRand"){
		do {
			if (xSpread) {dx=2.*(G4UniformRand()-0.5);dx2 = dx*dx;dx*=xSpread;} 
			if (ySpread) {dy=2.*(G4UniformRand()-0.5);dy2 = dy*dy;dy*=ySpread;} 
			if (zSpread) {dz=2.*(G4UniformRand()-0.5);dz2 = dz*dz;dz*=zSpread;} 
			if (dx2+dy2+dz2<=1.) gotit = true;
		} while (!gotit);
	}
	else if (PositionMode=="bRand"){
		if (xSpread) dx=2.*(G4UniformRand()-0.5)*xSpread;
		if (ySpread) dy=2.*(G4UniformRand()-0.5)*ySpread;
		if (zSpread) dz=2.*(G4UniformRand()-0.5)*zSpread;
	}
	else if (PositionMode=="source" || PositionMode=="target"){
	  // Make sure that the random position chosen is within the Target volume
	  G4Navigator* theNavigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

	  // Get a random position based on the spread (copied code from gRand)
	  do {
			//dx=G4RandGauss::shoot(0,xSpread);
			//dy=G4RandGauss::shoot(0,ySpread);
			//dz=G4RandGauss::shoot(0,zSpread);
	    dx=G4RandFlat::shoot(-xSpread,xSpread);
	    dy=G4RandFlat::shoot(-ySpread,ySpread);
	    dz=G4RandFlat::shoot(-xSpread,zSpread);

	    G4ThreeVector position(x+dx, y+dy, z+dz);
	    CLHEP::HepEulerAngles rotation(45*deg, 90*deg, 0*deg);
	    G4ThreeVector new_position = position.rotate(rotation);
	    G4VPhysicalVolume* phys_volume = theNavigator->LocateGlobalPointAndSetup(new_position);

	    if (!phys_volume) {
	      continue; // if theNavigator didn't return a physical volume
	    }

	    if ( (PositionMode=="source" && phys_volume->GetName() == "Source") ||
		 (PositionMode=="target" && phys_volume->GetName() == "Target") ) {
	      gotit = true;
	    }
	  } while (!gotit);
	}

  G4ThreeVector position(x+dx, y+dy, z+dz);
  if (PositionMode == "source" || PositionMode == "target") {
    CLHEP::HepEulerAngles rotation(45*deg, 90*deg, 0*deg);
    position = position.rotate(rotation);
  }
  particleGun->SetParticlePosition(position);
}

void PrimaryGeneratorAction::SetUniformPosition(){
	MyVGeometryParameter* pMyVGeometryParameter = MyDetectorManager::GetMyDetectorManager()->GetSvc(UP_SubDet)->get_GeometryParameter();
	if (!pMyVGeometryParameter){
		std::cout<<"ERROR: in PrimaryGeneratorAction::SetUniformPosition cannot find : "<<UP_SubDet<<"!!!"<<std::endl;
		G4Exception("PrimaryGeneratorAction::SetUniformPosition()",
				"InvalidInput", FatalException,
				"cannot ");
	}
	if ( UP_Type == "Simple" ){
		SimpleGeometryParameter* pSimpleGeometryParameter = dynamic_cast<SimpleGeometryParameter*> (pMyVGeometryParameter);
		if (!pSimpleGeometryParameter){
			std::cout<<"ERROR: in PrimaryGeneratorAction::SetUniformPosition cannot convert : "<<UP_SubDet<<" from MyVGeometryParameter to SimpleGeometryParameter!!!"<<std::endl;
			G4Exception("PrimaryGeneratorAction::SetUniformPosition()",
					"InvalidInput", FatalException,
					"cannot convert from MyVGeometryParameter to SimpleGeometryParameter");
		}
		int index = pSimpleGeometryParameter->get_VolIndex(UP_Volume);
		if ( index == -1 ){
			std::cout<<"ERROR: in PrimaryGeneratorAction::SetUniformPosition cannot find : "<<UP_Volume<<"!!!"<<std::endl;
			G4Exception("PrimaryGeneratorAction::SetUniformPosition()",
					"InvalidInput", FatalException,
					"cannot convert volume");
		}
		G4String sol_type = pSimpleGeometryParameter->get_SolidType(index);
		G4ThreeVector pos(1,0,0);
		G4int n = pSimpleGeometryParameter->get_RepNo(index);
		G4int ivol = G4UniformRand()*n;
		if ( sol_type == "Tubs" ){
			G4double RMax,RMin,length,StartPhi,SpanPhi;
			G4int iTubs = pSimpleGeometryParameter->get_SolidIndex(index);
			RMax = pSimpleGeometryParameter->get_Tubs_RMax(iTubs,ivol);
			RMin = pSimpleGeometryParameter->get_Tubs_RMin(iTubs,ivol);
			length = pSimpleGeometryParameter->get_Tubs_Length(iTubs,ivol);
			StartPhi = pSimpleGeometryParameter->get_Tubs_StartAng(iTubs,ivol);
			SpanPhi = pSimpleGeometryParameter->get_Tubs_SpanAng(iTubs,ivol);

			G4double iz, iphi, ir;
			iz = G4UniformRand()*length -length/2;
			iphi = G4UniformRand()*SpanPhi + StartPhi;
			ir = G4UniformRand()*(RMax*RMax-RMin*RMin)+RMin*RMin;
			ir = sqrt(ir);
			pos.setPhi(iphi);
			pos.setRho(ir);
			pos.setZ(iz);
		}
		else{
			std::cout<<"ERROR: in PrimaryGeneratorAction::SetUniformPosition unsupported solid type: "<<sol_type<<"!!!"<<std::endl;
			G4Exception("PrimaryGeneratorAction::SetUniformPosition()",
					"InvalidInput", FatalException,
					"unsupported solid type");
		}
		G4double xp,yp,zp;
		xp = pSimpleGeometryParameter->get_PosX(index,ivol);
		yp = pSimpleGeometryParameter->get_PosY(index,ivol);
		zp = pSimpleGeometryParameter->get_PosZ(index,ivol);
		pos += G4ThreeVector(xp,yp,zp);
		G4String mot_volume = pSimpleGeometryParameter->get_MotherName(index);
		SimpleGeometryParameter * pmotSimpleGeometryParameter = 0;
		int temp_index = index;
		int mot_index = -1;
		while (mot_volume!="None"){
			pmotSimpleGeometryParameter = MyDetectorManager::GetMyDetectorManager()->GetParaFromVolume(mot_volume);
			int mot_index = pmotSimpleGeometryParameter->get_VolIndex(mot_volume);
			G4double mot_xp = pmotSimpleGeometryParameter->get_PosX(mot_index);
			G4double mot_yp = pmotSimpleGeometryParameter->get_PosY(mot_index);
			G4double mot_zp = pmotSimpleGeometryParameter->get_PosZ(mot_index);
			pos += G4ThreeVector(mot_xp,mot_yp,mot_zp);
			temp_index = mot_index;
			mot_volume = pmotSimpleGeometryParameter->get_MotherName(temp_index);
		}
		particleGun->SetParticlePosition(pos);
	}
	else{
		std::cout<<"ERROR: in PrimaryGeneratorAction::SetUniformPosition unsopported parameter class type: "<<UP_Type<<"!!!"<<std::endl;
		G4Exception("PrimaryGeneratorAction::SetUniformPosition()",
				"InvalidInput", FatalException,
				"unsopported parameter class type");
	}
}

void PrimaryGeneratorAction::BuildHistoFromFile(){
	G4String dir_name = getenv("CONFIGUREDATAROOT");
	if (dir_name[dir_name.size()-1] != '/') dir_name.append("/");

	// EnergyMode
	if (EnergyMode=="histo"){
		G4String full_infile_name = dir_name +  EM_hist_filename;
//		if (fp) delete fp;
		fp = new TFile(full_infile_name.c_str());
		if (fp==NULL) {
			std::cout<<"ERROR: Can not find file: "<<full_infile_name<<"!!!"<<std::endl;
			G4Exception("PrimaryGeneratorAction::BuildHistoFromFile()",
					"InvalidInput", FatalException,
					"Can not find file");
		}
		TH1F* h = (TH1F*)fp->Get(EM_hist_histname.c_str());
		if(h==NULL){
			std::cout<<"ERROR: Can not find file: "<<full_infile_name<<"!!!"<<std::endl;
			G4Exception("PrimaryGeneratorAction::BuildHistoFromFile()",
					"InvalidInput", FatalException,
					"Can not find file");
		}
		EM_hist = h;
	}

	// DirectionMode
	if (DirectionMode=="histo"){
		G4String full_infile_name = dir_name +  DM_hist_filename;
		if (fp) delete fp;
		fp = new TFile(full_infile_name.c_str());
		if (fp==NULL) {
			std::cout<<"ERROR: Can not find file: "<<full_infile_name<<"!!!"<<std::endl;
			G4Exception("PrimaryGeneratorAction::BuildHistoFromFile()",
					"InvalidInput", FatalException,
					"Can not find file");
		}
		TH1F* h = (TH1F*)fp->Get(DM_hist_histname.c_str());
		if(h==NULL){
			std::cout<<"ERROR: Can not find file: "<<full_infile_name<<"!!!"<<std::endl;
			G4Exception("PrimaryGeneratorAction::BuildHistoFromFile()",
					"InvalidInput", FatalException,
					"Can not find file");
		}
		DM_hist = h;
	}
	
	if (PositionMode =="histo"){
		G4String full_infile_name = dir_name +  PM_hist_filename;
//		if (fp) delete fp;
		fp = new TFile(full_infile_name.c_str());
		if (fp==NULL) {
			std::cout<<"ERROR: Can not find file: "<<full_infile_name<<"!!!"<<std::endl;
			G4Exception("PrimaryGeneratorAction::BuildHistoFromFile()",
					"InvalidInput", FatalException,
					"Can not find file");
		}
		TObject* obj=fp->Get(PM_hist_histname.c_str());
		if(obj==NULL){
			std::cout<<"ERROR: Can not find file: "<<full_infile_name<<"!!!"<<std::endl;
			G4Exception("PrimaryGeneratorAction::BuildHistoFromFile()",
					"InvalidInput", FatalException,
					"Can not find file");
		}
		if(obj->InheritsFrom("TH3")){
		  PM_hist = (TH3*) obj;
		
		}
		else if (obj->InheritsFrom("THnSparse")) {
		  PM_hist_sparse = (THnSparseF*) obj;
		  // should also check that it's 3dimenstional here
		}
		else {
			std::cout<<"ERROR: Cannot generate positions from non-3D histogram: "<<PM_hist_histname<<"!!!"<<std::endl;
			G4Exception("PrimaryGeneratorAction::BuildHistoFromFile()",
					"InvalidInput", FatalException,
					"Wrong hist type");
		}
	}

}

void PrimaryGeneratorAction::root_get_para(){
	int iEvent = (int) fmod(root_index,root_num);
	m_TChain->GetEntry(iEvent);
	root_index++;
}

void PrimaryGeneratorAction::root_build(){
	G4String dir_name = getenv("CONFIGUREDATAROOT");
	if (dir_name[dir_name.size()-1] != '/') dir_name.append("/");
	std::string m_TFile_name = dir_name + root_filename;
	if (root_filename[0] == '/')
		m_TFile_name = root_filename;
	//if (m_TChain) delete m_TChain;
	m_TChain = new TChain("t");
	m_TChain->Add(m_TFile_name.c_str());
	root_num = m_TChain->GetEntries();
	root_set_extra();
	if ( EnergyMode == "root" ){
		UseRoot = true;
		root_set_Energy();
	}
	if ( PositionMode == "root" ){
		UseRoot = true;
		root_set_Position();
	}
	if ( TimeMode == "root" ){
		UseRoot = true;
		root_set_Time();
	}
	if ( pidMode == "root" ){
		UseRoot = true;
		root_set_pid();
	}
	if ( RandMode == "root" ){
		root_set_Rand();
	}
}

void PrimaryGeneratorAction::root_set_Position(){
	m_TChain->SetBranchAddress("x", &root_double[0]);
	m_TChain->SetBranchAddress("y", &root_double[1]);
	m_TChain->SetBranchAddress("z", &root_double[2]);
}
void PrimaryGeneratorAction::root_set_Energy(){
	m_TChain->SetBranchAddress("px", &root_double[3]);
	m_TChain->SetBranchAddress("py", &root_double[4]);
	m_TChain->SetBranchAddress("pz", &root_double[5]);
}
void PrimaryGeneratorAction::root_set_Time(){
	m_TChain->SetBranchAddress("t", &root_double[6]);
}
void PrimaryGeneratorAction::root_set_pid(){
	m_TChain->SetBranchAddress("pid", &root_int[0]);
}
void PrimaryGeneratorAction::root_set_Rand(){
//	flag_R0 = m_TChain->SetBranchAddress("R0", &root_double[7]);
//	flag_R1 = m_TChain->SetBranchAddress("R1", &root_double[8]);
	flag_R0 = m_TChain->SetBranchAddress("R0", &root_int[3]);
	flag_R1 = m_TChain->SetBranchAddress("R1", &root_int[4]);
}
void PrimaryGeneratorAction::root_set_extra(){
	flag_weight=m_TChain->SetBranchAddress("weight", &root_double[9]);
	flag_ox=m_TChain->SetBranchAddress("ox", &root_double[10]);
	flag_oy=m_TChain->SetBranchAddress("oy", &root_double[11]);
	flag_oz=m_TChain->SetBranchAddress("oz", &root_double[12]);
	flag_opx=m_TChain->SetBranchAddress("opx", &root_double[13]);
	flag_opy=m_TChain->SetBranchAddress("opy", &root_double[14]);
	flag_opz=m_TChain->SetBranchAddress("opz", &root_double[15]);
	flag_ot=m_TChain->SetBranchAddress("ot", &root_double[16]);
	flag_ipx=m_TChain->SetBranchAddress("ipx", &root_double[17]);
	flag_ipy=m_TChain->SetBranchAddress("ipy", &root_double[18]);
	flag_ipz=m_TChain->SetBranchAddress("ipz", &root_double[19]);
	flag_process=m_TChain->SetBranchAddress("process",&root_str[0]);
	flag_volume=m_TChain->SetBranchAddress("volume",&root_str[1]);
	flag_ppid=m_TChain->SetBranchAddress("ppid",&root_int[1]);
	flag_ptid=m_TChain->SetBranchAddress("ptid",&root_int[2]);
}

void PrimaryGeneratorAction::ResetGen(G4String file_name){
	ReadCard(file_name);
	Initialize();
}

void PrimaryGeneratorAction::ReadCard(G4String file_name){
	std::cout<<" -- PrimaryGeneratorAction: Reading from \""<<file_name<<"\""<<std::endl;
	Reset();
	if(file_name[0] != '/'){ // Relative Dir
		G4String dir_name = getenv("CONFIGUREROOT");
		if (dir_name[dir_name.size()-1] != '/') dir_name.append("/");
		file_name = dir_name + file_name;
	}
	std::ifstream fin_card(file_name);
	if ( !fin_card ){
		std::cout<<"generator file \""<<file_name<<"\" is not available!!!"<<std::endl;
		G4Exception("PrimaryGeneratorAction::PrimaryGeneratorAction()","Run0031",
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
		std::string temp;
		buf_card>>keyword;
		if ( keyword == "Type:" ){
			buf_card>>fType;
		}
		else if ( keyword == "Particle:" ){
			buf_card>>ParticleName;
		}
		else if ( keyword == "Z:" ){
			buf_card>>Z;
		}
		else if ( keyword == "A:" ){
			buf_card>>A;
		}
		else if ( keyword == "C:" ){
			buf_card>>C;
		}
		else if ( keyword == "E:" ){
			buf_card>>E;
		}
		else if ( keyword == "Direction:" ){
			buf_card>>temp;
			Theta = CalFormula(ReplaceMacro(temp))*deg;
			buf_card>>temp;
			Phi = CalFormula(ReplaceMacro(temp))*deg;
		}
		else if ( keyword == "PhiSpread:" ){
			buf_card>>temp;
			PhiSpread = CalFormula(ReplaceMacro(temp))*deg;
		}
		else if ( keyword == "Ephi:" ){
			buf_card>>temp;
			Ephi = CalFormula(ReplaceMacro(temp))*deg;
		}
		else if ( keyword == "Etheta:" ){
			buf_card>>temp;
			Etheta = CalFormula(ReplaceMacro(temp))*deg;
		}
		else if ( keyword == "Epsi:" ){
			buf_card>>temp;
			Epsi = CalFormula(ReplaceMacro(temp))*deg;
		}
		else if ( keyword == "ThetaSpread:" ){
			buf_card>>temp;
			ThetaSpread = CalFormula(ReplaceMacro(temp))*deg;
		}
		else if ( keyword == "MomAmp:" ){
			buf_card>>temp;
			Pa = CalFormula(ReplaceMacro(temp))*MeV;
			EnergyType = 0;
		}
		else if ( keyword == "MomSpread:" ){
			buf_card>>temp;
			MomSpread = CalFormula(ReplaceMacro(temp))*MeV;
		}
		else if ( keyword == "Ekin:" ){
			buf_card>>temp;
			Ekin = CalFormula(ReplaceMacro(temp))*MeV;
			EnergyType = 1;
		}
		else if ( keyword == "EkinSpread:" ){
			buf_card>>temp;
			EkinSpread = CalFormula(ReplaceMacro(temp))*MeV;
		}
		else if ( keyword == "Position:" ){
			buf_card>>temp;
			x = CalFormula(ReplaceMacro(temp))*mm;
			buf_card>>temp;
			y = CalFormula(ReplaceMacro(temp))*mm;
			buf_card>>temp;
			z = CalFormula(ReplaceMacro(temp))*mm;
		}
		else if ( keyword == "PosSpread:" ){
			buf_card>>temp;
			xSpread = CalFormula(ReplaceMacro(temp))*mm;
			buf_card>>temp;
			ySpread = CalFormula(ReplaceMacro(temp))*mm;
			buf_card>>temp;
			zSpread = CalFormula(ReplaceMacro(temp))*mm;
		}
		else if ( keyword == "PosLimit:" ){
			buf_card>>temp;
			PosLimit2 = CalFormula(ReplaceMacro(temp))*mm;
			PosLimit2 *= PosLimit2;
		}
		else if ( keyword == "Time:" ){
			buf_card>>temp;
			t = CalFormula(ReplaceMacro(temp))*ns;
		}
		else if ( keyword == "RandMode:" ){
			buf_card>>RandMode;
		}
		else if ( keyword == "EnergyMode:" ){
			buf_card>>EnergyMode;
		}
		else if ( keyword == "PositionMode:" ){
			buf_card>>PositionMode;
		}
		else if ( keyword == "TimeMode:" ){
			buf_card>>TimeMode;
		}
		else if ( keyword == "pidMode:" ){
			buf_card>>pidMode;
		}
		else if ( keyword == "DirectionMode:" ){
			buf_card>>DirectionMode;
		}
		else if ( keyword == "ThetaMode:" ){
			buf_card>>ThetaMode;
		}
		else if ( keyword == "PhiMode:" ){
			buf_card>>PhiMode;
		}
		else if ( keyword == "EMHFN:" ){
			buf_card>>EM_hist_filename;
		}
		else if ( keyword == "EMHHN:" ){
			buf_card>>EM_hist_histname;
		}
		else if ( keyword == "DMHFN:" ){
			buf_card>>DM_hist_filename;
		}
		else if ( keyword == "DMHHN:" ){
			buf_card>>DM_hist_histname;
		}
		else if ( keyword == "PMHFN:" ){
			buf_card>>PM_hist_filename;
		}
		else if ( keyword == "PMHHN:" ){
			buf_card>>PM_hist_histname;
		}
		else if ( keyword == "RFN:" ){
			buf_card>>root_filename;
		}
		else if ( keyword == "RTN:" ){
			buf_card>>root_treename;
		}
		else if ( keyword == "UIV:" ){
			buf_card>>UP_Type>>UP_SubDet>>UP_Volume;
		}
		else if ( keyword == "DEFINE:" ){
			G4String MacroName;
			G4String MacroValue;
			buf_card>>MacroName>>MacroValue;
//			std::cout<<"found DEFINE:"<<std::endl; // to be deleted
			MacroValue = ReplaceMacro(MacroValue);
			bool foundName = false;
			for (int i = 0; i < knownValueNames.size(); i++){
				if (knownValueNames[i]==MacroName){ // If this name occurred once, replace the value
					foundName = true;
					knownValues[i]=MacroValue;
					break;
				}
			}
			if (!foundName){
				knownValueNames.push_back(MacroName);
				knownValues.push_back(MacroValue);
			}
		}
		else{
			std::cout<<"In PrimaryGeneratorAction::ReadCard, unknown name: '"<<keyword<<"' in file "<<file_name<<std::endl;
			std::cout<<"Will ignore this line!"<<std::endl;
		}
	}
	fin_card.close();
	buf_card.str("");
	buf_card.clear();
	Dump();
}

void PrimaryGeneratorAction::Reset(){
	fType = "simple";
	RandMode = "none";
	EnergyMode = "none";
	PositionMode = "none";
	TimeMode = "none";
	pidMode = "none";
	DirectionMode = "none";
	PhiMode = "none";
	ThetaMode = "none";
	ParticleName = "chargedgeantino";
	EM_hist_filename = "";
	EM_hist_histname = "";
	DM_hist_filename = "";
	DM_hist_histname = "";
	PM_hist_filename = "";
	PM_hist_histname = "";
	root_filename = "";
	root_treename = "";
	UP_SubDet = "";
	UP_Volume = "";
	UP_Type = "";
	EnergyType = 1;
	Z = 1;
	A = 1;
	C = 0;
	E = 0;
	Pa = 0;
	Ekin = 0;
	mass = 0;
	Px = 0;
	Py = 0;
	Pz = 0;
	Theta = 0;
	Phi = 0;
	x = 0;
	y = 0;
	z = 0;
	t = 0;
	xSpread = 0;
	ySpread = 0;
	zSpread = 0;
	PosLimit2 = 0;
	MomSpread = 0;
	EkinSpread = 0;
	ThetaSpread = 0;
	PhiSpread = 0;
	Ephi = 0;
	Etheta = 0;
	Epsi = 0;
	EM_hist = 0;
	DM_hist = 0;
	PM_hist = 0;
	root_num = 0;
	root_index = 0;
	UseRoot = false;
	flag_weight = false;
	flag_ox = false;
	flag_oy = false;
	flag_oz = false;
	flag_opx = false;
	flag_opy = false;
	flag_opz = false;
	flag_ipx = false;
	flag_ipy = false;
	flag_ipz = false;
	flag_ot = false;
	flag_process = false;
	flag_volume = false;
	flag_R0 = false;
	flag_R1 = false;
	flag_ppid = false;
	flag_ptid = false;
}

void PrimaryGeneratorAction::Initialize(){
	if (fType == "simple" || fType == "stable" || fType == "ion" ){
		G4int n_particle = 1;
		if (particleGun) delete particleGun;
		particleGun  = new G4ParticleGun(n_particle);
	}
	else {
		G4Exception("PrimaryGeneratorAction::PrimaryGeneratorAction()","Run0031",
				FatalException, "unknown generator type.");
	}

	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4ParticleDefinition* particle = particleTable->FindParticle(ParticleName);
	if (!particle){
		std::cout<<"ERROR: In PrimaryGeneratorAction::PrimaryGeneratorAction() Cannot find particle "<<ParticleName<<"!!!"<<std::endl;
		G4Exception("PrimaryGeneratorAction::PrimaryGeneratorAction()","Run0031",
				FatalException, "Cannot find particle.");
	}
	if (fType == "stable"){
		particle->SetPDGStable(true);
	}
	particleGun->SetParticleDefinition(particle);
	mass = particleGun->GetParticleDefinition()->GetPDGMass();
	G4ThreeVector dir(1,0,0);
	dir.setTheta(Theta);
	dir.setPhi(Phi);
	particleGun->SetParticleMomentumDirection(dir.unit());
	if (EnergyType==0){
		Ekin = sqrt(Pa*Pa+mass*mass)-mass;
	}
	particleGun->SetParticleEnergy(Ekin);
	particleGun->SetParticlePosition(G4ThreeVector(x,y,z));
	particleGun->SetParticleTime(t);

	if (EnergyMode == "histo" || DirectionMode == "histo" || PositionMode == "histo" ){
		BuildHistoFromFile();
	}
	UseRoot = false;
	if ( EnergyMode == "root" || PositionMode == "root" || TimeMode == "root" || pidMode == "root" || RandMode == "root"){
		UseRoot = true;
		root_build();
	}
	root_double[9]=1; // weight
	root_double[10]=-1;
	root_double[11]=-1;
	root_double[12]=-1;
	root_double[13]=-1;
	root_double[14]=-1;
	root_double[15]=-1;
	root_double[16]=-1;
	root_str[0]=0;
	root_str[1]=0;
	root_int[1]=-1;
	root_int[2]=-1;
}

void PrimaryGeneratorAction::Dump(){
	std::cout<<"---------------PrimaryGeneratorAction---------------------"<<std::endl;
	std::cout<<"Type:                                         "<<fType<<std::endl;
	if (fType=="ion"){
	std::cout<<"Z:                                            "<<Z<<std::endl;
	std::cout<<"A:                                            "<<A<<std::endl;
	std::cout<<"C:                                            "<<C<<std::endl;
	std::cout<<"E:                                            "<<E<<std::endl;
	}
	else{
	std::cout<<"Particle:                                     "<<ParticleName<<std::endl;
	}
	std::cout<<"Default Momentum Direction: theta =           "<<Theta/deg<<"deg, phi = "<<Phi/deg<<"deg"<<std::endl;
	std::cout<<"Default Phi Spread:                           "<<PhiSpread/deg<<"deg"<<std::endl;
	std::cout<<"Default Ephi Spread:                          "<<Ephi/deg<<"deg"<<std::endl;
	std::cout<<"Default Etheta Spread:                        "<<Etheta/deg<<"deg"<<std::endl;
	std::cout<<"Default Epsi Spread:                          "<<Epsi/deg<<"deg"<<std::endl;
	std::cout<<"Default Theta Spread:                         "<<ThetaSpread/deg<<"deg"<<std::endl;
	if (EnergyType==0){
	std::cout<<"Default Momentum Amplitude:                   "<<Pa/MeV<<"MeV"<<std::endl;
	std::cout<<"Default Momentum Spread(MeV/c):               ("<<MomSpread/MeV<<std::endl;
	}
	else if (EnergyType==1){
	std::cout<<"Default Kinetic Energy:                       "<<Ekin/MeV<<"MeV"<<std::endl;
	std::cout<<"Default Energy Spread(MeV):                   ("<<EkinSpread/MeV<<std::endl;
	}
	std::cout<<"Default Position(mm):                         ("<<x/mm<<", "<<y/mm<<", "<<z/mm<<")"<<std::endl;
	std::cout<<"Default Position Spread(mm):                  ("<<xSpread/mm<<", "<<ySpread/mm<<", "<<zSpread/mm<<")"<<std::endl;
	std::cout<<"Default Position Limit(mm):                   ("<<sqrt(PosLimit2)/mm<<std::endl;
	std::cout<<"Default Time(ns):                             ("<<t/ns<<std::endl;
	std::cout<<"EnergyMode:                                   "<<EnergyMode<<std::endl;
	std::cout<<"PositionMode:                                 "<<PositionMode<<std::endl;
	std::cout<<"TimeMode:                                     "<<TimeMode<<std::endl;
	std::cout<<"pidMode:                                      "<<pidMode<<std::endl;
	std::cout<<"Uniform In sub-detector:                      "<<UP_SubDet<<std::endl;
	std::cout<<"  Volume:                                     "<<UP_Volume<<std::endl;
	std::cout<<"  Type:                                       "<<UP_Type<<std::endl;
	std::cout<<"DirectionMode:                                "<<DirectionMode<<std::endl;
	std::cout<<"PhiMode:                                      "<<PhiMode<<std::endl;
	std::cout<<"ThetaMode:                                    "<<ThetaMode<<std::endl;
	std::cout<<"File Name for EnergyMode = histo:             "<<EM_hist_filename<<std::endl;
	std::cout<<"Histogram Name for EnergyMode = histo:        "<<EM_hist_histname<<std::endl;
	std::cout<<"File Name for DirectionMode = histo:          "<<DM_hist_filename<<std::endl;
	std::cout<<"Histogram Name for DirectionMode = histo:     "<<DM_hist_histname<<std::endl;
	std::cout<<"File Name for PositionMode = histo:          "<<PM_hist_filename<<std::endl;
	std::cout<<"Histogram Name for PositionMode = histo:     "<<PM_hist_histname<<std::endl;
	std::cout<<"File Name for Energy/Position/TimeMode = root: "<<root_filename<<std::endl;
	std::cout<<"Tree Name for Energy/Position/TimeMode = root: "<<root_treename<<std::endl;
	std::cout<<"---------------------------------------------------------"<<std::endl;
}
