#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH2.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TFitResult.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using std::cout;
using std::endl;

void EvdE_protonsAsPrimary_ben(std::string filename) {

  //TFile* file = new TFile(filename.c_str(), "READ");
  //TTree* tree = (TTree*) file->Get("tree");
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
  //std::vector<double>* Ot = 0;
  std::vector<int>* charge = 0;

  tree->SetBranchAddress("M_particleName", &particleName);
  tree->SetBranchAddress("M_volName"     , &volName);
  tree->SetBranchAddress("M_ovolName"    , &ovolName);
  tree->SetBranchAddress("M_oprocess"    , &oprocess);
  tree->SetBranchAddress("M_edep"        , &edep);
  tree->SetBranchAddress("M_stopped"     , &stopped);
  //tree->SetBranchAddress("M_Ot"          , &Ot);

  TFile* outfile=TFile::Open("out_hists.root","recreate");

  const int n_bins = 300;
  const double lower_limit = 0;
  const double upper_limit_thick = 2.5e4;
  const double upper_limit_thin  = 1e4;
  
  TH2* hEvdE=new TH2F("hEvdE","E vs dE plot",n_bins,lower_limit,upper_limit_thick,n_bins,lower_limit,upper_limit_thin);
  TH2* hEvdE_stopped=new TH2F("hEvdE_stopped","E vs dE plot for stopped protons",n_bins,lower_limit,upper_limit_thick,n_bins,lower_limit,upper_limit_thin);
  TH2* hEvdE_punch=new TH2F("hEvdE_punch","E vs dE plot for stopped protons",n_bins,lower_limit,upper_limit_thick,n_bins,lower_limit,upper_limit_thin);
  TH1* hN_thin=new TH1F("hN_thin","Number of hits contributing to dE",50,0,50);
  TH1* hN_thick=new TH1F("hN_thick","Number of hits contributing to E",50,0,50);

  for (int i_entry = 0; i_entry < tree->GetEntries(); ++i_entry) {
    tree->GetEvent(i_entry);
     
    if (i_entry % 10000 == 0) {
      std::cout << i_entry << " / " << tree->GetEntries() << std::endl;
    }
    double dE = 0;
    double E = 0;
    bool thick_stop = false;
    int N_thin=0, N_thick=0;

    for (unsigned iElement = 0; iElement < particleName->size();  ++iElement) {
       // Check we have a proton
       if(particleName->at(iElement)!="proton") continue;

       // Check it came from the target or as a primary particle
       if(oprocess->at(iElement)!="NULL" && ovolName->at(iElement)!="ESi1") continue;

       double i_edep = edep->at(iElement)*1e6; // convert to keV

       // If this is a hit in the SiL thick or thin:
       if(volName->at(iElement)=="ESi2"){
         ++N_thick; 
         E+=i_edep;
         // Since this is thick, check if the proton stopped
         if(stopped->at(iElement)) thick_stop=true;
       } else if(volName->at(iElement)=="dESi2"){
         ++N_thin; 
         dE+=i_edep;
       } else {
         continue;
       } 
    }
    // Since we have 1 proton in per event, when all elements are inspected in this event we can fill the EvdE plot
    if(N_thin >0 && N_thick>0){
      hEvdE->Fill(E+dE, dE);
      if(thick_stop) hEvdE_stopped->Fill(E+dE, dE);
      else hEvdE_punch->Fill(E+dE, dE);
    }
    hN_thin->Fill(N_thin);
    hN_thick->Fill(N_thick);
  }

  outfile->Write();
}
