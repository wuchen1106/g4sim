#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TH3.h"
#include "TCanvas.h"
#include <iostream>
#include <string>

void InitialTargetPositions(std::string filename, std::string draw_option, int colour);

void Comparison() {
  TCanvas* c1 = new TCanvas("c1", "c1");

  InitialTargetPositions("output/raw_g4sim_100k_base_test.root", "", kRed);
  InitialTargetPositions("output/raw_g4sim_100k_clockwise_test.root", "SAME", kBlue);

  c1->Update();
}

void InitialTargetPositions(std::string filename, std::string draw_option, int colour) {

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tree = (TTree*) file->Get("tree");

  //  TH3F* target_positions = new TH3F("target_positions", "target_positions", 100,-10,10, 100,-10,10, 100,-10,10);
  //  target_positions->GetXaxis()->SetTitle("x [cm]");
  //  target_positions->GetYaxis()->SetTitle("y [cm]");
  //  target_positions->GetZaxis()->SetTitle("z [cm]");

  TH2F* xz_plane = new TH2F("xz_plane", "xz_plane", 100,-10,10, 100,-10,10);
  xz_plane->GetXaxis()->SetTitle("z [cm]");
  xz_plane->GetYaxis()->SetTitle("y [cm]");

  std::vector<std::string>* oprocess = 0;
  std::vector<double>* x = 0;
  std::vector<double>* local_x = 0;
  std::vector<double>* y = 0;
  std::vector<double>* local_y = 0;
  std::vector<double>* z = 0;
  std::vector<double>* local_z = 0;

  
  TBranch* br_oprocess = tree->GetBranch("M_oprocess");
  TBranch* br_x = tree->GetBranch("M_x");
  TBranch* br_local_x = tree->GetBranch("M_local_x");
  TBranch* br_y = tree->GetBranch("M_y");
  TBranch* br_local_y = tree->GetBranch("M_local_y");
  TBranch* br_z = tree->GetBranch("M_z");
  TBranch* br_local_z = tree->GetBranch("M_local_z");

  br_oprocess->SetAddress(&oprocess);
  br_x->SetAddress(&x);
  br_local_x->SetAddress(&local_x);
  br_y->SetAddress(&y);
  br_local_y->SetAddress(&local_y);
  br_z->SetAddress(&z);
  br_local_z->SetAddress(&local_z);

  int n_protons = 0;
  for (int iEntry = 0; iEntry < tree->GetEntries(); ++iEntry) {

    tree->GetEvent(iEntry);

    for (int iElement = 0; iElement < oprocess->size();  ++iElement) {

      if (oprocess->at(iElement) == "NULL") {

	//std::cout << "A photon reached the Germanium!" << std::endl;
	//	target_positions->Fill(x->at(iElement), y->at(iElement), z->at(iElement));
	xz_plane->Fill(z->at(iElement), x->at(iElement));
      }
    }
  }
  //  target_positions->Draw("LEGO");
  xz_plane->Draw(draw_option.c_str());
  xz_plane->SetMarkerColor(colour);
}

