#include <iostream>
#include <fstream>
#include <cmath>

#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"

struct Arm{
  std::string armname;
  TH2F* hEvdEAll;
  TH2F* hEvdEBand;
  TH1F* hBandProfile;

  int n_entry_threshold;  // the number of entries that we determine as the start of a peak
  int first_dE_of_band; // which is the dE value at the start of the band
};

struct ParticleType {
  std::string type_name;
  int n_selected;
  int n_total;
};

struct Case {
  std::string identifier;
  std::vector<Arm> arms;
  std::vector<ParticleType> particle_types;

  std::string input_filename;
  std::string baseplotname;
  int n_entry_threshold;  // the number of entries that we determine as the start of a peak
  int first_dE_of_band; // which is the dE value at the start of the band

} data, MC;

void ExtractProtonBand(Case* this_case);
int GetNPeaks(TH1* hist, int n_entry_threshold);
int FindNextPeak(TH1* hist, int start_bin, int n_entry_threshold);
int CalculateEfficienciesAndPurities(Case* this_case);

void DataAndMC() {
  data.identifier = "data"; data.input_filename = "~/data/out/v92/total.root";
  data.baseplotname = "TME_Al50_EvdE/all_particles/ARM_EvdE";
  data.n_entry_threshold = 15;
  data.first_dE_of_band = 1900;


  MC.identifier = "MC"; MC.input_filename = "plots.root";
  MC.baseplotname = "hAll_EvdE_ARM";
  MC.n_entry_threshold = 2;
  MC.first_dE_of_band = 2000;

  std::vector<Case*> cases;
  cases.push_back(&data);
  cases.push_back(&MC);

  // Loop through and extract the proton bands
  for (std::vector<Case*>::iterator i_case = cases.begin(); i_case != cases.end(); ++i_case) {
    Arm left_arm, right_arm;
    left_arm.armname = "SiL"; right_arm.armname = "SiR";
    left_arm.n_entry_threshold = (*i_case)->n_entry_threshold; right_arm.n_entry_threshold = (*i_case)->n_entry_threshold;
    left_arm.first_dE_of_band = (*i_case)->first_dE_of_band; right_arm.first_dE_of_band = (*i_case)->first_dE_of_band;
    (*i_case)->arms.push_back(left_arm); (*i_case)->arms.push_back(right_arm);

    ParticleType proton_stopped, proton_not_stopped, deuteron, triton, alpha;
    proton_stopped.type_name = "proton_stopped";
    proton_not_stopped.type_name = "proton_not_stopped";
    deuteron.type_name = "deuteron";
    triton.type_name = "triton";
    alpha.type_name = "alpha";
    (*i_case)->particle_types.push_back(proton_stopped); (*i_case)->particle_types.push_back(proton_not_stopped); 
    (*i_case)->particle_types.push_back(deuteron); (*i_case)->particle_types.push_back(triton); (*i_case)->particle_types.push_back(alpha);

    ExtractProtonBand(*i_case);
  }

  // Calculate the detector smear for the left arm
  Arm* left_arm_data = &(data.arms.at(0));
  Arm* left_arm_MC = &(MC.arms.at(0));

  TH1F* hProfile_data = left_arm_data->hBandProfile;
  TH1F* hProfile_MC = left_arm_MC->hBandProfile;

  int n_bins = hProfile_data->GetNbinsX();
  double total_det_rms = 0;
  int n_good_det_rms = 0;
  for (int i_bin = 1; i_bin <= n_bins; ++i_bin) {
    double data_rms = hProfile_data->GetBinError(i_bin);
    double MC_rms = hProfile_MC->GetBinError(i_bin);

    if (data_rms > MC_rms) {
      ++n_good_det_rms;
      double det_rms = std::sqrt(data_rms*data_rms - MC_rms*MC_rms);
      total_det_rms += det_rms;
      std::cout << "Bin #" << i_bin << ": data_rms = " << data_rms << ", MC = " << MC_rms << ", det_rms = " << det_rms << std::endl;
    }
  }
  std::cout << "Average Det RMS = " << total_det_rms / n_good_det_rms << std::endl;
  

  CalculateEfficienciesAndPurities(&MC);
}

void ExtractProtonBand(Case* this_case) {

  double low_energy_cut = 1000; // keV
  double high_energy_cut = 10000; // keV
  
  TFile* file = new TFile(this_case->input_filename.c_str(), "READ");
  //  std::string debug_filename = "output_" + this_case->identifier + ".txt";
  //  std::ofstream output(debug_filename.c_str(), std::ofstream::out);
  
  for (std::vector<Arm>::iterator i_arm = this_case->arms.begin(); i_arm != this_case->arms.end(); ++i_arm) {
    std::string canvasname = "c_" + this_case->identifier + "_" + i_arm->armname;
    TCanvas* c = new TCanvas(canvasname.c_str(), canvasname.c_str());

    std::string profile_filename = "pid-profile-" + this_case->identifier + "-" + i_arm->armname + ".txt";
    std::ofstream output(profile_filename.c_str(), std::ofstream::out);
    
    std::string histname = this_case->baseplotname;
    histname.replace(this_case->baseplotname.find("ARM"), 3, i_arm->armname);
    i_arm->hEvdEAll = (TH2F*) (file->Get(histname.c_str()))->Clone();
    i_arm->hEvdEAll->SetDirectory(0);
    //    output << this_case->identifier << " " << i_arm->armname << std::endl;
    //    i_arm->hEvdEAll->RebinX(2);
    //    i_arm->hEvdEAll->RebinY(2);
    
    int n_bins_x = i_arm->hEvdEAll->GetXaxis()->GetNbins();
    int min_x = i_arm->hEvdEAll->GetXaxis()->GetXmin();
    int max_x = i_arm->hEvdEAll->GetXaxis()->GetXmax();
    int n_bins_y = i_arm->hEvdEAll->GetYaxis()->GetNbins();
    int min_y = i_arm->hEvdEAll->GetYaxis()->GetXmin();
    int max_y = i_arm->hEvdEAll->GetYaxis()->GetXmax();
        
    histname = "hEvdEBand_" + i_arm->armname;
    i_arm->hEvdEBand = new TH2F(histname.c_str(), histname.c_str(), n_bins_x,min_x,max_x, n_bins_y,min_y,max_y);
    i_arm->hEvdEBand->SetDirectory(0);
    
    histname = "hBandProfile_" + i_arm->armname;
    i_arm->hBandProfile = new TH1F(histname.c_str(), histname.c_str(), n_bins_x,min_x,max_x);
    i_arm->hBandProfile->SetDirectory(0);
    
    histname = "hNPeaks_" + i_arm->armname;
    TH1F* hNPeaks = new TH1F(histname.c_str(), "", n_bins_x,min_x,max_x);
    hNPeaks->SetDirectory(0);
    
    // Loop through the projection of each energy to try and find the proton band
    int start_bin = i_arm->hEvdEAll->GetXaxis()->FindBin(low_energy_cut);
    int stop_bin = i_arm->hEvdEAll->GetXaxis()->FindBin(high_energy_cut);
    for (int i_bin = start_bin; i_bin <= stop_bin; ++i_bin) {
      double i_energy = i_arm->hEvdEAll->GetXaxis()->GetBinLowEdge(i_bin);

      TH1* hProjection = i_arm->hEvdEAll->ProjectionY("_py", i_bin, i_bin);
      
      int n_peaks = GetNPeaks(hProjection, i_arm->n_entry_threshold);
      hNPeaks->Fill(i_energy, n_peaks);
      
      // Loop through the peaks
      int peak_start_bin;
      bool found_peak = false;
      for (int i_peak = 0; i_peak < n_peaks; ++i_peak) {
	// Get the location of the start of this peak
	int counter = 0;
	peak_start_bin = 1;
      	while (counter <= i_peak) {
	  peak_start_bin = FindNextPeak(hProjection, peak_start_bin, i_arm->n_entry_threshold);
	  ++counter;
	}
	  
	// Is this peak close enough to where we were last time
	double peak_start_dE = hProjection->GetBinLowEdge(peak_start_bin);
	int content = hProjection->GetBinContent(peak_start_bin);
	double difference = std::fabs(peak_start_dE - i_arm->first_dE_of_band);
	//	output << i_energy << ": Peak #" << i_peak+1 << " of " << n_peaks << ": Peak Start = " << peak_start_bin << ": " << i_arm->first_dE_of_band << " - " << peak_start_dE << " = " << difference << " (" << content << " entries)" << std::endl;
	if (difference < 250) {
	  // This is the peak we want to take for the selected band
	  found_peak = true;
	  i_arm->first_dE_of_band = peak_start_dE;
	  break; // from the loop through the peaks
	}
      }
      if (found_peak == false) {
	continue; // to the next energy bin
      }
      // Now fill in the bins until we get a bin content that is not part of the peak
      int j_bin = peak_start_bin;
      int bin_content = hProjection->GetBinContent(j_bin);
      while (bin_content > i_arm->n_entry_threshold) {
	bin_content = hProjection->GetBinContent(j_bin);
	if (bin_content > i_arm->n_entry_threshold) {
	  i_arm->hEvdEBand->SetBinContent(i_bin, j_bin, bin_content);
	}
	++j_bin;
      }
    
      hProjection = i_arm->hEvdEBand->ProjectionY("_py", i_bin, i_bin);
      double mean = hProjection->GetMean();
      double rms = hProjection->GetRMS();
      output << i_energy << " " << mean << " " << rms << std::endl;
      i_arm->hBandProfile->SetBinContent(i_bin, mean);
      i_arm->hBandProfile->SetBinError(i_bin, rms);
      //	std::cout << "Bin #" << i_bin << ": bin_location = " << peak_start_bin << std::endl;
    } // end loop through energy bins
       
    //    hNPeaks->Draw();
    //    i_arm->hEvdEAll->Draw("COLZ");
    i_arm->hEvdEBand->Draw("COLZ");
    //    hStoppedProtonProjection->Draw();
    //    i_arm->hBandProfile->Draw();
  }
  file->Close();
}

// Gets the number of "peaks", just based on the number of times we go to zero entries
int GetNPeaks(TH1* hist, int n_entry_threshold) {
  int n_bins = hist->GetNbinsX();
  int n_peaks = -1; // start at -1 since we will always go through the while loop once, even if we don't find the next peak
  int peak_start_bin = 1;

  while (peak_start_bin != n_bins) {
    peak_start_bin = FindNextPeak(hist, peak_start_bin, n_entry_threshold);
    ++n_peaks;
  }
  return n_peaks;
}

// Finds the next peak and returns the bin number
int FindNextPeak(TH1* hist, int start_bin, int n_entry_threshold) {
  int n_bins = hist->GetNbinsX();
  int return_bin = n_bins;
  int prev_bin_content = hist->GetBinContent(start_bin);
  
  for (int i_bin = start_bin+1; i_bin <= n_bins; ++i_bin) {
    int bin_content = hist->GetBinContent(i_bin);

    if (prev_bin_content <= n_entry_threshold && bin_content > n_entry_threshold) {
      return_bin = i_bin;
      break;
    }   
    prev_bin_content = bin_content;
  }
  return return_bin;
}

int CalculateEfficienciesAndPurities(Case* this_case) {
  // Want to know the efficiency and purity of the cut based on MC
  TFile* file = new TFile(this_case->input_filename.c_str(), "READ");
  for (std::vector<Arm>::iterator i_arm = this_case->arms.begin(); i_arm != this_case->arms.end(); ++i_arm) {

    // Reset these counters
    for (std::vector<ParticleType>::iterator i_particle_type = this_case->particle_types.begin(); i_particle_type != this_case->particle_types.end(); ++i_particle_type) {
      i_particle_type->n_selected = 0;
      i_particle_type->n_total = 0;
    }

    TH1F* hExtractedBand = i_arm->hBandProfile;
    int n_bins = hExtractedBand->GetNbinsX();

    // Loop through the extracted band profile
    for (int i_bin = 1; i_bin <= n_bins; ++i_bin) {
      double mean = hExtractedBand->GetBinContent(i_bin);
      double rms = hExtractedBand->GetBinError(i_bin);

      if (mean == 0) {
	continue;
      }

      int integral_low = hExtractedBand->FindBin(mean - 1*rms);
      int integral_high = hExtractedBand->FindBin(mean + 1*rms);

      // Loop through the particle types and count how many are within the cut and how many are in that energy in total
      for (std::vector<ParticleType>::iterator i_particle_type = this_case->particle_types.begin(); 
	   i_particle_type != this_case->particle_types.end(); ++i_particle_type) {
	
	std::string histname = "hEvdE_" + i_arm->armname + "_" + i_particle_type->type_name;
	TH2F* hParticleEvdE = (TH2F*) file->Get(histname.c_str());

	if (!hParticleEvdE) {
	  std::cout << "ERROR: Need histograms of each particle type to calculate efficiencies" << std::endl;
	  std::cout << "Quitting" << std::endl;
	  return 1;
	}

	int n_selected_this_bin = hParticleEvdE->ProjectionY("_py", i_bin, i_bin)->Integral(integral_low, integral_high);
	int n_total_this_bin = hParticleEvdE->ProjectionY("_py", i_bin, i_bin)->Integral();

	i_particle_type->n_selected += n_selected_this_bin;
	i_particle_type->n_total += n_total_this_bin;
      } // end loop through particle types
    } // end loop through energy bins

    std::cout << "Efficiencies: " << std::endl;
    int total_in_cut = 0;
    for (std::vector<ParticleType>::iterator i_particle_type = this_case->particle_types.begin(); 
	 i_particle_type != this_case->particle_types.end(); ++i_particle_type) {
      
      std::cout << this_case->identifier << " " << i_arm->armname 
		<< ": (n_" << i_particle_type->type_name << "_in_cut / n_total_" << i_particle_type->type_name + ") = " 
		<< i_particle_type->n_selected << " / " << i_particle_type->n_total << " = " 
		<< (double) i_particle_type->n_selected / i_particle_type->n_total << std::endl;

      total_in_cut += i_particle_type->n_selected;
    }

    std::cout << "Purities: " << std::endl;
    for (std::vector<ParticleType>::iterator i_particle_type = this_case->particle_types.begin(); 
	 i_particle_type != this_case->particle_types.end(); ++i_particle_type) {

      std::cout << this_case->identifier << " " << i_arm->armname 
		<< ": (n_" + i_particle_type->type_name + "_in_cut / n_total_selected) = " 
		<< i_particle_type->n_selected << " / " << total_in_cut << " = " 
		<< (double) i_particle_type->n_selected / total_in_cut << std::endl;
    } // end loop through particles types
  } // end loop through arms
  return 0;
}
