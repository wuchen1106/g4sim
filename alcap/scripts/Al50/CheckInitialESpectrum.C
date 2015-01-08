void CheckInitialESpectrum(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tree = (TTree*) file->Get("tree");

  double bin_width = 250; //keV
  double x_low = 0;
  double x_high = 15000;
  int n_bins_x = (x_high - x_low) / bin_width;
  
  TH1D* hInitialESpectrum = new TH1D("hInitialESpectrum", "Initial Energy Spectrum Generated", n_bins_x,x_low,x_high);
  hInitialESpectrum->SetXTitle("Energy [keV]");
  tree->Draw("(sqrt(i_px*i_px*1e6 + i_py*i_py*1e6 + i_pz*i_pz*1e6 + 938272*938272) - 938272)>>hInitialESpectrum");

}
