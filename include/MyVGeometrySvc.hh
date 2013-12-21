//---------------------------------------------------------------------------//
//Description: 1. Setup MyVGeometry geometry and manage associated sensitive detectors
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

#ifndef MyVGeometrySvc_h
#define MyVGeometrySvc_h 1

#include "globals.hh"

#include "MyConfigure.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class MyVGeometryParameter;

class MyVGeometrySvc : public MyConfigure
{
	public:
		MyVGeometrySvc(G4String name, G4String opt = "");
		virtual ~MyVGeometrySvc();

		//=> Crucial functions
		virtual G4VPhysicalVolume* SetGeometry(){return 0;} //Setup geometry. Should be called before the begin of a run

		void ReadCard(G4String);

		//Special functions
		void ConstructVolumes(){};

		G4VPhysicalVolume* PlaceVolumes(){return 0;}

		//=> public functions
		G4LogicalVolume* get_logicalVolume(G4String name);

		//=> Access
		MyVGeometryParameter* get_GeometryParameter(){ return m_GeometryParameter; }

		int get_VerboseLevel(){ return fVerboseLevel; }

		G4String get_Name(){ return m_Name; }

		//=> Modify
		void set_GeometryParameter(MyVGeometryParameter* val){ m_GeometryParameter = val; }

		void set_VerboseLevel(int val){ fVerboseLevel = val; }

		void set_Name(G4String val){ m_Name = val; }

	private:

		MyVGeometryParameter* m_GeometryParameter;

		int fVerboseLevel;

		G4String m_Name;
};

#endif

