void MakeTURTLEInput() {

  TFile* out_file = new TFile("data/TURTLE_fits.root", "RECREATE");

  double horizontal_center = -0.321;
  double horizontal_fwhm = 0.799;
  double vertical_center = -0.001;
  double vertical_fwhm = 0.435;
  double conversion_factor = 2.355; // conversion factor between FWHM and sigma (FWHM = 2.355*sigma)

  TF1* final_focus_horizontal = new TF1("final_focus_horizontal", "TMath::Gaus(x, [0], [1])", -1.5,1.5);
  final_focus_horizontal->SetParameter(0, horizontal_center);
  final_focus_horizontal->SetParameter(1, horizontal_fwhm / conversion_factor);
  //  final_focus_horizontal->Draw();

  TF1* final_focus_vertical = new TF1("final_focus_vertical", "TMath::Gaus(x, [0], [1])", -1.5,1.5);
  final_focus_vertical->SetParameter(0, vertical_center);
  final_focus_vertical->SetParameter(1, vertical_fwhm / conversion_factor);
  //  final_focus_vertical->Draw();

  final_focus_horizontal->Write();
  final_focus_vertical->Write();

  TDirectory* prev_dir = gDirectory;
  TFile* muPC_beam_dist_file = new TFile("data/hist.root");
  TH2F* hMuPCBeamDist = (TH2F*) muPC_beam_dist_file->Get("muPC/hmuPC_XYWires");
  //  muPC_beam_dist_file->Close();

  gDirectory = prev_dir;
  hMuPCBeamDist->Write();
}
