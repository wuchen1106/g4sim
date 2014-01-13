//---------------------------------------------------------------------------//
//Description: 1. Setup MyVGeometry geometry and manage associated sensitive detectors
//             2. Communicate with associated sensitive detectors
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment: about fVerboseLevel:
//         You can change it by calling SetVerbose(int) somewhere
//         or modifying input card before starting a run.
//         Either way would set verbose of selected sensitive detectors to the same level
//         0:   Only Warnings and Errors
//         1:   Important information
//         >=5: All information
//---------------------------------------------------------------------------//
#include "MyVGeometrySvc.hh"

#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"

#include <string>
#include <iostream>
//#include <unistd.h>

#include "MyVGeometryParameter.hh"

MyVGeometrySvc::MyVGeometrySvc(G4String name, G4String opt )
	:MyConfigure()
{
	if ( opt == "" ){
		std::cout<<"MyVGeometrySvc is a virtual class, should be called with opt not empty!"<<std::endl;
    G4Exception("MyVGeometrySvc::MyVGeometrySvc()","Run0031",
        FatalException, "illegal construction.");
	}
	set_Name(name);
}

MyVGeometrySvc::~MyVGeometrySvc(){
	std::cout<<"======>In ~MyVGeometrySvc, delete GeometryParameter at ("<<(void*)m_GeometryParameter<<")!"<<std::endl;
	delete m_GeometryParameter;
}

void MyVGeometrySvc::ReadCard( G4String file_name ){
	size_t sLast = file_name.last('/');
	if(sLast==std::string::npos){ // file name only
		G4String dir_name = getenv("CONFIGUREROOT");
		if (dir_name[dir_name.size()-1] != '/') dir_name.append("/");
		file_name = dir_name + file_name;
	}
	m_GeometryParameter->InitFromFile(file_name);
}

G4LogicalVolume* MyVGeometrySvc::get_logicalVolume(G4String name){
	//get mother_volume
  G4LogicalVolumeStore* logicalVolumeStore = G4LogicalVolumeStore::GetInstance();
  G4LogicalVolume* log_volume = logicalVolumeStore->GetVolume(name,false);
  if (!log_volume){
    std::cout<<"Logical volume "<<name<<" of MyVGeometry can not be found!!!"<<std::endl;
    G4Exception("MyVGeometrySvc::get_logicalVolume()","Run0031",
        FatalException, "unknown logical volume.");
  }
  return log_volume;
}
