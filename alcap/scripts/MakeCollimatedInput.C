void MakeCollimatedInput() {

  TFile* input = new TFile("output/raw_g4sim.root", "READ");
  TTree* tree = (TTree*) input->Get("tree");
  int n_entries = =tree->GetEntries();

  TH3F* hPxPyPz = new TH3F("hPxPyPz", "hPxPyPz", 100,-5,5, 100,-5,5, 350,0,35);
  TH3F* hXPxPz = new TH3F("hXPxPz", "hXPxPz", 80,-4,4, 100,-5,5, 350,0,35);
  TH3F* hYPyPz = new TH3F("hYPyPz", "hYPyPz", 80,-4,4, 100,-5,5, 350,0,35);
  tree->Draw("M_pz*1000:M_py*1000:M_px*1000>>hPxPyPz", "M_particleName == \"mu-\" && M_volName == \"ColMon\"");
  tree->Draw("M_pz*1000:M_px*1000:M_x>>hXPxPz", "M_particleName == \"mu-\" && M_volName == \"ColMon\"");
  tree->Draw("M_pz*1000:M_py*1000:M_y>>hYPyPz", "M_particleName == \"mu-\" && M_volName == \"ColMon\"");

  TH3F* hPosition = new TH3F("hPosition", "hPosition", 80,-4,4, 80,-4,4, 10,-8,-7);
  tree->Draw("M_z:M_y:M_x>>hPosition", "M_particleName == \"mu-\" && M_volName == \"ColMon\"");

  TH2F* hXYTree = new TH2F("hXYTree", "hXYTree", 80,-4,4, 80,-4,4);
  TH2F* hXYRandom = new TH2F("hXYRandom", "hXYRandom", 80,-4,4, 80,-4,4);
  tree->Draw("M_y:M_x>>hXYTree", "M_particleName == \"mu-\" && M_volName == \"ColMon\"");

  TH2F* hPxYTree = new TH2F("hPxYTree", "hPxYTree", 100,-5,5, 80,-4,4);
  TH2F* hPxYRandom = new TH2F("hPxYRandom", "hPxYRandom", 100,-5,5, 80,-4,4);
  tree->Draw("M_y:M_px*1000>>hPxYTree", "M_particleName == \"mu-\" && M_volName == \"ColMon\"");

  TH2F* hPyXTree = new TH2F("hPyXTree", "hPyXTree", 100,-5,5, 80,-4,4);
  TH2F* hPyXRandom = new TH2F("hPyXRandom", "hPyXRandom", 100,-5,5, 80,-4,4);
  tree->Draw("M_x:M_py*1000>>hPyXTree", "M_particleName == \"mu-\" && M_volName == \"ColMon\"");

  /*  double px, py, pz;
  for (int i = 0; i < hPxPyPz->GetEntries(); ++i) {
    bool found = false;
    while (!found) {
      hPxPyPz->GetRandom3(px, py, pz);
    
      // First get the slice in the X-Z plane
      TH3F* hist_for_x = hXPxPz->Clone();
      int bin = hist_for_x->GetZaxis()->FindBin(pz);
      hist_for_x->GetZaxis()->SetRange(bin, bin);
      TH2F* hXPx = hist_for_x->Project3D("xy");

      // Now get the slice in the y plane so that we can randomly select a value for x
      bin = hXPx->GetXaxis()->FindBin(px);
      hXPx->GetXaxis()->SetRange(bin, bin);
      TH1D* hX = hXPx->ProjectionY();

      // Randomly select a value for x
      double x = hX->GetRandom();

      // Delete the hists
      delete hist_for_x;
      delete hXPx;
      delete hX;

      // Now do the same of the y axis
      // First get the slice in the Y-Z plane
      TH3F* hist_for_y = hYPyPz->Clone();
      bin = hist_for_y->GetZaxis()->FindBin(pz);
      hist_for_y->GetZaxis()->SetRange(bin, bin);
      TH2F* hYPy = hist_for_y->Project3D("xy");

      // Now get the slice in the y plane so that we can randomly select a value for y
      bin = hYPy->GetXaxis()->FindBin(py);
      //    std::cout << "bin: " << bin << std::endl;
      hYPy->GetXaxis()->SetRange(bin, bin);
      TH1D* hY = hYPy->ProjectionY();

      // Randomly select a value for y
      double y = hY->GetRandom();

      // Delete the hists
      delete hist_for_y;
      delete hYPy;
      delete hY;

      // Track back to make sure we did pass ellipse
      double delta = 2; // monitor is 20mm in front of hole
      double p_total = std::sqrt(px*px + py*py + pz*pz);

      double unit_px = px / p_total;
      double trackback_x = x - unit_px*delta;
      double unit_py = py / p_total;
      double trackback_y = y - unit_py*delta;

      double ellipse = (trackback_x*trackback_x)/(1.6*1.6) + (trackback_y*trackback_y)/(2.5*2.5);
      //      std::cout << x << ", " << y << " = " << ellipse << "?" << std::endl;
      if (ellipse <= 1) {
	//	std::cout << ellipse << " OK" << std::endl;
	found = true;
      }
    }

    hXYRandom->Fill(x, y);
    hPxYRandom->Fill(px, y);
    hPyXRandom->Fill(py, x);
    //    std::cout << "(px, py, pz) = (" << px << ", " << py << ", " << pz << ")" << std::endl;
    //    std::cout << "(x, y) = (" << x << ", " << y << ")" << std::endl;
  }
  */
  TFile* output = new TFile("CollimatedInput.root", "RECREATE");
  hPxPyPz->Write();
  hXPxPz->Write();
  hYPyPz->Write();
  hPosition->Write();

  hXYTree->Write();
  hXYRandom->Write();
  hPxYTree->Write();
  hPxYRandom->Write();
  hPyXTree->Write();
  hPyXRandom->Write();

  output->Close();
}
