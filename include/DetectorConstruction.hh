//---------------------------------------------------------------------------//
//Description: Geant4 Class
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;

class DetectorMessenger;
class MyDetectorManager;
class MyFieldSvc;

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:

    DetectorConstruction();
    ~DetectorConstruction();

    G4VPhysicalVolume* Construct();     

		void UpdateGeometry();

	private:

		G4VPhysicalVolume* ConstructDetector();

	private:

    DetectorMessenger* detectorMessenger;  //pointer to the Messenger

		MyFieldSvc* pMyFieldSvc;
		MyDetectorManager* pMyDetectorManager;
};

#endif
