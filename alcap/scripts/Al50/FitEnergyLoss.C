void FitEnergyLoss() {

  TFile* file = new TFile("../../output/raw_g4sim.root", "READ");
  TTree* tree = (TTree*) file->Get("tree");

  TH1F* hist = new TH1F("ELoss", "ELoss", 100,0,0.03);
  hist->SetTitle("Energy loss of muons between the end of the beam pipe and the collimator");
  hist->GetXaxis()->SetTitle("Energy Loss [GeV]");
  hist->GetYaxis()->SetTitle("Number of Particles");
  hist->SetStats(false);
  hist->SetLineWidth(2);

  tree->Draw("0.02996 - sqrt(M_px*M_px + M_py*M_py + M_pz*M_pz)>>ELoss", "M_particleName==\"mu-\" && M_volName==\"ColMon\"", "", 450000);

  TF1* fn = new TF1("fn", "[0]*TMath::Landau(x, [1], [2]) + [3]*TMath::Exp([4]*x^[5] + [6]) + [7]*TMath::Gaus(x, [8], [9])", 0, 0.03);
  /*
  fn->SetParameter(0, 100);
  fn->SetParameter(1, 0.015);
  fn->SetParameter(2, 0.002);
  fn->SetParameter(3, 10);
  fn->SetParameter(4, 0.01);
  fn->SetParameter(5, 1);
  fn->SetParameter(6, -0.1);
  fn->SetParameter(7, 100);
  fn->SetParameter(8, 0.015);
  fn->SetParameter(9, 0.002);

  hist->Fit(fn, "R");
  */
  
 // These are the values I got  
  fn->SetParameter(0, 14051.2);
  fn->SetParameter(1, 0.0141187);
  fn->SetParameter(2, 0.000738656);
  fn->SetParameter(3, -0.0127883);
  fn->SetParameter(4, 0.0599257);
  fn->SetParameter(5, 11.5222);
  fn->SetParameter(6, -1.30946);
  fn->SetParameter(7, 2510.06);
  fn->SetParameter(8, 0.0155356);
  fn->SetParameter(9, 0.00170543);
  //  hist->SetStats(true);
  //  gStyle->SetOptFit(11111);
  hist->Fit(fn, "R");
  //  fn->Draw("SAME");
  
  
}
