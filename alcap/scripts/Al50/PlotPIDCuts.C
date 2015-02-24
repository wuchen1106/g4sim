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
    tree->Draw("p_stop_mean:energy>>hist", "", "");
    tree->Draw("proton_mean:energy>>hist2", "", "SAME");
    tree->Print();
  }

}
