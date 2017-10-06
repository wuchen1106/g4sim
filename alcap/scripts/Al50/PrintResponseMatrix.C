void PrintResponseMatrix() {

  std::string savelocation = "~/g4sim/alcap/plots/Unfolding";
  const int n_arms = 2;

  std::string filename = "output.root";
  TFile* file = new TFile(filename.c_str());
  RooUnfoldResponse* SiL_Response = (RooUnfoldResponse*) file->Get("SiL_response");
  RooUnfoldResponse* SiR_Response = (RooUnfoldResponse*) file->Get("SiR_response");

  RooUnfoldResponse* responses[n_arms] = {SiL_Response, SiR_Response};
  std::string arm_names[n_arms] = {"SiL", "SiR"};

  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {

    TH2F* hResponse = responses[i_arm]->Hresponse();
    hResponse->SetXTitle("Observed E [keV]");
    hResponse->SetXTitle("True E [keV]");
    hResponse->Draw("COLZ");
    c1->SetLogz(0);
    std::string plotname = savelocation+"/"+arm_names[i_arm]+"_ResponseMatrix";
    std::string pdfname = plotname+".pdf";
    std::string pngname = plotname+".png";
    c1->SaveAs(pdfname.c_str());
    c1->SaveAs(pngname.c_str());
  }
}
