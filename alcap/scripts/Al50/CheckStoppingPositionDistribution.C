void CheckStoppingPositionDistribution(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "READ");
  THnSparseF* hGlobalStoppingPositions = (THnSparseF*) file->Get("hGlobalStoppingPositions");

  double bin_width = 0.0001; // cm
  double x_low = -5;
  double x_high = 5;
  int n_bins = (x_high - x_low) / bin_width;
  TH3F* hRandomStoppingPositions = new TH3F("hRandomStoppingPositions", "Positions of Stopped Muons in Target (random, global coords)", n_bins,x_low,x_high, n_bins,x_low,x_high, n_bins,x_low,x_high);

  double random[3];

  for (int i_particle = 0; i_particle < 10000; ++i_particle) {
    hGlobalStoppingPositions->GetRandom(random);
    hRandomStoppingPositions->Fill(random[0], random[1], random[2]);
  }
  hRandomStoppingPositions->Draw();
}
