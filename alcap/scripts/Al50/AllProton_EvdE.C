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
  std::vector<double>* ekin = 0;
  
  TBranch* br_particleName = tree->GetBranch("M_particleName");
  TBranch* br_volName = tree->GetBranch("M_volName");
  TBranch* br_ovolName = tree->GetBranch("M_ovolName");
  TBranch* br_edep = tree->GetBranch("M_edep");
  TBranch* br_stopped = tree->GetBranch("M_stopped");
  TBranch* br_ekin = tree->GetBranch("M_ekin");
  
  br_particleName->SetAddress(&particleName);
  br_volName->SetAddress(&volName);
  br_ovolName->SetAddress(&ovolName);
  br_edep->SetAddress(&edep);
  br_stopped->SetAddress(&stopped);
  br_ekin->SetAddress(&ekin);

  double max_true_ekin = 40000;
  double max_total_edep = 25000;
  double max_thin_edep = 3000;
  double min_edep = 0;
  double edep_width = 100;
  int n_bins_total_edep = (max_total_edep - min_edep) / edep_width;
  int n_bins_thin_edep = (max_thin_edep - min_edep) / edep_width;
  int n_bins_true_ekin = (max_true_ekin - min_edep) / edep_width;
  
  TH2F* hEvdE_SiR_stopped = new TH2F("hEvdE_SiR_stopped", "hEvdE_SiR_stopped", n_bins_total_edep,min_edep,max_total_edep, n_bins_thin_edep,min_edep,max_thin_edep);
  hEvdE_SiR_stopped->SetMarkerColor(kRed);
  hEvdE_SiR_stopped->SetMarkerStyle(7);
  hEvdE_SiR_stopped->SetXTitle("E_{1} + E_{2} [keV]");
  hEvdE_SiR_stopped->GetYaxis()->SetTitleOffset(1.4);
  hEvdE_SiR_stopped->SetYTitle("E_{1} [keV]");

  TH2F* hEvdE_SiR_not_stopped = new TH2F("hEvdE_SiR_not_stopped", "hEvdE_SiR_not_stopped", n_bins_total_edep,min_edep,max_total_edep, n_bins_thin_edep,min_edep,max_thin_edep);
  hEvdE_SiR_not_stopped->SetMarkerColor(kBlue);
  hEvdE_SiR_not_stopped->SetMarkerStyle(7);
  hEvdE_SiR_not_stopped->SetXTitle("E_{1} + E_{2} [keV]");
  hEvdE_SiR_not_stopped->GetYaxis()->SetTitleOffset(1.4);
  hEvdE_SiR_not_stopped->SetYTitle("E_{1} [keV]");

  TH2F* hEvdE_SiR_not_stopped_true = new TH2F("hEvdE_SiR_not_stopped_true", "hEvdE_SiR_not_stopped_true", n_bins_true_ekin,min_edep,max_true_ekin, n_bins_total_edep,min_edep,max_total_edep);
  hEvdE_SiR_not_stopped_true->SetMarkerColor(kBlue);
  hEvdE_SiR_not_stopped_true->SetMarkerStyle(7);
  hEvdE_SiR_not_stopped_true->SetYTitle("E_{1} + E_{2} [keV]");
  hEvdE_SiR_not_stopped_true->GetYaxis()->SetTitleOffset(1.4);
  hEvdE_SiR_not_stopped_true->SetXTitle("Kinetic Energy at Thin Layer [keV]");
  
  
  for (int iEntry = 0; iEntry < tree->GetEntries(); ++iEntry) {
    tree->GetEvent(iEntry);
      
    double dE = 0;
    double E = 0;
    double thin_ekin;
    double thick_ekin;
    bool thick_hit = false;
    bool thin_hit = false;
    bool stopped_proton;
    for (int iElement = 0; iElement < particleName->size();  ++iElement) {

      if (particleName->at(iElement) == "proton" && volName->at(iElement) == "SiR2" && ovolName->at(iElement) == "Target") {
	thick_hit = true;
	E = edep->at(iElement)*1e6; // convert to keV
	thick_ekin = ekin->at(iElement)*1e6;
	if (stopped->at(iElement) == 1) {
	  stopped_proton = true;
	}
	else {
	  stopped_proton = false;
	}
      }
      else if (particleName->at(iElement) == "proton" && volName->at(iElement) == "SiR1" && ovolName->at(iElement) == "Target") {
	thin_hit = true;
	dE = edep->at(iElement)*1e6;
	thin_ekin = ekin->at(iElement)*1e6;
      }
	
      if (thick_hit && thin_hit) { // assuming that the two hits will be next to each other in the tree

	if (stopped_proton == true) {
	  hEvdE_SiR_stopped->Fill(E+dE, dE);
	}
	else {
	  hEvdE_SiR_not_stopped->Fill(E+dE, dE);
	  hEvdE_SiR_not_stopped_true->Fill(thin_ekin, E+dE);
	}
	thick_hit = false;
	thin_hit = false; // reset to false in case there is another proton in this event
      }
    }
  }

  hEvdE_SiR_stopped->Draw();
  hEvdE_SiR_not_stopped->Draw("SAMES");
}
