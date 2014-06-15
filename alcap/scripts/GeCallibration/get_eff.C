TString MyData = getenv("MYDATA");

void get_eff(){

	// About Peaks
	std::vector<double> vEnergy; // keV
	std::vector<double> vSpread; // keV
	std::vector<double> vRatio;
	std::vector<double> vCount;
	vEnergy.push_back(121.78);
	vRatio.push_back(.282435);

	vEnergy.push_back(244.7);
	vRatio.push_back(.076972);

	vEnergy.push_back(344.27);
	vRatio.push_back(.266054);

	vEnergy.push_back(778.9);
	vRatio.push_back(.129626);

	vEnergy.push_back(964.01);
	vRatio.push_back(.147230);

	vEnergy.push_back(1085.78);
	vRatio.push_back(.102016);

	vEnergy.push_back(1407.95);
	vRatio.push_back(.210716);
	for (int i = 0; i<vEnergy.size(); i++){
		vCount.push_back(0);
//		vSpread.push_back(vEnergy[i]/1000.);
		vSpread.push_back(2);
	}
	double nTotal = 0;
	double nFound = 0;

	// About this run
	std::vector<TString> DirName;
	std::vector<int> nRuns;
	std::vector<TString> FileNames;
	 // ########Should Modify#########
	TString runName = "GeCallibration001";
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
