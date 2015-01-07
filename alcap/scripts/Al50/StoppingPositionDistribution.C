void StoppingPositionDistribution(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tree = (TTree*) file->Get("tree");
  
  TH3F* hLocalStoppingPositions = new TH3F("hLocalStoppingPositions", "Positions of Stopped Muons in Target (local coords)", 10,-5,5, 10,-5,5, 60,-30,30);
  hLocalStoppingPositions->SetXTitle("Local X [cm]");
  hLocalStoppingPositions->SetYTitle("Local Y [cm]");
  hLocalStoppingPositions->SetZTitle("Local Z [#mum]");
  tree->Draw("M_local_Oz*10000:M_local_Oy:M_local_Ox>>hLocalStoppingPositions", "M_particleName==\"mu-\" && M_volName==\"Target\" && M_stopped==1", "");

  TH3F* hGlobalStoppingPositions = new TH3F("hGlobalStoppingPositions", "Positions of Stopped Muons in Target (global coords)", 10,-5,5, 10,-5,5, 60,-30,30);
  hGlobalStoppingPositions->SetXTitle("Global X [cm]");
  hGlobalStoppingPositions->SetYTitle("Global Y [cm]");
  hGlobalStoppingPositions->SetZTitle("Global Z [cm]");
  tree->Draw("M_Oz:M_Oy:M_Ox>>hGlobalStoppingPositions", "M_particleName==\"mu-\" && M_volName==\"Target\" && M_stopped==1", "");

  TFile* out_file = new TFile("output.root", "RECREATE");
  hLocalStoppingPositions->Write();
  hGlobalStoppingPositions->Write();
  out_file->Close();
}
