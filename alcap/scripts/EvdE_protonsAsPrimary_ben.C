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
  //std::vector<int>* charge = 0;
  //double *px=0, *py=0, *pz=0;
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
  //tree->SetBranchAddress("M_Ot"        , &Ot);

  TFile* outfile=TFile::Open("out_hists.root","recreate");

  const int n_bins = 400;
  const double lower_limit = 0;
  const double upper_limit_thick = 25;
  const double upper_limit_thin  = 10;
  const double upper_limit_input = 25;
  
  TH2* hEvdE=new TH2F("hEvdE","E vs dE; Total Energy (MeV); Energy depositied in Thin (MeV)",n_bins,lower_limit,upper_limit_thick,n_bins,lower_limit,upper_limit_thin);
  TH2* hEvdE_stopped=new TH2F("hEvdE_stopped","E vs dE plot for stopped protons; Total Energy (MeV); Energy depositied in Thin (MeV)",n_bins,lower_limit,upper_limit_thick,n_bins,lower_limit,upper_limit_thin);
  TH2* hEvdE_punch=new TH2F("hEvdE_punch","E vs dE plot for punch-through protons; Total Energy (MeV); Energy depositied in Thin (MeV)",n_bins,lower_limit,upper_limit_thick,n_bins,lower_limit,upper_limit_thin);
  TH1* hN_thin=new TH1F("hN_thin","Number of hits contributing to dE; Number of hits",50,0,50);
  TH1* hN_thick=new TH1F("hN_thick","Number of hits contributing to E; Number of hits",50,0,50);
  TH2* hTransfer=new TH2F("hTransfer","Transfer function; Energy at Target (MeV); Energy at SiL (MeV)",n_bins,lower_limit,upper_limit_input,n_bins,lower_limit,upper_limit_thick);
  TH1* hInput=new TH1F("hInput","Input energy; Energy (MeV)",n_bins,0,upper_limit_input);
  TH2* hInputDirection=new TH2F("hInputDirection","Input Direction; Phi (degree); Theta (degree)",360,-180,180,180,-1,1);
  TH2* hInputDirection_stopped=new TH2F("hInputDirection_stopped","Input Direction of Protons stopping in SiL; Phi (degree); Theta (degree)",360,-180,180,180,-1,1);
  TH3* hInputDirection_3D=new TH3F("hInputDirection_3D","Input Direction of Protons stopping in SiL",60,-1,1,60,-1,1,60,-1,1);
  TH1* hAcceptence=new TH1F("hAcceptence","Acceptance as a function of energy; Energy (MeV); Acceptance (%)",n_bins,0,upper_limit_input);

  for (int i_entry = 0; i_entry < tree->GetEntries(); ++i_entry) {
    tree->GetEvent(i_entry);
     
    if (i_entry % 10000 == 0) {
      std::cout << i_entry << " / " << tree->GetEntries() << std::endl;
    }
    double dE = 0;
    double E = 0;
    bool thick_stop = false;
    int N_thin=0, N_thick=0;
    
    // Calculate the energy at the target
    const double rho_sq = py*py + pz*pz;
    const double rho = sqrt(rho_sq);
    const double P_sq = rho_sq + px*px;
    const double P = sqrt(P_sq);
    const double E_k_orig=sqrt(P_sq + M_sq) - M;
    const double phi=std::atan2(pz,py)*degree;
    const double theta=px/P;
    //const double theta=std::atan2(rho,px)*degree;

    for (unsigned iElement = 0; iElement < particleName->size();  ++iElement) {
       // Check we have a proton
       if(particleName->at(iElement)!=particle) continue;

       // Check it came from the target or as a primary particle
       if(oprocess->at(iElement)!="NULL" && ovolName->at(iElement)!="ESi1") continue;

       double i_edep = edep->at(iElement)*1e3; // convert to MeV

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
      if(thick_stop) {
        hEvdE_stopped->Fill(E+dE, dE);
        hTransfer->Fill(E_k_orig,E+dE);
	hInputDirection_stopped->Fill(phi,theta);
        hAcceptence->Fill(E_k_orig);
      } else {
        hEvdE_punch->Fill(E+dE, dE);    
      }
    }
    hInput->Fill(E_k_orig);
    hInputDirection->Fill(phi,theta);
    hInputDirection_3D->Fill(px/P,py/P,pz/P);
    hN_thin->Fill(N_thin);
    hN_thick->Fill(N_thick);
  }
  hAcceptence->Divide(hInput);
  hAcceptence->Scale(100);

  outfile->Write();
}
