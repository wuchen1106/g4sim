#include <iostream>
#include <fstream>
#include <string>
#include <vector>


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
    std::cout << input_filename << std::endl;
    //    std::ifsteam input;
  }

}
