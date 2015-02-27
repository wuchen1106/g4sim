#include <iostream>

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"

struct Arm{
  std::string armname;
  std::string armnumber;
  TH2F* hEvdEAll;
  TH2F* hEvdEBand;
} LeftArm, RightArm;

int GetNPeaks(TH1* hist);
int FindNextPeak(TH1* hist, int start_bin);

void MCProtonBand() {
  
  LeftArm.armname = "SiL"; RightArm.armname = "SiR";
  LeftArm.armnumber = "1"; RightArm.armnumber = "2";
  std::vector<Arm> arms;
  arms.push_back(LeftArm); arms.push_back(RightArm);

  TFile* file = new TFile("plots.root", "READ");
  
  for (std::vector<Arm>::iterator i_arm = arms.begin(); i_arm != arms.end(); ++i_arm) {
    std::string histname = "All;" + i_arm->armnumber;
    i_arm->hEvdEAll = (TH2F*) file->Get(histname.c_str());

    int n_bins_x = i_arm->hEvdEAll->GetXaxis()->GetNbins();
    int min_x = i_arm->hEvdEAll->GetXaxis()->GetXmin();
    int max_x = i_arm->hEvdEAll->GetXaxis()->GetXmax();
    int n_bins_y = i_arm->hEvdEAll->GetYaxis()->GetNbins();
    int min_y = i_arm->hEvdEAll->GetYaxis()->GetXmin();
    int max_y = i_arm->hEvdEAll->GetYaxis()->GetXmax();

    histname = "hEvdEBand_" + i_arm->armname;
    i_arm->hEvdEBand = new TH2F(histname.c_str(), histname.c_str(), n_bins_x,min_x,max_x, n_bins_y,min_y,max_y);
    TH1F* hNPeaks = new TH1F("hNPeaks", "hNPeaks", 5,0,5);

    // Loop through the projection of each energy to try and find the proton band
    for (int i_bin = 1; i_bin <= n_bins_x; ++i_bin) {
      TH1* hProjection = i_arm->hEvdEAll->ProjectionY("_py", i_bin, i_bin);

      int n_peaks = GetNPeaks(hProjection);
      hNPeaks->Fill(n_peaks);

      if (n_peaks >= 2) {
	// Get the location of the start of the second peak
	int peak_start = 1;
	int counter = 0;
	while (counter < 2) {
	  peak_start = FindNextPeak(hProjection, peak_start);
	  ++counter;
	}

	// Now fill in the bins until we get a bin content of 0
	int j_bin = peak_start;
	int bin_content = hProjection->GetBinContent(j_bin);
	while (bin_content != 0) {
	  bin_content = hProjection->GetBinContent(j_bin);
	  i_arm->hEvdEBand->SetBinContent(i_bin, j_bin, bin_content);
	  ++j_bin;
	}
	//	std::cout << "Bin #" << i_bin << ": bin_location = " << peak_start << std::endl;
      }
      //      std::cout << "Bin # " << i_bin << ": " << GetNPeaks(hProjection) << " peaks" << std::endl;
    }
    //    hNPeaks->Draw();
    //    i_arm->hEvdEAll->Draw("COLZ");
    i_arm->hEvdEBand->Draw("COLZ");
  }
}

// Gets the number of "peaks", just based on the number of times we go to zero entries
int GetNPeaks(TH1* hist) {
  int n_bins = hist->GetNbinsX();
  int prev_bin_content = 0;
  int n_peaks = -1; // start at -1 since we will always go through the while loop once, even if we don't find the next peak
  int peak_start = 1;

  while (peak_start != n_bins) {
    peak_start = FindNextPeak(hist, peak_start);
    ++n_peaks;
  }
  return n_peaks;
}

// Finds the next peak and returns the bin number
int FindNextPeak(TH1* hist, int start_bin) {
  int n_bins = hist->GetNbinsX();
  int return_bin = n_bins;
  int prev_bin_content = hist->GetBinContent(start_bin);

  for (int i_bin = start_bin+1; i_bin <= n_bins; ++i_bin) {
    int bin_content = hist->GetBinContent(i_bin);
    if (prev_bin_content == 0 && bin_content != 0) {
      return_bin = i_bin;
      break;
    }   
    prev_bin_content = bin_content;
  }
  return return_bin;
}
