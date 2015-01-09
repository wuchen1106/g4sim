// Use this script to convert the kinetic energy spectrum Nam unfolded
// to the momentum amplitude spectrum that is needed for the "histo" EnergyMode
// in g4sim

void ConvertESpectrumToPSpectrum(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "UPDATE");
  TTree* tree = (TTree*) file->Get("tree");
  TH1D* hreco_l = (TH1D*) file->Get("hreco_l");

  double proton_mass = 938272; // keV

  double bin_width = 5; //MeV (chosen so that we don't get gaps in the distribution)
  double x_low = 0;
  double x_high = 170;
  int n_bins_x = (x_high - x_low) / bin_width;
  
  TH1D* hInitialPSpectrum = new TH1D("hInitialPSpectrum", "Unfolded Momentum Spectrum of Protons", n_bins_x,x_low,x_high);
  hInitialPSpectrum->SetXTitle("Momentum Amplitude [MeV/c]");

  for (int i_bin = 1; i_bin <= hreco_l->GetNbinsX(); ++i_bin) {
    double bin_center = hreco_l->GetBinCenter(i_bin); // kinetic energy
    double bin_content = hreco_l->GetBinContent(i_bin);

    double new_bin_center = (sqrt(bin_center*bin_center + 2*proton_mass*bin_center)) / 1e3); // convert to MeV
    std::cout << bin_center << " keV --> " << new_bin_center << " MeV/c " << std::endl;
    int new_bin_number = hInitialPSpectrum->FindBin(new_bin_center);
    hInitialPSpectrum->SetBinContent(new_bin_number, bin_content);
  }
  hInitialPSpectrum->Draw();
  hInitialPSpectrum->Write();

}
