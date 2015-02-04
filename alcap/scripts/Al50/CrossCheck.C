// The cross check where we take Nam's ESpectrum and put it through the MC
void CrossCheck(std::string MC_plots_filename, std::string data_filename) {

  TFile* MC_plots_file = new TFile(MC_plots_filename.c_str(), "READ");
  TH1D* MC_truth_hist = (TH1D*) MC_plots_file->Get("hInitialESpectrum");
  TH1D* MC_SiL_hist = (TH1D*) MC_plots_file->Get("hEvdE_SiL_proton_stopped_px");
  TH1D* MC_SiR_hist = (TH1D*) MC_plots_file->Get("hEvdE_SiR_proton_stopped_px");
  MC_SiL_hist->SetLineColor(kRed);

  TFile* data_file = new TFile(data_filename.c_str(), "READ");
  TH1D* data_hist = (TH1D*) data_file->Get("SiL_EvdE_px");

  double rebin_factor = data_hist->GetNbinsX() / MC_SiL_hist->GetNbinsX();
  data_hist->Rebin(rebin_factor);

  std::cout << "MC truth  (4 - 8 MeV): " << MC_truth_hist->Integral(MC_truth_hist->FindBin(4000), MC_truth_hist->FindBin(8000)) << std::endl;
  std::cout << "MC SiL folded (4 - 8 MeV): " << MC_SiL_hist->Integral(MC_SiL_hist->FindBin(4000), MC_SiL_hist->FindBin(8000)) << std::endl;
  std::cout << "MC SiR folded (4 - 8 MeV): " << MC_SiR_hist->Integral(MC_SiR_hist->FindBin(4000), MC_SiR_hist->FindBin(8000)) << std::endl;

  MC_SiL_hist->GetXaxis()->SetRangeUser(0, 10500);
  data_hist->GetXaxis()->SetRangeUser(0, 10500);

  double MC_integral = MC_SiL_hist->Integral(MC_SiL_hist->GetXaxis()->FindBin(0), MC_SiL_hist->GetXaxis()->FindBin(10500), "width");
  double data_integral = data_hist->Integral(data_hist->GetXaxis()->FindBin(0), data_hist->GetXaxis()->FindBin(10500), "width");
  std::cout << "Integral between 0 and 10.5 MeV = " << MC_integral << " and " << data_integral << std::endl;
  double scale_factor = MC_integral / data_integral;
  MC_SiL_hist->Scale(1.0/scale_factor);
  
  MC_SiL_hist->Draw("HIST E");
  data_hist->Draw("SAME HIST E");
}
