void StoppingPositionDistribution(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tree = (TTree*) file->Get("tree");
  
  TH3F* hStoppingPositions = new TH3F("hStoppingPositions", "Positions of Stopped Muons in Target", 10,-5,5, 10,-5,5, 60,-30,30);
  hStoppingPositions->SetXTitle("Local X [cm]");
  hStoppingPositions->SetYTitle("Local Y [cm]");
  hStoppingPositions->SetZTitle("Local Z [#mum]");
  tree->Draw("M_local_Oz*10000:M_local_Oy:M_local_Ox>>hStoppingPositions", "M_particleName==\"mu-\" && M_volName==\"Target\" && M_stopped==1", "");

  TFile* out_file = new TFile("output.root", "RECREATE");
  hStoppingPositions->Write();
  out_file->Close();
}
