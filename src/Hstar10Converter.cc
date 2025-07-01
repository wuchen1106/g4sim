#include "Hstar10Converter.hh"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <stdexcept>

void Hstar10Converter::LoadFromFile(const std::string& filename)
{
  energy_MeV.clear();
  Hstar10_pSvcm2.clear();

  std::ifstream infile(filename);
  if (!infile.is_open()) {
    throw std::runtime_error("Cannot open file: " + filename);
  }

  std::string line;
  while (std::getline(infile, line)) {
    std::istringstream iss(line);
    double E_GeV, coeff_Svcm2, stdev;
    if (!(iss >> E_GeV >> coeff_Svcm2 >> stdev)) {
      continue; // Skip malformed lines
    }

    // Convert: E [GeV] → [MeV], Coeff [Sv cm²] → [pSv cm²]
    double E_MeV = E_GeV * 1e3;
    double coeff_pSvcm2 = coeff_Svcm2 * 1e12;

    energy_MeV.push_back(E_MeV);
    Hstar10_pSvcm2.push_back(coeff_pSvcm2);
  }

  if (energy_MeV.empty()) {
    throw std::runtime_error("No data loaded from file: " + filename);
  }
}

double Hstar10Converter::GetHstar10(double E_MeV)
{
  if (energy_MeV.empty()) {
    throw std::runtime_error("No coefficients loaded!");
  }

  if (E_MeV <= energy_MeV.front()) return Hstar10_pSvcm2.front();
  if (E_MeV >= energy_MeV.back()) return Hstar10_pSvcm2.back();

  for (size_t i = 1; i < energy_MeV.size(); ++i) {
    if (E_MeV < energy_MeV[i]) {
      double E1 = energy_MeV[i-1];
      double E2 = energy_MeV[i];
      double H1 = Hstar10_pSvcm2[i-1];
      double H2 = Hstar10_pSvcm2[i];

      double logE = std::log(E_MeV);
      double logE1 = std::log(E1);
      double logE2 = std::log(E2);
      double logH1 = std::log(H1);
      double logH2 = std::log(H2);

      double logH = logH1 + (logH2 - logH1) * (logE - logE1) / (logE2 - logE1);
      return std::exp(logH);
    }
  }

  return 0.0;
}
