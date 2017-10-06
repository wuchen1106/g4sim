#include "TH1F.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLegend.h"

#include <iostream>

void GeometryTolerances(std::string filename) {

  // Get the basefilename so we can name otuput files
  size_t secondslash = filename.rfind("/")+1; // don't want to include the slash                                                                      
  size_t firstdot = filename.rfind(".");
  size_t n_chars = firstdot - secondslash;
  std::string base_filename = filename.substr(secondslash, n_chars);

  std::string plotfilename = "plot_" + base_filename + ".root";
  TFile* plot_output = new TFile(plotfilename.c_str(), "RECREATE");

  // Prepare to read the results in from a text file
  TTree* tree = new TTree();
  tree->ReadFile(filename.c_str(), "setting/C:n_muons/I:n_protons_right/I:n_protons_left/I");
  //  tree->Print();

  char* setting_name = new char[256];
  int n_muons, n_protons_right, n_protons_left;

  TBranch* br_setting_name = tree->GetBranch("setting");
  TBranch* br_n_muons = tree->GetBranch("n_muons");
  TBranch* br_n_protons_right = tree->GetBranch("n_protons_right");
  TBranch* br_n_protons_left = tree->GetBranch("n_protons_left");

  br_setting_name->SetAddress(setting_name);
  br_n_muons->SetAddress(&n_muons);
  br_n_protons_right->SetAddress(&n_protons_right);
  br_n_protons_left->SetAddress(&n_protons_left);

  // Add branches for storing intermediate calculations
  double ratio_all_protons;
  double error_all_protons;
  double ratio_right_protons;
  double error_right_protons;
  double ratio_left_protons;
  double error_left_protons;
  double asymmetry;
  double error_asymmetry;

  TBranch* br_ratio_all_protons = tree->Branch("ratio_all_protons", &ratio_all_protons, "ratio_all_protons/D");
  TBranch* br_error_all_protons = tree->Branch("error_all_protons", &error_all_protons, "error_all_protons/D");
  TBranch* br_ratio_right_protons = tree->Branch("ratio_right_protons", &ratio_right_protons, "ratio_right_protons/D");
  TBranch* br_error_right_protons = tree->Branch("error_right_protons", &error_right_protons, "error_right_protons/D");
  TBranch* br_ratio_left_protons = tree->Branch("ratio_left_protons", &ratio_left_protons, "ratio_left_protons/D");
  TBranch* br_error_left_protons = tree->Branch("error_left_protons", &error_left_protons, "error_left_protons/D");
  TBranch* br_asymmetry = tree->Branch("asymmetry", &asymmetry, "asymmetry/D");
  TBranch* br_error_asymmetry = tree->Branch("error_asymmetry", &error_asymmetry, "error_asymmetry/D");


  // Create the histograms
  int n_settings = tree->GetEntries();
  TH1F* all_protons  = new TH1F("Histogram_all", "Plot of the number of stopped protons per muon in both SiR2 and SiL2 for different positions of target", n_settings,0,n_settings);
  TH1F* right_protons  = new TH1F("Histogram_right", "Plot of the number of stopped protons per muon in SiR2 (ESi1) for different positions of target", n_settings,0,n_settings);
  TH1F* left_protons  = new TH1F("Histogram_left", "Plot of the number of stopped protons per muon in SiL2 (ESi2) for different positions of target", n_settings,0,n_settings);

  TH1F* asymmetries = new TH1F("Asymmetries", "Plot of the asymmetry between left and right silicon arms", n_settings,0,n_settings);

  // Read the results from a text file
  for (int i_entry = 0; i_entry < n_settings; ++i_entry) {
    tree->GetEntry(i_entry);

    int n_all_protons = n_protons_left + n_protons_right;
    ratio_all_protons = (double)n_all_protons / (double)n_muons;
    error_all_protons = sqrt(n_all_protons) / (double)n_muons;
    all_protons->Fill(setting_name, ratio_all_protons);
    all_protons->SetBinError(i_entry+1, error_all_protons); // bins are numbered 1 to n

    ratio_right_protons = (double)n_protons_right / (double)n_muons;
    error_right_protons = sqrt(n_protons_right) / (double)n_muons;
    right_protons->Fill(setting_name, ratio_right_protons);
    right_protons->SetBinError(i_entry+1, error_right_protons); // bins are numbered 1 to n

    ratio_left_protons = (double)n_protons_left / (double)n_muons;
    error_left_protons = sqrt(n_protons_left) / (double)n_muons;
    left_protons->Fill(setting_name, ratio_left_protons);
    left_protons->SetBinError(i_entry+1, error_left_protons); // bins are numbered 1 to n 

    double difference = n_protons_left - n_protons_right;
    asymmetry = difference / (double)n_all_protons;
    error_asymmetry = asymmetry*asymmetry*( (n_all_protons / (difference*difference)) +
					    (1.0 / n_all_protons) );
    error_asymmetry = std::sqrt(error_asymmetry);
    asymmetries->Fill(setting_name, asymmetry);
    asymmetries->SetBinError(i_entry+1, error_asymmetry);
    tree->Fill();
  }
  //  tree->Print();


  all_protons->SetStats(false);
  right_protons->SetStats(false);
  left_protons->SetStats(false);

  all_protons->GetYaxis()->SetTitle("N_{p} / N_{#mu-stop}");
  all_protons->GetYaxis()->SetLabelSize(0.025);
  all_protons->GetYaxis()->SetRangeUser(0,0.004);

  all_protons->SetLineColor(kBlack);
  all_protons->SetLineWidth(2);
  right_protons->SetLineColor(kRed);
  right_protons->SetLineWidth(2);
  left_protons->SetLineColor(kBlue);
  left_protons->SetLineWidth(2);

  TCanvas* c1 = new TCanvas("c1", "c1");
  all_protons->Draw("HIST E");
  right_protons->Draw("SAME E");
  left_protons->Draw("SAME E");

  TLegend* legend = new TLegend(0.3, 0.85, 0.6, 0.75, "");
  legend->SetBorderSize(0);
  legend->SetTextSize(0.04);
  legend->SetFillColor(kWhite);
  legend->AddEntry(all_protons, "Protons in SiR2 and SiL2", "l");
  legend->AddEntry(right_protons, "Protons in SiR2", "l");
  legend->AddEntry(left_protons, "Protons in SiL2", "l");
  legend->Draw();
  c1->Update();

  double base_ratio = 0;
  double base_asymmetry = 0;
  for (int i_entry = 0; i_entry < n_settings; ++i_entry) {
    tree->GetEntry(i_entry);

    if (strcmp(setting_name, "base") == 0) {
      base_ratio = ratio_all_protons;
      base_asymmetry = asymmetry;
    }
    std::cout << "**Setting: " << setting_name << " ** " << std::endl;
    std::cout << "N_mu = " << n_muons << std::endl;
    std::cout << "N_protons_left = " << n_protons_left << std::endl;
    std::cout << "N_protons_right = " << n_protons_right << std::endl << std::endl;

    std::cout << "N_protons / N_mu = " << ratio_all_protons << std::endl;


    double difference = ratio_all_protons - base_ratio;
    double diff_over_base = difference / base_ratio;
    double squared_total = (diff_over_base*diff_over_base);
    std::cout << "Fractional Difference from \"base\" = " << diff_over_base << std::endl << std::endl;

    double fractional_asymmetry_change = (asymmetry - base_asymmetry)/base_asymmetry;
    std::cout << "Asymmetry = " << asymmetry << " +- " << error_asymmetry << std::endl;
    std::cout << "Fractional Difference from \"base\" = " << fractional_asymmetry_change << std::endl << std::endl;
  }

  TCanvas* c2 = new TCanvas("c2", "c2");
  asymmetries->SetStats(false);
  asymmetries->GetYaxis()->SetTitle("Asymmetry");
  asymmetries->SetLineWidth(2);
  asymmetries->Draw("HIST E");
  c2->Update();
  //  std::cout << "Total in quadrature = " << sqrt(squared_total) << std::endl;

  all_protons->Write();
  left_protons->Write();
  right_protons->Write();
  asymmetries->Write();
}
