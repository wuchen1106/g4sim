void PlotBeamline() {

  TFile* file = new TFile("../output/raw_g4sim.root", "READ");
  TTree* tree = (TTree*) file->Get("tree");

  const int n_monitors = 17;
  std::string monitor_names[n_monitors] = { "MuTrigger", "MuFoil", "MuSCA", "MuSC_FD", "MuSC", "MuSC_FU", "Mon", "MuPCUSWindow", "MuPC", "MuPCDSWindow", "BeamWindow", "BeamPortMon", "BeamPortCentreMon", "CollimatorLead", "ColMon", "Target", "FullBW" };

  TCanvas* c1 = new TCanvas("c1", "c1");

  /*  for (int i_monitor = 0; i_monitor < n_monitors; ++i_monitor) {
    std::string monitor_name = monitor_names[i_monitor];

    std::string histname = "hXY_" + monitor_name;
    std::string drawcmd = "M_y*10:M_x*10>>"+histname;
    std::string cutcmd = "M_particleName==\"mu-\" && M_volName==\""+monitor_name+"\"";
    std::string pngname = histname+".png";
    TH2D* hXY = new TH2D(histname.c_str(), histname.c_str(), 100,-50,50, 100,-50,50);
    hXY->GetXaxis()->SetTitle("x [mm]");
    hXY->GetYaxis()->SetTitle("y [mm]");
    tree->Draw(drawcmd.c_str(), cutcmd.c_str(), "COLZ");
    c1->SaveAs(pngname.c_str());
    delete hXY;

    histname = "hMom_" + monitor_name;
    drawcmd = "sqrt(M_px*M_px + M_py*M_py + M_pz*M_pz)*1000>>"+histname;
    //    cutcmd = "M_particleName==\"mu-\" && M_volName==\""+monitor_name+"\"";
    pngname = histname+".png";
    TH1D* hMom = new TH1D(histname.c_str(), histname.c_str(), 100,0,35);
    hMom->GetXaxis()->SetTitle("Momentum [MeV/c]");
    tree->Draw(drawcmd.c_str(), cutcmd.c_str(), "COLZ");
    c1->SaveAs(pngname.c_str());
    delete hMom;

  }
  */
  TH2D* hZCoords = new TH2D("hZCoords", "hZCoords", 10,0,-1, 5000,-350,150);
  tree->Draw("M_z*10:M_volName>>hZCoords", "M_particleName==\"mu-\" && (M_volName!=\"ESi1\" && M_volName!=\"dESi1\" && M_volName!=\"ESi2\" && M_volName!=\"dESi2\" && M_volName!=\"Ge\")", "COLZ");
  hZCoords->SetStats(false);
  hZCoords->GetYaxis()->SetTitle("z [mm]");
  c1->SaveAs("hZCoords.png");

  TH1D* hNumbers = hZCoords->ProjectionX();
  hNumbers->Draw();
  c1->SaveAs("hNumbers.png");

  TH2D* hMomentum = new TH2D("hMomentum", "hMomentum", 10,0,-1, 100,0,35);
  tree->Draw("sqrt(M_px*M_px + M_py*M_py + M_pz*M_pz)*1000:M_volName>>hMomentum", "M_particleName==\"mu-\" && (M_volName!=\"ESi1\" && M_volName!=\"dESi1\" && M_volName!=\"ESi2\" && M_volName!=\"dESi2\" && M_volName!=\"Ge\")", "COLZ");
  hMomentum->SetStats(false);
  hMomentum->GetYaxis()->SetTitle("Momentum [MeV/c]");
  c1->SaveAs("hMomentum.png");

  c1->SetLogz(1);
  c1->SaveAs("hMomentum_LogZ.png");

}
