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

static const double degree = 45 / std::atan(1.);

template <typename T>
void SetBranchAddress(TTree* tree, const char* name, T*& addr){
tree->SetBranchAddress(name,&addr);
tree->SetBranchStatus(name,true);
}

struct HistSet{
  TH1 *hEnergy, *hEvZ, *hZ, *hVolName;
  std::string cut_name, cut_title;
  void Fill(double energy, double z, const std::string& volName){
     hEnergy->Fill(energy);
     hEvZ->Fill(z,energy);
     hZ->Fill(z);
     hVolName->Fill(volName.c_str(),1);
  }
  TH1* Make1D(std::string name, std::string title, int n_bins, int x_min, int x_max){
    TH1* hist=new TH1F(name.c_str(),title.c_str(),n_bins,x_min,x_max);
    cout<<"name '"<<name<<"'\n title: '"<<title<<"'"<<endl;
    return hist;
  }
  TH1* Make2D(std::string name, std::string title, int x_bins, int x_min, int x_max, int y_bins, int y_min, int y_max){
    TH1* hist=new TH2F(name.c_str(),title.c_str(),x_bins,x_min,x_max,y_bins,y_min,y_max);
    cout<<"name '"<<name<<"'\n title: '"<<title<<"'"<<endl;
    return hist;
  }
  HistSet():hEnergy(0),hEvZ(0){};
  void Init(const char* name, const char* title){
    cut_name=name;
    cut_title=title;
    const double n_bins=400;
    const double e_min=0;
    const double e_max=30; 
    const double z_min=-0.1;
    const double z_max=0.1;

    hEnergy=Make1D( Form("hEdep_%s",cut_name.c_str()),
                    Form("Energy deposited in the target {%s}; Energy (MeV)",cut_title.c_str()),
                     n_bins,e_min, e_max);

    hEvZ=Make2D( Form("hEvZ_%s",cut_name.c_str()),
                    Form("E vs dE { %s }; Stopping Depth (cm); Total Energy (MeV)",cut_title.c_str()),
                     n_bins,z_min, z_max,
                     n_bins,e_min, e_max);

    hZ=Make1D( Form("hZ_%s",cut_name.c_str()),
               Form("Depth in target {%s}; Depth (cm)",cut_title.c_str()),
               n_bins,z_min, z_max);

    hVolName=Make1D( Form("hVolName_%s",cut_name.c_str()),
               Form("Volume name {%s}; Depth (cm)",cut_title.c_str()),
               n_bins,0,-1);

  }
};

namespace Stop{
  enum Type { kAll=0, kStop, kNotStop, kNTypes};
  const char* Titles[kNTypes]= { "", "Stopped", "Not Stopped"};
  const char* Names[kNTypes] = { "", "Stopped", "NotStopped" };
}
namespace Cut{
  enum Type { kAll=0, kMuon,kCharged, kNTypes};
  const char* Titles[kNTypes]= { "All", "Muons", "Charged"};
  const char* Names[kNTypes] = { "", "Muons", "Charged"};
}

void MakeMuonInputRoot2(std::string filename, std::string VolCutName="ESi2") {

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
  std::vector<double>* local_x = 0;
  std::vector<double>* local_y = 0;
  std::vector<double>* local_z = 0;
  std::vector<int>* stopped = 0;
  //std::vector<double>* Ot = 0;
  std::vector<int>* charge = 0;
  //double *px=0, *py=0, *pz=0;
  double px=0, py=0, pz=0;

  tree->SetBranchStatus("*",false);
  SetBranchAddress(tree, "M_edep"        , edep);
  SetBranchAddress(tree, "M_local_Oz"    , local_z);
  SetBranchAddress(tree, "M_stopped"     , stopped);
  SetBranchAddress(tree, "M_volName"     , volName);
  SetBranchAddress(tree, "M_charge"      , charge);
  SetBranchAddress(tree, "M_particleName", particleName);
  //SetBranchAddress(tree, "M_ovolName"    , ovolName);
  //SetBranchAddress(tree, "M_oprocess"    , oprocess);
  //SetBranchAddress(tree, "i_px"          , px);
  //SetBranchAddress(tree, "i_py"          , py);
  //SetBranchAddress(tree, "i_pz"          , pz);
  //SetBranchAddress(tree, "M_local_Ox"    , local_x);
  //SetBranchAddress(tree, "M_local_Oy"    , local_y);

  TFile* outfile=TFile::Open("out_hists.root","recreate");

  const int n_bins = 400;
  const double lower_limit = 0;
  const double upper_limit_thick = 25;
  const double upper_limit_thin  = 10;
  const double upper_limit_input = 25;
  
  HistSet hists[Stop::kNTypes][Cut::kNTypes];
  for(unsigned s=0; s<Stop::kNTypes; ++s){
    for(unsigned c=0; c<Cut::kNTypes; ++c){
       hists[s][c].Init(Form("%s_%s",Stop::Names[s],Cut::Names[c]), Form("%s %s",Stop::Titles[s],Cut::Titles[c]));
    }
  }

  for (int i_entry = 0; i_entry < tree->GetEntries(); ++i_entry) {
    tree->GetEvent(i_entry);
     
    if (i_entry % 10000 == 0) {
      std::cout << i_entry << " / " << tree->GetEntries() << std::endl;
    }
    
    // Calculate the energy at the target
    //const double rho_sq = py*py + pz*pz;
    //const double rho = sqrt(rho_sq);
    //const double P_sq = rho_sq + px*px;
    //const double P = sqrt(P_sq);
    //const double E_k_orig=sqrt(P_sq + M_sq) - M;
    //const double phi=std::atan2(pz,py)*degree;
    //const double theta=px/P;
    //const double theta=std::atan2(rho,px)*degree;

    for (unsigned iElement = 0; iElement < particleName->size();  ++iElement) {
       // Is this a hit in the target?
       if(volName->at(iElement)!=VolCutName) continue;

       const double i_edep = edep->at(iElement)*1e3; // convert to MeV
       const double i_local_z = local_z->at(iElement)*1e3; // convert to MeV
       const std::string& i_volName = volName->at(iElement); // convert to MeV

       // Did it stop?
       const Stop::Type i_stopped=stopped->at(iElement)?Stop::kStop:Stop::kNotStop;
       hists[Stop::kAll][Cut::kAll].Fill(i_edep,i_local_z,i_volName);
       hists[i_stopped][Cut::kAll].Fill(i_edep,i_local_z,i_volName);
       if(particleName->at(iElement)!="mu-"){
         hists[Stop::kAll][Cut::kMuon].Fill(i_edep,i_local_z,i_volName);
         hists[i_stopped][Cut::kMuon].Fill(i_edep,i_local_z,i_volName);
       } if(charge->at(iElement) != 0) {
         hists[Stop::kAll][Cut::kCharged].Fill(i_edep,i_local_z,i_volName);
         hists[i_stopped][Cut::kCharged].Fill(i_edep,i_local_z,i_volName);
       }
    }
  }

  outfile->Write();
}
