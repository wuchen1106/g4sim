{
  gROOT->SetBatch();
  TFile *fin01 = new TFile("../output/GeCalibration_34cm_10mmAl_point_run2/1_job0.raw");
  TFile *fin03 = new TFile("../output/GeCalibration_34cm_10mmAl_point_run2/3_job0.raw");
  TFile *fin04 = new TFile("../output/GeCalibration_34cm_10mmAl_point_run2/4_job0.raw");
  TFile *fin05 = new TFile("../output/GeCalibration_34cm_10mmAl_point_run2/5_job0.raw");
  TFile *fin06 = new TFile("../output/GeCalibration_34cm_10mmAl_point_run2/6_job0.raw");
  TFile *fin07 = new TFile("../output/GeCalibration_34cm_10mmAl_point_run2/7_job0.raw");
  TFile *fin10 = new TFile("../output/GeCalibration_34cm_10mmAl_point_run2/0_job1.raw");
  TFile *fin20 = new TFile("../output/GeCalibration_34cm_10mmAl_point_run2/0_job2.raw");

  TTree *tree01 = fin01->Get("tree");
  TTree *tree03 = fin04->Get("tree");
  TTree *tree04 = fin04->Get("tree");
  TTree *tree05 = fin05->Get("tree");
  TTree *tree06 = fin06->Get("tree");
  TTree *tree07 = fin07->Get("tree");
  TTree *tree10 = fin10->Get("tree");
  TTree *tree20 = fin20->Get("tree");

  tree01->Draw("M_edep*1E6>>h01(1024)","","goff");
  tree03->Draw("M_edep*1E6>>h03(1024)","","goff");
  tree04->Draw("M_edep*1E6>>h04(1024)","","goff");
  tree05->Draw("M_edep*1E6>>h05(1024)","","goff");
  tree06->Draw("M_edep*1E6>>h06(1024)","","goff");
  tree07->Draw("M_edep*1E6>>h07(1024)","","goff");
  tree10->Draw("M_edep*1E6>>h10(1024)","","goff");
  tree20->Draw("M_edep*1E6>>h20(1024)","","goff");
  
  TH1F* h01 = gDirectory->Get("h01");
  TH1F* h03 = gDirectory->Get("h03");
  TH1F* h04 = gDirectory->Get("h04");
  TH1F* h05 = gDirectory->Get("h05");
  TH1F* h06 = gDirectory->Get("h06");
  TH1F* h07 = gDirectory->Get("h07");
  TH1F* h10 = gDirectory->Get("h10");
  TH1F* h20 = gDirectory->Get("h20");

  h01->Print();
  h03->Print();
  h04->Print();
  h05->Print();
  h06->Print();
  h07->Print();
  h10->Print();
  h20->Print();
  
  TCanvas *c = new TCanvas("c","c", 1000, 1000);
  c->Divide(3,3);
  c->cd(1);
  h01->Draw();
  c->cd(2);
  h03->Draw();
  c->cd(3);
  h04->Draw();
  c->cd(4);
  h05->Draw();
  c->cd(5);
  h06->Draw();
  c->cd(6);
  h07->Draw();
  c->cd(7);
  h10->Draw();
  c->cd(8);
  h20->Draw();
  c->Print("rndmchck.pdf");
}
