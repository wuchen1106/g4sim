//---------------------------------------------------------------------------//
//Description: agent of ROOT
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "MyRoot.hh"

#include <stdlib.h>

MyRoot* MyRoot::fMyRoot = 0;

MyRoot::MyRoot()
	:m_file(0)
{
  if (fMyRoot){
    G4Exception("MyRoot::MyRoot()","Run0031",
        FatalException, "MyRoot constructed twice.");
  }
  fVerbose = 0;
  fPrintModule = 0;
  fMyRoot = this;
	if ( fVerbose >= 5 ){
		printf("MyRoot\n");
	}
}

MyRoot::~MyRoot()
{
	m_tree->Delete();
	if (m_file) delete m_file;
	if ( fVerbose >= 5 ){
		printf("~MyRoot\n");
	}
}

void MyRoot::CreateTree(std::string tree_name, int fCircular){
  m_tree = new TTree(tree_name.c_str(),tree_name.c_str());
  nbFilled = 0;
//  m_tree->SetName(tree_name);
  //set circular
  if (fCircular){
		m_tree->SetCircular(fCircular);
	}
	if ( fVerbose >= 5 ){
		std::cout<<"In MyRoot::CreateTree"<<std::endl;
		std::cout<<"  New tree booked!!"<<std::endl;
		std::cout<<"  Name: "<<tree_name<<std::endl;
		std::cout<<"  Circular: "<<fCircular<<std::endl;
	}
}

void MyRoot::OpenFile(std::string file_name)
{
	if (m_file) delete m_file;
  m_file = new TFile (file_name.c_str(), "RECREATE");
	if ( fVerbose >= 5 ){
		std::cout<<"In MyRoot::OpenFile"<<std::endl;
		std::cout<<"  New file open!!"<<std::endl;
		std::cout<<"  Name: "<<file_name<<std::endl;
	}
}

MyRoot* MyRoot::GetMyRoot(){
  if ( !fMyRoot ){
    fMyRoot = new MyRoot;
  }
  return fMyRoot;
}

void MyRoot::Fill(){
  m_tree->Fill();
	nbFilled++;
	if ( fVerbose >= 5 ){
		if ( fPrintModule ){
			if ( nbFilled%fPrintModule == 0 ){
				std::cout<<"In MyRoot::Fill"<<std::endl;
				std::cout<<"  m_tree->Fill()"<<std::endl;
				std::cout<<"  "<<nbFilled<<" times for this tree"<<std::endl;
			}
		}
	}
}

void MyRoot::Save(){
	m_tree->AutoSave();
	if ( fVerbose >= 5 ){
		std::cout<<"In MyRoot::Save"<<std::endl;
		std::cout<<"  m_tree->AutoSave()"<<std::endl;
	}
}

int MyRoot::FlushBaskets(){
	if ( fVerbose >= 5 ){
		std::cout<<"In MyRoot::FlushBaskets"<<std::endl;
		std::cout<<"  m_tree->FlushBaskets()"<<std::endl;
	}
	return (m_tree->FlushBaskets());
}

void MyRoot::Write(){
  m_tree->Write();
	if ( fVerbose >= 5 ){
		std::cout<<"In MyRoot::Write"<<std::endl;
		std::cout<<"  m_tree->Write()"<<std::endl;
	}
}

void MyRoot::Close(){
  m_file->Close();
	if ( fVerbose >= 5 ){
		std::cout<<"In MyRoot::Close"<<std::endl;
		std::cout<<"  m_file->Close()"<<std::endl;
	}
}

void MyRoot::SetBranch(std::string name, std::vector<double>* pVecD){
  m_tree->Branch(name.c_str(), pVecD);
	if ( fVerbose >= 5 ){
		std::cout<<"In MyRoot::SetBranch"<<std::endl;
		std::cout<<"  New branch created!!"<<std::endl;
		std::cout<<"  Name: "<<name<<std::endl;
		std::cout<<"  Type: vector<double>"<<std::endl;
	}
}

void MyRoot::SetBranch(std::string name, std::vector<int>* pVecI){
  m_tree->Branch(name.c_str(), pVecI);
	if ( fVerbose >= 5 ){
		std::cout<<"In MyRoot::SetBranch"<<std::endl;
		std::cout<<"  New branch created!!"<<std::endl;
		std::cout<<"  Name: "<<name<<std::endl;
		std::cout<<"  Type: vector<int>"<<std::endl;
	}
}

void MyRoot::SetBranch(std::string name, int* pI ){
  m_tree->Branch(name.c_str(), pI);
	if ( fVerbose >= 5 ){
		std::cout<<"In MyRoot::SetBranch"<<std::endl;
		std::cout<<"  New branch created!!"<<std::endl;
		std::cout<<"  Name: "<<name<<std::endl;
		std::cout<<"  Type: int"<<std::endl;
	}
}

void MyRoot::SetBranch(std::string name, double* pD ){
  m_tree->Branch(name.c_str(), pD);
	if ( fVerbose >= 5 ){
		std::cout<<"In MyRoot::SetBranch"<<std::endl;
		std::cout<<"  New branch created!!"<<std::endl;
		std::cout<<"  Name: "<<name<<std::endl;
		std::cout<<"  Type: double"<<std::endl;
	}
}

void MyRoot::SetBranch(std::string name, std::vector<std::string>* pVecCa){
  m_tree->Branch(name.c_str(), pVecCa);
	if ( fVerbose >= 5 ){
		std::cout<<"In MyRoot::SetBranch"<<std::endl;
		std::cout<<"  New branch created!!"<<std::endl;
		std::cout<<"  Name: "<<name<<std::endl;
		std::cout<<"  Type: vector<string>"<<std::endl;
	}
}
