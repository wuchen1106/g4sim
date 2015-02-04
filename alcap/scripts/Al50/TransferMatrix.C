// NB this assumes you are working on a MC run where protons were the initial particle
// generated inside the target

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TSystem.h"

#include "RooUnfold.h"

#include <iostream>
#include <cmath>

extern TSystem* gSystem;

struct Arm {
  std::string detname;
  std::string monname;
  RooUnfoldResponse response;
  int n_hits;
  int n_misses;
};

void TransferMatrix(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tree = (TTree*) file->Get("tree");
  
  double bin_width = 200;
  double energy_high = 25000;
  double energy_low = 0;
  int n_bins = (energy_high - energy_low) / bin_width;

  std::vector<Arm> arms;
  Arm right_arm;
  Arm left_arm;
  right_arm.detname = "SiR";
  right_arm.monname = "ESi1";
  std::string responsename = right_arm.detname + "_response";
  right_arm.response = RooUnfoldResponse(n_bins,energy_low,energy_high, responsename.c_str());
  right_arm.n_hits = 0;
  right_arm.n_misses = 0;
  left_arm.detname = "SiL";
  left_arm.monname = "ESi2";
  responsename = left_arm.detname + "_response";
  left_arm.response = RooUnfoldResponse(n_bins,energy_low,energy_high, responsename.c_str());
  left_arm.n_hits = 0;
  left_arm.n_misses = 0;
  arms.push_back(right_arm);
  arms.push_back(left_arm);

  std::vector<std::string>* particleName = 0;
  std::vector<std::string>* volName = 0;
  std::vector<double>* edep = 0;
  std::vector<int>* stopped = 0;
  double i_px = 0;
  double i_py = 0;
  double i_pz = 0;
  std::vector<int>* tid = 0;
  std::vector<double>* t = 0;
  std::vector<double>* px = 0;
  std::vector<double>* py = 0;
  std::vector<double>* pz = 0;
  
  TBranch* br_particleName = tree->GetBranch("M_particleName");
  TBranch* br_volName = tree->GetBranch("M_volName");
  TBranch* br_edep = tree->GetBranch("M_edep");
  TBranch* br_stopped = tree->GetBranch("M_stopped");
  TBranch* br_i_px = tree->GetBranch("i_px");
  TBranch* br_i_py = tree->GetBranch("i_py");
  TBranch* br_i_pz = tree->GetBranch("i_pz");
  TBranch* br_tid = tree->GetBranch("M_tid");
  TBranch* br_t = tree->GetBranch("M_t");
  TBranch* br_px = tree->GetBranch("M_px");
  TBranch* br_py = tree->GetBranch("M_py");
  TBranch* br_pz = tree->GetBranch("M_pz");
  
  br_particleName->SetAddress(&particleName);
  br_volName->SetAddress(&volName);
  br_edep->SetAddress(&edep);
  br_stopped->SetAddress(&stopped);
  br_i_px->SetAddress(&i_px);
  br_i_py->SetAddress(&i_py);
  br_i_pz->SetAddress(&i_pz);
  br_tid->SetAddress(&tid);
  br_t->SetAddress(&t);
  br_px->SetAddress(&px);
  br_py->SetAddress(&py);
  br_pz->SetAddress(&pz);

  double proton_mass = 938.272*1000; // convert to keV
  
  int n_entries = tree->GetEntries();
  for (int iEntry = 0; iEntry < n_entries; ++iEntry) {
      tree->GetEvent(iEntry);
      
      if (iEntry % 10000 == 0) {
	std::cout << iEntry << " / " << n_entries << std::endl;
      }

      double dE = 0;
      double E = 0;
      double true_kinetic_energy = 0;
      bool thick_hit = false;
      bool thin_hit = false;
      double thick_trackID = 0;
      double thin_trackID = 0;
      double thick_time = 0;
      double thick_px = 0;
      double thick_py = 0;
      double thick_pz = 0;
      double thin_time = 0;
      double thin_px = 0;
      double thin_py = 0;
      double thin_pz = 0;
      std::vector<Arm>::iterator thick_arm; // not sure which arm the proton will be found in
      std::vector<Arm>::iterator thin_arm; // not sure which arm the proton will be found in

      // Calculate true energy here
      double true_px = i_px*1e3;
      double true_py = i_py*1e3;
      double true_pz = i_pz*1e3;
      double squared_mom = true_px*true_px + true_py*true_py + true_pz*true_pz;
      double total_energy = std::sqrt(squared_mom + proton_mass*proton_mass);
      true_kinetic_energy = total_energy - proton_mass;

      //      std::cout << "Entry #" << iEntry << ": true kinetic energy = " << true_kinetic_energy << std::endl;

      for (int iElement = 0; iElement < particleName->size();  ++iElement) {

	std::string i_particleName = particleName->at(iElement);
	std::string i_volName = volName->at(iElement);
	
	// Loop through the arms
	for (std::vector<Arm>::iterator i_arm = arms.begin(); i_arm != arms.end(); ++i_arm) {
	  std::string thick_monname = i_arm->monname;
	  std::string thin_monname = "d"+i_arm->monname;

	  if (i_particleName == "proton" && i_volName == thick_monname && stopped->at(iElement) == 1) {
	    thick_hit = true;
	    E = edep->at(iElement)*1e6; // convert to keV
	    thick_trackID = tid->at(iElement);
	    thick_time = t->at(iElement);
	    thick_arm = i_arm;
	    thick_px = px->at(iElement)*1e6;
	    thick_py = py->at(iElement)*1e6;
	    thick_pz = pz->at(iElement)*1e6;
	  }
	  else if (i_particleName == "proton" && i_volName == thin_monname) {
	    thin_hit = true;
	    dE = edep->at(iElement)*1e6;
	    thin_trackID = tid->at(iElement);
	    thin_time = t->at(iElement);
	    thin_arm = i_arm;
	    thin_px = px->at(iElement)*1e6;
	    thin_py = py->at(iElement)*1e6;
	    thin_pz = pz->at(iElement)*1e6;
	  }
	}
      }
      
      // After looping through the elements, we will hopefully have found the initial proton again
      if (thick_hit && thin_hit) {
	// For debugging
	//	std::cout << "Track IDs (thin, thick) = (" << thin_trackID << ", " << thick_trackID << ")" << std::endl;
	//	std::cout << "Times  (thin, thick) = (" << thin_time << ", " << thick_time << ")" << std::endl;
	//	std::cout << "Arms  (thin, thick) = (" << thin_arm->detname << ", " << thick_arm->detname << ")" << std::endl;
	
	double observed_E = E+dE;
	if (true_kinetic_energy < observed_E) {
	  std::cout << "Entry # " << iEntry << ": " << thick_trackID << ", " << thin_trackID << std::endl;
	  std::cout << "E = " << E << ", dE = " << dE << std::endl;
	  std::cout << "Thick t = " << thick_time << ", thin t = " << thin_time << std::endl;
	  std::cout << "Thick p = ("<< thick_px << ", " << thick_py << ", " << thick_pz << ")" << std::endl;
	  std::cout << "Thin p = ("<< thin_px << ", " << thin_py << ", " << thin_pz << ")" << std::endl;
	  std::cout << "True p = ("<< true_px << ", " << true_py << ", " << true_pz << ")" << std::endl;
	  std::cout << "That's weird... " << true_kinetic_energy << " < " << observed_E << std::endl;
	}
	
	for (std::vector<Arm>::iterator i_arm = arms.begin(); i_arm != arms.end(); ++i_arm) {	
	  if (i_arm == thick_arm) {
	    i_arm->response.Fill(observed_E, true_kinetic_energy); // this arm saw the proton
	    i_arm->n_hits++;
	  }
	  else {
	    i_arm->response.Miss(true_kinetic_energy); // the other one didn't
	    i_arm->n_misses++;
	  }
	}
      }
      else {
	//	std::cout << true_kinetic_energy << " keV missed" << std::endl;
	for (std::vector<Arm>::iterator i_arm = arms.begin(); i_arm != arms.end(); ++i_arm) {
	  i_arm->response.Miss(true_kinetic_energy); // both arms missed the proton
	  i_arm->n_misses++;
	}
      }
  }
  
  TFile* out_file = new TFile("output.root", "RECREATE");
  for (std::vector<Arm>::iterator i_arm = arms.begin(); i_arm != arms.end(); ++i_arm) {
    i_arm->response.Hresponse()->SetXTitle("Observed E [keV]");
    i_arm->response.Hresponse()->SetYTitle("True E [keV]");
    i_arm->response.Write();
    std::cout << i_arm->detname << ": " << i_arm->n_hits << " hits and " << i_arm->n_misses << " misses" << std::endl;
  }
  out_file->Close();
}
