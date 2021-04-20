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

  void SetBranch(std::string name, std::vector<double>* pVecD);
  void SetBranch(std::string name, std::vector<int>* pVecI);
  void SetBranch(std::string name, int* pI);
  void SetBranch(std::string name, double* pD);
  void SetBranch(std::string name, std::vector<std::string>* pVecCa);

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

