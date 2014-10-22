void MakeCollimatedInput() {

  TFile* input = new TFile("output/raw_g4sim.root", "READ");
  TTree* tree = (TTree*) input->Get("tree");
  int n_entries = =tree->GetEntries();

  TH3F* hMomentum = new TH3F("hMomentum", "hMomentum", 100,-5,5, 100,-5,5, 350,0,35);
  TH3F* hPosition = new TH3F("hPosition", "hPosition", 80,-4,4, 80,-4,4, 10,-8,-7);
  tree->Draw("M_pz*1000:M_py*1000:M_px*1000>>hMomentum", "M_particleName == \"mu-\" && M_volName == \"ColMon\"");
  tree->Draw("M_z:M_y:M_x>>hPosition", "M_particleName == \"mu-\" && M_volName == \"ColMon\"");

  TH1F* hOriginalPX = new TH1F("hOriginalPX", "hOriginalPX", 100,-5,5);
  TH1F* hOriginalPY = new TH1F("hOriginalPY", "hOriginalPY", 100,-5,5);
  TH1F* hOriginalPZ = new TH1F("hOriginalPZ", "hOriginalPZ", 350,0,35);
  tree->Draw("M_px*1000>>hOriginalPX", "M_particleName == \"mu-\" && M_volName == \"ColMon\"");
  tree->Draw("M_py*1000>>hOriginalPY", "M_particleName == \"mu-\" && M_volName == \"ColMon\"");
  tree->Draw("M_pz*1000>>hOriginalPZ", "M_particleName == \"mu-\" && M_volName == \"ColMon\"");

  TH1F* hRandomPX = new TH1F("hRandomPX", "hRandomPX", 100,-5,5);
  TH1F* hRandomPY = new TH1F("hRandomPY", "hRandomPY", 100,-5,5);
  TH1F* hRandomPZ = new TH1F("hRandomPZ", "hRandomPZ", 350,0,35);
  hRandomPX->SetLineColor(kRed);
  hRandomPY->SetLineColor(kRed);
  hRandomPZ->SetLineColor(kRed);


  TH1F* hOriginalX = new TH1F("hOriginalX", "hOriginalX", 80,-4,4);
  TH1F* hOriginalY = new TH1F("hOriginalY", "hOriginalY", 80,-4,4);
  TH1F* hOriginalZ = new TH1F("hOriginalZ", "hOriginalZ", 10,-8,-7);
  tree->Draw("M_x>>hOriginalX", "M_particleName == \"mu-\" && M_volName == \"ColMon\"");
  tree->Draw("M_y>>hOriginalY", "M_particleName == \"mu-\" && M_volName == \"ColMon\"");
  tree->Draw("M_z>>hOriginalZ", "M_particleName == \"mu-\" && M_volName == \"ColMon\"");

  TH1F* hRandomX = new TH1F("hRandomX", "hRandomX", 80,-4,4);
  TH1F* hRandomY = new TH1F("hRandomY", "hRandomY", 80,-4,4);
  TH1F* hRandomZ = new TH1F("hRandomZ", "hRandomZ", 10,-8,-7);
  hRandomX->SetLineColor(kRed);
  hRandomY->SetLineColor(kRed);
  hRandomZ->SetLineColor(kRed);

  double px, py, pz;
  double x, y, z;
  for (int i = 0; i < 2*hMomentum->GetEntries(); ++i) {
    hMomentum->GetRandom3(px, py, pz);
    hRandomPX->Fill(px);
    hRandomPY->Fill(py);
    hRandomPZ->Fill(pz);

    hPosition->GetRandom3(x, y, z);
    hRandomX->Fill(x);
    hRandomY->Fill(y);
    hRandomZ->Fill(z);
  }

  TCanvas* c_px = new TCanvas("c_px", "c_px");
  hRandomPX->Scale(0.5);
  hOriginalPX->Draw();
  hRandomPX->Draw("SAMES");
  c_px->Update();

  TCanvas* c_py = new TCanvas("c_py", "c_py");
  hOriginalPY->Draw();
  hRandomPY->Scale(0.5);
  hRandomPY->Draw("SAMES");
  c_py->Update();

  TCanvas* c_pz = new TCanvas("c_pz", "c_pz");
  hOriginalPZ->Draw();
  hRandomPZ->Scale(0.5);
  hRandomPZ->Draw("SAMES");
  c_pz->Update();


  TCanvas* c_x = new TCanvas("c_x", "c_x");
  hRandomX->Scale(0.5);
  hOriginalX->Draw();
  hRandomX->Draw("SAMES");
  c_x->Update();

  TCanvas* c_y = new TCanvas("c_y", "c_y");
  hOriginalY->Draw();
  hRandomY->Scale(0.5);
  hRandomY->Draw("SAMES");
  c_y->Update();

  TCanvas* c_z = new TCanvas("c_z", "c_z");
  hOriginalZ->Draw();
  hRandomZ->Scale(0.5);
  hRandomZ->Draw("SAMES");
  c_z->Update();

}
