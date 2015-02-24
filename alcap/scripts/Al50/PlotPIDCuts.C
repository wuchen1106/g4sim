#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "TTree.h"
#include "TH1.h"

struct Arm {
  std::string armname;
} LeftArm, RightArm;

struct ParticleType {
  std::string particle_type_name;

  double mean;
  TBranch* br_mean;

  double rms;
  TBranch* br_rms;
} p_stop, proton, deuteron, triton, alpha;

void PlotPIDCuts() {

  // Set up the arms
  LeftArm.armname = "left";
  RightArm.armname = "right";

  std::vector<Arm> arms;
  arms.push_back(LeftArm);
  arms.push_back(RightArm);

  // Set up the particle types
  p_stop.particle_type_name = "p_stop";
  p_stop.mean = 0; p_stop.rms = 0;
  proton.particle_type_name = "proton";
  proton.mean = 0; proton.rms = 0;
  deuteron.particle_type_name = "deuteron";
  deuteron.mean = 0; deuteron.rms = 0;
  triton.particle_type_name = "triton";
  triton.mean = 0; triton.rms = 0;
  alpha.particle_type_name = "alpha";
  alpha.mean = 0; alpha.rms = 0;

  std::vector<ParticleType> particle_types;
  particle_types.push_back(p_stop);
  particle_types.push_back(proton);
  particle_types.push_back(deuteron);
  particle_types.push_back(triton);
  particle_types.push_back(alpha);

  // Loop through the arms
  for (std::vector<Arm>::const_iterator i_arm = arms.begin(); i_arm != arms.end(); ++i_arm) {
    std::string input_filename = "pid-cuts-" + i_arm->armname + ".txt";
    TTree* tree = new TTree();
    tree->ReadFile(input_filename.c_str());

    double energy = 0;
    TBranch* br_energy = tree->GetBranch("energy");
    br_energy->SetAddress(&energy);

    for (std::vector<ParticleType>::iterator i_type = particle_types.begin(); i_type != particle_types.end(); ++i_type) {
      std::string branchname = i_type->particle_type_name + "_mean";
      i_type->br_mean = tree->GetBranch(branchname.c_str());
      i_type->br_mean->SetAddress(&i_type->mean);

      branchname = i_type->particle_type_name + "_rms";
      i_type->br_rms = tree->GetBranch(branchname.c_str());
      i_type->br_rms->SetAddress(&i_type->rms);
    }
    
    TH1D* profile = new TH1D("profile", "profile", tree->GetEntries(),0,25000);

    for (int i_entry = 0; i_entry < tree->GetEntries(); ++i_entry) {
      tree->GetEntry(i_entry);
      //      std::cout << energy << " "; 
      for (std::vector<ParticleType>::iterator i_type = particle_types.begin(); i_type != particle_types.begin()+1; ++i_type) {
	std::cout << i_type->mean << " " << i_type->rms << std::endl;
	profile->SetBinContent(profile->FindBin(energy), i_type->mean);
	profile->SetBinError(profile->FindBin(energy), i_type->rms);
      }
      //      std::cout << std::endl;
    }
    profile->Draw("E");
  }

}
