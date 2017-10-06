//---------------------------------------------------------------------------//
//Description: Geant4 Class
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "DetectorConstruction.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4UserLimits.hh"

#include "DetectorMessenger.hh"
#include "MaterialSvc.hh"
#include "MyFieldSvc.hh"
#include "MyDetectorManager.hh"

#include <stdlib.h>

DetectorConstruction::DetectorConstruction(){
  detectorMessenger = new DetectorMessenger(this);
	pMyFieldSvc = MyFieldSvc::GetMyFieldSvc();
	pMyDetectorManager = MyDetectorManager::GetMyDetectorManager();
}

DetectorConstruction::~DetectorConstruction(){
  delete detectorMessenger;
}

G4VPhysicalVolume* DetectorConstruction::Construct(){
	G4String CardName;

	//=>Material
	CardName = getenv("MATERIALLISTROOT");
	MaterialSvc* materialSvc = MaterialSvc::GetMaterialSvc();
	materialSvc->SetMaterial(CardName);

	//=>MagField
	CardName = getenv("MAGFIELDCARDROOT");
	pMyFieldSvc->ReadCard( CardName );

	//=>Geometry
	CardName = getenv("GEOCARDROOT");
	pMyDetectorManager->ReadCard( CardName );

	//=>ConstructDetector
  return ConstructDetector();
}

G4VPhysicalVolume* DetectorConstruction::ConstructDetector()
{
	// Clean old geometry, if any
	G4GeometryManager::GetInstance()->OpenGeometry();
	G4PhysicalVolumeStore::GetInstance()->Clean();
	G4LogicalVolumeStore::GetInstance()->Clean();
	G4SolidStore::GetInstance()->Clean();

	// Geometry
	world_pvol = pMyDetectorManager->SetGeometry();

	//MagField
	G4LogicalVolume* world_lvol = world_pvol->GetLogicalVolume();
	pMyFieldSvc->SetField(world_lvol);

	// reset userlimit
	world_pvol->GetLogicalVolume()->SetUserLimits(new G4UserLimits());

	//always return the physical world
	return world_pvol;
}

#include "G4RunManager.hh"

void DetectorConstruction::ReloadGeo(G4String file_name){
	pMyDetectorManager->ClearGeo();
	pMyDetectorManager->ReadCard(file_name);
	UpdateGeometry();
}

void DetectorConstruction::ResetMag(G4String file_name){
	pMyFieldSvc->ReadCard(file_name);
	pMyFieldSvc->SetField(world_pvol->GetLogicalVolume());
}

void DetectorConstruction::UpdateGeometry()
{
	G4RunManager::GetRunManager()->DefineWorldVolume(ConstructDetector());
}
