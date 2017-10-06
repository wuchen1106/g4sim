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

// This ROOT macro counts the number of photons that hit the Germanium
void GeSpectrum() {

  // Define the time cuts we want to make
  double prompt_time = 200; // ns
  double capture_time = 2000; // ns

  // Create the canvases
  TCanvas* c_all = new TCanvas("c_all", "c_all");
  TCanvas* c_prompt = new TCanvas("c_prompt", "c_prompt");
  TCanvas* c_capture = new TCanvas("c_capture", "c_capture");
  TCanvas* c_delayed = new TCanvas("c_delayed", "c_delayed");

  // Get the MC file and tree
  TFile* file = new TFile("GeSpectrum_Al50_2014-10-30.root", "READ");
  TTree* tree = (TTree*) file->Get("tree");

  // Get the branches we want from the tree
  std::vector<std::string>* particleName = 0;
  std::vector<std::string>* volName = 0;
  std::vector<double>* edep = 0;
  std::vector<double>* Ot = 0;
  std::vector<std::string>* ovolName = 0;
  std::vector<std::string>* oprocess = 0;
  
  TBranch* br_particleName = tree->GetBranch("M_particleName");
  TBranch* br_volName = tree->GetBranch("M_volName");
  TBranch* br_edep = tree->GetBranch("M_edep");
  TBranch* br_Ot = tree->GetBranch("M_Ot"); // post-step point
  TBranch* br_ovolName = tree->GetBranch("M_ovolName");
  TBranch* br_oprocess = tree->GetBranch("M_oprocess");

  br_particleName->SetAddress(&particleName);
  br_volName->SetAddress(&volName);
  br_edep->SetAddress(&edep);
  br_Ot->SetAddress(&Ot);
  br_ovolName->SetAddress(&ovolName);
  br_oprocess->SetAddress(&oprocess);

  // Create the MC histograms
  std::stringstream histtitle_base;
  std::stringstream histtitle;
  histtitle_base << "Plot of MC Germanium Spectrum";
  histtitle << histtitle_base.str();
  TH1F* hGeSpectrum_all_mc = new TH1F("hGeSpectrum_all_mc", histtitle.str().c_str(), 2000,0,2000);
  histtitle.str("");

  histtitle << histtitle_base.str() << " (t < " << prompt_time << " ns)";
  TH1F* hGeSpectrum_prompt_mc = new TH1F("hGeSpectrum_prompt_mc", histtitle.str().c_str(), 2000,0,2000);
  histtitle.str("");

  histtitle << histtitle_base.str() << " (" << prompt_time << " t < " << capture_time << " ns)";
  TH1F* hGeSpectrum_capture_mc = new TH1F("hGeSpectrum_capture_mc", histtitle.str().c_str(), 2000,0,2000);
  histtitle.str("");

  histtitle << histtitle_base.str() << " (t > " << capture_time << " ns)";
  TH1F* hGeSpectrum_delayed_mc = new TH1F("hGeSpectrum_delayed_mc", histtitle.str().c_str(), 2000,0,2000);
  histtitle.str("");

  // Start counting photons
  int n_photons = 0;

  // Get the energy resolution smear ready
  double resolution = 2; // keV
  TF1* gaussian = new TF1("gaus", "TMath::Gaus(x,0,[0])",-10,10);
  gaussian->SetParameter(0, resolution);

  // Fill the MC histograms
  for (int iEntry = 0; iEntry < tree->GetEntries(); ++iEntry) {

    tree->GetEvent(iEntry);
    double muSc_time = 0;
    bool have_muSc_hit = false;
    for (int iElement = 0; iElement < particleName->size();  ++iElement) {

      if (have_muSc_hit == false) {
	if (particleName->at(iElement) == "mu-" && volName->at(iElement) == "MuSC") {
	  muSc_time = Ot->at(iElement);
	  have_muSc_hit = true;
	}
      }
      else {
	if (particleName->at(iElement) == "gamma" && volName->at(iElement) == "Ge") {
	  //std::cout << "A photon reached the Germanium!" << std::endl;
	  if (edep->at(iElement)*1e6 != 0) {
	    double time = Ot->at(iElement) - muSc_time;

	    double energy = edep->at(iElement)*1e6;
	    double smear = gaussian->GetRandom();// Need to add a Gaussian smear
	    energy += smear;
	    
	    // Now just need to fill different plots
	    hGeSpectrum_all_mc->Fill(energy);
	    if (time < prompt_time) {
	      hGeSpectrum_prompt_mc->Fill(energy);
	    }
	    else if (time > prompt_time && time < capture_time) {
	      hGeSpectrum_capture_mc->Fill(energy);
	    }
	    else if (time > capture_time) {
	      hGeSpectrum_delayed_mc->Fill(energy);
	    }
	    //	  std::cout << "Low energy photon: E_dep = " << edep->at(iElement)*1e6 << std::endl;
	  }
	  ++n_photons;
	}
      }
    }
  }

  // Get the number of MC muons and scale the MC histograms
  double n_muons_mc = tree->GetEntries();
  hGeSpectrum_all_mc->Scale(1.0/n_muons_mc);
  hGeSpectrum_prompt_mc->Scale(1.0/n_muons_mc);
  hGeSpectrum_capture_mc->Scale(1.0/n_muons_mc);
  hGeSpectrum_delayed_mc->Scale(1.0/n_muons_mc);






  // Get the file with the data plots in
  TFile* data = new TFile("GeSpectrumData.root", "READ");

  // Get the data histograms
  // NB: We need to loop through and add the values one-by-one
  // since the histograms in the file are TH1Is which
  // were getting messed up when using the Scale function
  TH1F* hGeSpectrum_all_data = (TH1F*) data->Get("GeSpectrum/hEnergy")->Clone();
  //  TH1F* hGeSpectrum_all_data = new TH1F("hGeSpectrum_all_data", "All photons in Ge data", 2000,0,2000);
  //  for (int i_bin = 1; i_bin <= hJohnGeSpectrum_all_data->GetNbinsX(); ++i_bin) {
  //    hGeSpectrum_all_data->Fill(hJohnGeSpectrum_all_data->GetBinCenter(i_bin), hJohnGeSpectrum_all_data->GetBinContent(i_bin));
  // }
  TH1D* hGeSpectrum_prompt_data = ((TH2D*) data->Get("GeSpectrum/hTimeEnergy"))->ProjectionY();
  //  TH1F* hGeSpectrum_prompt_data = new TH1F("hGeSpectrum_prompt_data", "Prompt photons in Ge data", 2000,0,2000);
  //  for (int i_bin = 1; i_bin <= hJohnGeSpectrum_prompt_data->GetNbinsX(); ++i_bin) {
  //    hGeSpectrum_prompt_data->Fill(hJohnGeSpectrum_prompt_data->GetBinCenter(i_bin), hJohnGeSpectrum_prompt_data->GetBinContent(i_bin));
  //  }
  TH1F* hGeSpectrum_capture_data = (TH1F*) data->Get("GeSpectrum/hEnergyOOT")->Clone();
  //  TH1F* hGeSpectrum_capture_data = new TH1F("hGeSpectrum_capture_data", "Capture photons in Ge data", 2000,0,2000);
  //  for (int i_bin = 1; i_bin <= hJohnGeSpectrum_capture_data->GetNbinsX(); ++i_bin) {
  //    hGeSpectrum_capture_data->Fill(hJohnGeSpectrum_capture_data->GetBinCenter(i_bin), hJohnGeSpectrum_capture_data->GetBinContent(i_bin));
  //  }
  TH1F* hGeSpectrum_delayed_data = (TH1F*) data->Get("GeSpectrum/hEnergyFarOOT")->Clone();
  //  TH1F* hGeSpectrum_delayed_data = new TH1F("hGeSpectrum_delayed_data", "Delayed photons in Ge data", 2000,0,2000);
  //  for (int i_bin = 1; i_bin <= hJohnGeSpectrum_delayed_data->GetNbinsX(); ++i_bin) {
  //    hGeSpectrum_delayed_data->Fill(hJohnGeSpectrum_delayed_data->GetBinCenter(i_bin), hJohnGeSpectrum_delayed_data->GetBinContent(i_bin));
  //  }

  // Get the number of muons in the data and normalise the axes
  double n_muons_data = 2.76e7; // 3.9e8
  hGeSpectrum_all_data->Scale(1.0/n_muons_data);
  hGeSpectrum_prompt_data->Scale(1.0/n_muons_data);
  hGeSpectrum_capture_data->Scale(1.0/n_muons_data);
  hGeSpectrum_delayed_data->Scale(1.0/n_muons_data);


  // Set axis titles
  hGeSpectrum_all_data->GetXaxis()->SetTitle("Energy [keV]");
  hGeSpectrum_prompt_data->GetXaxis()->SetTitle("Energy [keV]");
  hGeSpectrum_capture_data->GetXaxis()->SetTitle("Energy [keV]");
  hGeSpectrum_delayed_data->GetXaxis()->SetTitle("Energy [keV]");
  hGeSpectrum_all_data->GetYaxis()->SetTitle("Number of Photons [per stopped muon]");
  hGeSpectrum_prompt_data->GetYaxis()->SetTitle("Number of Photons [per stopped muon]");
  hGeSpectrum_capture_data->GetYaxis()->SetTitle("Number of Photons [per stopped muon]");
  hGeSpectrum_delayed_data->GetYaxis()->SetTitle("Number of Photons [per stopped muon]");

  hGeSpectrum_all_mc->GetXaxis()->SetTitle("Energy [keV]");
  hGeSpectrum_prompt_mc->GetXaxis()->SetTitle("Energy [keV]");
  hGeSpectrum_capture_mc->GetXaxis()->SetTitle("Energy [keV]");
  hGeSpectrum_delayed_mc->GetXaxis()->SetTitle("Energy [keV]");
  hGeSpectrum_all_mc->GetYaxis()->SetTitle("Number of Photons [per stopped muon]");
  hGeSpectrum_prompt_mc->GetYaxis()->SetTitle("Number of Photons [per stopped muon]");
  hGeSpectrum_capture_mc->GetYaxis()->SetTitle("Number of Photons [per stopped muon]");
  hGeSpectrum_delayed_mc->GetYaxis()->SetTitle("Number of Photons [per stopped muon]");

  // May want to reset axes in a bit
  hGeSpectrum_all_data->Rebin(2);
  hGeSpectrum_prompt_data->Rebin(2);
  hGeSpectrum_capture_data->Rebin(2);
  hGeSpectrum_delayed_data->Rebin(2);
  hGeSpectrum_all_mc->Rebin(2);
  hGeSpectrum_prompt_mc->Rebin(2);
  hGeSpectrum_capture_mc->Rebin(2);
  hGeSpectrum_delayed_mc->Rebin(2);

  hGeSpectrum_all_data->GetXaxis()->SetRangeUser(50,1800);
  hGeSpectrum_prompt_data->GetXaxis()->SetRangeUser(50,1800);
  hGeSpectrum_capture_data->GetXaxis()->SetRangeUser(50,1800);
  hGeSpectrum_delayed_data->GetXaxis()->SetRangeUser(50,1800);
  hGeSpectrum_all_mc->GetXaxis()->SetRangeUser(50,1800);
  hGeSpectrum_prompt_mc->GetXaxis()->SetRangeUser(50,1800);
  hGeSpectrum_capture_mc->GetXaxis()->SetRangeUser(50,1800);
  hGeSpectrum_delayed_mc->GetXaxis()->SetRangeUser(50,1800);


  // May want to background subtract in a bit
  //  TSpectrum* spectrum = new TSpectrum();
  //  TH1* hBackground = spectrum->Background(hGeSpectrum_all_data, 5);
  //  hGeSpectrum_all_data->Add(hBackground, -1);


  // Now put everything on the canvases
  c_all->cd();
  hGeSpectrum_all_mc->SetLineColor(kRed);
  hGeSpectrum_all_data->Draw();
  hGeSpectrum_all_mc->Draw("SAMES");

  c_prompt->cd();
  hGeSpectrum_prompt_mc->SetLineColor(kRed);
  hGeSpectrum_prompt_data->Draw();
  hGeSpectrum_prompt_mc->Draw("SAMES");

  c_capture->cd();
  hGeSpectrum_capture_mc->SetLineColor(kRed);
  hGeSpectrum_capture_data->Draw();
  hGeSpectrum_capture_mc->Draw("SAMES");

  c_delayed->cd();
  hGeSpectrum_delayed_mc->SetLineColor(kRed);
  hGeSpectrum_delayed_data->Draw();
  hGeSpectrum_delayed_mc->Draw("SAMES");


  // Add a legend
  TLegend* legend = new TLegend(0.3, 0.85, 0.6, 0.75, "");
  legend->SetBorderSize(0);
  legend->SetTextSize(0.04);
  legend->SetFillColor(kWhite);

  legend->AddEntry(hGeSpectrum_all_data, "Data", "l");
  legend->AddEntry(hGeSpectrum_all_mc, "MC", "l");
  //  legend->AddEntry(hBackground, "Background", "l");
  /*  legend->AddEntry(hGeSpectrum_all_mc, "All", "l");
  std::stringstream legendlabel;
  legendlabel << "Prompt (t < " << prompt_time << " ns)";
  legend->AddEntry(hGeSpectrum_prompt_mc, legendlabel.str().c_str(), "l");
  legendlabel.str("");

  legendlabel << "Capture (" << prompt_time << " ns < t < " << capture_time << " ns)";
  legend->AddEntry(hGeSpectrum_capture_mc, legendlabel.str().c_str(), "l");
  legendlabel.str("");

  legendlabel << "Delayed (t > " << capture_time << " ns)" << std::endl;
  legend->AddEntry(hGeSpectrum_delayed_mc, legendlabel.str().c_str(), "l");
  legendlabel.str("");
  */

  c_all->cd();
  legend->Draw();
  c_prompt->cd();
  legend->Draw();
  c_capture->cd();
  legend->Draw();
  c_delayed->cd();
  legend->Draw();
}
