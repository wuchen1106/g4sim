//---------------------------------------------------------------------------//
//Description: Setup PrimaryGenerator
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "PrimaryGeneratorAction.hh"

#include "PrimaryGeneratorMessenger.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "Randomize.hh"

//supported geometry
#include "MyDetectorManager.hh"
#include "MyVGeometrySvc.hh"
#include "MyVGeometryParameter.hh"
#include "SimpleGeometryParameter.hh"

#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TChain.h"

#include <stdlib.h>
#include <iostream>
#include <fstream>

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
	//create a messenger for this class
	gunMessenger = new PrimaryGeneratorMessenger(this);

	// default particle kinematic
	G4String file_name = getenv("GENFILEROOT");
	size_t sLast = file_name.last('/');
	if(sLast==std::string::npos){ // file name only
		G4String dir_name = getenv("CONFIGUREROOT");
		if (dir_name[dir_name.size()-1] != '/') dir_name.append("/");
		file_name = dir_name + file_name;
	}
	ReadCard(file_name);
	if (fType == "simple"){
		G4int n_particle = 1;
		particleGun  = new G4ParticleGun(n_particle);
	}
	else {
		G4Exception("PrimaryGeneratorAction::PrimaryGeneratorAction()","Run0031",
				FatalException, "unknown generator type.");
	}

	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4ParticleDefinition* particle = particleTable->FindParticle(ParticleName);
	if (!particle){
		std::cout<<"ERROR: In PrimaryGeneratorAction::PrimaryGeneratorAction() Cannot find particle "<<ParticleName<<"!!!"<<std::endl;
		G4Exception("PrimaryGeneratorAction::PrimaryGeneratorAction()","Run0031",
				FatalException, "Cannot find particle.");
	}
	particleGun->SetParticleDefinition(particle);
	G4ThreeVector dir(1,0,0);
	dir.setTheta(Theta);
	dir.setPhi(Phi);
	particleGun->SetParticleMomentumDirection(dir);
	particleGun->SetParticleMomentum(Pa);
	particleGun->SetParticlePosition(G4ThreeVector(x,y,z));
	particleGun->SetParticleTime(t);

	if (EnergyMode == "histo" || DirectionMode == "histo" ){
		BuildHistoFromFile();
	}
	UseRoot = false;
	if ( EnergyMode == "root" || PositionMode == "root" || TimeMode == "root" ){
		UseRoot = true;
		root_build();
	}
	if ( EnergyMode == "root" ){
		UseRoot = true;
		root_set_Energy();
	}
	if ( PositionMode == "root" ){
		UseRoot = true;
		root_set_Position();
	}
	if ( TimeMode == "root" ){
		UseRoot = true;
		root_set_Time();
	}
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
	delete particleGun;
	delete gunMessenger;
	delete EM_hist;
	delete DM_hist;
	delete m_TChain;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	//this function is called at the begining of event
	// 
	if (UseRoot){
		root_get_para();
	}

	if ( EnergyMode == "histo"){
		G4double mom = EM_hist->GetRandom() * MeV;
		particleGun->SetParticleMomentum(mom);
	}
	else if ( EnergyMode == "root" ){
		//std::cout<<"PGA EM = root!"
		//	     <<", ("<<root_para[3]
		//	     <<","<<root_para[4]
		//	     <<","<<root_para[5]
		//	     <<") MeV"
		//	     <<std::endl;
		particleGun->SetParticleMomentum(G4ThreeVector(root_para[3] * MeV, root_para[4] * MeV, root_para[5] * MeV));
	}
	else if ( EnergyMode == "RMC" ){
		particleGun->SetParticleMomentum(100*MeV+2*MeV*G4UniformRand());
	}
	else if ( EnergyMode != "none" ){
		std::cout<<"ERROR: unknown EnergyMode: "<<EnergyMode<<"!!!"<<std::endl;
		G4Exception("PrimaryGeneratorAction::GeneratePrimaries()",
				"InvalidSetup", FatalException,
				"unknown EnergyMode");
	}

	if ( DirectionMode == "uniform" ){
		SetUniformDirection();
	}
	else if ( DirectionMode == "histo" ){
		G4double theta = DM_hist->GetRandom() * rad;
		G4double phi = G4UniformRand() * 360. *deg;
		G4ThreeVector dir_3Vec(1, 1, 1);
		dir_3Vec.setTheta(theta);
		dir_3Vec.setPhi(phi);
		particleGun->SetParticleMomentumDirection(dir_3Vec);
	}
	else if ( DirectionMode != "none" ){
		std::cout<<"ERROR: unknown DirectionMode: "<<DirectionMode<<"!!!"<<std::endl;
		G4Exception("PrimaryGeneratorAction::GeneratePrimaries()",
				"InvalidSetup", FatalException,
				"unknown DirectionMode");
	}

	if ( PositionMode == "uniform" ){
		SetUniformPosition();
	}
	else if ( PositionMode == "root" ){
		//std::cout<<"PGA PM = root!"
		//	     <<", ("<<root_para[0]
		//	     <<","<<root_para[1]
		//	     <<","<<root_para[2]
		//	     <<") mm"
		//	     <<std::endl;
		particleGun->SetParticlePosition(G4ThreeVector(root_para[0] * mm, root_para[1] * mm, (root_para[2])*mm));
	}
	else if ( PositionMode != "none" ){
		std::cout<<"ERROR: unknown PositionMode: "<<PositionMode<<"!!!"<<std::endl;
		G4Exception("PrimaryGeneratorAction::GeneratePrimaries()",
				"InvalidSetup", FatalException,
				"unknown PositionMode");
	}

	if ( TimeMode == "root" ){
		//std::cout<<"PGA TM = root!"
		//	     <<", ("<<root_para[7]
		//	     <<") ns"
		//	     <<std::endl;
		particleGun->SetParticleTime(root_para[6]*ns);
	}
	else if ( TimeMode != "none" ){
		std::cout<<"ERROR: unknown TimeMode: "<<TimeMode<<"!!!"<<std::endl;
		G4Exception("PrimaryGeneratorAction::GeneratePrimaries()",
				"InvalidSetup", FatalException,
				"unknown TimeMode");
	}

	particleGun->GeneratePrimaryVertex(anEvent);
}

void PrimaryGeneratorAction::SetUniformDirection(){
	G4double dir_x = 0., dir_y = 0., dir_z = 0.;
	G4bool dir_OK = false;
	while( !dir_OK ){
		dir_x=G4UniformRand()-0.5;
		dir_y=G4UniformRand()-0.5;
		dir_z=G4UniformRand()-0.5;
		if ( dir_x*dir_x + dir_y*dir_y + dir_z*dir_z <= 0.025 && dir_x*dir_x + dir_y*dir_y + dir_z*dir_z != 0) dir_OK = true;
	}
	G4ThreeVector dir_3Vec(dir_x, dir_y, dir_z);
	particleGun->SetParticleMomentumDirection(dir_3Vec);
}

void PrimaryGeneratorAction::SetUniformPosition(){
	MyVGeometryParameter* pMyVGeometryParameter = MyDetectorManager::GetMyDetectorManager()->GetSvc(UP_SubDet)->get_GeometryParameter();
	if (!pMyVGeometryParameter){
		std::cout<<"ERROR: in PrimaryGeneratorAction::SetUniformPosition cannot find : "<<UP_SubDet<<"!!!"<<std::endl;
		G4Exception("PrimaryGeneratorAction::SetUniformPosition()",
				"InvalidInput", FatalException,
				"cannot ");
	}
	if ( UP_Type == "Simple" ){
		SimpleGeometryParameter* pSimpleGeometryParameter = dynamic_cast<SimpleGeometryParameter*> (pMyVGeometryParameter);
		if (!pSimpleGeometryParameter){
			std::cout<<"ERROR: in PrimaryGeneratorAction::SetUniformPosition cannot convert : "<<UP_SubDet<<" from MyVGeometryParameter to SimpleGeometryParameter!!!"<<std::endl;
			G4Exception("PrimaryGeneratorAction::SetUniformPosition()",
					"InvalidInput", FatalException,
					"cannot convert from MyVGeometryParameter to SimpleGeometryParameter");
		}
		int index = pSimpleGeometryParameter->get_VolIndex(UP_Volume);
		if ( index == -1 ){
			std::cout<<"ERROR: in PrimaryGeneratorAction::SetUniformPosition cannot find : "<<UP_Volume<<"!!!"<<std::endl;
			G4Exception("PrimaryGeneratorAction::SetUniformPosition()",
					"InvalidInput", FatalException,
					"cannot convert volume");
		}
		G4String sol_type = pSimpleGeometryParameter->get_SolidType(index);
		if ( sol_type == "Tubs" ){
			G4double RMax,RMin,length,StartPhi,SpanPhi;
			G4double xp,yp,zp,space,n,theta,phi;
			G4int iTubs = pSimpleGeometryParameter->get_SolidIndex(index);
			xp = pSimpleGeometryParameter->get_PosX(index);
			yp = pSimpleGeometryParameter->get_PosY(index);
			zp = pSimpleGeometryParameter->get_PosZ(index);
			space = pSimpleGeometryParameter->get_Space(index);
			n = pSimpleGeometryParameter->get_RepNo(index);
			theta = pSimpleGeometryParameter->get_DirTheta(index);
			phi = pSimpleGeometryParameter->get_DirPhi(index);
			RMax = pSimpleGeometryParameter->get_Tubs_RMax(iTubs);
			RMin = pSimpleGeometryParameter->get_Tubs_RMin(iTubs);
			length = pSimpleGeometryParameter->get_Tubs_Length(iTubs);
			StartPhi = pSimpleGeometryParameter->get_Tubs_StartAng(iTubs);
			SpanPhi = pSimpleGeometryParameter->get_Tubs_SpanAng(iTubs);

			G4double iz, iphi, ir;
			G4int ivol;
			iz = G4UniformRand()*length -length/2;
			iphi = G4UniformRand()*SpanPhi + StartPhi;
			ir = G4UniformRand()*(RMax*RMax-RMin*RMin)+RMin*RMin;
			ir = sqrt(ir);
			ivol = G4UniformRand()*n;
			G4ThreeVector pos(1,0,0);
			pos.setPhi(iphi);
			pos.setRho(ir);
			pos.setZ(iz);
			pos += G4ThreeVector(xp,yp,zp);
			G4ThreeVector dir(1, 0, 0);
			dir.setTheta(theta);
			dir.setPhi(phi);
			dir.setMag(space);
			pos += dir * (2*ivol - n + 1)/2.;
			particleGun->SetParticlePosition(pos);
		}
		else{
			std::cout<<"ERROR: in PrimaryGeneratorAction::SetUniformPosition unsupported solid type: "<<sol_type<<"!!!"<<std::endl;
			G4Exception("PrimaryGeneratorAction::SetUniformPosition()",
					"InvalidInput", FatalException,
					"unsupported solid type");
		}
	}
	else{
		std::cout<<"ERROR: in PrimaryGeneratorAction::SetUniformPosition unsopported parameter class type: "<<UP_Type<<"!!!"<<std::endl;
		G4Exception("PrimaryGeneratorAction::SetUniformPosition()",
				"InvalidInput", FatalException,
				"unsopported parameter class type");
	}
}

void PrimaryGeneratorAction::BuildHistoFromFile(){
	G4String dir_name = getenv("CONFIGUREDATAROOT");
	if (dir_name[dir_name.size()-1] != '/') dir_name.append("/");

	// EnergyMode
	if (EnergyMode=="histo"){
		G4String full_infile_name = dir_name +  EM_hist_filename;
		if (fp) delete fp;
		fp = new TFile(full_infile_name.c_str());
		if (fp==NULL) {
			std::cout<<"ERROR: Can not find file: "<<full_infile_name<<"!!!"<<std::endl;
			G4Exception("PrimaryGeneratorAction::BuildHistoFromFile()",
					"InvalidInput", FatalException,
					"Can not find file");
		}
		TH1F* h = (TH1F*)fp->Get(EM_hist_histname.c_str());
		if(h==NULL){
			std::cout<<"ERROR: Can not find file: "<<full_infile_name<<"!!!"<<std::endl;
			G4Exception("PrimaryGeneratorAction::BuildHistoFromFile()",
					"InvalidInput", FatalException,
					"Can not find file");
		}
		EM_hist = h;
	}

	// DirectionMode
	if (DirectionMode=="histo"){
		G4String full_infile_name = dir_name +  DM_hist_filename;
		if (fp) delete fp;
		fp = new TFile(full_infile_name.c_str());
		if (fp==NULL) {
			std::cout<<"ERROR: Can not find file: "<<full_infile_name<<"!!!"<<std::endl;
			G4Exception("PrimaryGeneratorAction::BuildHistoFromFile()",
					"InvalidInput", FatalException,
					"Can not find file");
		}
		TH1F* h = (TH1F*)fp->Get(DM_hist_histname.c_str());
		if(h==NULL){
			std::cout<<"ERROR: Can not find file: "<<full_infile_name<<"!!!"<<std::endl;
			G4Exception("PrimaryGeneratorAction::BuildHistoFromFile()",
					"InvalidInput", FatalException,
					"Can not find file");
		}
		DM_hist = h;
	}
}

void PrimaryGeneratorAction::root_get_para(){
	int iEvent = (int) fmod(root_index,root_num);
	m_TChain->GetEntry(iEvent);
	root_index++;
}

void PrimaryGeneratorAction::root_build(){
	G4String dir_name = getenv("CONFIGUREDATAROOT");
	if (dir_name[dir_name.size()-1] != '/') dir_name.append("/");
	std::string m_TFile_name = dir_name + root_filename;
	if (m_TChain) delete m_TChain;
	m_TChain = new TChain("t");
	m_TChain->Add(m_TFile_name.c_str());
	root_num = m_TChain->GetEntries();
}

void PrimaryGeneratorAction::root_set_Position(){
	m_TChain->SetBranchAddress("x", &root_para[0]);
	m_TChain->SetBranchAddress("y", &root_para[1]);
	m_TChain->SetBranchAddress("z", &root_para[2]);
}
void PrimaryGeneratorAction::root_set_Energy(){
	m_TChain->SetBranchAddress("px", &root_para[3]);
	m_TChain->SetBranchAddress("py", &root_para[4]);
	m_TChain->SetBranchAddress("pz", &root_para[5]);
}
void PrimaryGeneratorAction::root_set_Time(){
	m_TChain->SetBranchAddress("t", &root_para[6]);
}

void PrimaryGeneratorAction::ReadCard(G4String file_name){
	std::ifstream fin_card(file_name);
	if ( !fin_card ){
		std::cout<<"generator file \""<<file_name<<"\" is not available!!!"<<std::endl;
		G4Exception("PrimaryGeneratorAction::PrimaryGeneratorAction()","Run0031",
				FatalException, "generator file is not available.");
	}
	std::stringstream buf_card;
	std::string s_card;
	while(getline(fin_card,s_card)){
		buf_card.str("");
		buf_card.clear();
		buf_card<<s_card;
		const char* c = s_card.c_str();
		int length = strlen(c);
		int offset = 0;
		for ( ; offset < length; offset++ ){
			if ( c[offset] != ' ' ) break;
		}
		if ( c[offset] == '#' ) continue;
		else if ( c[offset] == '/' && c[offset+1] == '/' ) continue;
		else if ( length - offset == 0 ) continue;
		std::string keyword;
		buf_card>>keyword;
		if ( keyword == "Type:" ){
			buf_card>>fType;
			continue;
		}
		else if ( keyword == "Particle:" ){
			buf_card>>ParticleName;
		}
		else if ( keyword == "Direction:" ){
			buf_card>>Theta>>Phi;
			Theta *= deg;
			Phi *= deg;
		}
		else if ( keyword == "MomAmp:" ){
			buf_card>>Pa;
			Pa *= MeV;
		}
		else if ( keyword == "Position:" ){
			buf_card>>x>>y>>z;
			x *= mm;
			y *= mm;
			z *= mm;
		}
		else if ( keyword == "Time:" ){
			buf_card>>t;
			t *= ns;
		}
		else if ( keyword == "EnergyMode:" ){
			buf_card>>EnergyMode;
			continue;
		}
		else if ( keyword == "PositionMode:" ){
			buf_card>>PositionMode;
			continue;
		}
		else if ( keyword == "TimeMode:" ){
			buf_card>>TimeMode;
			continue;
		}
		else if ( keyword == "DirectionMode:" ){
			buf_card>>DirectionMode;
			continue;
		}
		else if ( keyword == "EMHFN:" ){
			buf_card>>EM_hist_filename;
			continue;
		}
		else if ( keyword == "EMHHN:" ){
			buf_card>>EM_hist_histname;
			continue;
		}
		else if ( keyword == "DMHFN:" ){
			buf_card>>DM_hist_filename;
			continue;
		}
		else if ( keyword == "DMHHN:" ){
			buf_card>>DM_hist_histname;
			continue;
		}
		else if ( keyword == "RFN:" ){
			buf_card>>root_filename;
			continue;
		}
		else if ( keyword == "RTN:" ){
			buf_card>>root_treename;
			continue;
		}
		else if ( keyword == "UIV:" ){
			buf_card>>UP_Type>>UP_SubDet>>UP_Volume;
			continue;
		}
		else{
			std::cout<<"In MyFieldSvc::SetField, unknown name: "<<keyword<<" in file "<<file_name<<std::endl;
			std::cout<<"Will ignore this line!"<<std::endl;
		}
	}
	fin_card.close();
	buf_card.str("");
	buf_card.clear();
	Dump();
}
void PrimaryGeneratorAction::Dump(){
	std::cout<<"---------------PrimaryGeneratorAction---------------------"<<std::endl;
	std::cout<<"Type:                                         "<<fType<<std::endl;
	std::cout<<"Particle:                                     "<<ParticleName<<std::endl;
	std::cout<<"Default Momentum Direction: theta =           "<<Theta/deg<<"deg, phi = "<<Phi/deg<<"deg"<<std::endl;
	std::cout<<"Default Momentum Amplitude:                   "<<Pa/MeV<<"MeV"<<std::endl;
	std::cout<<"Default Position(cm):                         ("<<x/cm<<", "<<y/cm<<", "<<z/cm<<")"<<std::endl;
	std::cout<<"Default Time(ns):                             ("<<t/ns<<std::endl;
	std::cout<<"EnergyMode:                                   "<<EnergyMode<<std::endl;
	std::cout<<"PositionMode:                                 "<<PositionMode<<std::endl;
	std::cout<<"TimeMode:                                     "<<TimeMode<<std::endl;
	std::cout<<"Uniform In sub-detector:                      "<<UP_SubDet<<std::endl;
	std::cout<<"  Volume:                                     "<<UP_Volume<<std::endl;
	std::cout<<"  Type:                                       "<<UP_Type<<std::endl;
	std::cout<<"DirectionMode:                                "<<DirectionMode<<std::endl;
	std::cout<<"File Name for EnergyMode = histo:             "<<EM_hist_filename<<std::endl;
	std::cout<<"Histogram Name for EnergyMode = histo:        "<<EM_hist_histname<<std::endl;
	std::cout<<"File Name for DirectionMode = histo:          "<<DM_hist_filename<<std::endl;
	std::cout<<"Histogram Name for DirectionMode = histo:     "<<DM_hist_histname<<std::endl;
	std::cout<<"File Name for Energy/Position/TimeMode = root: "<<root_filename<<std::endl;
	std::cout<<"Tree Name for Energy/Position/TimeMode = root: "<<root_treename<<std::endl;
	std::cout<<"---------------------------------------------------------"<<std::endl;
}
