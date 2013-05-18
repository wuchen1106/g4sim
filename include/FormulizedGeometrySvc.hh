//---------------------------------------------------------------------------//
//Description: 1. Setup FormulizedGeometry geometry and manage associated sensitive detectors
//             2. Communicate with associated sensitive detectors
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment: about fVerboseLevel:
//         You can change it by calling SetVerbose(int) somewhere
//         or modifying input card before starting a run.
//         Either way would set verbose of selected sensitive detectors to the same level
//         0:    Only Warnings and Errors
//         1:    Important information
//         >= 5: All information
//---------------------------------------------------------------------------//

#ifndef FormulizedGeometrySvc_h
#define FormulizedGeometrySvc_h 1

#include "globals.hh"

#include "SimpleGeometrySvc.hh"

class G4VPhysicalVolume;
class FormulizedGeometryParameter;

class FormulizedGeometrySvc : public SimpleGeometrySvc
{
  public:
    FormulizedGeometrySvc(G4String name, G4String opt = "");
    virtual ~FormulizedGeometrySvc();

    virtual G4VPhysicalVolume* SetGeometry();//Setup geometry. Should be called before the begin of a run

		//Special functions
		void ConstructVolumes();

		G4VPhysicalVolume* PlaceVolumes();

		//=>Access
		FormulizedGeometryParameter* get_GeometryParameter(){ return m_GeometryParameter; }

		//=>Modify
		void set_GeometryParameter( FormulizedGeometryParameter* val );
	
	private:

		FormulizedGeometryParameter* m_GeometryParameter;

};

#endif

