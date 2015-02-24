#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "TTree.h"
#include "TH1.h"
#include "TCanvas.h"

struct Arm {
  std::string armname;
  TCanvas* canvas;
};

struct ParticleType {
  std::string particle_type_name;
  int colour;

  double mean;
  TBranch* br_mean;

  double rms;
  TBranch* br_rms;

  TH1D* profile;
};

void PlotPIDCuts(std::string identifier, std::string location, std::string left_name, std::string right_name);

void AllPIDCuts() {

  // MC
  PlotPIDCuts("MC");
  // Data
  PlotPIDCuts("data", "/gpfs/home/edmonds_a/AlcapDAQ/analyzer/rootana/scripts/Al50/");
}

void PlotPIDCuts(std::string identifier, std::string location = "") {

  // Set up the arms
  Arm LeftArm, RightArm;
  LeftArm.armname = "SiL";
  RightArm.armname = "SiR";

  std::vector<Arm> arms;
  arms.push_back(LeftArm);
  arms.push_back(RightArm);

  // Set up the particle types
  ParticleType  p_stop, proton, deuteron, triton, alpha;
  p_stop.particle_type_name = "p_stop";
  p_stop.mean = 0; p_stop.rms = 0; p_stop.colour = kRed;
  proton.particle_type_name = "proton";
  proton.mean = 0; proton.rms = 0; proton.colour = kBlue;
  deuteron.particle_type_name = "deuteron";
  deuteron.mean = 0; deuteron.rms = 0; deuteron.colour = kCyan;
  triton.particle_type_name = "triton";
  triton.mean = 0; triton.rms = 0; triton.colour = kMagenta;
  alpha.particle_type_name = "alpha";
  alpha.mean = 0; alpha.rms = 0; alpha.colour = kSpring;

  std::vector<ParticleType> particle_types;
  particle_types.push_back(p_stop);
  particle_types.push_back(proton);
  particle_types.push_back(deuteron);
  particle_types.push_back(triton);
  particle_types.push_back(alpha);

  // Loop through the arms
  for (std::vector<Arm>::iterator i_arm = arms.begin(); i_arm != arms.end(); ++i_arm) {
    // Create the canvas for this arm
    std::string canvasname = "c_" + i_arm->armname;
    i_arm->canvas = new TCanvas(canvasname.c_str(), canvasname.c_str());

    std::string input_filename = location + "pid-cuts-" + i_arm->armname + ".txt";
    TTree* tree = new TTree();
    tree->ReadFile(input_filename.c_str());

    // Another branch that will be used for all types
    double energy = 0;
    TBranch* br_energy = tree->GetBranch("energy");
    br_energy->SetAddress(&energy);

    // Loop through the types and set up the branches
    for (std::vector<ParticleType>::iterator i_type = particle_types.begin(); i_type != particle_types.end(); ++i_type) {
      std::string branchname = i_type->particle_type_name + "_mean";
      i_type->br_mean = tree->GetBranch(branchname.c_str());
      i_type->br_mean->SetAddress(&i_type->mean);

      branchname = i_type->particle_type_name + "_rms";
      i_type->br_rms = tree->GetBranch(branchname.c_str());
      i_type->br_rms->SetAddress(&i_type->rms);

      std::string profilename = "profile_" + identifier + "_" + i_arm->armname + "_" + i_type->particle_type_name;
      i_type->profile = new TH1D(profilename.c_str(), profilename.c_str(), tree->GetEntries(),0,25000);
      i_type->profile->SetLineColor(i_type->colour);
      i_type->profile->SetLineWidth(2);
    }

    // Loop through the entries in this arm's tree and create the profile plot
    for (int i_entry = 0; i_entry < tree->GetEntries(); ++i_entry) {
      tree->GetEntry(i_entry);
      //      std::cout << energy << " ";
      double stopped_proton_mean = -1; // a nonsensical number so that we only store the mean of the first particle type
      for (std::vector<ParticleType>::iterator i_type = particle_types.begin(); i_type != particle_types.end(); ++i_type) {
	//	std::cout << i_type->mean << " " << i_type->rms << std::endl;
	if (stopped_proton_mean == -1) {
	  stopped_proton_mean = i_type->mean;

	  if (stopped_proton_mean == 0) {
	    stopped_proton_mean = 1; // avoid dividing by 0 later
	  }
	}

	i_type->profile->SetBinContent(i_type->profile->FindBin(energy), i_type->mean / stopped_proton_mean);
	i_type->profile->SetBinError(i_type->profile->FindBin(energy), i_type->rms / stopped_proton_mean);
      }
      //      std::cout << std::endl;
    }

    // Now loop through and draw all the profile plots together
    for (std::vector<ParticleType>::iterator i_type = particle_types.begin(); i_type != particle_types.end(); ++i_type) {
      i_type->profile->Draw("SAME E");
    }
    std::string pdfname = "pid-profiles-" + identifier + "-" + i_arm->armname + ".pdf";
    i_arm->canvas->Print(pdfname.c_str());
  }

}
