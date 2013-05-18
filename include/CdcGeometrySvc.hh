//---------------------------------------------------------------------------//
//Description: 1. Setup Cdc geometry and manage associated sensitive detectors
//             2. Communicate with associated sensitive detectors
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 2 Aug, 2012
//Modified: 6 Nov, 2012 by Wu Chen(wuchen@mail.ihep.ac.cn)
//Modified: 7 Jan, 2013 by Wu Chen(wuchen@mail.ihep.ac.cn)
//          Added Boolean Volumes
//Comment: about fVerboseLevel:
//         or modifying input card before starting a run.
//         Either way would set verbose of selected sensitive detectors to the same level
//         0:    Only Warnings and Errors
//         1:    Important information
//         >= 5: All information
//---------------------------------------------------------------------------//

#ifndef CdcGeometrySvc_h
#define CdcGeometrySvc_h 1

#include "globals.hh"

#include "SimpleGeometrySvc.hh"

class CdcGeometryParameter;

class CdcGeometrySvc : public SimpleGeometrySvc
{
  public:
    CdcGeometrySvc(G4String name, G4String opt = "");
    virtual ~CdcGeometrySvc();

    virtual G4VPhysicalVolume* SetGeometry();//Setup geometry. Should be called before the begin of a run

		//Special functions
		void ConstructVolumes();

		G4VPhysicalVolume* PlaceVolumes(){return 0;}

		//=>Access
		CdcGeometryParameter* get_GeometryParameter(){ return m_GeometryParameter; }

		//=>Modify
		void set_GeometryParameter( CdcGeometryParameter* val );

	private:

		CdcGeometryParameter* m_GeometryParameter;

};

#endif

