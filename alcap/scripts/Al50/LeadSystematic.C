#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TCanvas.h"

struct Case {

  std::string casename;

  TCanvas* canvas;
  
  TH1F* hArrivalTime;
  TH1F* hArrivalTime_ovolNameTarget;
  TH1F* hArrivalTime_ovolNameNotTarget;
} TrueArrivals, FastPulses, SlowPulses;

void LeadSystematic(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tree = (TTree*) file->Get("tree");

  TrueArrivals.casename = "true"; FastPulses.casename = "fast"; SlowPulses.casename = "slow";
  std::vector<Case> cases;
  cases.push_back(TrueArrivals);
  cases.push_back(FastPulses);
  cases.push_back(SlowPulses);

  std::vector<std::string>* particleName = 0;
  std::vector<std::string>* volName = 0;
  std::vector<std::string>* ovolName = 0;
  std::vector<double>* Ot = 0;
  
  TBranch* br_particleName = tree->GetBranch("M_particleName");
  TBranch* br_volName = tree->GetBranch("M_volName");
  TBranch* br_ovolName = tree->GetBranch("M_ovolName");
  TBranch* br_Ot = tree->GetBranch("M_Ot");
  
  br_particleName->SetAddress(&particleName);
  br_volName->SetAddress(&volName);
  br_ovolName->SetAddress(&ovolName);
  br_Ot->SetAddress(&Ot);


  double bin_width = 50;
  double min_time = 0;
  double max_time = 5000;
  int n_bins = (max_time - min_time) / bin_width;

  // Loop through the different cases
  for (std::vector<Case>::iterator i_case = cases.begin(); i_case != cases.end(); ++i_case) {
    std::string canvasname = "c_" + i_case->casename;
    i_case->canvas = new TCanvas(canvasname.c_str(), canvasname.c_str());

    std::string histname = "hArrivalTime_" + i_case->casename;
    i_case->hArrivalTime = new TH1F(histname.c_str(), histname.c_str(), n_bins,min_time,max_time);
    histname += "_ovolNameTarget";
    i_case->hArrivalTime_ovolNameTarget = new TH1F(histname.c_str(), histname.c_str(), n_bins,min_time,max_time);
    histname += "_not";
    i_case->hArrivalTime_ovolNameNotTarget = new TH1F(histname.c_str(), histname.c_str(), n_bins,min_time,max_time);

    i_case->hArrivalTime->SetLineColor(kBlack);
    i_case->hArrivalTime_ovolNameTarget->SetLineColor(kBlue);
    i_case->hArrivalTime_ovolNameNotTarget->SetLineColor(kRed);

    int n_entries = 10000;
    for (int i_entry = 0; i_entry < n_entries; ++i_entry) {
      tree->GetEvent(i_entry);
     
      if (i_entry % 10000 == 0) {
	std::cout << i_entry << " / " << n_entries << std::endl;
      }
    
      for (int i_element = 0; i_element < particleName->size();  ++i_element) {

	std::string i_volName = volName->at(i_element);
	std::string i_ovolName = ovolName->at(i_element);
	std::string i_particleName = particleName->at(i_element);
	double i_Ot = Ot->at(i_element);

	if (i_volName=="ESi1" && i_particleName=="proton") {
	
	  i_case->hArrivalTime->Fill(i_Ot);

	  if (i_ovolName == "Target") {
	    i_case->hArrivalTime_ovolNameTarget->Fill(i_Ot);
	  }
	  else {
	    i_case->hArrivalTime_ovolNameNotTarget->Fill(i_Ot);
	  }
	}
      } // end loop through particles in this entry
    } // end loop through entries

    // Draw the plots
    i_case->hArrivalTime->Draw();
    i_case->hArrivalTime_ovolNameTarget->Draw("SAME");
    i_case->hArrivalTime_ovolNameNotTarget->Draw("SAME");
  } // end loop through cases
}
