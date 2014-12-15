{
  double moms[7] = {1.04, 1.06, 1.07, 1.08, 1.09, 1.10, 1.12};
  int colors[7] = {kRed, kMagenta, kOrange, kOrange - 6, kGreen -3, kCyan+1, kBlue};

  TCanvas *c = new TCanvas();
  TLegend *leg = new TLegend(0.78, 0.4, 0.89, 0.89, "", "NDC");
  for (int imom = 0; imom < 1; ++imom)
  {
    cout<<moms[imom]<<": ";
    TString fin_name = Form("../../output/Al100_%.2f/0_job0.raw", moms[imom]);
    TFile *fin = new TFile(fin_name);
    TTree *t = (TTree*)fin->Get("tree");
    TString histname = Form("h%.2f", moms[imom]);
    TString drawcmd = "1000*(M_local_Oz - 0.45)>>" + histname + "(50, -125, 25)";
    /*cout<<histname<<", "<<drawcmd<<endl;*/
    /*t->Draw(drawcmd, "M_volName==\"Target\" && M_stopped==1 && M_particleName==\"mu-\" && M_local_Oz>0.35");*/
    t->Draw(drawcmd, "M_volName==\"Target\" && M_stopped==1 && M_particleName==\"mu-\"");
    TH1F *hist = (TH1F*)gDirectory->Get(histname);
    hist->SetLineColor(colors[imom]);
    hist->SetLineWidth(2);
    hist->GetYaxis()->SetRangeUser(0, 2.8*hist->GetMaximum());
    hist->SetTitle("");
    hist->SetXTitle("Depth [#mum]");
    hist->SetYTitle("Counts");
    hist->GetXaxis()->SetTitleSize(0.05);
    hist->GetYaxis()->SetTitleSize(0.05);

    TString leg_entry = Form("%.2f", moms[imom]);
    leg->AddEntry(hist, leg_entry);
    gStyle->SetOptStat(0);
    cout<< (hist->GetEntries()/50000.) <<endl;
  }
  for (int imom = 1; imom < 7; ++imom)
  {
    cout<<moms[imom]<<": ";
    TString fin_name = Form("../../output/Al100_%.2f/0_job0.raw", moms[imom]);
    TFile *fin = new TFile(fin_name);
    TTree *t = (TTree*)fin->Get("tree");
    TString histname = Form("h%.2f", moms[imom]);
    TString drawcmd = "1000*(M_local_Oz - 0.45)>>" + histname + "(50, -125, 25)";
    /*cout<<histname<<", "<<drawcmd<<endl;*/
    t->Draw(drawcmd, "M_volName==\"Target\" && M_stopped==1 && M_particleName==\"mu-\" && M_local_Oz>0.35", "same");
    TH1F *hist = (TH1F*)gDirectory->Get(histname);
    hist->SetLineColor(colors[imom]);
    hist->SetLineWidth(2);
    cout<< (hist->GetEntries()/50000.) <<endl;

    TString leg_entry = Form("%.2f", moms[imom]);
    leg->AddEntry(hist, leg_entry);
  }

  leg->Draw();
  c->Print("al100_scan_mc.pdf");


  /*TFile *f2 = new TFile("../../output/Al100_1.09/0_job0.raw");*/
  /*TTree *t09 = (TTree*)f2->Get("tree");*/

  /*t09->Draw("(M_local_Oz - 0.45)*1000 >>h09(100, -200, 200)","M_volName==\"Target\" && M_stopped==1 && M_particleName==\"mu-\" && M_local_Oz>0.35");*/
  /*cout<<((TH1F*)gDirectory->Get("h09"))->GetEntries()<<endl;*/
}
