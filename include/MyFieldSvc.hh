//---------------------------------------------------------------------------//
//Description: Setup MagField
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#ifndef MyFieldSvc_h
#define MyFieldSvc_h 1

#include "globals.hh"

class MyFieldSvcMessenger;
class G4ChordFinder;
class G4FieldManager;
class G4UniformMagField;
class G4UniformElectricField;
class G4MagIntegratorStepper;
class G4MagInt_Driver;
class G4EqMagElectricField;
class G4LogicalVolume;

class MyFieldSvc
{
  public:
    MyFieldSvc();
     ~MyFieldSvc();

    static MyFieldSvc* GetMyFieldSvc();

    void ReadCard(G4String);

    void SetField(G4LogicalVolume* fLogicWorld);

		void SetFieldType( G4String val ){ fType = val; }

		void SetMagIntensity( G4double val ){ UniF_Intensity = val; }
		void SetMagTheta( G4double val ){ UniF_Theta = val; }
		void SetMagPhi( G4double val ){ UniF_Phi = val; }

		void SetEleIntensity( G4double val ){ UniEF_Intensity = val; }
		void SetEleTheta( G4double val ){ UniEF_Theta = val; }
		void SetElePhi( G4double val ){ UniEF_Phi = val; }
		void SetStepper();

  private:

    void Dump();

		void UpdateField( G4String opt = "" );
		G4FieldManager*  GetGlobalFieldManager();

  private:

    static MyFieldSvc* fMyFieldSvc;
		MyFieldSvcMessenger* fMyFieldSvcMessenger;   //messenger of this class
		G4ChordFinder* fChordFinder;

		G4String            fType;

		//Magnetic Field
    G4UniformMagField*  fMagField;
		G4double            UniF_Intensity;
		G4double            UniF_Theta;
		G4double            UniF_Phi;

		//Electric Field
		G4FieldManager*         fFieldManager;
    G4UniformElectricField*  fEleField;
		G4MagIntegratorStepper* fStepper;
		G4MagInt_Driver*        fIntgrDriver;
		G4EqMagElectricField   *fEquation;
		G4double            UniEF_Intensity;
		G4double            UniEF_Theta;
		G4double            UniEF_Phi;
		G4double            UniEF_StepL;
		G4int               UniEF_StepT;
};

#endif

