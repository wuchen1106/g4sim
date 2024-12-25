//---------------------------------------------------------------------------//
//Description: 1. Setup SimpleGeometry geometry and manage associated sensitive detectors
//             2. Communicate with associated sensitive detectors
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Modified: 11, Jan, 2013
//          Support G4Hype, G4TwistedTube
//Comment: about fVerboseLevel:
//         You can change it by calling SetVerbose(int) somewhere
//         or modifying input card before starting a run.
//         Either way would set verbose of selected sensitive detectors to the same level
//         0:    Only Warnings and Errors
//         1:    Important information
//         >= 5: All information
//---------------------------------------------------------------------------//

#ifndef SimpleGeometrySvc_h
#define SimpleGeometrySvc_h 1

#include "myglobals.hh"

#include "MyVGeometrySvc.hh"

class G4VPhysicalVolume;
class SimpleGeometryParameter;
class G4UserLimits;

class SimpleGeometrySvc : public MyVGeometrySvc
{
  public:
    SimpleGeometrySvc(G4String name, G4String opt = "");
    virtual ~SimpleGeometrySvc();

    virtual G4VPhysicalVolume* SetGeometry();//Setup geometry. Should be called before the begin of a run

		//Special functions
		void ConstructVolumes();

		G4VPhysicalVolume* PlaceVolumes();

		//=>Access
		SimpleGeometryParameter* get_GeometryParameter(){ return m_GeometryParameter; }

		//=>Modify
		void set_GeometryParameter( SimpleGeometryParameter* val );
	
	private:

		SimpleGeometryParameter* m_GeometryParameter;
                G4UserLimits * fStepLimit; 

};

#endif

