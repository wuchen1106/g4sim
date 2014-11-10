// This ROOT macro counts the number of protons that stop in the thick silicon detectors
void ProtonCounting(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "READ");

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
  int n_protons_right = 0;
  int n_protons_left = 0;
  int n_stopped_muons = 0;
  for (int iEntry = 0; iEntry < tree->GetEntries(); ++iEntry) {

    tree->GetEvent(iEntry);

    for (int iElement = 0; iElement < particleName->size();  ++iElement) {

      if (particleName->at(iElement) == "proton" && volName->at(iElement) == "ESi1" && stopped->at(iElement) == 1) {
	++n_protons_right;
	++n_protons;
      }
      else if (particleName->at(iElement) == "proton" && volName->at(iElement) == "ESi2" && stopped->at(iElement) == 1) {
	++n_protons_left;
	++n_protons;
      }
      else if (particleName->at(iElement) == "mu-" && volName->at(iElement) == "Target" && stopped->at(iElement) == 1) {
	++n_stopped_muons;
      }
    }
  }

  std::cout << "There were " << n_protons << " protons that reached the thick silicon detectors out of " << n_stopped_muons << " stopped muons (" << tree->GetEntries() << " input muons." << std::endl;
  std::cout << "N_right = " << n_protons_right << ", N_left = " << n_protons_left << std::endl;
  std::cout << "Acceptance = " << (double) n_protons / n_stopped_muons << std::endl;
}
