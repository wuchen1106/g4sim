//---------------------------------------------------------------------------//
//Description: Manage MySD classes
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "globals.hh"
#include "G4SDManager.hh"
#include "G4DigiManager.hh"

#include <sstream>
#include <fstream>
#include <string.h>
#include <iostream>

//supported SD
#include "CdcSD.hh"
#include "CdcSimpleSD.hh"
#include "CdcIonSD.hh"
#include "MonitorSD.hh"
#include "CdcLayerSD.hh"
#include "KillerSD.hh"

//supported Svc
#include "SimpleGeometrySvc.hh"
#include "CdcGeometrySvc.hh"
#include "FormulizedGeometrySvc.hh"

#include "MyVGeometrySvc.hh"
#include "MyVGeometryParameter.hh"

#include "MyDetectorManagerMessenger.hh"
#include "MyDetectorManager.hh"

#include "DEBUG.hh"

MyDetectorManager* MyDetectorManager::fMyDetectorManager = 0;

MyDetectorManager::MyDetectorManager()
	:fVerboseLevel(0)
{
  if (fMyDetectorManager){
    G4Exception("MyDetectorManager::MyDetectorManager()","Run0031",
        FatalException, "MyDetectorManager constructed twice.");
  }
  fMyDetectorManager = this;
	fSDman = G4SDManager::GetSDMpointer();
	if (!fSDman) DEBUG("Can not find G4SDManager.");
  m_MyDetectorManagerMessenger = new MyDetectorManagerMessenger(this);
}

MyDetectorManager::~MyDetectorManager()
{
	delete m_MyDetectorManagerMessenger;
}

MyDetectorManager* MyDetectorManager::GetMyDetectorManager(){
  if ( !fMyDetectorManager ){
    fMyDetectorManager = new MyDetectorManager;
  }
  return fMyDetectorManager;
}

//***************************************ReadCard********************************************
//OBJECTIVE: Call recorded geometry service objects recursively to read input file for geometry settings
//PROCEDURE:
//0. delete existing geometry service objects
//1. read $CONFIGUREROOT/file_name(or file_name), decide how many services are needed and what files they need.
//2. create(or modify if exist) geometry service objects, push to vector, and call SetGeometry recursively with those file names.
//NOTICE.1: file_name here should either be a complete file name with path or a pure file name w.r.t $CONFIGUREROOT,
//  cause MyVGeometry calsses would prepend $CONFIGUREROOT to file_name if not found '/' in it.
void MyDetectorManager::ReadCard(G4String file_name){
	//delete and clear
	for ( int i = 0; i < fMyVGeometrySvc.size(); i++ ){
		if (fMyVGeometrySvc[i]) delete (fMyVGeometrySvc[i]);
	}
	fMyVGeometrySvc.clear();
	fSvcName.clear();

	//get the file
	size_t sLast = file_name.last('/');
	if(sLast==std::string::npos){ // file name only
		G4String dir_name = getenv("CONFIGUREROOT");
		if (dir_name[dir_name.size()-1] != '/') dir_name.append("/");
		file_name = dir_name + file_name;
	}
	//std::cout<<"MyDetectorManager: Reading \""<<file_name<<"\""<<std::endl;
  std::ifstream fin_card(file_name);
  if ( !fin_card ){
    std::cout<<"Geometry file: "<<file_name<<" is not available!!!"<<std::endl;
    G4Exception("MyDetectorManager::ReadCard()","Run0031",
        FatalException, "Geometry file is not available.");
  }

	//read line by line
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

		G4String name, type, file;
		MyVGeometrySvc* aGSvc = 0;
		buf_card>>name;
		//std::cout<<"MyDetectorManager: "<<buf_card.str()<<std::endl;
		if ( name == "VerboseLevel" ){
			buf_card>>fVerboseLevel;
		}
		else{
			buf_card>>type>>file;
			if( type == "Simple" ){
				aGSvc = new SimpleGeometrySvc( name );
			}
			else if( type == "Cdc" ){
				aGSvc = new CdcGeometrySvc( name );
			}
			else if( type == "Formulized" ){
				aGSvc = new FormulizedGeometrySvc( name );
			}
			else{
				std::cout<<"In MyDetectorManager::ReadCard, unsupported GeometrySvc type: "<<type<<"! Will ignore this line!"<<std::endl;
			}
		}
		if (aGSvc){
			aGSvc->set_VerboseLevel(fVerboseLevel);
			//push and call ReadCard
			fMyVGeometrySvc.push_back(aGSvc);
			fSvcName.push_back(name);
			aGSvc->ReadCard(file);
		}
	}
}

//***************************************SetGeometry********************************************
//OBJECTIVE: Call recorded geometry service objects recursively to setup geometry
//NOTICE.1: a return value is needed as the pointer to the physical volume of world.
//  According to conventions, this pointer would be returned by the first geometry service object.
G4VPhysicalVolume* MyDetectorManager::SetGeometry(){
	G4VPhysicalVolume* vol_world = (fMyVGeometrySvc[0])->SetGeometry();
	for ( int i = 1; i < fMyVGeometrySvc.size(); i++ ){
		(fMyVGeometrySvc[i])->SetGeometry();
	}
	return vol_world;
}

//**************************************ReadOutputCard********************************************
//OBJECTIVE: Call recorded SD objects recursively to read the input file for output customising.
//NOTICE.1: file_name here should either be a complete file name with path or a pure file name w.r.t $CONFIGUREROOT,
//  cause MySD calsses would prepend $CONFIGUREROOT to file_name if not found '/' in it.
void MyDetectorManager::ReadOutputCard( G4String file_name ){
	for ( int i = 0; i < fSDList.size(); i++ ){
		fSDList[i]->ReadOutputCard(file_name);
	}
}

//*****************************************SetBranch**********************************************
//OBJECTIVE: Call recorded SD objects recursively to setup branches.
void MyDetectorManager::SetBranch(){
	for ( int i = 0; i < fSDList.size(); i++ ){
		fSDList[i]->SetBranch();
	}
}

//************************GetSD*********************************
//OBJECTIVE: Get the wanted SD. Create if not exists.
//VolName: name of logical volume.
//SDName: type of sensitive detector.
//        NOTICE: if a '/' is found in SDName, then SDName would be considered as "VolName/SDName"
//                and the parameter VolName would be ignored.
//pointer: the pointer to the geometry service object which managed this volume.
//PROCEDURE:
//1. Search for (VolName, SDName).
//2. If this pair exists, then return the corresponding G4VSensitiveDetector*
//   If not, then record this pair, new a G4VSensitiveDetector according to the type(SDName),
//     push the pointer to a vector, and return the pointer.
//NOTICE.1: All sensitive detector classes used here are assumed to inherit from both 
//  G4VSensitiveDetector and MySD, so mind the constructor and set_GeometryParameter
//NOTICE.2: Here I use VolName + SDName as the name of SD object to allow different volumes to use
//  the same type of SD while keeping different copies( In my case, they would create different branches in output file).
G4VSensitiveDetector* MyDetectorManager::GetSD(G4String VolName, G4String SDName, MyVGeometrySvc* pointer){
	MYDETM_LINEINFO()
	MyVGeometryParameter* pPara = pointer->get_GeometryParameter();

	//Safety check
	int index = -1;
	for ( int i = 0; i < fMyVGeometrySvc.size(); i++ ){
		if ( (void*) pointer == (void*) fMyVGeometrySvc[i]) index = i;
	}
	if (index == -1){
		std::cout<<"WRONG!!! unrecorded MyVGeometrySvc pointer!!!"<<std::endl;
    G4Exception("MyDetectorManager::GetSD()","Run0031",
        FatalException, "unrecorded MyVGeometrySvc pointer.");
	}

	//check SDName
	G4String newVolName;
	G4String newSDName;
	size_t sLast = SDName.last('/');
	if(sLast==std::string::npos){ // Sensitive Detector name only
		newVolName = VolName;
		newSDName = SDName;
	}
	else{
		newVolName = SDName.substr(0,sLast);
		newSDName = SDName.substr(sLast+1,SDName.length()-sLast-1);
	}

	//Search for (VolName, SDName).
	MYDETM_LINEINFO()
	MySD* aMySD = 0;
	for ( int i = 0; i < fSDList.size(); i++ ){
		if ( fSDName[i] == newSDName ){
			if ( fVolName[i] == newVolName ){
				aMySD = fSDList[i];
				if ( !aMySD ) DEBUG("An element of fSDList is zero!"); //G4Exception would be called
				//Found it. Modify
				aMySD->set_GeometryParameter(pPara);
				break;
			}
		}
	}
	MYDETM_LINEINFO()

	//if not found, then create a new one w.r.t SDName
	G4VSensitiveDetector* aG4SD = 0;
	if ( !aMySD ){
		G4String FullSDName = newVolName + "/" + newSDName;
		if ( newSDName == "CdcSD" ){
			aG4SD = new CdcSD( FullSDName, pPara );
			fSDman->AddNewDetector( aG4SD );
		}
		else if ( newSDName == "CdcSimpleSD" ){
			aG4SD = new CdcSimpleSD( FullSDName, pPara );
			fSDman->AddNewDetector( aG4SD );
		}
		else if ( newSDName == "KillerSD" ){
			aG4SD = new KillerSD( FullSDName, pPara );
			fSDman->AddNewDetector( aG4SD );
		}
		else if ( newSDName == "CdcIonSD" ){
			aG4SD = new CdcIonSD( FullSDName, pPara );
			fSDman->AddNewDetector( aG4SD );
		}
		else if ( newSDName == "MonitorSD" ){
			aG4SD = new MonitorSD( FullSDName, pPara );
			fSDman->AddNewDetector( aG4SD );
		}
		else if ( newSDName == "CdcLayerSD" ){
			aG4SD = new CdcLayerSD( FullSDName, pPara );
			fSDman->AddNewDetector( aG4SD );
		}
		else if ( newSDName != "none" ) {//"none" means the user does not want to use a SD. aSD set to 0 and will not be pushed to vecto
			DEBUG("Sensitive detector type " + newSDName + " is not supported yet!!!");
		}

		if ( aG4SD ){
			MySD* aSD = dynamic_cast<MySD*> (aG4SD);
			aSD->SetVerbose(fVerboseLevel);
			if ( aSD == NULL ){
				G4String content = "Cannot convert "+newSDName+" from G4VSensitiveDetector* to MySD* at construction";
				DEBUG(content);
			}
			fSDList.push_back(aSD);
			fVolName.push_back(newVolName);
			fSDName.push_back(newSDName);
		}
	}
	MYDETM_LINEINFO()
	else {
		aMySD->SetVerbose(fVerboseLevel);
		aG4SD = dynamic_cast<G4VSensitiveDetector*> (aMySD);
		if ( aG4SD == NULL ){
			G4String content = "Cannot convert "+newSDName+" from G4VSensitiveDetector* to MySD* at construction";
			DEBUG(content);
		}
	}
	MYDETM_LINEINFO()

	return aG4SD;
}

G4VSensitiveDetector* MyDetectorManager::GetSD(G4String VolName, G4String SDName){

	//check SDName
	G4String newVolName;
	G4String newSDName;
	size_t sLast = SDName.last('/');
	if(sLast==std::string::npos){ // Sensitive Detector name only
		newVolName = VolName;
		newSDName = SDName;
	}
	else{
		newVolName = SDName.substr(0,sLast);
		newSDName = SDName.substr(sLast+1,SDName.length()-sLast-1);
	}

	//Search for (VolName, SDName).
	MySD* aMySD = 0;
	//std::cout<<"Searching for ["<<newVolName<<","<<newSDName<<"]"<<std::endl;
	for ( int i = 0; i < fSDList.size(); i++ ){
		//std::cout<<"["<<fVolName[i]<<","<<fSDName[i]<<"]"<<std::endl;
		if ( fSDName[i] == newSDName ){
			//std::cout<<"VolName Matched!"<<std::endl;
			if ( fVolName[i] == newVolName ){
				//std::cout<<"SDName Matched!"<<std::endl;
				aMySD = fSDList[i];
				if ( !aMySD ) DEBUG("An element of fSDList is zero!"); //G4Exception would be called
				break;
			}
		}
	}

	//if not found ...
	G4VSensitiveDetector* aG4SD = 0;
	if ( !aMySD ){
	}
	else {
		aG4SD = dynamic_cast<G4VSensitiveDetector*> (aMySD);
		if ( aG4SD == NULL ){
			G4String content = "Cannot convert "+newSDName+" from G4VSensitiveDetector* to MySD* at construction";
			DEBUG(content);
		}
	}

	return aG4SD;
}

//**************************************RegistorDM************************************************
//OBJECTIVE: For digitizing control.
//Not well supported yet...
void MyDetectorManager::RegistorDM( G4String DMName ){
	if ( DMName != "none" ) fDMName.push_back(DMName);
}

//**************************************Digitize************************************************
//OBJECTIVE: For digitizing control.
//Not well supported yet...
void MyDetectorManager::Digitize(){
	for ( int i = 0; i < fDMName.size(); i++ ){
		G4String DMName = fDMName[i];
		G4DigiManager::GetDMpointer()->Digitize(DMName);
	}
}

//**************************************DEBUG************************************************
//OBJECTIVE: For debugging.
void MyDetectorManager::DEBUG(G4String content){
	G4Exception("MyDetectorManager","Run0031",
			FatalException, content);
}
