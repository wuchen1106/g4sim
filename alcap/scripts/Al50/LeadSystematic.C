#include "TFile.h"
#include "TTree.h"
#include "TH1.h"

void LeadSystematic(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tree = (TTree*) file->Get("tree");

  double bin_width = 50;
  double min_time = 0;
  double max_time = 5000;
  int n_bins = (max_time - min_time) / bin_width;

  TH1F* hArrivalTime = new TH1F("hArrivalTime", "hArrivalTime", n_bins,min_time,max_time);
  TH1F* hArrivalTime_ovolNameTarget = new TH1F("hArrivalTime_ovolNameTarget", "hArrivalTime_ovolNameTarget", n_bins,min_time,max_time);
  TH1F* hArrivalTime_ovolNameNotTarget = new TH1F("hArrivalTime_ovolNameNotTarget", "hArrivalTime_ovolNameNotTarget", n_bins,min_time,max_time);

  hArrivalTime->SetLineColor(kBlack);
  hArrivalTime_ovolNameTarget->SetLineColor(kBlue);
  hArrivalTime_ovolNameNotTarget->SetLineColor(kRed);

  int n_entries = tree->GetEntries();
  tree->Draw("M_t>>hArrivalTime", "M_volName==\"ESi1\" && M_particleName==\"proton\"", "", n_entries);
  tree->Draw("M_t>>hArrivalTime_ovolNameTarget", "M_ovolName==\"Target\" && M_volName==\"ESi1\" && M_particleName==\"proton\"", "SAME", n_entries);
  tree->Draw("M_t>>hArrivalTime_ovolNameNotTarget", "M_ovolName!=\"Target\" && M_volName==\"ESi1\" && M_particleName==\"proton\"", "SAME", n_entries);

}
