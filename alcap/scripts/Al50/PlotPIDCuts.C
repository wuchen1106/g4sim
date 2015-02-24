#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "TTree.h"

struct Arm {
  std::string armname;
} LeftArm, RightArm;

void PlotPIDCuts() {

  LeftArm.armname = "left";
  RightArm.armname = "right";

  std::vector<Arm> arms;
  arms.push_back(LeftArm);
  arms.push_back(RightArm);

  // Loop through the arms
  for (std::vector<Arm>::const_iterator i_arm = arms.begin(); i_arm != arms.end(); ++i_arm) {
    std::string input_filename = "pid-cuts-" + i_arm->armname + ".txt";
    TTree* tree = new TTree();
    tree->ReadFile(input_filename.c_str());

    double energy = 0;
    TBranch* br_energy = tree->GetBranch("energy");
    br_energy->SetAddress(&energy);

    double p_stop_mean = 0;
    TBranch* br_mean = tree->GetBranch("p_stop_mean");
    br_mean->SetAddress(&p_stop_mean);

    double p_stop_rms = 0;
    TBranch* br_rms = tree->GetBranch("p_stop_rms");
    br_rms->SetAddress(&p_stop_rms);

    for (int i_entry = 0; i_entry < tree->GetEntries(); ++i_entry) {
      tree->GetEntry(i_entry);
      std::cout << energy << " " << p_stop_mean << " " << p_stop_rms << std::endl;
    }
  }

}
