#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH3.h"
#include "TH2.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TFitResult.h"

#include <iostream>
#include <vector>
#include <string>
#include <cmath>

using std::cout;
using std::endl;

static const std::string particle = "proton"; // Name of particle being checked 
static const double M = 938.27;               // Mass of particle being checked (MeV)
static const double M_sq = M*M; 
static const double degree = 45 / std::atan(1.);

void R15b_MC(std::string filename) {

  TChain* tree = new TChain("tree");
  tree->Add(filename.c_str());
  cout<<tree->GetNtrees()<<endl;
  if(tree->GetNtrees()<1) return;
  
  std::vector<std::string>* particleName = 0;
  std::vector<std::string>* volName = 0;
  std::vector<std::string>* ovolName = 0;
  std::vector<std::string>* oprocess = 0;
  std::vector<double>* edep = 0;
  std::vector<int>* stopped = 0;
  double px=0, py=0, pz=0;

  tree->SetBranchAddress("M_particleName", &particleName);
  tree->SetBranchAddress("M_volName"     , &volName);
  tree->SetBranchAddress("M_ovolName"    , &ovolName);
  tree->SetBranchAddress("M_oprocess"    , &oprocess);
  tree->SetBranchAddress("M_edep"        , &edep);
  tree->SetBranchAddress("M_stopped"     , &stopped);
  tree->SetBranchAddress("i_px"          , &px);
  tree->SetBranchAddress("i_py"          , &py);
  tree->SetBranchAddress("i_pz"          , &pz);

  TFile* outfile=TFile::Open("out_hists.root","recreate");

  const int n_bins = 400;
  const double lower_limit = 0;
  const double upper_limit_thick = 25;
  const double upper_limit_thin  = 10;
  const double upper_limit_input = 25;
  
  TNtuple *SiLHits = new TNtuple("SiLHits", "", "SiL1Energy:SiL2Energy:SiL1Time:SiL2Time");
  TNtuple *SiRHits = new TNtuple("SiRHits", "", "SiR1Energy:SiR2Energy:SiR3Energy:SiR1Time:SiR2Time:SiR3Time");

  for (int i_entry = 0; i_entry < tree->GetEntries(); ++i_entry) {
    tree->GetEvent(i_entry);
     
    if (i_entry % 10000 == 0) {
      std::cout << i_entry << " / " << tree->GetEntries() << std::endl;
    }
    Double_t SiL1Energy=0, SiL2Energy=0, SiL3Energy=0;
    Double_t SiR1Energy=0, SiR2Energy=0, SiR3Energy=0;
    int left_thin=0, left_thick=0;
    int right_thin=0, right_thick=0, right_thick2=0;
    
    for (unsigned iElement = 0; iElement < particleName->size();  ++iElement) {
      // Check we have a proton
      if(particleName->at(iElement)!=particle) continue;

      // Check it came from the target or as a primary particle
      if(oprocess->at(iElement)!="NULL" && ovolName->at(iElement)!="SiL1") continue;

      double i_edep = edep->at(iElement)*1e6; //energyin keV to match the unit used in the experiment

      //Left detector
      if(volName->at(iElement) == "SiL1") {
        ++left_thin;
        SiL1Energy+=i_edep;
      } else if(volName->at(iElement) == "SiL2") { //not turned on
//        SiL2Energy+=i_edep;
      } else if(volName->at(iElement) == "SiL3") { //is the middle detector
        ++left_thick;
        SiL3Energy+=i_edep;
      }

      //Right detector
      else if(volName->at(iElement) == "SiR1") {
        ++right_thin;
        SiR1Energy+=i_edep;
      } else if(volName->at(iElement) == "SiR2") {
        ++right_thick;
        SiR2Energy+=i_edep;
      } else if(volName->at(iElement) == "SiR3") {
        SiR3Energy+=i_edep;
      }
    }
    if(left_thin>0 && left_thick>0) {
      SiLHits->Fill(SiL1Energy, SiL3Energy, 0.0f, -99999.0f, -99999.0f, -99999.0f);
    }
    if(right_thin>0 && right_thick>0) {
      SiRHits->Fill(SiR1Energy, SiR2Energy, SiR3Energy, -99999.0f, -99999.0f, -99999.0f);
    }
  }
  outfile->Write();
}
