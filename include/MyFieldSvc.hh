//---------------------------------------------------------------------------//
//Description: Setup MagField
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#ifndef MyFieldSvc_h
#define MyFieldSvc_h 1

#include "myglobals.hh"
#include <vector>

class MyFieldSvcMessenger;
class G4ChordFinder;
class G4FieldManager;
class G4ElectroMagneticField;
class G4MagIntegratorStepper;
class G4MagInt_Driver;
class G4EqMagElectricField;
class G4LogicalVolume;
class MyFieldMap;

class MyFieldSvc
{
	public:
		MyFieldSvc();
		~MyFieldSvc();

		static MyFieldSvc* GetMyFieldSvc();

		void ReadCard(G4String);

		void AddMap(G4String,double,double=0,double=0,double=0);

		void Reset();

		void SetField(G4LogicalVolume* fLogicWorld  = 0);

		void SetFieldType( G4String val ){ fType = val; }

		void SetMagIntensity( G4double val ){ UniM_Intensity = val; }
		void SetMagTheta( G4double val ){ UniM_Theta = val; }
		void SetMagPhi( G4double val ){ UniM_Phi = val; }

		void SetEleIntensity( G4double val ){ UniE_Intensity = val; }
		void SetEleTheta( G4double val ){ UniE_Theta = val; }
		void SetElePhi( G4double val ){ UniE_Phi = val; }

		void SetEleVoltageScaled( G4double val ){ CylE_VoltScaled = val; }

		void SetStepper();

	private:

		void Dump();

		void UpdateField();
		G4FieldManager*  GetGlobalFieldManager();

	private:

		static MyFieldSvc*      fMyFieldSvc;
		MyFieldSvcMessenger*    fMyFieldSvcMessenger;   //messenger of this class

		//ElectroMagnetic Field
		G4ChordFinder*          fChordFinder;
		G4FieldManager*         fFieldManager;
		G4ElectroMagneticField* fMagField;
		G4MagIntegratorStepper* fStepper;
		G4MagInt_Driver*        fIntgrDriver;
		G4EqMagElectricField*   fEquation;

		G4String            fType; // simple or map
		G4String            fMType; // uniform (or solenoid)
		G4String            fEType; // uniform or cylinder

		// properties for simple fields
		G4double            UniM_Intensity;
		G4double            UniM_Theta;
		G4double            UniM_Phi;
		G4double            UniE_Intensity;
		G4double            UniE_Theta;
		G4double            UniE_Phi;
		G4double            CylE_VoltScaled;
		G4double            EF_StepL;
		G4int               EF_StepT;

		// properties for map fields
		std::vector<G4String>          fFieldMapFilenames;        // input filenames for fieldmaps
		std::vector<G4double>          fFieldMapScalings;   // multiples the fields in each file by this factor
		std::vector<G4double>          fFieldMapX0;   // 
		std::vector<G4double>          fFieldMapY0;   // 
		std::vector<G4double>          fFieldMapZ0;   // 
		std::vector<MyFieldMap*>       fFieldMaps;                // field map objects. Pointer is barely used as fieldmaps are defined
};

#endif

