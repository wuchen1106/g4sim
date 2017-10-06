// Use this script to convert the kinetic energy spectrum Nam unfolded
// to the momentum amplitude spectrum that is needed for the "histo" EnergyMode
// in g4sim

void ConvertESpectrumToPSpectrum(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "UPDATE");
  TTree* tree = (TTree*) file->Get("tree");
  TH1D* hreco_l = (TH1D*) file->Get("hreco_l");

  std::cout << "Bin Width = " << hreco_l->GetBinWidth(1) << std::endl;
  std::cout << "N Bins = " << hreco_l->GetNbinsX() << std::endl;
  double proton_mass = 938272; // keV

  int n_bins_x = hreco_l->GetNbinsX();
  double e_low = hreco_l->GetBinLowEdge(1);
  double e_high = hreco_l->GetBinLowEdge(hreco_l->GetNbinsX()+1);
  double x_low = (sqrt(e_low*e_low + 2*proton_mass*e_low)) / 1e3;
  double x_high = (sqrt(e_high*e_high + 2*proton_mass*e_high)) / 1e3;;
  std::cout << "PSpectrum: " << x_low << " --> " << x_high << std::endl;

  TH1D* hInitialPSpectrum = new TH1D("hInitialPSpectrum", "Unfolded Momentum Spectrum of Protons", n_bins_x,x_low,x_high);
  hInitialPSpectrum->SetXTitle("Momentum Amplitude [MeV/c]");

  for (int i_bin = 1; i_bin <= hreco_l->GetNbinsX(); ++i_bin) {
    double bin_center = hreco_l->GetBinCenter(i_bin); // kinetic energy
    double bin_content = hreco_l->GetBinContent(i_bin);

    double new_bin_center = (sqrt(bin_center*bin_center + 2*proton_mass*bin_center)) / 1e3; // convert to MeV
    std::cout << bin_center << " keV --> " << new_bin_center << " MeV/c " << std::endl;
    int new_bin_number = hInitialPSpectrum->FindBin(new_bin_center);
    std::cout << "Bin Numbers: " << i_bin << " --> " << new_bin_number << std::endl;
    hInitialPSpectrum->SetBinContent(new_bin_number, bin_content);
  }
  hInitialPSpectrum->Draw();
  hInitialPSpectrum->Write();

}
