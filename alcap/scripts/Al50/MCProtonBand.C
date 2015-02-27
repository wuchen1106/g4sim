#include <iostream>

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"

struct Arm{
  std::string armname;
  std::string armnumber;
  TH2F* hEvdEAll;
} LeftArm, RightArm;

int GetNPeaks(TH1* hist);

void MCProtonBand() {
  
  LeftArm.armname = "SiL"; RightArm.armname = "SiR";
  LeftArm.armnumber = "1"; RightArm.armnumber = "2";
  std::vector<Arm> arms;
  arms.push_back(LeftArm); arms.push_back(RightArm);

  TFile* file = new TFile("plots.root", "READ");
  
  for (std::vector<Arm>::iterator i_arm = arms.begin(); i_arm != arms.end(); ++i_arm) {
    std::string histname = "All;" + i_arm->armnumber;
    i_arm->hEvdEAll = (TH2F*) file->Get(histname.c_str());

    // Loop through the projection of each energy to try and find the proton band
    int n_bins_x = i_arm->hEvdEAll->GetXaxis()->GetNbins();

    TH1F* hNPeaks = new TH1F("hNPeaks", "hNPeaks", 5,0,5);
    for (int i_bin = 1; i_bin <= n_bins_x; ++i_bin) {
      TH1* hProjection = i_arm->hEvdEAll->ProjectionY("_py", i_bin, i_bin);
      hNPeaks->Fill(GetNPeaks(hProjection));
      //      std::cout << "Bin # " << i_bin << ": " << GetNPeaks(hProjection) << " peaks" << std::endl;
    }
    hNPeaks->Draw();
    //    i_arm->hEvdEAll->Draw("COLZ");
  }
}

int GetNPeaks(TH1* hist) {
  int n_bins = hist->GetNbinsX();
  int prev_bin_content = 0;
  int n_peaks = 0;
  for (int i_bin = 1; i_bin <= n_bins; ++i_bin) {

    int bin_content = hist->GetBinContent(i_bin);
    if (prev_bin_content == 0 && bin_content != 0) {
      ++n_peaks;
    }
    prev_bin_content = bin_content;
  }
  return n_peaks;
}
