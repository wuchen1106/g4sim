#include <iostream>
#include <sstream>

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TF1.h"

struct Case {

  std::string casename;

  TCanvas* canvas;
  
  TH1F* hArrivalTime;
  TH1F* hArrivalTime_Aluminium;
  TH1F* hArrivalTime_Lead;
  TH1F* hArrivalTime_Silicon;
  TH1F* hArrivalTime_Other;
  TH1F* hArrivalTime_ScatteredMuons;

  double time_resolution;
  TF1* gaussian;
} TrueArrivals, ThickPulses, ThinPulses;

void LeadSystematic(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tree = (TTree*) file->Get("tree");

  TrueArrivals.casename = "true"; ThickPulses.casename = "thick"; ThinPulses.casename = "thin";
  TrueArrivals.time_resolution = 0; ThickPulses.time_resolution = 80; ThinPulses.time_resolution = 200;
  std::vector<Case> cases;
  cases.push_back(TrueArrivals);
  cases.push_back(ThickPulses);
  cases.push_back(ThinPulses);

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
  double min_time = -1000;
  double max_time = 5000;
  int n_bins = (max_time - min_time) / bin_width;

  // Loop through the different cases
  for (std::vector<Case>::iterator i_case = cases.begin(); i_case != cases.end(); ++i_case) {
    std::string canvasname = "c_" + i_case->casename;
    i_case->canvas = new TCanvas(canvasname.c_str(), canvasname.c_str());

    i_case->gaussian = new TF1("gaussian", "TMath::Gaus(x, 0, [0])", -1000,1000);
    i_case->gaussian->SetParameter(0, i_case->time_resolution);

    std::string basename = "hArrivalTime_" + i_case->casename;
    i_case->hArrivalTime = new TH1F(basename.c_str(), basename.c_str(), n_bins,min_time,max_time);
    std::string histname = basename + "_Aluminium";
    i_case->hArrivalTime_Aluminium = new TH1F(histname.c_str(), histname.c_str(), n_bins,min_time,max_time);
    histname = basename + "_Lead";
    i_case->hArrivalTime_Lead = new TH1F(histname.c_str(), histname.c_str(), n_bins,min_time,max_time);
    histname = basename + "_Silicon";
    i_case->hArrivalTime_Silicon = new TH1F(histname.c_str(), histname.c_str(), n_bins,min_time,max_time);
    histname = basename + "_Other";
    i_case->hArrivalTime_Other = new TH1F(histname.c_str(), histname.c_str(), n_bins,min_time,max_time);
    histname = basename + "_ScatteredMuons";
    i_case->hArrivalTime_ScatteredMuons = new TH1F(histname.c_str(), histname.c_str(), n_bins,min_time,max_time);

    i_case->hArrivalTime->SetLineColor(kBlack);
    i_case->hArrivalTime_Aluminium->SetLineColor(kBlue);
    i_case->hArrivalTime_Lead->SetLineColor(kRed);

  }

  int n_entries = tree->GetEntries();
  for (int i_entry = 0; i_entry < n_entries; ++i_entry) {
    tree->GetEvent(i_entry);
    
    if (i_entry % 1000000 == 0) {
      std::cout << i_entry << " / " << n_entries << std::endl;
    }
    
    for (int i_element = 0; i_element < particleName->size();  ++i_element) {
      
      std::string i_volName = volName->at(i_element);
      std::string i_ovolName = ovolName->at(i_element);
      std::string i_particleName = particleName->at(i_element);
      double i_Ot = Ot->at(i_element);
      
      // Loop through the cases
      for (std::vector<Case>::iterator i_case = cases.begin(); i_case != cases.end(); ++i_case) {
	double smear;
	if (i_case->gaussian->GetParameter(0) == 0) {
	  smear = 0;
	}
	else {
	  smear = i_case->gaussian->GetRandom();
	}
	//	std::cout << i_case->casename << ": smear = " << smear << std::endl;
	i_Ot += smear;

	if (i_volName=="ESi1" || i_volName=="dESi1" || i_volName=="ESi2" || i_volName=="dESi2") {
	  if (i_particleName=="proton") {
	
	    i_case->hArrivalTime->Fill(i_Ot);

	    if (i_ovolName == "Target") {
	      i_case->hArrivalTime_Aluminium->Fill(i_Ot);
	    }
	    else if (i_ovolName == "BackWall" || i_ovolName == "CollimatorLead" || i_ovolName == "TargetBorderShielding") {
	      i_case->hArrivalTime_Lead->Fill(i_Ot);
	    }
	    else if (i_ovolName == "ESi1" || i_ovolName == "dESi1" || i_ovolName == "ESi2" || i_ovolName == "dESi2") {
	      i_case->hArrivalTime_Silicon->Fill(i_Ot);
	    }
	    else {
	      i_case->hArrivalTime_Other->Fill(i_Ot);
	    }
	  }
	  else if (i_particleName=="mu-") {
	    i_case->hArrivalTime_ScatteredMuons->Fill(i_Ot);
	  }
	}
      } // end loop through cases
    }  // end loop through particles in this entry
  }// end loop through entries

  for (std::vector<Case>::iterator i_case = cases.begin(); i_case != cases.end(); ++i_case) {
    // Draw the plots
    i_case->hArrivalTime->Draw();
    i_case->hArrivalTime_Aluminium->Draw("SAME");
    i_case->hArrivalTime_Lead->Draw("SAME");

    // Save the canvases
    i_case->canvas->SetLogy(1);
    std::stringstream picname;
    picname << "LeadSystematic_" << i_case->casename << "_time-res-" << i_case->time_resolution << "ns_log-y";
    std::string pdfname = picname.str() + ".pdf";
    std::string pngname = picname.str() + ".png";
    i_case->canvas->Print(pdfname.c_str());
    i_case->canvas->Print(pngname.c_str());
  } // end loop through cases

  // Summmarise the reults right at the end
  for (std::vector<Case>::iterator i_case = cases.begin(); i_case != cases.end(); ++i_case) {
    // Determine the fraction of target and non-target within a time cut
    double low_time_cut = 100; // ns
    double high_time_cut = max_time;
    int low_integral_bin = i_case->hArrivalTime->FindBin(low_time_cut);
    int high_integral_bin = i_case->hArrivalTime->FindBin(high_time_cut);

    double total = i_case->hArrivalTime->GetEntries();
    double total_aluminium = i_case->hArrivalTime_Aluminium->GetEntries();
    double total_lead = i_case->hArrivalTime_Lead->GetEntries();
    double total_silicon = i_case->hArrivalTime_Silicon->GetEntries();
    double total_other = i_case->hArrivalTime_Other->GetEntries();
    double total_scattered_muons = i_case->hArrivalTime_ScatteredMuons->GetEntries();

    double total_in_cut = i_case->hArrivalTime->Integral(low_integral_bin, high_integral_bin);
    double aluminium_in_cut = i_case->hArrivalTime_Aluminium->Integral(low_integral_bin, high_integral_bin);
    double lead_in_cut = i_case->hArrivalTime_Lead->Integral(low_integral_bin, high_integral_bin);
    double silicon_in_cut = i_case->hArrivalTime_Silicon->Integral(low_integral_bin, high_integral_bin);
    double other_in_cut = i_case->hArrivalTime_Other->Integral(low_integral_bin, high_integral_bin);
    double scattered_muons_in_cut = i_case->hArrivalTime_ScatteredMuons->Integral(low_integral_bin, high_integral_bin);

    std::cout << i_case->casename << ": " << std::endl;

    std::cout << "\tEfficiency (Al) = " << aluminium_in_cut << " / " << total_aluminium << " = " << aluminium_in_cut / total_aluminium << std::endl;
    std::cout << "\tEfficiency (Pb) = " << lead_in_cut << " / " << total_lead << " = " << lead_in_cut / total_lead << std::endl;
    std::cout << "\tEfficiency (Si) = " << silicon_in_cut << " / " << total_silicon << " = " << silicon_in_cut / total_silicon << std::endl;
    std::cout << "\tEfficiency (other) = " << other_in_cut << " / " << total_other << " = " << other_in_cut / total_other << std::endl;
    std::cout << "\tEfficiency (mu-) = " << scattered_muons_in_cut << " / " << total_scattered_muons << " = " << scattered_muons_in_cut / total_scattered_muons << std::endl;
    std::cout << "\tPurity (Al) = " << aluminium_in_cut << " / " << total_in_cut<< " = " << aluminium_in_cut / total_in_cut << std::endl;
    std::cout << "\tPurity (Pb) = " << lead_in_cut << " / " << total_in_cut << " = " << lead_in_cut / total_in_cut << std::endl;
    std::cout << "\tPurity (Si) = " << silicon_in_cut << " / " << total_in_cut << " = " << silicon_in_cut / total_in_cut << std::endl;
    std::cout << "\tPurity (other) = " << other_in_cut << " / " << total_in_cut << " = " << other_in_cut / total_in_cut << std::endl;
    std::cout << "\tPurity (mu-) = " << scattered_muons_in_cut << " / " << total_in_cut << " = " << scattered_muons_in_cut / total_in_cut << std::endl;
    std::cout << "=======================================" << std::endl;
  }
}
