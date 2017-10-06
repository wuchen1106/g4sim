//---------------------------------------------------------------------------//
//Description: agent of ROOT
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#ifndef MyRoot_h
#define MyRoot_h 1

#include <string>

#include "G4UnitsTable.hh"
#include "myglobals.hh"
#include "TFile.h"
#include "TTree.h"

class MyRoot
{
public:
  MyRoot();
   ~MyRoot();

  static MyRoot* GetMyRoot();

  void OpenFile(std::string filename);
  void CreateTree(std::string branchname, int fCircular = 0);

  void Fill();

  template <typename Type>
  void SetBranch(const std::string& name, Type* var){
  m_tree->Branch(name.c_str(), var);
	if ( fVerbose >= 5 ){
		std::cout<<"In MyRoot::SetBranch"<<std::endl;
		std::cout<<"  New branch created!!"<<std::endl;
		std::cout<<"  Name: "<<name<<std::endl;
}
}

  void SetBranch(const std::string& name, std::vector<double>* pVecD);
  void SetBranch(const std::string& name, std::vector<int>* pVecI);
  void SetBranch(const std::string& name, int* pI);
  void SetBranch(const std::string& name, double* pD);
  void SetBranch(const std::string& name, std::vector<std::string>* pVecCa);

	void Save();

	int FlushBaskets();

  void Write();

  void Close();

	void SetVerbose( int val ){fVerbose = val;};
	void SetPrintModulo( int val ){fPrintModulo = val;};

private:

  static MyRoot* fMyRoot;

	int fVerbose;
	int fPrintModulo;
	int nbFilled;

  //about root file
  TFile* m_file;
  TTree* m_tree;
};

#endif

