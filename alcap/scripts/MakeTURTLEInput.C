void MakeTURTLEInput() {

  TFile* out_file = new TFile("TURTLE_fits.root", "RECREATE");

  const int n_xpos_bins = 10;
  TH1F* hXPositions = new TH1F("hXPositions", "X Positions from TURTLE", n_xpos_bins,-0.5,0.5);
  hXPositions->GetXaxis()->SetTitle("x [cm]");

  const int n_ypos_bins = 10;
  TH1F* hYPositions = new TH1F("hYPositions", "Y Positions from TURTLE", n_ypos_bins,-0.5,0.5);
  hYPositions->GetXaxis()->SetTitle("x [cm]");

  const int n_xang_bins = 30;
  TH1F* hXAngles = new TH1F("hXAngles", "X Angles from TURTLE", n_xang_bins,-200,100);
  hXAngles->GetXaxis()->SetTitle("x' [mrad]");

  const int n_yang_bins = 20;
  TH1F* hYAngles = new TH1F("hYAngles", "Y Angles from TURTLE", n_yang_bins,-200,200);
  hYAngles->GetXaxis()->SetTitle("y' [mrad]");

  double x_positions[n_xpos_bins] = {3694, 4850, 5306, 5529, 5620, 5604, 5347, 5053, 4733, 3289};
  for (int i_bin = 1; i_bin <= n_xpos_bins; ++i_bin) {

    hXPositions->SetBinContent(i_bin, x_positions[i_bin-1]);
  }

  double y_positions[n_ypos_bins] = {224, 1097, 3729, 8639, 10819, 10798, 8618, 3816, 1056, 229};
  for (int i_bin = 1; i_bin <= n_ypos_bins; ++i_bin) {

    hYPositions->SetBinContent(i_bin, y_positions[i_bin-1]);
  }

  double x_angles[n_xang_bins] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 665, 5214, 7968, 10117, 10492, 8969, 5529, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  for (int i_bin = 1; i_bin <= n_xang_bins; ++i_bin) {

    hXAngles->SetBinContent(i_bin, x_angles[i_bin-1]);
  }

  double y_angles[n_yang_bins] = {0, 555, 2764, 2802, 2970, 3092, 2975, 3108, 3071, 3087, 3098, 3144, 3158, 3066, 3001, 2931, 2840, 2763, 600, 0};
  for (int i_bin = 1; i_bin <= n_yang_bins; ++i_bin) {

    hYAngles->SetBinContent(i_bin, y_angles[i_bin-1]);
  }

  TFitResultPtr x_position_result =  hXPositions->Fit("gaus", "S");
  TFitResultPtr x_angle_result =  hXAngles->Fit("gaus", "S");
  TFitResultPtr y_position_result =  hYPositions->Fit("gaus", "S");
  x_position_result->Write();
  x_angle_result->Write();
  y_position_result->Write();

  /*  std::cout << y_angle_result->NPar() << std::endl;
  std::cout << y_angle_result->Index("p0") << std::endl;
  std::cout << *(y_angle_result->GetParams()) << std::endl;
  std::cout << y_angle_result->IsParameterBound(0) << std::endl;
  double lower, upper;
  y_angle_result->ParameterBounds(0,lower,upper);
  std::cout << lower << ", " << upper << std:::endl;
  */
  /*
  TH1F* hRandom = new TH1F("hRandom", "hRandom", n_xpos_bins*10, -0.5,0.5);

  for (int i_draw = 0; i_draw < 10000000; ++i_draw) {
    double val = hXPositions->GetRandom();
    hRandom->Fill(val);
  }
  hRandom->Draw();
  */
}
