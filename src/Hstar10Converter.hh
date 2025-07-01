// Hstar10Converter.hh

#ifndef Hstar10Converter_h
#define Hstar10Converter_h 1

#include "globals.hh"
#include <vector>
#include <string>

class Hstar10Converter {
public:
  Hstar10Converter() = default;
  ~Hstar10Converter() = default;

  void LoadFromFile(const std::string& filename);

  double GetHstar10(double E_MeV);

private:
  std::vector<double> energy_MeV;
  std::vector<double> Hstar10_pSvcm2;
};

#endif
