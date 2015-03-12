#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH2.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TLatex.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

struct ParticleType {
  std::string type;
  std::string particle_name;
  int stopped;

  TH2F* hEvdE;
  TH1D* hProjection;
  TF1* fit;
  int colour;

  int n_entries_covered;
} stopped_protons, not_stop_protons, deuterons, tritons, alphas;

struct Arm {
  std::string outfilename;
  std::string detname;
  std::string monname;
  std::vector<ParticleType> hists;
  TH2F* hAllEvdE;

  double lower_time_cut;
  double upper_time_cut;
} LeftArm, RightArm;

void AllParticle_EvdE(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tree = (TTree*) file->Get("tree");
  TFile* out_file = new TFile("plots.root", "RECREATE");

  std::vector<std::string>* particleName = 0;
  std::vector<std::string>* volName = 0;
  std::vector<std::string>* ovolName = 0;
  std::vector<double>* edep = 0;
  std::vector<int>* stopped = 0;
  std::vector<double>* Ot = 0;
  std::vector<int>* charge = 0;
  
  TBranch* br_particleName = tree->GetBranch("M_particleName");
  TBranch* br_volName = tree->GetBranch("M_volName");
  TBranch* br_ovolName = tree->GetBranch("M_ovolName");
  TBranch* br_edep = tree->GetBranch("M_edep");
  TBranch* br_stopped = tree->GetBranch("M_stopped");
  TBranch* br_Ot = tree->GetBranch("M_Ot");
  TBranch* br_charge = tree->GetBranch("M_charge");
  
  br_particleName->SetAddress(&particleName);
  br_volName->SetAddress(&volName);
  br_ovolName->SetAddress(&ovolName);
  br_edep->SetAddress(&edep);
  br_stopped->SetAddress(&stopped);
  br_Ot->SetAddress(&Ot);
  br_charge->SetAddress(&charge);

  const double bin_width = 100;
  const double x_lower_limit = 0;
  const double x_upper_limit = 25000;
  int n_bins_x = (x_upper_limit - x_lower_limit) / bin_width;
  const double y_lower_limit = 0;
  const double y_upper_limit = 10000;
  int n_bins_y = (y_upper_limit - y_lower_limit) / bin_width;

  const int n_arms = 2;
  LeftArm.outfilename = "pid-cuts-SiL.txt";
  LeftArm.detname = "SiL";
  LeftArm.monname = "ESi2";
  LeftArm.lower_time_cut = 0;
  LeftArm.upper_time_cut = 9999999;
  RightArm.outfilename = "pid-cuts-SiR.txt";
  RightArm.detname = "SiR";
  RightArm.monname = "ESi1";
  RightArm.lower_time_cut = 0;
  RightArm.upper_time_cut = 9999999;
  Arm arms[n_arms] = {LeftArm, RightArm};

  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
    stopped_protons.type = "proton_stopped"; stopped_protons.particle_name = "proton"; stopped_protons.stopped = 1; stopped_protons.colour = kRed;
    not_stop_protons.type = "proton_not_stopped"; not_stop_protons.particle_name = "proton"; not_stop_protons.stopped = 0; not_stop_protons.colour = kBlue;
    deuterons.type = "deuteron"; deuterons.particle_name = "deuteron"; deuterons.stopped = 1; deuterons.colour = kCyan;
    tritons.type = "triton"; tritons.particle_name = "triton"; tritons.stopped = 1; tritons.colour = kMagenta;
    alphas.type = "alpha"; alphas.particle_name = "alpha"; alphas.stopped = 1; alphas.colour = kSpring;

    arms[i_arm].hists.push_back(stopped_protons);
    arms[i_arm].hists.push_back(not_stop_protons);
    arms[i_arm].hists.push_back(deuterons);
    arms[i_arm].hists.push_back(tritons);
    arms[i_arm].hists.push_back(alphas);

    std::string histname = "hAll_EvdE_" + arms[i_arm].detname;
    std::string histtitle = "EvdE plot for the " + arms[i_arm].detname + " detector";
    arms[i_arm].hAllEvdE = new TH2F(histname.c_str(), histtitle.c_str(), n_bins_x,x_lower_limit,x_upper_limit, n_bins_y,y_lower_limit,y_upper_limit);
    arms[i_arm].hAllEvdE->SetXTitle("E_{1} + E_{2} [keV]");
    arms[i_arm].hAllEvdE->GetYaxis()->SetTitleOffset(1.4);
    arms[i_arm].hAllEvdE->SetYTitle("E_{1} [keV]");

    for (std::vector<ParticleType>::iterator i_type = arms[i_arm].hists.begin(); i_type != arms[i_arm].hists.end(); ++i_type) {
      std::string histname = "hEvdE_"+ arms[i_arm].detname + "_" + i_type->type;
      TH2F* temp = new TH2F(histname.c_str(), histname.c_str(), n_bins_x,x_lower_limit,x_upper_limit, n_bins_y,y_lower_limit,y_upper_limit);
      temp->SetLineColor(i_type->colour);
      temp->SetLineWidth(2);
      temp->SetMarkerColor(i_type->colour);
      temp->SetMarkerStyle(7);
      temp->SetXTitle("E_{1} + E_{2} [keV]");
      temp->GetYaxis()->SetTitleOffset(1.4);
      temp->SetYTitle("E_{1} [keV]");
      i_type->hEvdE = temp;

      i_type->n_entries_covered = 0;
    }
  }

  for (int i_entry = 0; i_entry < tree->GetEntries(); ++i_entry) {
    tree->GetEvent(i_entry);
     
    if (i_entry % 10000 == 0) {
      std::cout << i_entry << " / " << tree->GetEntries() << std::endl;
    }
    double dE = 0;
    double E = 0;
    bool thick_hit = false;
    bool thin_hit = false;

    for (int iElement = 0; iElement < particleName->size();  ++iElement) {
      int i_charge = charge->at(iElement);
      std::string i_volName = volName->at(iElement);
      std::string i_ovolName = ovolName->at(iElement);
      double i_Ot = Ot->at(iElement);
      double i_edep = edep->at(iElement)*1e6; // convert to keV
      int i_stopped = stopped->at(iElement);

      // Loop through the arms
      for (int i_arm = 0; i_arm < n_arms; ++i_arm) {

	if (i_charge != 0) {
	  if (i_volName == arms[i_arm].monname && i_ovolName == "Target") {
	  //	  std::cout << "Ot = " << Ot->at(iElement) << std::endl;
	    double thick_time = i_Ot;
	    if (thick_time > arms[i_arm].lower_time_cut && thick_time < arms[i_arm].upper_time_cut) {
	      E = i_edep;
	      thick_hit = true;
	    }	  
	  }
	  else if (i_volName == ("d"+arms[i_arm].monname) && i_ovolName == "Target") {
	    double thin_time = i_Ot;
	    if (thin_time > arms[i_arm].lower_time_cut && thin_time < arms[i_arm].upper_time_cut) {
	      dE = i_edep;
	      thin_hit = true;
	    }
	  }
	
	  if (thick_hit && thin_hit) { // assuming that the two hits will be next to each other in the tree

	    // Loop through the names
	    std::string particle_name = particleName->at(iElement);
	    for (std::vector<ParticleType>::iterator i_type = arms[i_arm].hists.begin(); i_type != arms[i_arm].hists.end(); ++i_type) {
	      bool plot = false;
		    
	      if (particle_name == i_type->particle_name && i_stopped == i_type->stopped) {
		(i_type->hEvdE)->Fill(E+dE, dE);
	      }
	    }
	    arms[i_arm].hAllEvdE->Fill(E+dE, dE); // fill in the "all" plot
	    thick_hit = false;
	    thin_hit = false; // reset to false in case there is another proton in this event
	  }
	}
      } // end loop through arms
    }
  }

  TLegend* legend = new TLegend(0.45, 0.8, 0.7, 0.5, "Legend");
  legend->SetBorderSize(0);
  legend->SetTextSize(0.04);
  legend->SetFillColor(kWhite);
  for (std::vector<ParticleType>::iterator i_type = arms[0].hists.begin(); i_type != arms[0].hists.end(); ++i_type) {
    std::string label = i_type->type;
    std::replace(label.begin(), label.end(), '_', ' ');
    legend->AddEntry(i_type->hEvdE, label.c_str(), "p");
  }


  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {

    // For each E bin plot the range of dEs for each particle
    std::ofstream output(arms[i_arm].outfilename.c_str(), std::ofstream::out);
    output << "energy/D:p_stop_mean/D:p_stop_rms/D:proton_mean/D:proton_rms/D:deuteron_mean/D:deuteron_rms/D:triton_mean/D:triton_rms/D:alpha_mean/D:alpha_rms/D";
    
    for (int i_energy = x_lower_limit; i_energy <= x_upper_limit; i_energy += bin_width) {
      output << std::endl << i_energy << " ";
      TCanvas* c1 = new TCanvas("c1", "c1");
      c1->SetLeftMargin(0.11);
  
      // Loop through the histograms
      double maximum = 0;
      for (std::vector<ParticleType>::iterator i_type = arms[i_arm].hists.begin(); i_type != arms[i_arm].hists.end(); ++i_type) {
	int bin = (i_type->hEvdE)->GetXaxis()->FindBin(i_energy);

	std::stringstream append;
	append << "_py_" << i_energy;

	std::string projection_name = i_type->type + append.str();
	std::string projection_title = "hProjection_" + arms[i_arm].detname + append.str();
	i_type->hProjection = i_type->hEvdE->ProjectionY(projection_name.c_str(), bin, bin);
	i_type->hProjection->SetLineColor(i_type->colour);
	i_type->hProjection->SetLineWidth(2);
	i_type->hProjection->SetXTitle("E_{1} [keV]");
	i_type->hProjection->SetTitle(projection_name.c_str());
	
	double mean = i_type->hProjection->GetMean();
	double rms = i_type->hProjection->GetRMS();
	output << mean << " " << rms << " ";

	int integral_low = i_type->hProjection->FindBin(mean - 2*rms);
	int integral_high = i_type->hProjection->FindBin(mean + 2*rms);
	i_type->n_entries_covered += i_type->hProjection->Integral(integral_low, integral_high);
	/*	
	TFitResultPtr fit;
	if (i_type->hProjection->GetEntries() > 0) {
	  i_type->fit = new TF1("gaussian", "[0]*TMath::Gaus(x, [1], [2])", lower_limit, upper_limit);
	  i_type->fit->SetParameter(1, i_type->hProjection->GetMean());
	  i_type->fit->SetParameter(2, i_type->hProjection->GetRMS()+0.1);
	  i_type->fit->SetLineColor(i_type->colour);
	  fit = i_type->hProjection->Fit("gaussian", "QNS");
	  if ((Int_t) fit == 0) { // success  
	    i_type->fit->SetParameter(0, fit->Parameter(0));
	    i_type->fit->SetParameter(1, fit->Parameter(1));
	    i_type->fit->SetParameter(2, fit->Parameter(2));
	  }
	}
	else {
	  i_type->fit = NULL;
	}
	*/
	
	if (i_type->hProjection->GetMaximum() > maximum) {
	  maximum = i_type->hProjection->GetMaximum();
	}
	append.str("");
      }
      
      for (std::vector<ParticleType>::iterator i_type = arms[i_arm].hists.begin(); i_type != arms[i_arm].hists.end(); ++i_type) {
	i_type->hProjection->SetMaximum(maximum+0.1*maximum);
	i_type->hProjection->Draw("SAME");
	if (i_type->fit) {
	  i_type->fit->Draw("SAME");
	}
	i_type->hProjection->Write();
      }
      
      // Now save all the plots
      legend->Draw();
      std::stringstream plotname;
      plotname << "plot_" << arms[i_arm].detname << "_" << i_energy << "keV";
      std::string pdfname = plotname.str() + ".pdf";
      std::string pngname = plotname.str() + ".png";
      //      c1->SaveAs(pdfname.c_str());
      //      c1->SaveAs(pngname.c_str());

      delete c1;
    }

    for (std::vector<ParticleType>::iterator i_type = arms[i_arm].hists.begin(); i_type != arms[i_arm].hists.end(); ++i_type) {
      std::cout << "AE: " << arms[i_arm].detname << ": " << i_type->type << ": Fraction covered by profile = " << i_type->n_entries_covered << " / " << i_type->hEvdE->GetEntries() << " = " << (double) i_type->n_entries_covered / i_type->hEvdE->GetEntries() << std::endl;
      i_type->n_entries_covered = 0;
    }
  }

  // Now print the PID plots
  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
    TCanvas* c1 = new TCanvas("c1", "c1");
    for (std::vector<ParticleType>::iterator i_type = arms[i_arm].hists.begin(); i_type != arms[i_arm].hists.end(); ++i_type) {
      i_type->hEvdE->SetStats(false);
      i_type->hEvdE->Draw("SAME");
      i_type->hEvdE->Write();
      std::cout << "AE: " << i_type->type << " mean x = " << i_type->hEvdE->GetMean(1) << ", mean y = " << i_type->hEvdE->GetMean(2) << std::endl;
      std::string histtitle = "dE/dx Plot for the "+ arms[i_arm].detname+ " Detector";
      i_type->hEvdE->SetTitle("");
    }
    legend->Draw();

    TLatex text;
    text.SetTextAlign(12);
    text.DrawLatex(3000, 18000, "#bf{#it{Monte Carlo}}");

    std::string plotname = "EvdE_PID_" + arms[i_arm].detname;
    std::string pngname = plotname + ".png";
    std::string pdfname = plotname + ".pdf";
    //c1->SaveAs(pngname.c_str());
    c1->SaveAs(pdfname.c_str());

    for (std::vector<ParticleType>::iterator i_type = arms[i_arm].hists.begin(); i_type != arms[i_arm].hists.end(); ++i_type) {
      i_type->hEvdE->ProjectionX()->Draw();
      i_type->hEvdE->ProjectionX()->Write();

      // Save the full spectrum
      std::string plotname = "ESpectrum_" + i_type->type + "_"+ arms[i_arm].detname;
      std::string pngname = plotname + ".png";
      std::string pdfname = plotname + ".pdf";
      //      c1->SaveAs(pngname.c_str());
      //      c1->SaveAs(pdfname.c_str());      
    }

    arms[i_arm].hAllEvdE->SetStats(false);
    arms[i_arm].hAllEvdE->Draw("COLZ");
    arms[i_arm].hAllEvdE->Write();
    plotname = "EvdE_All_" + arms[i_arm].detname;
    pngname = plotname + ".png";
    pdfname = plotname + ".pdf";
    c1->SetLogz(1);
    //    c1->SaveAs(pngname.c_str());
    //    c1->SaveAs(pdfname.c_str());
    c1->SetLogz(0);

    delete c1;
  }
  out_file->Close();
}
