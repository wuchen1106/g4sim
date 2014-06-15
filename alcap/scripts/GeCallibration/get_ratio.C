TString MyData = getenv("MYDATA");

void get_ratio(){

	// About Peaks
	std::vector<double> vEnergy; // keV
	std::vector<double> vSpread; // keV
	std::vector<double> vCount;
	vEnergy.push_back(121.78);
	vEnergy.push_back(244.7);
	vEnergy.push_back(344.27);
	vEnergy.push_back(778.9);
	vEnergy.push_back(964.01);
	vEnergy.push_back(1085.78);
	vEnergy.push_back(1407.95);
	for (int i = 0; i<vEnergy.size(); i++){
		vCount.push_back(0);
		vSpread.push_back(vEnergy[i]/1000.);
	}
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
//			if (pid==22&&volume=="Source"&&process=="RadioactiveDecay"&&ptid==1){
			if (pid==22&&volume=="Source"&&process=="RadioactiveDecay"){
				nGam++;
				for (int iPeak = 0; iPeak<vEnergy.size(); iPeak++){
					if (e>vEnergy[iPeak]-vSpread[iPeak]&&e<vEnergy[iPeak]+vSpread[iPeak]){
						vCount[iPeak]+=weight;
					}
				}
			}
		}
//		if (nGam>1) std::cout<<"nGam = "<<nGam<<std::endl;
		if (nGam)
			nFound+=weight;
		nTotal+=weight;
	}
	std::cout<<"Total events: "<<nTotal<<std::endl;
	std::cout<<"Events with RadioactiveDecay Gamma Ray: "<<nFound<<std::endl;
	double nFromPeaks = 0;
	for(int iPeak = 0; iPeak < vEnergy.size(); iPeak++){
		std::cout<<"Peak["<<iPeak<<"]: "<<vEnergy[iPeak]<<"+-"<<vSpread[iPeak]/vEnergy[iPeak]*100<<"% keV, "<<vCount[iPeak]/nTotal<<std::endl;
		nFromPeaks+=vCount[iPeak];
	}
	std::cout<<"Events with Gamma from Peaks: "<<nFromPeaks<<std::endl;

	// Write
//	t->Write();
}
