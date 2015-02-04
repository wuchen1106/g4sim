#include "TLegend.h"
#include "TH1.h"
#include "TH2.h"
#include "TSpectrum.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include <iostream>
#include <sstream>

void StoppedMuonVolumes() {

  const int n_files = 3;

  // Define the time cuts we want to make
  double prompt_time = 200; // ns
  double capture_time = 2000; // ns

  // Create the canvases
  TCanvas* canvases[n_files];
  for (int i_file = 0; i_file < n_files; ++i_file) {
    std::stringstream canvasname;
    canvasname << "c" << i_file << std::endl;
    canvases[i_file] = new TCanvas(canvasname.str().c_str());
  }

  // Get the files we want
  TFile* files[n_files];
  files[0] = new TFile("GeSpectrum_woutThetaSpread.root", "READ");
  files[1] = new TFile("GeSpectrum_woutThetaSpread_wNarrowMomSpread.root", "READ");
  files[2] = new TFile("GeSpectrum_woutThetaSpread_wNarrowMomSpread_wHigherMom.root", "READ");

  TTree* trees[n_files];
  for (int i_file = 0; i_file < n_files; ++i_file) {
    trees[i_file] = (TTree*) files[i_file]->Get("tree");
  }

  
  // Get the branches we want from the tree
  std::vector<std::string>* particleName[n_files];
  std::vector<std::string>* volName[n_files];
  std::vector<double>* edep[n_files];
  std::vector<double>* Ot[n_files];
  std::vector<std::string>* ovolName[n_files];
  std::vector<std::string>* oprocess[n_files];

  for (int i_file = 0; i_file < n_files; ++i_file) {
    particleName[i_file] = 0;
    volName[i_file] = 0;
    edep[i_file] = 0;
    Ot[i_file] = 0;
    ovolName[i_file] = 0;
    oprocess[i_file] = 0;
  }
  
  TBranch* br_particleName[n_files];
  TBranch* br_volName[n_files]; 
  TBranch* br_edep[n_files];
  TBranch* br_Ot[n_files];
  TBranch* br_ovolName[n_files];
  TBranch* br_oprocess[n_files];

  for (int i_file = 0; i_file < n_files; ++i_file) {
    br_particleName[i_file] = trees[i_file]->GetBranch("M_particleName");
    br_volName[i_file] = trees[i_file]->GetBranch("M_volName");
    br_edep[i_file] = trees[i_file]->GetBranch("M_edep");
    br_Ot[i_file] = trees[i_file]->GetBranch("M_Ot"); // post-step point
    br_ovolName[i_file] = trees[i_file]->GetBranch("M_ovolName");
    br_oprocess[i_file] = trees[i_file]->GetBranch("M_oprocess");

    br_particleName[i_file]->SetAddress(&particleName[i_file]);
    br_volName[i_file]->SetAddress(&volName[i_file]);
    br_edep[i_file]->SetAddress(&edep[i_file]);
    br_Ot[i_file]->SetAddress(&Ot[i_file]);
    br_ovolName[i_file]->SetAddress(&ovolName[i_file]);
    br_oprocess[i_file]->SetAddress(&oprocess[i_file]);
  }

  TH1F* histograms[n_files];
  histograms[0] = new TH1F("woutThetaSpread", "woutThetaSpread", 20,-1,0);
  histograms[0]->SetLineColor(kBlack);
  histograms[1] = new TH1F("wNarrowMomSpread", "wNarrowSpread", 20,-1,0);
  histograms[1]->SetLineColor(kRed);
  histograms[2] = new TH1F("wHigherMom", "wHigherMom", 20,-1,0);
  histograms[2]->SetLineColor(kBlue);

  // Get the energy resolution smear ready
  double resolution = 2; // keV
  TF1* gaussian = new TF1("gaus", "TMath::Gaus(x,0,[0])",-10,10);
  gaussian->SetParameter(0, resolution);

  // Loop through the files
  for (int i_file = 0; i_file < n_files; ++i_file) {

    // Fill the MC histograms
    for (int iEntry = 0; iEntry < trees[i_file]->GetEntries(); ++iEntry) {
      trees[i_file]->GetEvent(iEntry);

      double muSc_time = 0;
      bool have_muSc_hit = false;
      for (int iElement = 0; iElement < particleName[i_file]->size();  ++iElement) {

	if (have_muSc_hit == false) {
	  if (particleName[i_file]->at(iElement) == "mu-" && volName[i_file]->at(iElement) == "MuSC") {
	    muSc_time = Ot[i_file]->at(iElement);
	    have_muSc_hit = true;
	  }
	}
	else {
	  if (particleName[i_file]->at(iElement) == "gamma" && volName[i_file]->at(iElement) == "Ge") {
	    //std::cout << "A photon reached the Germanium!" << std::endl;
	    if (edep[i_file]->at(iElement)*1e6 != 0) {
	      double time = Ot[i_file]->at(iElement) - muSc_time;

	      double energy = edep[i_file]->at(iElement)*1e6;
	      double smear = gaussian->GetRandom();// Need to add a Gaussian smear
	      energy += smear;
	    
	      // Now just need to fill different plots
	      if (time < prompt_time) {
		if (energy >= 60 && energy <= 80) {
		  histograms[i_file]->Fill( (ovolName[i_file]->at(iElement)).c_str(), 1);
		}
		else if (energy >= 85 && energy <= 105) {
		  histograms[i_file]->Fill( (ovolName[i_file]->at(iElement)).c_str(), 1);
		}

		if (energy >= 120 && energy <= 135) {
		  histograms[i_file]->Fill( (ovolName[i_file]->at(iElement)).c_str(), 1);
		}

	      }
	      /*	      else if (time > prompt_time && time < capture_time) {
		hGeSpectrum_capture_mc->Fill(energy);
	      }
	      else if (time > capture_time) {
		hGeSpectrum_delayed_mc->Fill(energy);
	      }
	      */
	      //	  std::cout << "Low energy photon: E_dep = " << edep->at(iElement)*1e6 << std::endl;
	    }
	  }
	}
      }
    }
  }

  for (int i_file = 0; i_file < n_files; ++i_file) {
    canvases[i_file]->cd();
    histograms[i_file]->Draw();
  }

  // Add a legend
  TLegend* legend = new TLegend(0.3, 0.85, 0.6, 0.75, "");
  legend->SetBorderSize(0);
  legend->SetTextSize(0.04);
  legend->SetFillColor(kWhite);

  legend->AddEntry(histograms[0], "w/out Theta Spread", "l");
  legend->AddEntry(histograms[1], "w/ Narrower Momentum Spread", "l");
  legend->AddEntry(histograms[2], "w/ Higher Momentum", "l");
  legend->Draw();
}
