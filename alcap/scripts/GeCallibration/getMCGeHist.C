TH1 * getMCGeHist (std::string datapath = "./../../", 
    std::string runName = "GeCallibration_32cm_Al10mm")
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
