{
  double moms[7] = {1.04, 1.06, 1.07, 1.08, 1.09, 1.10, 1.12};
  for (int imom = 0; imom < 7; ++imom)
  {
    cout<<moms[imom]<<": ";
    TString fin_name = Form("../../output/Al100_%.2f/0_job0.raw", moms[imom]);
    TFile *fin = new TFile(fin_name);
    TTree *t = (TTree*)fin->Get("tree");
    TString histname = Form("h%.2f", moms[imom]);
    TString drawcmd = "M_local_Oz>>" + histname + "(100, -0.5, 0.5)";
    /*cout<<histname<<", "<<drawcmd<<endl;*/
    t->Draw(drawcmd, "M_volName==\"Target\" && M_stopped==1 && M_particleName==\"mu-\" && M_local_Oz>0.35");
    TH1F *hist = (TH1F*)gDirectory->Get(histname);
    cout<< (hist->GetEntries()/50000.) <<endl;
  }

  TFile *f2 = new TFile("../../output/Al100_1.09/0_job0.raw");
  TTree *t09 = (TTree*)f2->Get("tree");

  t09->Draw("(M_local_Oz - 0.45)*1000 >>h09(100, -200, 200)","M_volName==\"Target\" && M_stopped==1 && M_particleName==\"mu-\" && M_local_Oz>0.35");
  cout<<((TH1F*)gDirectory->Get("h09"))->GetEntries()<<endl;
}
