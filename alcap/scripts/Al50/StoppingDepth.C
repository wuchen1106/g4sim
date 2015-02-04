// This ROOT macro plots the stopping depth of the muons in local coordinates
void StoppingDepth(std::string filename) {

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->SetRightMargin(0.15);
  int n_entries = 1000000000;

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tree = (TTree*) file->Get("tree");
  double n_input_muons = tree->GetEntries();

  size_t secondslash = filename.rfind("/")+1; // don't want to include the slash
  size_t firstdot = filename.rfind(".");
  size_t n_chars = firstdot - secondslash;
  std::string base_filename = filename.substr(secondslash, n_chars);
  std::string histtitle = "Plot of the Stopping Depth of Muons in " + base_filename;
  TH2F* hStopDepth = new TH2F("hStopDepth", histtitle.c_str(), 120,-6,6, 120,-30,30);
  hStopDepth->GetYaxis()->SetTitle("Local Z [#mum]");
  hStopDepth->GetXaxis()->SetTitle("Local X [cm]");
  hStopDepth->GetZaxis()->SetTitle("N_{stop-#mu} / N_{input-#mu}");
  hStopDepth->Scale(1.0 / n_input_muons);
  hStopDepth->SetStats(false);

  tree->Draw("M_local_Oz*10000:M_local_Ox>>hStopDepth", "M_particleName==\"mu-\" && M_volName==\"Target\" && M_stopped == 1", "COLZ", n_entries);
  
  c1->Update();
  std::string canvasname = "hStopDepth_" + base_filename + ".png";
  c1->Print(canvasname.c_str());

  TH1D* hStopZDepth = hStopDepth->ProjectionY();
  hStopZDepth->Draw();
  hStopZDepth->SetStats(false);
  hStopZDepth->GetYaxis()->SetTitle("N_{stop-#mu} / N_{input-#mu}");
  hStopZDepth->Scale(1.0 / n_input_muons);
  c1->Update();
  canvasname = "hStopZDepth_" + base_filename + ".png";
  c1->Print(canvasname.c_str());

  TH2F* hStopDepthVsTime = new TH2F("hStopDepthVsTime", histtitle.c_str(), 120,-30,30, 120,0,12);
  hStopDepthVsTime->GetXaxis()->SetTitle("Local Z [#mum]");
  hStopDepthVsTime->GetYaxis()->SetTitle("Time [ns]");
  hStopDepthVsTime->GetZaxis()->SetTitle("N_{stop-#mu} / N_{input-#mu}");
  hStopDepthVsTime->Scale(1.0 / n_input_muons);
  hStopDepthVsTime->SetStats(false);

  tree->Draw("M_Ot:M_local_Oz*10000>>hStopDepthVsTime", "M_particleName==\"mu-\" && M_volName==\"Target\" && M_stopped == 1", "COLZ", n_entries);
  
  c1->Update();
  canvasname = "hStopDepthVsTime_" + base_filename + ".png";
  c1->Print(canvasname.c_str());

  TH2F* hStopDepthVsMomentum = new TH2F("hStopDepthVsMomentum", histtitle.c_str(), 120,-30,30, 120,0,30);
  hStopDepthVsMomentum->GetXaxis()->SetTitle("Local Z [#mum]");
  hStopDepthVsMomentum->GetYaxis()->SetTitle("Momentum [MeV]");
  hStopDepthVsMomentum->GetZaxis()->SetTitle("N_{stop-#mu} / N_{input-#mu}");
  hStopDepthVsMomentum->Scale(1.0 / n_input_muons);
  hStopDepthVsMomentum->SetStats(false);

  tree->Draw("sqrt(M_px*M_px + M_py*M_py+ M_pz*M_pz)*1000:M_local_Oz*10000>>hStopDepthVsMomentum", "M_particleName==\"mu-\" && M_volName==\"Target\" && M_stopped == 1", "COLZ", n_entries);
  
  c1->Update();
  canvasname = "hStopDepthVsMomentum_" + base_filename + ".png";
  c1->Print(canvasname.c_str());
  //  file->Close();

  std::string outfilename = "plot_" + base_filename + ".root";
  TFile* output = new TFile(outfilename.c_str(), "RECREATE");
  hStopDepth->Write();
  hStopZDepth->Write();
  hStopDepthVsTime->Write();
  hStopDepthVsMomentum->Write();
  output->Close();
}


