void getMCGeHistAndSave(std::vector<std::string> runNames,
    std::string outputname = "",
    std::string datapath = "../../output/" )
{
  if (outputname == "")
    outputname = runNames.at(0);

  TString fout_name = datapath + "hist" + outputname + ".root";

  if (!gSystem->AccessPathName(fout_name))
  {
    cout<<fout_name<<" exists!"<<endl;
    return;
  }

	// Read Input Files
	TChain *c = new TChain("tree");

  for (int i = 0; i < runNames.size(); ++i)
  {
    // Read Input Files
    std::string runName = runNames.at(i);
    TString dirname(datapath + runName);
    TSystemDirectory dir("dir", dirname);

    TList *files = dir.GetListOfFiles();
    if (files) {
      TSystemFile *file;
      TString fname;
      TIter next(files);
      cout<<files->GetName()<<endl;
      while ((file=(TSystemFile*)next())) 
      {
        fname = file->GetName();
        if (!file->IsDirectory() && fname.EndsWith(".raw") && 
            fname.BeginsWith("0_")) 
        {
          c->Add(dirname + "/" + fname);
          /*cout << fname.Data() << endl;*/
        }
      }
    }
  }
  cout<<"Processing "<<c->GetNtrees()<<" trees"<<endl;

	// Set Branches
	std::vector<double> *M_edep;
  std::vector<std::string> *volName;

	c->SetBranchAddress("M_edep",&M_edep);
  c->SetBranchAddress("M_volName",&volName);

  double res = 0.25; // kEV
  double Elo = 0.;
  double Ehi = 1500.;
  int Nbins = (int)((Ehi - Elo)/res);

  TString histname = "hMC" + outputname;
  TString histtitle = "Ge energy spectrum run " + outputname;
  TH1D *hEdepGe = new TH1D(histname, histtitle, Nbins, Elo, Ehi);

  TRandom3 *r = new TRandom3();

	int nEvents = c->GetEntries();
	std::cout<<"nEvents = "<<nEvents<<std::endl;
	std::stringstream buff;
  for (int iEvent = 0; iEvent < nEvents; iEvent++ ){
    c->GetEntry(iEvent);
    for(int iHit = 0; iHit<M_edep->size(); iHit++)
    {
      double edep = (*M_edep)[iHit]*1e6; // keV
      if ((*volName)[iHit] == "Ge")
      {
        hEdepGe->Fill(r->Gaus(edep, 1.)); // smearing by 1 keV
      }
    }
  }
  
  TFile *fout = new TFile(fout_name, "recreate");
  hEdepGe->Write();
  fout->Close();
}

TH1 * getMCGeHist ( std::string runName = "GeCallibration_32cm_Al10mm",
    std::string datapath = "./../../" )
{
	// Read Input Files
	TChain *c = new TChain("tree");

  TString dirname(datapath + runName);
  TSystemDirectory dir("dir", dirname);
  TList *files = dir.GetListOfFiles();
  if (files) {
    TSystemFile *file;
    TString fname;
    TIter next(files);
    cout<<files->GetName()<<endl;
    while ((file=(TSystemFile*)next())) {
      fname = file->GetName();
      if (!file->IsDirectory() && fname.EndsWith(".raw")) {
        c->Add(dirname + "/" + fname);
        /*cout << fname.Data() << endl;*/
      }
    }
  }

	// Set Branches
	std::vector<double> *M_edep;
	double weight;
  std::vector<std::string> *volName;

	c->SetBranchAddress("M_edep",&M_edep);
	c->SetBranchAddress("weight",&weight);
  c->SetBranchAddress("M_volName",&volName);

  double res = 0.25; // kEV
  double Elo = 0.;
  double Ehi = 1500.;
  int Nbins = (int)((Ehi - Elo)/res);
  TH1D *hEdepGe = new TH1D("hEdepGe", "hEdepGe", Nbins, Elo, Ehi);

  TRandom3 *r = new TRandom3();

	int nEvents = c->GetEntries();
	std::cout<<"nEvents = "<<nEvents<<std::endl;
	std::stringstream buff;
  for (int iEvent = 0; iEvent < nEvents; iEvent++ ){
    c->GetEntry(iEvent);
    for(int iHit = 0; iHit<M_edep->size(); iHit++)
    {
      double edep = (*M_edep)[iHit]*1e6; // keV
      if ((*volName)[iHit] == "Ge")
      {
        hEdepGe->Fill(r->Gaus(edep, 1.)); // smearing by 1 keV
      }
    }
  }

  return hEdepGe;
}

void getMCGeHistAndSave(std::string runName = "GeCalibration_34cm_10mmAl_point",
    std::string datapath = "../../output/", 
    std::string outputpath = "")
{
  TString fout_name; 
  if (outputpath == "")
    fout_name = datapath + "hist" + runName + ".root";
  else
    fout_name = outputpath + "hist" + runName + ".root";

  if (!gSystem->AccessPathName(fout_name))
  {
    cout<<fout_name<<" exists!"<<endl;
    return;
  }

  TH1D *hEdepGe = getMCGeHist(runName, datapath);
  TString histname = "hMC" + runName;
  TString histtitle = "Ge energy spectrum run " + runName;
  hEdepGe->SetName(histname);
  hEdepGe->SetTitle(histtitle);
  
  TFile *fout = new TFile(fout_name, "recreate");
  hEdepGe->Write();
  fout->Close();
}


void list_files(const char *dirname=".", const char *ext=".root")
{
  TSystemDirectory dir(dirname, dirname);
  TList *files = dir.GetListOfFiles();
  if (files) {
    TSystemFile *file;
    TString fname;
    TIter next(files);
    while ((file=(TSystemFile*)next())) {
      fname = file->GetName();
      if (!file->IsDirectory() && fname.EndsWith(ext)) {
        cout << fname.Data() << endl;
      }
    }
  }
}
