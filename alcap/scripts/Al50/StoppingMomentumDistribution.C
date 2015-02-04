#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "THnSparse.h"
#include "TBranch.h"
#include "TLegend.h"

#include <iostream>

void StoppingMomentumDistribution(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tree = (TTree*) file->Get("tree");
  
  double bin_width = 0.5;
  double momentum_min = 0;
  double momentum_max = 30; // MeV
  int n_bins = (momentum_max - momentum_min) / bin_width;

  /*  TH1F* hInitialMomentum = new TH1F("hInitialMomentum", "Momentum of Muons Initial on Target", n_bins,momentum_min,momentum_max);
  hInitialMomentum->SetXTitle("Momentum [keV]");
  hInitialMomentum->SetYTitle("Number of Muons");
  hInitialMomentum->SetStats(false);
  hInitialMomentum->SetLineWidth(2);
  //  hInitialMomentum->SetZTitle("Local Z [#mum]");
  tree->Draw("sqrt(i_px*i_px + i_py*i_py + i_pz*i_pz)>>hInitialMomentum");
  */

  TH1F* hIncidentMomentum = new TH1F("hIncidentMomentum", "Momentum of Muons Incident on Target", n_bins,momentum_min,momentum_max);
  hIncidentMomentum->SetXTitle("Momentum [MeV]");
  hIncidentMomentum->SetYTitle("Count per Input Muon");
  hIncidentMomentum->SetStats(false);
  hIncidentMomentum->SetLineWidth(2);
  //  hIncidentMomentum->SetZTitle("Local Z [#mum]");
  tree->Draw("sqrt(M_px*M_px + M_py*M_py + M_pz*M_pz)*1e3>>hIncidentMomentum", "M_particleName==\"mu-\" && M_volName==\"Target\"", "SAME");

  TH1F* hStoppedMomentum = new TH1F("hStoppedMomentum", "Momentum of Muons Stopped in Target", n_bins,momentum_min,momentum_max);
  hStoppedMomentum->SetXTitle("Momentum [MeV]");
  hStoppedMomentum->SetYTitle("Count per Input Muon");
  hStoppedMomentum->SetFillColor(kSpring);
  //  hStoppedMomentum->SetZTitle("Local Z [#mum]");
  tree->Draw("sqrt(M_px*M_px + M_py*M_py + M_pz*M_pz)*1e3>>hStoppedMomentum", "M_particleName==\"mu-\" && M_volName==\"Target\" && M_stopped==1", "SAME");

  hIncidentMomentum->Scale(1.0 / tree->GetEntries());
  hStoppedMomentum->Scale(1.0 / tree->GetEntries());

  hIncidentMomentum->SetTitle("Momentum of Muons at Target");

  TLegend* legend = new TLegend(0.14, 0.85, 0.44, 0.75, "");
  legend->SetBorderSize(0);
  legend->SetTextSize(0.04);
  legend->SetFillColor(kWhite);
  legend->AddEntry(hIncidentMomentum, "Incident Muons", "l");
  legend->AddEntry(hStoppedMomentum, "Stopped Muons", "f");
  legend->Draw();
  //  c1->Update();


  // NB Useful if the MC was run in muPC mode
  double n_stopped = hStoppedMomentum->GetEntries();
  double error_n_stopped = std::sqrt(n_stopped);
  double n_input = tree->GetEntries();
  double fraction_stopped = n_stopped / n_input;
  double error_fraction_stopped = fraction_stopped * sqrt((error_n_stopped/n_stopped)*(error_n_stopped/n_stopped));
  std::cout << "Fraction of mu-stopped = " << n_stopped << " / " << n_input 
	    << " = " << fraction_stopped << " +- " << error_fraction_stopped << std::endl;
}
