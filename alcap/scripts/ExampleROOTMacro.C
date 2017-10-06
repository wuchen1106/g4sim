// This ROOT macro counts the number of photons that hit the Germanium
void ExampleROOTMacro() {

  TFile* file = new TFile("../output/raw_g4sim.root", "READ");

  TTree* tree = (TTree*) file->Get("tree");

  std::vector<std::string>* particleName = 0;
  std::vector<std::string>* volName = 0;
  
  TBranch* br_particleName = tree->GetBranch("M_particleName");
  TBranch* br_volName = tree->GetBranch("M_volName");

  br_particleName->SetAddress(&particleName);
  br_volName->SetAddress(&volName);

  int n_photons = 0;
  for (int iEntry = 0; iEntry < tree->GetEntries(); ++iEntry) {

    tree->GetEvent(iEntry);

    for (int iElement = 0; iElement < particleName->size();  ++iElement) {

      if (particleName->at(iElement) == "gamma" && volName->at(iElement) == "Ge") {
	//std::cout << "A photon reached the Germanium!" << std::endl;
	++n_photons;
      }
    }
  }

  std::cout << "There were " << n_photons << " photons that reached the Germanium detector out of " << tree->GetEntries() << std::endl;
  std::cout << "Acceptance = " << (double) n_photons / tree->GetEntries() << std::endl;
}
