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
};

void TransferMatrix(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tree = (TTree*) file->Get("tree");
  
  std::vector<Arm> arms;
  Arm right_arm;
  Arm left_arm;
  right_arm.detname = "SiR";
  right_arm.monname = "ESi1";
  std::string responsename = right_arm.detname + "_response";
  right_arm.response = RooUnfoldResponse(25,0,10000, responsename.c_str());
  left_arm.detname = "SiL";
  left_arm.monname = "ESi2";
  responsename = left_arm.detname + "_response";
  left_arm.response = RooUnfoldResponse(25,0,10000, responsename.c_str());
  arms.push_back(right_arm);
  arms.push_back(left_arm);

  std::vector<std::string>* particleName = 0;
  std::vector<std::string>* volName = 0;
  std::vector<std::string>* ovolName = 0;
  std::vector<double>* edep = 0;
  std::vector<int>* stopped = 0;
  std::vector<double>* opx = 0;
  std::vector<double>* opy = 0;
  std::vector<double>* opz = 0;
  std::vector<int>* tid = 0;
  std::vector<double>* t = 0;
  std::vector<double>* px = 0;
  std::vector<double>* py = 0;
  std::vector<double>* pz = 0;
  
  TBranch* br_particleName = tree->GetBranch("M_particleName");
  TBranch* br_volName = tree->GetBranch("M_volName");
  TBranch* br_ovolName = tree->GetBranch("M_ovolName");
  TBranch* br_edep = tree->GetBranch("M_edep");
  TBranch* br_stopped = tree->GetBranch("M_stopped");
  TBranch* br_opx = tree->GetBranch("M_opx");
  TBranch* br_opy = tree->GetBranch("M_opy");
  TBranch* br_opz = tree->GetBranch("M_opz");
  TBranch* br_tid = tree->GetBranch("M_tid");
  TBranch* br_t = tree->GetBranch("M_t");
  TBranch* br_px = tree->GetBranch("M_px");
  TBranch* br_py = tree->GetBranch("M_py");
  TBranch* br_pz = tree->GetBranch("M_pz");

  
  br_particleName->SetAddress(&particleName);
  br_volName->SetAddress(&volName);
  br_ovolName->SetAddress(&ovolName);
  br_edep->SetAddress(&edep);
  br_stopped->SetAddress(&stopped);
  br_opx->SetAddress(&opx);
  br_opy->SetAddress(&opy);
  br_opz->SetAddress(&opz);
  br_tid->SetAddress(&tid);
  br_t->SetAddress(&t);
  br_px->SetAddress(&px);
  br_py->SetAddress(&py);
  br_pz->SetAddress(&pz);

  double proton_mass = 938.272*1000; // convert to keV
  
  TH2F* hTransfer_SiR = new TH2F("hTransfer_SiR", "hTransfer_SiR", 25,0,10000, 25,0,10000);
  hTransfer_SiR->SetXTitle("Observed Energy [keV]");
  hTransfer_SiR->SetYTitle("True Energy [keV]");
  
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
      double thin_time = 0;
      double thick_px = 0;
      double thick_py = 0;
      double thick_pz = 0;
      double thin_px = 0;
      double thin_py = 0;
      double thin_pz = 0;

      for (int iElement = 0; iElement < particleName->size();  ++iElement) {
	
	// Loop through the arms
	for (std::vector<Arm>::iterator i_arm = arms.begin(); i_arm != arms.end(); ++i_arm) {
	  std::string thick_monname = i_arm->monname;
	  std::string thin_monname = "d"+i_arm->monname;

	  if (particleName->at(iElement) == "proton" && volName->at(iElement) == thick_monname && ovolName->at(iElement) == "Target") {
	    thick_hit = true;
	    E = edep->at(iElement)*1e6; // convert to keV
	    thick_trackID = tid->at(iElement);
	    thick_time = t->at(iElement);
	    thick_px = px->at(iElement)*1e6;
	    thick_py = py->at(iElement)*1e6;
	    thick_pz = pz->at(iElement)*1e6;
	    
	    // Calculate true energy here
	    double true_px = opx->at(iElement)*1e6;
	    double true_py = opy->at(iElement)*1e6;
	    double true_pz = opz->at(iElement)*1e6;
	    double squared_mom = true_px*true_px + true_py*true_py + true_pz*true_pz;
	    double total_energy = std::sqrt(squared_mom + proton_mass*proton_mass);
	    true_kinetic_energy = total_energy - proton_mass;
	    if (iEntry == 279485) {
	      std::cout << "Truth (px, py, pz) = (" << true_px << ", " << true_py << ", " << true_pz << ") keV" << std::endl;
	      std::cout << "p^2 = " << squared_mom << ", m = " << proton_mass << ", E_t = " << total_energy << std::endl;
	      std::cout << "Thick E = " << E << std::endl;
	      std::cout << "Thick t = " << t->at(iElement) << " ns" << std::endl;
	    }
	  
	  }
	  else if (particleName->at(iElement) == "proton" && volName->at(iElement) == thin_monname && ovolName->at(iElement) == "Target") {
	    thin_hit = true;
	    dE = edep->at(iElement)*1e6;
	    thin_trackID = tid->at(iElement);
	    thin_time = t->at(iElement);
	    thin_px = px->at(iElement)*1e6;
	    thin_py = py->at(iElement)*1e6;
	    thin_pz = pz->at(iElement)*1e6;

	    double true_px = opx->at(iElement)*1e6;
	    double true_py = opy->at(iElement)*1e6;
	    double true_pz = opz->at(iElement)*1e6;
	    double squared_mom = true_px*true_px + true_py*true_py + true_pz*true_pz;
	    double total_energy = std::sqrt(squared_mom + proton_mass*proton_mass);
	    true_kinetic_energy = total_energy - proton_mass;

	    if (iEntry == 279485) {
	      std::cout << "Truth (px, py, pz) = (" << true_px << ", " << true_py << ", " << true_pz << ") keV" << std::endl;
	      std::cout << "p^2 = " << squared_mom << ", m = " << proton_mass << ", E_t = " << total_energy << std::endl;
	      std::cout << "Thin E = " << dE << std::endl;
	      std::cout << "Thin t = " << t->at(iElement) << " ns" << std::endl;
	    }
	  }
	
	
	  if (thick_hit && thin_hit) { // assuming that the two hits will be next to each other in the tree
	    double observed_E = E+dE;
	    if (true_kinetic_energy < observed_E) {
	      std::cout << "Entry # " << iEntry << ": " << thick_trackID << ", " << thin_trackID << std::endl;
	      std::cout << "E = " << E << ", dE = " << dE << std::endl;
	      std::cout << "Thick t = " << thick_time << ", thin t = " << thin_time << std::endl;
	      std::cout << "Thick p = (" << thick_px << ", " << thick_py << ", " << thick_pz << "), thin p = ("
			<< thin_px << ", " << thin_py << ", " << thin_pz << ")" << std::endl;
	      std::cout << "That's weird... " << true_kinetic_energy << " < " << observed_E << std::endl;
	    }
	    //	  else {
	    //	    hTransfer_SiR->Fill(observed_E, true_kinetic_energy);
	    i_arm->response.Fill(observed_E, true_kinetic_energy);
	    //	  }
	    thick_hit = false;
	    thin_hit = false; // reset to false in case there is another proton in this event
	    E = dE = true_kinetic_energy = thick_trackID = thin_trackID = 0;
	  }
	}
      }
  }
  //  hTransfer_SiR->Draw("COLZ");

       TFile* out_file = new TFile("output.root", "RECREATE");
       for (std::vector<Arm>::iterator i_arm = arms.begin(); i_arm != arms.end(); ++i_arm) {
	 i_arm->response.Hresponse()->SetXTitle("Observed E [keV]");
	 i_arm->response.Hresponse()->SetYTitle("True E [keV]");
	 i_arm->response.Write();
       }
       out_file->Close();
}
