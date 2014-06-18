TString MyData = getenv("MYDATA");

void get_peaks(){

	// About Peaks
	double nTotal = 0;
	double nFound = 0;

	// About this run
	std::vector<TString> DirName;
	std::vector<int> nRuns;
	std::vector<TString> FileNames;
	 // ########Should Modify#########
	TString runName = "Ge001";
	FileNames.push_back("../../output/"+runName+".root");
//	DirName.push_back(MyData+"/"+runName);
//	nRuns.push_back(7);
	nAtoms = 7e6;
	 // ########Should Modify#########

	// Read Input Files
	TChain *c = new TChain("tree");
	std::cout<<"FileNames.size() = "<<(FileNames.size())<<std::endl;
	for (int i = 0; i<FileNames.size(); i++){
		std::cout<<"FileNames["<<i<<"] = \""<<FileNames[i]<<"\""<<std::endl;
		c->Add(FileNames[i]);
	}
	std::stringstream buff;
	std::cout<<"nRuns = "<<nRuns.size()<<std::endl;
	for (int iRun = 0; iRun < nRuns.size(); iRun++ ){
		for (int i = 0; i<nRuns[iRun]; i++){
			buff.str("");
			buff.clear();
			buff<<DirName[iRun]<<"/"<<i<<"_job0.raw";
			c->Add( buff.str().c_str());
		}
	}

	// Set Branches
	std::vector<double> *McTruth_e;
	std::vector<int> *McTruth_pid;
	std::vector<int> *McTruth_ptid;
	std::vector<std::string> *McTruth_volume;
	std::vector<std::string> *McTruth_process;
	double weight;

	c->SetBranchAddress("McTruth_process",&McTruth_process);
	c->SetBranchAddress("McTruth_volume",&McTruth_volume);
	c->SetBranchAddress("McTruth_pid",&McTruth_pid);
	c->SetBranchAddress("McTruth_ptid",&McTruth_ptid);
	c->SetBranchAddress("McTruth_e",&McTruth_e);
	c->SetBranchAddress("weight",&weight);

	// Set Output File
//	f = new TFile(runName+".output.root","RECREATE");
//	TTree *t  = new TTree("t","t");

	// Prepare histogram
	TH1D * h1 = new TH1D("h1","h1",30000,0,1500);

	// Loop in events
	int nEvents = c->GetEntries();
	std::cout<<"nEvents = "<<nEvents<<std::endl;
	std::stringstream buff;
	for (int iEvent = 0; iEvent < nEvents; iEvent++ ){
		if (iEvent%1000==0) std::cout<<(double)iEvent/nEvents*100<<" % ..."<<std::endl;
		c->GetEntry(iEvent);
		int nGam = 0;
		for(int iMc = 0; iMc<McTruth_pid->size(); iMc++){
			int pid = (*McTruth_pid)[iMc];
			int ptid = (*McTruth_ptid)[iMc];
			TString volume = (*McTruth_volume)[iMc];
			TString process = (*McTruth_process)[iMc];
			double e = (*McTruth_e)[iMc]*1e6; //keV
			if (pid==22&&volume=="Source"&&process=="RadioactiveDecay"){
				nGam++;
				h1->Fill(e,weight);
			}
		}
		if (nGam)
			nFound+=weight;
		nTotal+=weight;
	}

	double TotalArea = h1->Integral();
	int iPeak = 0;
	double delta = (h1->GetBinCenter(h1->GetNbinsX())-h1->GetBinCenter(0))/h1->GetNbinsX()/2;
	double nFromPeaks = 0;
	std::cout.setf(std::ios_base::fixed);
	std::cout.precision(3);
	std::cout<<"Total events: "<<nTotal<<std::endl;
	std::cout<<"Events with RadioactiveDecay Gamma Ray: "<<nFound<<std::endl;
	std::cout<<"Peaks with ratio over 5%: (error +-"<<delta<<"keV)"<<std::endl;
	std::cout<<"\tIndex\tEnergy\t\tRatio"<<std::endl;
	std::cout<<"\t     \tkeV   \t\tNgam/Nevents"<<std::endl;
	for (int i = 0; i<=h1->GetNbinsX(); i++){
		double height = h1->GetBinContent(i);
		double ratio = height/nTotal;
		if (ratio>0.05){
			std::cout<<"\t"<<iPeak++<<"\t"<<h1->GetBinCenter(i)<<"\t\t"<<ratio<<std::endl;
			nFromPeaks+=height;
		}
	}
	std::cout<<"Gammas from all peaks: "<<TotalArea<<std::endl;
	std::cout<<"Gammas from highest "<<iPeak<<" peaks: "<<nFromPeaks<<std::endl;

	// Write
//	t->Write();
}
