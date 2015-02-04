#include "TFile.h"
#include "TTree.h"
#include "TH3F.h"
#include "THnSparse.h"
#include "TBranch.h"

#include <iostream>

void StoppingPositionDistribution(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tree = (TTree*) file->Get("tree");
  
  TH3F* hLocalStoppingPositions = new TH3F("hLocalStoppingPositions", "Positions of Stopped Muons in Target (local coords)", 10,-5,5, 10,-5,5, 60,-30,30);
  hLocalStoppingPositions->SetXTitle("Local X [cm]");
  hLocalStoppingPositions->SetYTitle("Local Y [cm]");
  hLocalStoppingPositions->SetZTitle("Local Z [#mum]");
  tree->Draw("M_local_Oz*10000:M_local_Oy:M_local_Ox>>hLocalStoppingPositions", "M_particleName==\"mu-\" && M_volName==\"Target\" && M_stopped==1", "");


  double bin_width = 0.0001; // cm
  const int n_dimensions = 3;
  double x_low[n_dimensions] = {-5, -5, -5};
  double x_high[n_dimensions] = {5, 5, 5};
  int n_bins_one_axis = (x_high[0] - x_low[0]) / bin_width;
  int n_bins[n_dimensions] = {n_bins_one_axis, n_bins_one_axis, n_bins_one_axis};
  std::cout << "AE: n_bins_one_axis = " << n_bins_one_axis << std::endl;

  THnSparseF* hGlobalStoppingPositions = new THnSparseF("hGlobalStoppingPositions", "Positions of Stopped Muons in Target (global coords)", n_dimensions, n_bins, x_low, x_high);
  hGlobalStoppingPositions->GetAxis(0)->SetTitle("Global X [cm]");
  hGlobalStoppingPositions->GetAxis(1)->SetTitle("Global Y [cm]");
  hGlobalStoppingPositions->GetAxis(2)->SetTitle("Global Z [cm]");

  std::vector<std::string>* particleName = 0;
  std::vector<std::string>* volName = 0;
  std::vector<int>* stopped = 0;
  std::vector<double>* Ox = 0;
  std::vector<double>* Oy = 0;
  std::vector<double>* Oz = 0;
  
  TBranch* br_particleName = tree->GetBranch("M_particleName");
  TBranch* br_volName = tree->GetBranch("M_volName");
  TBranch* br_stopped = tree->GetBranch("M_stopped");
  TBranch* br_Ox = tree->GetBranch("M_Ox");
  TBranch* br_Oy = tree->GetBranch("M_Oy");
  TBranch* br_Oz = tree->GetBranch("M_Oz");
  
  br_particleName->SetAddress(&particleName);
  br_volName->SetAddress(&volName);
  br_stopped->SetAddress(&stopped);
  br_Ox->SetAddress(&Ox);
  br_Oy->SetAddress(&Oy);
  br_Oz->SetAddress(&Oz);

  int n_entries = tree->GetEntries();
  for (int iEntry = 0; iEntry < n_entries; ++iEntry) {
      tree->GetEvent(iEntry);
      
      if (iEntry % 10000 == 0) {
	std::cout << iEntry << " / " << n_entries << std::endl;
      }

      for (int iElement = 0; iElement < particleName->size();  ++iElement) {

	std::string i_particleName = particleName->at(iElement);
	std::string i_volName = volName->at(iElement);

	if (i_particleName == "mu-" && i_volName == "Target" && stopped->at(iElement) == 1) {
	  double coord[n_dimensions] = {Ox->at(iElement), Oy->at(iElement), Oz->at(iElement)};
	  hGlobalStoppingPositions->Fill(coord);
	}
      }
  }

  //    tree->Draw("M_Oz:M_Oy:M_Ox>>hGlobalStoppingPositions", "M_particleName==\"mu-\" && M_volName==\"Target\" && M_stopped==1", "", 10000);

  TFile* out_file = new TFile("output.root", "RECREATE");
  hLocalStoppingPositions->Write();
  hGlobalStoppingPositions->Write();
  out_file->Close();
}
