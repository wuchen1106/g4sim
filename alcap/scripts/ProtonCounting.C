// This ROOT macro counts the number of protons that stop in the thick silicon detectors
void ProtonCounting() {

  TFile* file = new TFile("../output/raw_g4sim_1M_target-10deg-anticlockwise.root", "READ");

  TTree* tree = (TTree*) file->Get("tree");

  std::vector<std::string>* particleName = 0;
  std::vector<std::string>* volName = 0;
  std::vector<int>* stopped = 0;
  
  TBranch* br_particleName = tree->GetBranch("M_particleName");
  TBranch* br_volName = tree->GetBranch("M_volName");
  TBranch* br_stopped = tree->GetBranch("M_stopped");

  br_particleName->SetAddress(&particleName);
  br_volName->SetAddress(&volName);
  br_stopped->SetAddress(&stopped);

  int n_protons = 0;
  for (int iEntry = 0; iEntry < tree->GetEntries(); ++iEntry) {

    tree->GetEvent(iEntry);

    for (int iElement = 0; iElement < particleName->size();  ++iElement) {

      if (particleName->at(iElement) == "proton" && (volName->at(iElement) == "ESi1" || volName->at(iElement) == "ESi2") && stopped->at(iElement) == 1) {
	//std::cout << "A photon reached the Germanium!" << std::endl;
	++n_protons;
      }
    }
  }

  std::cout << "There were " << n_protons << " protons that reached the thick silicon detectors out of " << tree->GetEntries() << " muons." << std::endl;
  std::cout << "Acceptance = " << (double) n_protons / tree->GetEntries() << std::endl;
}
