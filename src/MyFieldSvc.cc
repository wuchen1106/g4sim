//---------------------------------------------------------------------------//
//Description: Setup MagField
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "MyFieldSvc.hh"

#include "G4UnitsTable.hh"
#include "myglobals.hh"

#include "G4SimpleEMField.hh"
#include "G4UniformElectricField.hh"
#include "G4UniformMagField.hh"
#include "G4MagneticField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4EquationOfMotion.hh"
#include "G4EqMagElectricField.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4MagIntegratorDriver.hh"
#include "G4ChordFinder.hh"

#include "G4ExplicitEuler.hh"
#include "G4ImplicitEuler.hh"
#include "G4SimpleRunge.hh"
#include "G4SimpleHeum.hh"
#include "G4ClassicalRK4.hh"
#include "G4HelixExplicitEuler.hh"
#include "G4HelixImplicitEuler.hh"
#include "G4HelixSimpleRunge.hh"
#include "G4CashKarpRKF45.hh"
#include "G4RKG3_Stepper.hh"

#include <sstream>
#include <fstream>
#include <string.h>
#include <iostream>

#include "MyGlobalField.hh"
#include "MyFieldMap.hh"

#include "MyFieldSvcMessenger.hh"

MyFieldSvc* MyFieldSvc::fMyFieldSvc = 0;

	MyFieldSvc::MyFieldSvc()
: fChordFinder(0), fStepper(0), fIntgrDriver(0), fEMField(0), fType("none"), fEType(""), fMType("")
{
	if (fMyFieldSvc){
		G4Exception("MyFieldSvc::MyFieldSvc()","Run0031",
				FatalException, "MyFieldSvc constructed twice.");
	}
	fMyFieldSvc = this;
	fFieldManager = GetGlobalFieldManager();

	//Magnetic Field
    (void)MyGlobalField::getObject();

	//Electric Field
	fEquation = new G4EqMagElectricField(fEMField); 

	fMyFieldSvcMessenger= new MyFieldSvcMessenger(this);
}

MyFieldSvc::~MyFieldSvc()
{
	if(fEMField) delete fEMField;
	delete fMyFieldSvcMessenger;
}

MyFieldSvc* MyFieldSvc::GetMyFieldSvc(){
	if ( !fMyFieldSvc ){
		fMyFieldSvc = new MyFieldSvc;
	}
	return fMyFieldSvc;
}

void MyFieldSvc::SetField(G4LogicalVolume* fLogicWorld){
	Dump();
	//Setup the field
	if(fEMField) delete fEMField;
	if(fFieldMaps.size()>0){
		for (int ifm = 0; ifm < fFieldMaps.size(); ifm++){
			if (fFieldMaps[ifm]) delete fFieldMaps[ifm];
		}
		fFieldMaps.clear();
	}

	if ( fType == "none" ){
        fEMField = 0;
	}
    else if ( fType == "simple" ){
        fEMField = new G4SimpleEMField();
        if ( fMType == "uniform" ){
            G4ThreeVector MagField_vec(1,0,0);
            MagField_vec.setTheta(UniM_Theta);
            MagField_vec.setPhi(UniM_Phi);
            MagField_vec = MagField_vec.unit() * UniM_Intensity;
            if(MagField_vec!= G4ThreeVector(0.,0.,0.))
            { 
                fEMField->SetMagUniformVector(MagField_vec);
            }
        }

        if ( fEType == "uniform" ){
            G4ThreeVector EleField_vec(1,0,0);
            EleField_vec.setTheta(UniE_Theta);
            EleField_vec.setPhi(UniE_Phi);
            EleField_vec = EleField_vec.unit() * UniE_Intensity;
            if(EleField_vec!= G4ThreeVector(0.,0.,0.))
            { 
                fEMField->SetEleUniformVector(EleField_vec);
            }
        }
        else if ( fEType == "cylinder" ){
            if (CylE_VoltScaled){
                fEMField->SetEleCylinderVoltScaled(CylE_VoltScaled);
            }
        }
        fEMField->SetField();
        fEquation->SetFieldObj(fEMField);
	}
	else if ( fType == "fieldMap" ){
		for(G4int ifieldmap = 0; ifieldmap < fFieldMapFilenames.size(); ifieldmap++){
			fFieldMaps.push_back(new MyFieldMap(fFieldMapFilenames[ifieldmap],
				  fFieldMapScalings[ifieldmap], // "current" in G4Beamline field parlance
				  1.0, // gradient
				  0.0, // time offset
				  fLogicWorld,
				  fFieldMapX0[ifieldmap], // So called origin. It's actually the coordiantes of the simulation origin point in the frame of magnetic field coordiantes.
				  fFieldMapY0[ifieldmap],
				  fFieldMapZ0[ifieldmap]
				  ));
				G4cout << "Adding MyFieldMap: "<< fFieldMapFilenames[ifieldmap]
					<<", "<<fFieldMapScalings[ifieldmap]
					<<", "<<fFieldMapX0[ifieldmap]
					<<", "<<fFieldMapY0[ifieldmap]
					<<", "<<fFieldMapZ0[ifieldmap]
					<<G4endl;
		}
	}

	UpdateField();
}

void MyFieldSvc::AddMap(G4String name, double scale, double x, double y, double z){
	//FIXME
	fType = "fieldMap";
	G4String dir_name = getenv("FIELDMAPSROOT");
	if (dir_name[dir_name.size()-1] != '/') dir_name.append("/");
	name = dir_name + name;
	fFieldMapFilenames.push_back(name);
	fFieldMapScalings.push_back(scale);
	fFieldMapX0.push_back(x);
	fFieldMapY0.push_back(y);
	fFieldMapZ0.push_back(z);
}

void MyFieldSvc::Reset(){
	fType = "none";
	fFieldMapFilenames.clear();
	fFieldMapScalings.clear();
}

/////////////////////////////////////////////////////////////////////////////
//
// Register this field to 'global' Field Manager and 
// Create Stepper and Chord Finder with predefined type, minstep (resp.)
//

void MyFieldSvc::UpdateField()
{
    if (fType == "simple" ){
        if (fEType!="none"){
            SetStepper();
            if(fChordFinder) delete fChordFinder;
            // fChordFinder = new G4ChordFinder( fEMField, EF_StepL, fStepper);
            fIntgrDriver = new G4MagInt_Driver(EF_StepL,fStepper,fStepper->GetNumberOfVariables() );
            fChordFinder = new G4ChordFinder(fIntgrDriver);
            fFieldManager->SetChordFinder( fChordFinder )
        }
        else if (fMType!="none"){
            fFieldManager->CreateChordFinder(fEMField);
        } 
        else{
            fFieldManager->SetDetectorField(fEMField);
        }
    }
	else if ( fType == "fieldMap" ){
		// Nothing else should be done. this part is handled by RunAction instead
	}
	else if ( fType == "none" ){
        fFieldManager->SetDetectorField(fEMField);
    }
}

/////////////////////////////////////////////////////////////////////////////
//
// Set stepper according to the stepper type
//

void MyFieldSvc::SetStepper()
{
	G4int nvar = 8;

	if(fStepper) delete fStepper;

	switch ( EF_StepT ) 
	{
		case 0:  
			fStepper = new G4ExplicitEuler( fEquation, nvar ); 
			G4cout<<"G4ExplicitEuler is calledS"<<G4endl;     
			break;
		case 1:  
			fStepper = new G4ImplicitEuler( fEquation, nvar );      
			G4cout<<"G4ImplicitEuler is called"<<G4endl;     
			break;
		case 2:  
			fStepper = new G4SimpleRunge( fEquation, nvar );        
			G4cout<<"G4SimpleRunge is called"<<G4endl;     
			break;
		case 3:  
			fStepper = new G4SimpleHeum( fEquation, nvar );         
			G4cout<<"G4SimpleHeum is called"<<G4endl;     
			break;
		case 4:  
			fStepper = new G4ClassicalRK4( fEquation, nvar );       
			G4cout<<"G4ClassicalRK4 (default) is called"<<G4endl;     
			break;
		case 5:  
			fStepper = new G4CashKarpRKF45( fEquation, nvar );      
			G4cout<<"G4CashKarpRKF45 is called"<<G4endl;     
			break;
		case 6:  
			fStepper = 0; // new G4RKG3_Stepper( fEquation, nvar );       
			G4cout<<"G4RKG3_Stepper is not currently working for Electric Field"<<G4endl;     
			break;
		case 7:  
			fStepper = 0; // new G4HelixExplicitEuler( fEquation ); 
			G4cout<<"G4HelixExplicitEuler is not valid for Electric Field"<<G4endl;     
			break;
		case 8:  
			fStepper = 0; // new G4HelixImplicitEuler( fEquation ); 
			G4cout<<"G4HelixImplicitEuler is not valid for Electric Field"<<G4endl;     
			break;
		case 9:  
			fStepper = 0; // new G4HelixSimpleRunge( fEquation );   
			G4cout<<"G4HelixSimpleRunge is not valid for Electric Field"<<G4endl;     
			break;
		default: fStepper = 0;
	}
}

////////////////////////////////////////////////////////////////////////////////
//
//  Utility method

G4FieldManager*  MyFieldSvc::GetGlobalFieldManager()
{
	return G4TransportationManager::GetTransportationManager()
		->GetFieldManager();
}

/////////////////////////////////////////////////////////////////////
//
// Interface
//

void MyFieldSvc::ReadCard( G4String file_name ){
	if(file_name[0] != '/'){ // Relative Dir
		G4String dir_name = getenv("CONFIGUREROOT");
		if (dir_name[dir_name.size()-1] != '/') dir_name.append("/");
		file_name = dir_name + file_name;
	}
	std::ifstream fin_card(file_name);
	if ( !fin_card ){
		std::cout<<"MagField card"<<file_name<<" is not available!!!"<<std::endl;
		G4Exception("MyFieldSvc::ReadCard()","Run0031",
				FatalException, "MagField card is not available.");
	}
	std::stringstream buf_card;
	std::string s_card;
	while(getline(fin_card,s_card)){
		buf_card.str("");
		buf_card.clear();
		buf_card<<s_card;
		const char* c = s_card.c_str();
		int length = strlen(c);
		int offset = 0;
		for ( ; offset < length; offset++ ){
			if ( c[offset] != ' ' ) break;
		}
		if ( c[offset] == '#' ) continue;
		else if ( c[offset] == '/' && c[offset+1] == '/' ) continue;
		else if ( length - offset == 0 ) continue;
		std::string keyword;
		buf_card>>keyword;
		if ( keyword == "TYPE:"){
			buf_card>>fType;
			continue;
		}
		else if ( keyword == "Step:" ){
			buf_card>>EF_StepL>>EF_StepT;
			EF_StepL *= mm;
        }
		else if ( keyword == "UniM:" ){
			buf_card>>UniM_Intensity>>UniM_Theta>>UniM_Phi;
			UniM_Intensity *= tesla;
			UniM_Theta *= deg;
			UniM_Phi *= deg;
			fMType = "Uniform";
		}
		else if ( keyword == "UniE:" ){
			buf_card>>UniE_Intensity>>UniE_Theta>>UniE_Phi;
			UniE_Intensity *= kilovolt/cm;
			UniE_Theta *= deg;
			UniE_Phi *= deg;
			fEType = "Uniform";
		}
		else if ( keyword == "CylE:" ){
		    buf_card>>CylE_VoltScaled;
		    CylE_VoltScaled *= volt;
			fEType = "Cylinder";
        }
		else if ( keyword == "Map:" ){
			G4String name;
			G4double scale;
			buf_card>>name>>scale;
			G4String dir_name = getenv("FIELDMAPSROOT");
			if (dir_name[dir_name.size()-1] != '/') dir_name.append("/");
			name = dir_name + name;
			fFieldMapFilenames.push_back(name);
			fFieldMapScalings.push_back(scale);
		}
		else{
			std::cout<<"In MyFieldSvc::ReadCard, unknown name: "<<keyword<<" in file "<<file_name<<std::endl;
			std::cout<<"Will ignore this line!"<<std::endl;
		}
	}
	fType.toLower();
	fEType.toLower();
	fMType.toLower();
	fin_card.close();
	buf_card.str("");
	buf_card.clear();
}

void MyFieldSvc::Dump(){
	std::cout<<"**********************MagField Settings***************************"<<std::endl;
	std::cout<<"Electric Magnetic Field Type: "<<fType<<std::endl;
	if ( fType == "simple" ){
        if ( fMType == "uniform" ){
            std::cout<<std::setiosflags(std::ios::left)<<std::setw(10) <<"Intensity"
                <<std::setiosflags(std::ios::left)<<std::setw(10) <<"Direction(Theta, Phi)"
                <<std::endl;
            std::cout<<std::setiosflags(std::ios::left)<<std::setw(10)<<"T"
                <<std::setiosflags(std::ios::left)<<std::setw(5) <<"deg"
                <<std::setiosflags(std::ios::left)<<std::setw(5) <<"deg"
                <<std::endl;
            std::cout<<std::setiosflags(std::ios::left)<<std::setw(10)<<UniM_Intensity/tesla
                <<std::setiosflags(std::ios::left)<<std::setw(5) <<UniM_Theta/deg
                <<std::setiosflags(std::ios::left)<<std::setw(5) <<UniM_Phi/deg
                <<std::endl;
        }

        if ( fEType == "uniform" ){
            std::cout<<std::setiosflags(std::ios::left)<<std::setw(10) <<"Intensity"
                <<std::setiosflags(std::ios::left)<<std::setw(20) <<"Dir(Theta,Phi)"
                <<std::setiosflags(std::ios::left)<<std::setw(6)  <<"StepL"
                <<std::setiosflags(std::ios::left)<<std::setw(6)  <<"StepT"
                <<std::endl;
            std::cout<<std::setiosflags(std::ios::left)<<std::setw(10)<<"kV/cm"
                <<std::setiosflags(std::ios::left)<<std::setw(10)<<"deg"
                <<std::setiosflags(std::ios::left)<<std::setw(10)<<"deg"
                <<std::setiosflags(std::ios::left)<<std::setw(6) <<"mm"
                <<std::endl;
            std::cout<<std::setiosflags(std::ios::left)<<std::setw(10)<<UniE_Intensity/kilovolt*cm
                <<std::setiosflags(std::ios::left)<<std::setw(10)<<UniE_Theta/deg
                <<std::setiosflags(std::ios::left)<<std::setw(10)<<UniE_Phi/deg
                <<std::setiosflags(std::ios::left)<<std::setw(6) <<EF_StepL/mm
                <<std::setiosflags(std::ios::left)<<std::setw(6) <<EF_StepT
                <<std::endl;
        }
        else if ( fEType == "cylinder" ){
            std::cout<<std::setiosflags(std::ios::left)<<std::setw(10) <<"VoltageScaled"
                <<std::setiosflags(std::ios::left)<<std::setw(6)  <<"StepL"
                <<std::setiosflags(std::ios::left)<<std::setw(6)  <<"StepT"
                <<std::endl;
            std::cout<<std::setiosflags(std::ios::left)<<std::setw(10)<<"kV"
                <<std::setiosflags(std::ios::left)<<std::setw(6) <<"mm"
                <<std::endl;
            std::cout<<std::setiosflags(std::ios::left)<<std::setw(10)<<CylE_VoltScaled/volt
                <<std::setiosflags(std::ios::left)<<std::setw(6) <<EF_StepL/mm
                <<std::setiosflags(std::ios::left)<<std::setw(6) <<EF_StepT
                <<std::endl;
        }
    }
	else if ( fType == "fieldMap" ){
		for(G4int ifieldmap = 0; ifieldmap < fFieldMapFilenames.size(); ifieldmap++){
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(30) <<fFieldMapFilenames[ifieldmap]<<" "
				<<std::setiosflags(std::ios::left)<<std::setw(6) <<fFieldMapScalings[ifieldmap]
				<<std::endl;
		}
	}
	else if ( fType == "none" ){
		std::cout<<"No Field!"<<std::endl;
	}
	std::cout<<"******************************************************************"<<std::endl;
}
