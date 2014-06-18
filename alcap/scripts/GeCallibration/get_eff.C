TString MyData = getenv("MYDATA");

void get_eff(){

	// About Peaks
	std::vector<double> vEnergy; // keV
	std::vector<double> vSpread; // keV
	std::vector<double> vRatio;
	std::vector<double> vCount;
	vEnergy.push_back(121.775);
	vRatio.push_back(0.282);

	vEnergy.push_back(244.675);
	vRatio.push_back(0.077);

	vEnergy.push_back(344.275);
	vRatio.push_back(0.266);

	vEnergy.push_back(778.925);
	vRatio.push_back(0.130);

	vEnergy.push_back(964.075);
	vRatio.push_back(0.146);

	vEnergy.push_back(1085.875);
	vRatio.push_back(0.102);

	vEnergy.push_back(1112.075);
	vRatio.push_back(0.137);

	vEnergy.push_back(1407.975);
	vRatio.push_back(0.211);

	for (int i = 0; i<vEnergy.size(); i++){
		vCount.push_back(0);
//		vSpread.push_back(vEnergy[i]/1000.);
		vSpread.push_back(0.025);
	}
	double nTotal = 0;
	double nFound = 0;

	// About this run
	std::vector<TString> DirName;
	std::vector<int> nRuns;
	std::vector<TString> FileNames;
	 // ########Should Modify#########
	TString runName = "GeCallibration_35cm";
//	FileNames.push_back("../../output/"+runName+".root");
	DirName.push_back(MyData+"/"+runName);
	nRuns.push_back(7);
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
		std::cout<<"DirName["<<i<<"] = \""<<DirName[i]<<"\""<<std::endl;
		for (int i = 0; i<nRuns[iRun]; i++){
			buff.str("");
			buff.clear();
			buff<<DirName[iRun]<<"/"<<i<<"_job0.raw";
			c->Add( buff.str().c_str());
		}
	}

	// Set Branches
	std::vector<double> *M_edep;
	double weight;

	c->SetBranchAddress("M_edep",&M_edep);
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
		for(int iHit = 0; iHit<M_edep->size(); iHit++){
			double edep = (*M_edep)[iHit]*1e6; // keV
			for (int iPeak = 0; iPeak<vEnergy.size(); iPeak++){
				if (edep>vEnergy[iPeak]-vSpread[iPeak]&&edep<vEnergy[iPeak]+vSpread[iPeak]){
					vCount[iPeak]+=weight;
				}
			}
		}
	}
	std::cout<<"Total events: "<<nAtoms<<std::endl;
	for(int iPeak = 0; iPeak < vEnergy.size(); iPeak++){
		std::cout<<"Peak["<<iPeak<<"]: "<<vEnergy[iPeak]<<"+-"<<vSpread[iPeak]/vEnergy[iPeak]*100<<"% keV, Efficiency: "<<vCount[iPeak]/nAtoms/vRatio[iPeak]<<std::endl;
	}
	std::cout<<"logE\t\t\tlogEff"<<std::endl;
	for(int iPeak = 0; iPeak < vEnergy.size(); iPeak++){
		std::cout<<log(vEnergy[iPeak])<<"\t\t\t"<<log(vCount[iPeak]/nAtoms/vRatio[iPeak])<<std::endl;
	}

	// Write
//	t->Write();
}
