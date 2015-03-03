#include <iostream>
#include <fstream>
#include <cmath>

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

  int n_entry_threshold; // the number of entries that we determine as the start of a peak
  int first_dE_of_band; // which is the dE value at the start of the band
} LeftArm, RightArm;

void ExtractProtonBand(std::string id, std::string filename, std::string baseplotname, int n_entry_threshold, int first_dE_of_band);
int GetNPeaks(TH1* hist, int n_entry_threshold);
int FindNextPeak(TH1* hist, int start_bin, int n_entry_threshold);

void DataAndMC() {
  // Data
  ExtractProtonBand("data", "~/data/out/v92/total.root", "TME_Al50_EvdE/all_particles/ARM_EvdE", 20, 2000);
  
  // MC
  ExtractProtonBand("MC", "plots.root", "hAll_EvdE_ARM", 1, 2000);
}

void ExtractProtonBand(std::string id, std::string filename, std::string baseplotname, int n_entry_threshold, int first_dE_of_band) {

  double low_energy_cut = 1000; // keV
  double high_energy_cut = 10000; // keV
  
  LeftArm.armname = "SiL"; RightArm.armname = "SiR";
  LeftArm.n_entry_threshold = n_entry_threshold; RightArm.n_entry_threshold = n_entry_threshold;
  LeftArm.first_dE_of_band = first_dE_of_band; RightArm.first_dE_of_band = first_dE_of_band;
  std::vector<Arm> arms;
  arms.push_back(LeftArm); arms.push_back(RightArm);

  TFile* file = new TFile(filename.c_str(), "READ");
  std::string debug_filename = "output_" + id + ".txt";
  std::ofstream output(debug_filename.c_str(), std::ofstream::out);
  
  for (std::vector<Arm>::iterator i_arm = arms.begin(); i_arm != arms.end(); ++i_arm) {
    std::string canvasname = "c_" + id + "_" + i_arm->armname;
    TCanvas* c = new TCanvas(canvasname.c_str(), canvasname.c_str());
    i_arm->n_selected = 0;
    
    std::string histname = baseplotname;
    histname.replace(baseplotname.find("ARM"), 3, i_arm->armname);
    i_arm->hEvdEAll = (TH2F*) file->Get(histname.c_str());
    output << id << " " << i_arm->armname << std::endl;
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
      double i_energy = i_arm->hEvdEAll->GetXaxis()->GetBinLowEdge(i_bin);

      TH1* hProjection = i_arm->hEvdEAll->ProjectionY("_py", i_bin, i_bin);
      
      int n_peaks = GetNPeaks(hProjection, i_arm->n_entry_threshold);
      hNPeaks->Fill(i_energy, n_peaks);
      
      // Loop through the peaks
      int peak_start_bin;
      bool found_peak = false;
      for (int i_peak = 0; i_peak < n_peaks; ++i_peak) {
	// Get the location of the start of this peak
	int counter = 0;
	peak_start_bin = 1;
      	while (counter <= i_peak) {
	  peak_start_bin = FindNextPeak(hProjection, peak_start_bin, i_arm->n_entry_threshold);
	  ++counter;
	}
	  
	// Is this peak close enough to where we were last time
	double peak_start_dE = hProjection->GetBinLowEdge(peak_start_bin);
	int content = hProjection->GetBinContent(peak_start_bin);
	double difference = std::fabs(peak_start_dE - i_arm->first_dE_of_band);
	output << i_energy << ": Peak #" << i_peak+1 << " of " << n_peaks << ": Peak Start = " << peak_start_bin << ": " << i_arm->first_dE_of_band << " - " << peak_start_dE << " = " << difference << " (" << content << " entries)" << std::endl;
	if (difference < 250) {
	  // This is the peak we want to take for the selected band
	  found_peak = true;
	  i_arm->first_dE_of_band = peak_start_dE;
	  break; // from the loop through the peaks
	}
      }
      if (found_peak == false) {
	continue; // to the next energy bin
      }
      // Now fill in the bins until we get a bin content that is not part of the peak
      int j_bin = peak_start_bin;
      int bin_content = hProjection->GetBinContent(j_bin);
      while (bin_content > i_arm->n_entry_threshold) {
	bin_content = hProjection->GetBinContent(j_bin);
	i_arm->hEvdEBand->SetBinContent(i_bin, j_bin, bin_content);
	++j_bin;
      }
    
      double mean = hProjection->GetMean();
      double rms = hProjection->GetRMS();
      //	std::cout << "Energy = " << i_energy << ", Mean = " << mean << ", rms = " << rms << std::endl;
      i_arm->hBandProfile->SetBinContent(i_bin, mean);
      i_arm->hBandProfile->SetBinError(i_bin, rms);
      //	std::cout << "Bin #" << i_bin << ": bin_location = " << peak_start_bin << std::endl;
      
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
      
    if (i_arm->hEvdEStoppedProtons) {
      TH1D* hStoppedProtonProjection = i_arm->hEvdEStoppedProtons->ProjectionX();
      int n_total_stopped_protons = hStoppedProtonProjection->Integral(start_bin, stop_bin);
      std::cout << i_arm->armname << ": Fraction selected = " << i_arm->n_selected << " / " << n_total_stopped_protons << " = " << (double) i_arm->n_selected / n_total_stopped_protons << std::endl;
    }
  
    //    hNPeaks->Draw();
    //    i_arm->hEvdEAll->Draw("COLZ");
    i_arm->hEvdEBand->Draw("COLZ");
    //    hStoppedProtonProjection->Draw();
    //    i_arm->hBandProfile->Draw();
    //    i_arm->hEvdEStoppedProtons->Draw("COLZ");
  }
}

// Gets the number of "peaks", just based on the number of times we go to zero entries
int GetNPeaks(TH1* hist, int n_entry_threshold) {
  int n_bins = hist->GetNbinsX();
  int n_peaks = -1; // start at -1 since we will always go through the while loop once, even if we don't find the next peak
  int peak_start_bin = 1;

  while (peak_start_bin != n_bins) {
    peak_start_bin = FindNextPeak(hist, peak_start_bin, n_entry_threshold);
    ++n_peaks;
  }
  return n_peaks;
}

// Finds the next peak and returns the bin number
int FindNextPeak(TH1* hist, int start_bin, int n_entry_threshold) {
  int n_bins = hist->GetNbinsX();
  int return_bin = n_bins;
  int prev_bin_content = hist->GetBinContent(start_bin);
  
  for (int i_bin = start_bin+1; i_bin <= n_bins; ++i_bin) {
    int bin_content = hist->GetBinContent(i_bin);

    if (prev_bin_content <= n_entry_threshold && bin_content > n_entry_threshold) {
      return_bin = i_bin;
      break;
    }   
    prev_bin_content = bin_content;
  }
  return return_bin;
}
