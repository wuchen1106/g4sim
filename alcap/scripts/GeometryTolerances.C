#include "TH1F.h"

void GeometryTolerances() {

  const int elements = 7;
  int n_muons[elements] =         {28846358, 34419499, 29815362, 33605572, 29682266, 29761365, 29118220};
  int n_right_protons[elements] = {23603,    24485,    28111,    27108,    24107,    24188,    23378};
  int n_left_protons[elements] =  {23583,    32304,    21226,    27214,    24045,    24003,    23091};
  double ratio_all_protons[elements];
  double error_all_protons[elements];
  double ratio_right_protons[elements];
  double error_right_protons[elements];
  double ratio_left_protons[elements];
  double error_left_protons[elements];
  double asymmetry[elements];
  std::string run_names[elements] = {"base", "target-1cm-left", "target-1cm-right", "target-1cm-forward", "target-1cm-backward", "target-10deg-clockwise", "target-10deg-anticlockwise"};

  TH1F* all_protons  = new TH1F("Histogram_all", "Plot of the number of stopped protons per muon in both SiR2 and SiL2 for different positions of target", elements,0,elements);
  TH1F* right_protons  = new TH1F("Histogram_right", "Plot of the number of stopped protons per muon in SiR2 (ESi1) for different positions of target", elements,0,elements);
  TH1F* left_protons  = new TH1F("Histogram_left", "Plot of the number of stopped protons per muon in SiL2 (ESi2) for different positions of target", elements,0,elements);

  for (int i_element = 0; i_element < elements; ++i_element) {
    
    int n_all_protons = n_left_protons[i_element] + n_right_protons[i_element];
    ratio_all_protons[i_element] = (double)n_all_protons / (double)n_muons[i_element];
    error_all_protons[i_element] = sqrt(n_all_protons) / (double)n_muons[i_element];
    all_protons->Fill(run_names[i_element].c_str(), ratio_all_protons[i_element]);
    all_protons->SetBinError(i_element+1, error_all_protons[i_element]); // bins are numbered 1 to n

    ratio_right_protons[i_element] = (double)n_right_protons[i_element] / (double)n_muons[i_element];
    error_right_protons[i_element] = sqrt(n_right_protons[i_element]) / (double)n_muons[i_element];
    right_protons->Fill(run_names[i_element].c_str(), ratio_right_protons[i_element]);
    right_protons->SetBinError(i_element+1, error_right_protons[i_element]); // bins are numbered 1 to n


    ratio_left_protons[i_element] = (double)n_left_protons[i_element] / (double)n_muons[i_element];
    error_left_protons[i_element] = sqrt(n_left_protons[i_element]) / (double)n_muons[i_element];
    left_protons->Fill(run_names[i_element].c_str(), ratio_left_protons[i_element]);
    left_protons->SetBinError(i_element+1, error_left_protons[i_element]); // bins are numbered 1 to n 

    asymmetry[i_element] = ((double)n_left_protons[i_element] - (double)n_right_protons[i_element]) / (double)n_all_protons[i_element];
  }

  all_protons->SetStats(false);
  right_protons->SetStats(false);
  left_protons->SetStats(false);

  all_protons->GetYaxis()->SetTitle("N_{p} / N_{#mu}");
  all_protons->GetYaxis()->SetLabelSize(0.025);
  all_protons->GetYaxis()->SetRangeUser(0,0.004);

  all_protons->SetLineColor(kBlack);
  all_protons->SetLineWidth(2);
  right_protons->SetLineColor(kRed);
  right_protons->SetLineWidth(2);
  left_protons->SetLineColor(kBlue);
  left_protons->SetLineWidth(2);

  all_protons->Draw("HIST E");
  right_protons->Draw("SAME E");
  left_protons->Draw("SAME E");

  TLegend* legend = new TLegend(0.3, 0.85, 0.6, 0.75, "");
  legend->SetBorderSize(0);
  legend->SetTextSize(0.04);
  legend->SetFillColor(kWhite);
  legend->AddEntry(all_protons, "Protons in SiR2 and SiL2", "l");
  legend->AddEntry(right_protons, "Protons in SiR2", "l");
  legend->AddEntry(left_protons, "Protons in SiL2", "l");
  legend->Draw();

  std::cout << std::endl << "Change from base:" << std::endl;
  double squared_total = 0;
  std::cout << "Setting\tN_muons  N_left  N_right  N_protons\/N_muon  Fractional Difference from base  Asymmetry  Fractional Difference from base\n";
  std::cout << "#######\t#######  ######  #######  ################  ###############################  #########  ###############################  \n";
  for (int i_element = 0; i_element < elements; ++i_element) {
    std::cout << run_names[i_element] << "\t" 
	      << n_muons[i_element] << "  "
	      << n_left_protons[i_element] << "  "
	      << n_right_protons[i_element] << "  "
	      << ratio_all_protons[i_element] << "  ";


    double difference = ratio_all_protons[i_element] - ratio_all_protons[0];
    double diff_over_base = difference / ratio_all_protons[0];
    squared_total += (diff_over_base*diff_over_base);


    double fractional_asymmetry_change = (asymmetry[i_element] - asymmetry[0])/asymmetry[0];

    std::cout << diff_over_base << "  " << asymmetry[i_element] << "  " << fractional_asymmetry_change << std::endl;
  }
  //  std::cout << "Total in quadrature = " << sqrt(squared_total) << std::endl;
}
