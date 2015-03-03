#include <iostream>
#include <fstream>
#include <cmath>

#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"

struct ParticleType {
  std::string type_name;
  double n_selected;
  double n_total;

  TH2F* hParticleEvdE;
  TH1F* hProfile;
  TF1* particle_band_gaussian;
};

struct Arm{
  std::string armname;
  TH2F* hEvdEAll;
  TH2F* hEvdEBand;
  TH1F* hBandProfile;

  int n_entry_threshold;  // the number of entries that we determine as the start of a peak
  int first_dE_of_band; // which is the dE value at the start of the band

  std::vector<ParticleType*> particle_types;

  double average_det_rms;

  TF1* extracted_band_gaussian;
};

struct Case {
  std::string identifier;
  std::vector<Arm*> arms;

  std::string input_filename;
  std::string baseplotname;
  int n_entry_threshold;  // the number of entries that we determine as the start of a peak
  int first_dE_of_band; // which is the dE value at the start of the band

  double dE_scale_factor; // want to scale the MC to the data
} data, MC;

void ExtractProtonBand(Arm* this_arm);
int GetNPeaks(TH1* hist, int n_entry_threshold);
int FindNextPeak(TH1* hist, int start_bin, int n_entry_threshold);
int CalculateEfficienciesAndPurities(Arm* this_arm);

void DataAndMC() {
  data.identifier = "data"; data.input_filename = "~/data/out/v92/total.root";
  data.baseplotname = "TME_Al50_EvdE/all_particles/ARM_EvdE";
  data.n_entry_threshold = 15;
  data.first_dE_of_band = 1900;
  data.dE_scale_factor = 1;


  MC.identifier = "MC"; MC.input_filename = "plots.root";
  MC.baseplotname = "hAll_EvdE_ARM";
  MC.n_entry_threshold = 2;
  MC.first_dE_of_band = 2000;
  MC.dE_scale_factor = 0.92;

  std::vector<Case*> cases;
  cases.push_back(&data);
  cases.push_back(&MC);

  // Loop through and extract the proton bands
  for (std::vector<Case*>::iterator i_case = cases.begin(); i_case != cases.end(); ++i_case) {
    Arm* left_arm = new Arm; Arm* right_arm = new Arm;
    left_arm->armname = "SiL"; right_arm->armname = "SiR";
    (*i_case)->arms.push_back(left_arm); (*i_case)->arms.push_back(right_arm);

    TFile* file = new TFile((*i_case)->input_filename.c_str(), "READ");
    // Initialise everything
    for (std::vector<Arm*>::iterator i_arm = (*i_case)->arms.begin(); i_arm != (*i_case)->arms.end(); ++i_arm) {
      (*i_arm)->n_entry_threshold = (*i_case)->n_entry_threshold;
      (*i_arm)->first_dE_of_band = (*i_case)->first_dE_of_band;

      std::string histname = (*i_case)->baseplotname;
      histname.replace((*i_case)->baseplotname.find("ARM"), 3, (*i_arm)->armname);
      (*i_arm)->hEvdEAll = (TH2F*) (file->Get(histname.c_str()))->Clone();
      (*i_arm)->hEvdEAll->SetDirectory(0);

      ParticleType* proton_stopped = new ParticleType; proton_stopped->type_name = "proton_stopped";
      ParticleType* proton_not_stopped = new ParticleType; proton_not_stopped->type_name = "proton_not_stopped";
      ParticleType* deuteron = new ParticleType; deuteron->type_name = "deuteron";
      ParticleType* triton = new ParticleType; triton->type_name = "triton"; 
      ParticleType* alpha = new ParticleType; alpha->type_name = "alpha";
      (*i_arm)->particle_types.push_back(proton_stopped); (*i_arm)->particle_types.push_back(proton_not_stopped); 
      (*i_arm)->particle_types.push_back(deuteron); (*i_arm)->particle_types.push_back(triton); (*i_arm)->particle_types.push_back(alpha);

      for (std::vector<ParticleType*>::iterator i_particle_type = (*i_arm)->particle_types.begin(); i_particle_type != (*i_arm)->particle_types.end(); ++i_particle_type) {
	std::string histname = "hEvdE_" + (*i_arm)->armname + "_" + (*i_particle_type)->type_name;
	(*i_particle_type)->hParticleEvdE = (TH2F*) file->Get(histname.c_str());
	if ( (*i_particle_type)->hParticleEvdE) {
	  (*i_particle_type)->hParticleEvdE->SetDirectory(0);
	}
      }

      // Scale EvdEs in here later....
      std::string canvasname = "c_" + (*i_case)->identifier + "_" + (*i_arm)->armname;
      TCanvas* c = new TCanvas(canvasname.c_str(), canvasname.c_str());

      ExtractProtonBand(*i_arm);

      (*i_arm)->hEvdEAll->Draw("COLZ");

    } // end loop through arms
    file->Close();
  } // end loop through cases

  // Calculate the detector smear
  // Loop through the arms of data and MC at the same time
  for (std::vector<Arm*>::iterator i_data_arm = data.arms.begin(), i_MC_arm = MC.arms.begin();
       i_data_arm != data.arms.end() && i_MC_arm != MC.arms.end();
       ++i_data_arm, ++i_MC_arm) {
    TH1F* hProfile_data = (*i_data_arm)->hBandProfile;
    TH1F* hProfile_MC = (*i_MC_arm)->hBandProfile;

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
	//	std::cout << "Bin #" << i_bin << ": data_rms = " << data_rms << ", MC = " << MC_rms << ", det_rms = " << det_rms << std::endl;
      }
    }
    (*i_MC_arm)->average_det_rms = 0;//total_det_rms / n_good_det_rms;
    std::cout << (*i_data_arm)->armname << " Average Det RMS = " << (*i_MC_arm)->average_det_rms << std::endl;

    CalculateEfficienciesAndPurities(*i_MC_arm);
  }

  // Write everything to an output file
  TFile* output_file = new TFile("proton_band.root", "RECREATE");

  for (std::vector<Case*>::iterator i_case = cases.begin(); i_case != cases.end(); ++i_case) {
    for (std::vector<Arm*>::iterator i_arm = (*i_case)->arms.begin(); i_arm != (*i_case)->arms.end(); ++i_arm) {
      if ((*i_arm)->hEvdEAll) {
	(*i_arm)->hEvdEAll->Write();
      }
      if ((*i_arm)->hEvdEBand) {
	(*i_arm)->hEvdEBand->Write();
      }
      if ((*i_arm)->hBandProfile) {
	(*i_arm)->hBandProfile->Write();
      }
      /*      if ((*i_arm)->extracted_band_gaussian) {
	(*i_arm)->extracted_band_gaussian->Write();
      }
      */

      for (std::vector<ParticleType*>::iterator i_particle_type = (*i_arm)->particle_types.begin(); i_particle_type != (*i_arm)->particle_types.end(); ++i_particle_type) {
	if ((*i_particle_type)->hParticleEvdE) {
	  (*i_particle_type)->hParticleEvdE->Write();
	}
	if ((*i_particle_type)->hProfile) {
	  (*i_particle_type)->hProfile->Write();
	}
	if ((*i_particle_type)->particle_band_gaussian) {
	  (*i_particle_type)->particle_band_gaussian->Write();
	}
      }
    }
  }
}

void ExtractProtonBand(Arm* this_arm) {

  double low_energy_cut = 1000; // keV
  double high_energy_cut = 10000; // keV
  
  int n_bins_x = this_arm->hEvdEAll->GetXaxis()->GetNbins();
  int min_x = this_arm->hEvdEAll->GetXaxis()->GetXmin();
  int max_x = this_arm->hEvdEAll->GetXaxis()->GetXmax();
  int n_bins_y = this_arm->hEvdEAll->GetYaxis()->GetNbins();
  int min_y = this_arm->hEvdEAll->GetYaxis()->GetXmin();
  int max_y = this_arm->hEvdEAll->GetYaxis()->GetXmax();
        
  std::string histname = "hEvdEBand_" + this_arm->armname;
  this_arm->hEvdEBand = new TH2F(histname.c_str(), histname.c_str(), n_bins_x,min_x,max_x, n_bins_y,min_y,max_y);
  this_arm->hEvdEBand->SetDirectory(0);
    
  histname = "hBandProfile_" + this_arm->armname;
  this_arm->hBandProfile = new TH1F(histname.c_str(), histname.c_str(), n_bins_x,min_x,max_x);
  this_arm->hBandProfile->SetDirectory(0);
  
  /*  histname = "hNPeaks_" + this_arm->armname;
  TH1F* hNPeaks = new TH1F(histname.c_str(), "", n_bins_x,min_x,max_x);
  hNPeaks->SetDirectory(0);
  */

  // Loop through the projection of each energy to try and find the proton band
  int start_bin = this_arm->hEvdEAll->GetXaxis()->FindBin(low_energy_cut);
  int stop_bin = this_arm->hEvdEAll->GetXaxis()->FindBin(high_energy_cut);
  for (int i_bin = start_bin; i_bin <= stop_bin; ++i_bin) {
    double i_energy = this_arm->hEvdEAll->GetXaxis()->GetBinLowEdge(i_bin);
    
    TH1* hProjection = this_arm->hEvdEAll->ProjectionY("_py", i_bin, i_bin);
    
    int n_peaks = GetNPeaks(hProjection, this_arm->n_entry_threshold);
    //    hNPeaks->Fill(i_energy, n_peaks);
    
    // Loop through the peaks
    int peak_start_bin;
    bool found_peak = false;
    for (int i_peak = 0; i_peak < n_peaks; ++i_peak) {
      // Get the location of the start of this peak
      int counter = 0;
      peak_start_bin = 1;
      while (counter <= i_peak) {
	peak_start_bin = FindNextPeak(hProjection, peak_start_bin, this_arm->n_entry_threshold);
	++counter;
      }
      
      // Is this peak close enough to where we were last time
      double peak_start_dE = hProjection->GetBinLowEdge(peak_start_bin);
      int content = hProjection->GetBinContent(peak_start_bin);
      double difference = std::fabs(peak_start_dE - this_arm->first_dE_of_band);
      //	output << i_energy << ": Peak #" << i_peak+1 << " of " << n_peaks << ": Peak Start = " << peak_start_bin << ": " << this_arm->first_dE_of_band << " - " << peak_start_dE << " = " << difference << " (" << content << " entries)" << std::endl;
      if (difference < 250) {
	// This is the peak we want to take for the selected band
	found_peak = true;
	this_arm->first_dE_of_band = peak_start_dE;
	break; // from the loop through the peaks
      }
    }
    if (found_peak == false) {
      continue; // to the next energy bin
    }
    // Now fill in the bins until we get a bin content that is not part of the peak
    int j_bin = peak_start_bin;
    int bin_content = hProjection->GetBinContent(j_bin);
    while (bin_content > this_arm->n_entry_threshold) {
      bin_content = hProjection->GetBinContent(j_bin);
      if (bin_content > this_arm->n_entry_threshold) {
	this_arm->hEvdEBand->SetBinContent(i_bin, j_bin, bin_content);
      }
      ++j_bin;
    }
    
    hProjection = this_arm->hEvdEBand->ProjectionY("_py", i_bin, i_bin);
    double mean = hProjection->GetMean();
    double rms = hProjection->GetRMS();
    //    output << i_energy << " " << mean << " " << rms << std::endl;
    this_arm->hBandProfile->SetBinContent(i_bin, mean);
    this_arm->hBandProfile->SetBinError(i_bin, rms);
    //	std::cout << "Bin #" << i_bin << ": bin_location = " << peak_start_bin << std::endl;
  } // end loop through energy bins
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

int CalculateEfficienciesAndPurities(Arm* this_arm) {
  // Want to know the efficiency and purity of the cut based on MC

  TH1F* hExtractedBand = this_arm->hBandProfile;
  int n_bins = hExtractedBand->GetNbinsX();
  double min_x = hExtractedBand->GetXaxis()->GetXmin();
  double max_x = hExtractedBand->GetXaxis()->GetXmax();

  double det_rms = this_arm->average_det_rms; // the extra detector smearing
  std::cout << this_arm->armname << std::endl;

  // Set things up for each particle type
  for (std::vector<ParticleType*>::iterator i_particle_type = this_arm->particle_types.begin(); i_particle_type != this_arm->particle_types.end(); ++i_particle_type) {
    (*i_particle_type)->n_selected = 0;
    (*i_particle_type)->n_total = 0;
    
    std::string histname = (*i_particle_type)->hParticleEvdE->GetName();
    histname += "_profile";
    (*i_particle_type)->hProfile = new TH1F(histname.c_str(), "", n_bins,min_x,max_x);
      
    if (!(*i_particle_type)->hParticleEvdE) {
      std::cout << "ERROR: Need histograms of each particle type to calculate efficiencies" << std::endl;
      std::cout << "Quitting" << std::endl;
      return 1;
    }	
  }

  // Loop through the extracted band profile
  for (int i_bin = 1; i_bin <= n_bins; ++i_bin) {
    double mean = hExtractedBand->GetBinContent(i_bin);
    double rms = hExtractedBand->GetBinError(i_bin);
    
    if (mean < 0.1) {
      continue;
    }
    
    // Want to add the detector smearing
    double new_rms = std::sqrt(rms*rms + det_rms*det_rms);
    hExtractedBand->SetBinError(i_bin, new_rms);
    
    this_arm->extracted_band_gaussian = new TF1("extracted_band_gaussian", "TMath::Gaus(x, 0, [0], 1)");
    this_arm->extracted_band_gaussian->SetParameter(0, new_rms);
    
    int integral_low = mean - 1*new_rms;
    int integral_high = mean + 1*new_rms;
    
    // Loop through the particle types and count how many are within the cut and how many are in that energy in total
    for (std::vector<ParticleType*>::iterator i_particle_type = this_arm->particle_types.begin(); 
	 i_particle_type != this_arm->particle_types.end(); ++i_particle_type) {
      
      // Need to add detector resolution to each particle band
      TH1D* hProjection = (*i_particle_type)->hParticleEvdE->ProjectionY("_py", i_bin, i_bin);
      double particle_mean = hProjection->GetMean();
      double particle_rms = hProjection->GetRMS();

      double new_particle_rms = std::sqrt(particle_rms*particle_rms + det_rms*det_rms);
      (*i_particle_type)->hProfile->SetBinContent(i_bin, particle_mean);
      (*i_particle_type)->hProfile->SetBinError(i_bin, new_particle_rms);

      if (particle_mean < 0.1 || new_particle_rms < 0.1) {
	continue;
      }
      (*i_particle_type)->particle_band_gaussian = new TF1("particle_band_gaussian", "TMath::Gaus(x, [0], [1], 1)", particle_mean-10*new_particle_rms, particle_mean+10*new_particle_rms);
      (*i_particle_type)->particle_band_gaussian->SetParameter(0, particle_mean);
      (*i_particle_type)->particle_band_gaussian->SetParameter(1, new_particle_rms);

      double n_selected_this_bin = (*i_particle_type)->particle_band_gaussian->Integral(integral_low, integral_high);
      double n_total_this_bin = 1; // should be normalised...

      (*i_particle_type)->n_selected += n_selected_this_bin;
      (*i_particle_type)->n_total += n_total_this_bin;
    } // end loop through particle types
  } // end loop through energy bins

  std::cout << "Efficiencies: " << std::endl;
  double total_in_cut = 0;
  for (std::vector<ParticleType*>::iterator i_particle_type = this_arm->particle_types.begin(); 
       i_particle_type != this_arm->particle_types.end(); ++i_particle_type) {
      
    std::cout << this_arm->armname 
	      << ": (n_" << (*i_particle_type)->type_name << "_in_cut / n_total_" << (*i_particle_type)->type_name + ") = " 
	      << (*i_particle_type)->n_selected << " / " << (*i_particle_type)->n_total << " = " 
	      << (double) (*i_particle_type)->n_selected / (*i_particle_type)->n_total << std::endl;

    total_in_cut += (*i_particle_type)->n_selected;
  }

  std::cout << "Purities: " << std::endl;
  for (std::vector<ParticleType*>::iterator i_particle_type = this_arm->particle_types.begin(); 
       i_particle_type != this_arm->particle_types.end(); ++i_particle_type) {
    
    std::cout << this_arm->armname 
	      << ": (n_" + (*i_particle_type)->type_name + "_in_cut / n_total_selected) = " 
	      << (*i_particle_type)->n_selected << " / " << total_in_cut << " = " 
	      << (double) (*i_particle_type)->n_selected / total_in_cut << std::endl;
  } // end loop through particles types

  return 0;
}
