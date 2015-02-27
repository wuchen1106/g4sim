#include <iostream>

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"

struct Arm{
  std::string armname;
  std::string armnumber;
  TH2F* hEvdEAll;
  TH2F* hEvdEBand;
  TH1F* hBandProfile;

  TH2F* hEvdEStoppedProtons;

  int n_selected;

  int n_trough; // the number of entries in the trough between peaks
} LeftArm, RightArm;

int GetNPeaks(TH1* hist, int n_trough);
int FindNextPeak(TH1* hist, int start_bin, int n_trough);

void MCProtonBand() {

  double low_energy_cut = 2000; // keV
  double high_energy_cut = 10000; // keV
  
  LeftArm.armname = "SiL"; RightArm.armname = "SiR";
  LeftArm.armnumber = "1"; RightArm.armnumber = "2";
  LeftArm.n_trough = 0; RightArm.n_trough = 0;
  std::vector<Arm> arms;
  arms.push_back(LeftArm); arms.push_back(RightArm);

  TFile* file = new TFile("plots.root", "READ");
  
  for (std::vector<Arm>::iterator i_arm = arms.begin(); i_arm != arms.end(); ++i_arm) {

    i_arm->n_selected = 0;
    
    std::string histname = "All;" + i_arm->armnumber;
    i_arm->hEvdEAll = (TH2F*) file->Get(histname.c_str());

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
    TH1F* hNPeaks = new TH1F("hNPeaks", "hNPeaks", 5,0,5);

    // Loop through the projection of each energy to try and find the proton band
    int start_bin = i_arm->hEvdEAll->GetXaxis()->FindBin(low_energy_cut);
    int stop_bin = i_arm->hEvdEAll->GetXaxis()->FindBin(high_energy_cut);
    for (int i_bin = start_bin; i_bin <= stop_bin; ++i_bin) {
      double i_energy = i_arm->hEvdEAll->GetXaxis()->GetBinCenter(i_bin);
      TH1* hProjection = i_arm->hEvdEAll->ProjectionY("_py", i_bin, i_bin);

      int n_peaks = GetNPeaks(hProjection, i_arm->n_trough);
      hNPeaks->Fill(n_peaks);

      if (n_peaks >= 2) {
	// Get the location of the start of the second peak
	int peak_start = 1;
	int counter = 0;
	while (counter < 2) {
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
	i_arm->n_selected += i_arm->hEvdEStoppedProtons->ProjectionY("_py", i_bin, i_bin)->Integral(integral_low, integral_high);
      }
    }

    TH1D* hStoppedProtonProjection = i_arm->hEvdEStoppedProtons->ProjectionX();
    int n_total_stopped_protons = hStoppedProtonProjection->Integral(start_bin, stop_bin);
    std::cout << i_arm->armname << ": Fraction selected = " << i_arm->n_selected << " / " << n_total_stopped_protons << " = " << (double) i_arm->n_selected / n_total_stopped_protons << std::endl;
    
    //    hNPeaks->Draw();
    //    i_arm->hEvdEAll->Draw("COLZ");
    i_arm->hEvdEBand->Draw("COLZ");
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
