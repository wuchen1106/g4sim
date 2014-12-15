#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH2.h"

void AllProton_EvdE(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tree = (TTree*) file->Get("tree");
  
  std::vector<std::string>* particleName = 0;
  std::vector<std::string>* volName = 0;
  std::vector<std::string>* ovolName = 0;
  std::vector<double>* edep = 0;
  std::vector<int>* stopped = 0;
  
  TBranch* br_particleName = tree->GetBranch("M_particleName");
  TBranch* br_volName = tree->GetBranch("M_volName");
  TBranch* br_ovolName = tree->GetBranch("M_ovolName");
  TBranch* br_edep = tree->GetBranch("M_edep");
  TBranch* br_stopped = tree->GetBranch("M_stopped");
  
  br_particleName->SetAddress(&particleName);
  br_volName->SetAddress(&volName);
  br_ovolName->SetAddress(&ovolName);
  br_edep->SetAddress(&edep);
  br_stopped->SetAddress(&stopped);
  
  TH2F* hEvdE_SiR = new TH2F("hEvdE_SiR", "hEvdE_SiR", 1000,0,10000, 1000,0,10000);
  
  for (int iEntry = 0; iEntry < tree->GetEntries(); ++iEntry) {
      tree->GetEvent(iEntry);
      
      double dE = 0;
      double E = 0;
      bool thick_hit = false;
      bool thin_hit = false;
      
      for (int iElement = 0; iElement < particleName->size();  ++iElement) {

	if (particleName->at(iElement) == "proton" && volName->at(iElement) == "ESi1" && ovolName->at(iElement) == "Target") {
	  thick_hit = true;
	  E = edep->at(iElement)*1e6; // convert to keV
	}
	else if (particleName->at(iElement) == "proton" && volName->at(iElement) == "dESi1" && ovolName->at(iElement) == "Target") {
	  thin_hit = true;
	  dE = edep->at(iElement)*1e6;
	}
	
	if (thick_hit && thin_hit) { // assuming that the two hits will be next to each other in the tree
	  hEvdE_SiR->Fill(E+dE, dE);
	  thick_hit = false;
	  thin_hit = false; // reset to false in case there is another proton in this event
	}
      }
    }
	 }
