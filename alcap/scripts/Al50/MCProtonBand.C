#include <iostream>

#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"

struct Arm{
  std::string armname;
  TH2F* hEvdEAll;
  TH2F* hEvdEBand;
  TH1F* hBandProfile;

  TH2F* hEvdEStoppedProtons;

  int n_selected;

  int n_trough; // the number of entries in the trough between peaks
  int peak_to_take; // which peak should we take (first, second, third etc.)
} LeftArm, RightArm;

void ProtonBand(std::string id, std::string filename, std::string baseplotname, int n_trough, int peak_to_take);
int GetNPeaks(TH1* hist, int n_trough);
int FindNextPeak(TH1* hist, int start_bin, int n_trough);

void DataAndMC() {
  // Data
  ProtonBand("data", "~/data/out/v92/total.root", "TME_Al50_EvdE/all_particles/ARM_EvdE", 20, 1);
  
  // MC
  ProtonBand("MC", "plots.root", "hAll_EvdE_ARM", 0, 2);
}

void ProtonBand(std::string id, std::string filename, std::string baseplotname, int n_trough, int peak_to_take) {

  double low_energy_cut = 2000; // keV
  double high_energy_cut = 10000; // keV
  
  LeftArm.armname = "SiL"; RightArm.armname = "SiR";
  LeftArm.n_trough = n_trough; RightArm.n_trough = n_trough;
  LeftArm.peak_to_take = peak_to_take; RightArm.peak_to_take = peak_to_take;
  std::vector<Arm> arms;
  arms.push_back(LeftArm); arms.push_back(RightArm);

  TFile* file = new TFile(filename.c_str(), "READ");
  
  for (std::vector<Arm>::iterator i_arm = arms.begin(); i_arm != arms.end(); ++i_arm) {
    std::string canvasname = "c_" + id + "_" + i_arm->armname;
    TCanvas* c = new TCanvas(canvasname.c_str(), canvasname.c_str());
    i_arm->n_selected = 0;
    
    std::string histname = baseplotname;
    histname.replace(baseplotname.find("ARM"), 3, i_arm->armname);
    i_arm->hEvdEAll = (TH2F*) file->Get(histname.c_str());
    //    i_arm->hEvdEAll->RebinX(2);
    //    i_arm->hEvdEAll->RebinY(2);

    int n_bins_x = i_arm->hEvdEAll->GetXaxis()->GetNbins();
    int min_x = i_arm->hEvdEAll->GetXaxis()->GetXmin();
    int max_x = i_arm->hEvdEAll->GetXaxis()->GetXmax();
    int n_bins_y = i_arm->hEvdEAll->GetYaxis()->GetNbins();
    int min_y = i_arm->hEvdEAll->GetYaxis()->GetXmin();
    int max_y = i_arm->hEvdEAll->GetYaxis()->GetXmax();

    histname = "hEvdE_" + i_arm->armname + "_proton_stopped";
    i_arm->hEvdEStoppedProtons = (TH2F*) file->Get(histname.c_str());

    histname = "hEvdEBand_" + i_arm->armname;
    i_arm->hEvdEBand = new TH2F(histname.c_str(), histname.c_str(), n_bins_x,min_x,max_x, n_bins_y,min_y,max_y);

    histname = "hBandProfile_" + i_arm->armname;
    i_arm->hBandProfile = new TH1F(histname.c_str(), histname.c_str(), n_bins_x,min_x,max_x);
    
    histname = "hNPeaks_" + i_arm->armname;
    TH1F* hNPeaks = new TH1F(histname.c_str(), "", n_bins_x,min_x,max_x);

    // Loop through the projection of each energy to try and find the proton band
    int start_bin = i_arm->hEvdEAll->GetXaxis()->FindBin(low_energy_cut);
    int stop_bin = i_arm->hEvdEAll->GetXaxis()->FindBin(high_energy_cut);
    for (int i_bin = start_bin; i_bin <= stop_bin; ++i_bin) {
      double i_energy = i_arm->hEvdEAll->GetXaxis()->GetBinCenter(i_bin);
      TH1* hProjection = i_arm->hEvdEAll->ProjectionY("_py", i_bin, i_bin);

      int n_peaks = GetNPeaks(hProjection, i_arm->n_trough);
      hNPeaks->Fill(i_energy, n_peaks);

      if (n_peaks >= i_arm->peak_to_take) {
	// Get the location of the start of the second peak
	int peak_start = 1;
	int counter = 0;
	while (counter < i_arm->peak_to_take) {
	  peak_start = FindNextPeak(hProjection, peak_start, i_arm->n_trough);
	  ++counter;
	}

	// Now fill in the bins until we get a bin content that is not part of the peak
	int j_bin = peak_start;
	int bin_content = hProjection->GetBinContent(j_bin);
	while (bin_content > i_arm->n_trough) {
	  bin_content = hProjection->GetBinContent(j_bin);
	  i_arm->hEvdEBand->SetBinContent(i_bin, j_bin, bin_content);
	  ++j_bin;
	}
	TH1* hProjection = i_arm->hEvdEBand->ProjectionY("_py", i_bin, i_bin);
	double mean = hProjection->GetMean();
	double rms = hProjection->GetRMS();
	//	std::cout << "Energy = " << i_energy << ", Mean = " << mean << ", rms = " << rms << std::endl;
	i_arm->hBandProfile->SetBinContent(i_bin, mean);
	i_arm->hBandProfile->SetBinError(i_bin, rms);
	//	std::cout << "Bin #" << i_bin << ": bin_location = " << peak_start << std::endl;

	// What fraction of what we know to be stopped protons by this profile
	int integral_low = hProjection->FindBin(mean - 1*rms);
	int integral_high = hProjection->FindBin(mean + 1*rms);
	if (i_arm->hEvdEStoppedProtons) {
	  int n_selected_this_bin = i_arm->hEvdEStoppedProtons->ProjectionY("_py", i_bin, i_bin)->Integral(integral_low, integral_high);
	  int n_proton_stopped_this_bin = i_arm->hEvdEStoppedProtons->ProjectionY("_py", i_bin, i_bin)->Integral();
	  //	  std::cout << "Bin #" << i_bin << ": " << n_selected_this_bin << " / " << n_proton_stopped_this_bin << " = " << (double)n_selected_this_bin / n_proton_stopped_this_bin << std::endl;
	  i_arm->n_selected += n_selected_this_bin;
	}
      }
    }

    if (i_arm->hEvdEStoppedProtons) {
      TH1D* hStoppedProtonProjection = i_arm->hEvdEStoppedProtons->ProjectionX();
      int n_total_stopped_protons = hStoppedProtonProjection->Integral(start_bin, stop_bin);
      std::cout << i_arm->armname << ": Fraction selected = " << i_arm->n_selected << " / " << n_total_stopped_protons << " = " << (double) i_arm->n_selected / n_total_stopped_protons << std::endl;
    }
    
    //    hNPeaks->Draw();
    //    i_arm->hEvdEAll->Draw("COLZ");
    //    i_arm->hEvdEBand->Draw("COLZ");
    //    hStoppedProtonProjection->Draw();
    //    i_arm->hBandProfile->Draw();
    //    i_arm->hEvdEStoppedProtons->Draw("COLZ");
  }
}

// Gets the number of "peaks", just based on the number of times we go to zero entries
int GetNPeaks(TH1* hist, int n_trough) {
  int n_bins = hist->GetNbinsX();
  int n_peaks = -1; // start at -1 since we will always go through the while loop once, even if we don't find the next peak
  int peak_start = 1;

  while (peak_start != n_bins) {
    peak_start = FindNextPeak(hist, peak_start, n_trough);
    ++n_peaks;
  }
  return n_peaks;
}

// Finds the next peak and returns the bin number
int FindNextPeak(TH1* hist, int start_bin, int n_trough) {
  int n_bins = hist->GetNbinsX();
  int return_bin = n_bins;
  int prev_bin_content = hist->GetBinContent(start_bin);

  for (int i_bin = start_bin+1; i_bin <= n_bins; ++i_bin) {
    int bin_content = hist->GetBinContent(i_bin);
    if (prev_bin_content <= n_trough && bin_content > n_trough) {
      return_bin = i_bin;
      break;
    }   
    prev_bin_content = bin_content;
  }
  return return_bin;
}
