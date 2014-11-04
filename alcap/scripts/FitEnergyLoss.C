void FitEnergyLoss() {

  TFile* file = new TFile("output/raw_g4sim_energy-loss.root", "READ");
  TTree* tree = (TTree*) file->Get("tree");

  TH1F* hist = new TH1F("hist", "hist", 150,0,0.03);
  tree->Draw("0.02996 - sqrt(M_px*M_px + M_py*M_py + M_pz*M_pz)>>hist", "M_particleName==\"mu-\" && M_volName==\"ColMon\"");

  TF1* fn = new TF1("fn", "[0]*TMath::Landau(x, [1], [2]) + [3]*TMath::Exp([4]*x^[5] + [6]) + [7]*TMath::Gaus(x, [8], [9])", 0, 0.03);
  
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

  fn->Draw("SAME");

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

  //  hist->Fit(fn2, "R+");

  /*  TF1* sum = new TF1("sum", myfunction, 0, 0.02, 6);
  double par[6];
  fn->GetParameters(&par[0]);
  fn2->GetParameters(&par[3]);
  for (int i = 0; i < 6; ++i) {
    sum->SetParameter(i, par[i]);
  }
  hist->Fit(sum, "R");
  */

  /*  TF1* sum = new TF1 ("sum", "fn + fn2", 0, 0.03);
  const int n_par = 6;
  double par[n_par];
  fn->GetParameters(&par[0]);
  fn2->GetParameters(&par[3]);
  for (int i = 0; i < n_par; ++i) {
    sum->FixParameter(i, par[i]);
  }
  sum->SetLineColor(kBlack);
  //  sum->Draw("SAME");
    hist->Fit(sum, "R+");
  */
}

Double_t myfunction(Double_t *x, Double_t *par)
{
  Float_t xx = x[0];
  Double_t f = 0;
  if (xx < 0.014) {
    f += par[3]*TMath::Exp(par[4]*xx + par[5]);
  }
  else if (xx >= 0.014) {
    f += par[0]*TMath::Gaus(xx, par[1], par[2]);
  }
  return f;
}
