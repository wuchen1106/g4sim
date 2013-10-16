//---------------------------------------------------------------------------//
//Description: 1. Setup FormulizedGeometry geometry and manage associated sensitive detectors
//             2. Communicate with associated sensitive detectors
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment: about fVerboseLevel:
//         or modifying input card before starting a run.
//         Either way would set verbose of selected sensitive detectors to the same level
//         0:   Only Warnings and Errors
//         1:   Important information
//         >=5: All information
//---------------------------------------------------------------------------//
#include "FormulizedGeometrySvc.hh"

#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4VSensitiveDetector.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4RotationMatrix.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"

#include <iostream>

#include "SimpleGeometrySvc.hh"
#include "FormulizedGeometryParameter.hh"
#include "MyDetectorManager.hh"

FormulizedGeometrySvc::FormulizedGeometrySvc(G4String name, G4String opt )
	: SimpleGeometrySvc(name, "S") // Use a not-empty opt to stop inherited class from creating new GeometryParameter
{
	if ( opt == "" ){//called directly by user. set up parameter class.
		FormulizedGeometryParameter *pointer = new FormulizedGeometryParameter(name);
//		std::cout<<"======>In FormulizedGeometrySvc, new FormulizedGeometryParameter at ("<<(void*)pointer<<")!"<<std::endl;
		set_GeometryParameter(pointer);
	}
}

FormulizedGeometrySvc::~FormulizedGeometrySvc(){
	printf("~FormulizedGeometrySvc");
}

//------------------------Modify-------------------------------
void FormulizedGeometrySvc::set_GeometryParameter( FormulizedGeometryParameter* val ){
	m_GeometryParameter = val;
	SimpleGeometrySvc::set_GeometryParameter(m_GeometryParameter);
}

//--------------------------SetGeometry----------------------------------
//virtual function
G4VPhysicalVolume* FormulizedGeometrySvc::SetGeometry(){
	m_GeometryParameter->Dump();
	ConstructVolumes();
	G4VPhysicalVolume *current = PlaceVolumes();
	return current;
}

//-------------------------Special functions-----------------------------
//=> ConstructVolumes
void FormulizedGeometrySvc::ConstructVolumes(){
	SimpleGeometrySvc::ConstructVolumes();
	bool vis;
	double r, g, b, t;
	std::stringstream buffer;
	G4Material* pttoMaterial;
	G4String name, mat_name, SDName; 
	G4String SolidType;
	G4int SolidIndex;
	G4double PosX, PosY, PosZ;
	G4String motVolName; 
	G4int SRepNo;
	G4int RepNo;
	bool checkOverlap = m_GeometryParameter->get_checkoverlap();
	int nVol = m_GeometryParameter->get_VolNo();
	for ( int i_Vol = 0; i_Vol < nVol; i_Vol++ ){
		std::ostringstream osname;
		SRepNo = m_GeometryParameter->get_SRepNo(i_Vol);
		RepNo = m_GeometryParameter->get_RepNo(i_Vol);
		motVolName = m_GeometryParameter->get_MotherName(i_Vol);
		G4LogicalVolume* log_container = get_logicalVolume(motVolName);
		SolidIndex = m_GeometryParameter->get_SolidIndex(i_Vol);
		SolidType = m_GeometryParameter->get_SolidType(i_Vol);
		SDName = m_GeometryParameter->get_SDName(i_Vol);
		mat_name = m_GeometryParameter->get_material(i_Vol);
		name = m_GeometryParameter->get_name(i_Vol);
		pttoMaterial = G4Material::GetMaterial(mat_name);
		if (!pttoMaterial){
			std::cout<<"Material "<<mat_name<<" is not defined!"<<std::endl;
			G4Exception("FormulizedGeometrySvc::ConstructVolumes()",
					"InvalidSetup", FatalException,
					"unknown material name");
		}
		for ( int i = 0; i < RepNo; i++ ){
			G4LogicalVolume* log_Vol;
			buffer.str("");
			buffer.clear();
			buffer<<name<<"_"<<i;
			G4String iname = buffer.str();
			if ( SolidType == "Box" ){
				G4double halfX, halfY, halfZ;
				halfX = m_GeometryParameter->get_Box_X(SolidIndex,i)/2;
				halfY = m_GeometryParameter->get_Box_Y(SolidIndex,i)/2;
				halfZ = m_GeometryParameter->get_Box_Z(SolidIndex,i)/2;
				G4Box* sol_Box=new G4Box(iname,halfX,halfY,halfZ);
				log_Vol = new G4LogicalVolume(sol_Box, pttoMaterial, iname,0,0,0);
			}
			else if ( SolidType == "Tubs" ){
				G4double RMax, RMin, halfLength, StartAng, SpanAng;
				RMax = m_GeometryParameter->get_Tubs_RMax(SolidIndex,i);
				RMin = m_GeometryParameter->get_Tubs_RMin(SolidIndex,i);
				halfLength = m_GeometryParameter->get_Tubs_Length(SolidIndex,i)/2;
				StartAng = m_GeometryParameter->get_Tubs_StartAng(SolidIndex,i);
				SpanAng = m_GeometryParameter->get_Tubs_SpanAng(SolidIndex,i);
				G4Tubs* sol_Tubs=new G4Tubs(iname,RMin,RMax,halfLength,StartAng,SpanAng);
				log_Vol = new G4LogicalVolume(sol_Tubs, pttoMaterial, iname,0,0,0);
			}
			else if ( SolidType == "Sphere" ){
				G4double RMax, RMin, StartPhi, SpanPhi, StartTheta, SpanTheta;
				RMax = m_GeometryParameter->get_Sphere_RMax(SolidIndex,i);
				RMin = m_GeometryParameter->get_Sphere_RMin(SolidIndex,i);
				StartPhi = m_GeometryParameter->get_Sphere_StartPhi(SolidIndex,i);
				SpanPhi = m_GeometryParameter->get_Sphere_SpanPhi(SolidIndex,i);
				StartTheta = m_GeometryParameter->get_Sphere_StartTheta(SolidIndex,i);
				SpanTheta = m_GeometryParameter->get_Sphere_SpanTheta(SolidIndex,i);
				G4Sphere* sol_Sphere=new G4Sphere(iname,RMin,RMax,StartPhi,SpanPhi,StartTheta,SpanTheta);
				log_Vol = new G4LogicalVolume(sol_Sphere, pttoMaterial, iname,0,0,0);
			}
			else {
				std::cout<<"SolidType "<<SolidType<<" is not supported yet!"<<std::endl;
				G4Exception("FormulizedGeometrySvc::ConstructVolumes()",
						"InvalidSetup", FatalException,
						"unknown SolidType");
			}
			G4VSensitiveDetector* aSD = MyDetectorManager::GetMyDetectorManager()->GetSD(iname, SDName, const_cast<FormulizedGeometrySvc*>(this) ); //This series of volumes will share the same SD object
			log_Vol->SetSensitiveDetector(aSD);
			//visual
			vis = m_GeometryParameter->get_vis(i_Vol);
			if (!vis){
				log_Vol->SetVisAttributes(G4VisAttributes::Invisible);
			}
			else{
				r = m_GeometryParameter->get_r(i_Vol);
				g = m_GeometryParameter->get_g(i_Vol);
				b = m_GeometryParameter->get_b(i_Vol);
				t = m_GeometryParameter->get_t(i_Vol);
				G4VisAttributes* visAttributes = new G4VisAttributes;
				visAttributes->SetVisibility(true);
				visAttributes->SetColour(r,g,b,t);
				log_Vol->SetVisAttributes(visAttributes);
			}
		}
	}
}

//=>Place Volumes
G4VPhysicalVolume* FormulizedGeometrySvc::PlaceVolumes(){
	std::stringstream buffer;
	G4VPhysicalVolume* world_pvol = 0;
	bool checkOverlap = m_GeometryParameter->get_checkoverlap();
	int nVol = m_GeometryParameter->get_VolNo();
	for ( int i_Vol = 0; i_Vol < nVol; i_Vol++ ){
		G4double PosX, PosY, PosZ;
		G4String name, motVolName; 
		G4int SRepNo;
		G4int RepNo;
		name = m_GeometryParameter->get_name(i_Vol);
		motVolName = m_GeometryParameter->get_MotherName(i_Vol);
		SRepNo = m_GeometryParameter->get_SRepNo(i_Vol);
		RepNo = m_GeometryParameter->get_RepNo(i_Vol);
		G4LogicalVolume* log_container;
		if ( name == "World" ) log_container = 0;
		else log_container = get_logicalVolume(motVolName);
		for ( int i = 0; i < RepNo; i++ ){
			buffer.str("");
			buffer.clear();
			buffer<<name<<"_"<<i;
			G4String iname = buffer.str();
			G4LogicalVolume* log_Vol = get_logicalVolume(iname);
			PosX  = m_GeometryParameter->get_PosX(i_Vol,i);
			PosY  = m_GeometryParameter->get_PosY(i_Vol,i);
			PosZ  = m_GeometryParameter->get_PosZ(i_Vol,i);
			G4ThreeVector pos(PosX ,PosY ,PosZ);
			G4double Ephi = m_GeometryParameter->get_Ephi(i_Vol,i);
			G4double Etheta = m_GeometryParameter->get_Etheta(i_Vol,i);
			G4double Epsi = m_GeometryParameter->get_Epsi(i_Vol,i);
			G4RotationMatrix* rotateMatrix=new G4RotationMatrix(Ephi,Etheta,Epsi);
			if ( iname == "World" ) rotateMatrix = 0;
			G4VPhysicalVolume* phy_Vol =
				new G4PVPlacement(rotateMatrix,                //rotation
					pos,                              //position
					log_Vol,                          //its logical volume
					iname,                             //its name
					log_container,                    //its mother volume
					false,                            //no boolean operation
					i+SRepNo,                                //copy number
					checkOverlap);                    //overlaps checking
			if ( iname == "World" ) world_pvol = phy_Vol;
		}
	}
	G4VPhysicalVolume *former = SimpleGeometrySvc::PlaceVolumes();
	if (!world_pvol) world_pvol=former;
	return world_pvol;
}
