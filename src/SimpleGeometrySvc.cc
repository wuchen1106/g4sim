//---------------------------------------------------------------------------//
//Description: 1. Setup SimpleGeometry geometry and manage associated sensitive detectors
//             2. Communicate with associated sensitive detectors
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Modified: 11, Jan, 2013
//          Support G4Hype, G4TwistedTube
//Comment: about fVerboseLevel:
//         or modifying input card before starting a run.
//         Either way would set verbose of selected sensitive detectors to the same level
//         0:   Only Warnings and Errors
//         1:   Important information
//         >=5: All information
//---------------------------------------------------------------------------//
#include "SimpleGeometrySvc.hh"

#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4VSensitiveDetector.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Hype.hh"
#include "G4TwistedTubs.hh"
#include "G4Cons.hh"
#include "G4Sphere.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4VisAttributes.hh"

#include <iostream>

#include "MyVGeometrySvc.hh"
#include "SimpleGeometryParameter.hh"
#include "MyDetectorManager.hh"

SimpleGeometrySvc::SimpleGeometrySvc(G4String name, G4String opt )
	: MyVGeometrySvc(name, "S") // Use a not-empty opt to stop inherited class from creating new GeometryParameter
{
	if ( opt == "" ){//called directly by user. set up parameter class.
		SimpleGeometryParameter *pointer = new SimpleGeometryParameter(name);
//		std::cout<<"======>In SimpleGeometrySvc, new SimpleGeometryParameter at ("<<(void*)pointer<<")!"<<std::endl;
		set_GeometryParameter(pointer);
	}
}

SimpleGeometrySvc::~SimpleGeometrySvc(){
	printf("~SimpleGeometrySvc");
}

//------------------------Modify-------------------------------
void SimpleGeometrySvc::set_GeometryParameter( SimpleGeometryParameter* val ){
	m_GeometryParameter = val;
	MyVGeometrySvc::set_GeometryParameter(m_GeometryParameter);
}

//--------------------------SetGeometry----------------------------------
//virtual function
G4VPhysicalVolume* SimpleGeometrySvc::SetGeometry(){
	m_GeometryParameter->Dump();
	MyVGeometrySvc::ConstructVolumes();
	ConstructVolumes();
	G4VPhysicalVolume *current = PlaceVolumes();
	G4VPhysicalVolume *former = MyVGeometrySvc::PlaceVolumes();
	return ( !current? former : current );
}

//-------------------------Special functions-----------------------------
//=> ConstructVolumes
void SimpleGeometrySvc::ConstructVolumes(){
	bool vis;
	double r, g, b;
	G4Material* pttoMaterial;

	int nVol = m_GeometryParameter->get_VolNo();
	for ( int i_Vol = 0; i_Vol < nVol; i_Vol++ ){
		G4String name, mat_name, SDName; 
		G4String SolidType;
		G4int SolidIndex;
		SolidIndex = m_GeometryParameter->get_SolidIndex(i_Vol);
		SolidType = m_GeometryParameter->get_SolidType(i_Vol);
		SDName = m_GeometryParameter->get_SDName(i_Vol);
		mat_name = m_GeometryParameter->get_material(i_Vol);
		name = m_GeometryParameter->get_name(i_Vol);
		pttoMaterial = G4Material::GetMaterial(mat_name);
		if (!pttoMaterial){
			std::cout<<"Material "<<mat_name<<" is not defined!"<<std::endl;
			G4Exception("SimpleGeometrySvc::ConstructVolumes()",
					"InvalidSetup", FatalException,
					"unknown material name");
		}
		G4LogicalVolume* log_Vol;
		if ( SolidType == "Box" ){
			G4double halfX, halfY, halfZ;
			halfX = m_GeometryParameter->get_Box_X(SolidIndex)/2;
			halfY = m_GeometryParameter->get_Box_Y(SolidIndex)/2;
			halfZ = m_GeometryParameter->get_Box_Z(SolidIndex)/2;
			G4Box* sol_Box=new G4Box(name,halfX,halfY,halfZ);
			log_Vol = new G4LogicalVolume(sol_Box, pttoMaterial, name,0,0,0);
		}
		else if ( SolidType == "Tubs" ){
			G4double RMax, RMin, halfLength, StartAng, SpanAng;
			RMax = m_GeometryParameter->get_Tubs_RMax(SolidIndex);
			RMin = m_GeometryParameter->get_Tubs_RMin(SolidIndex);
			halfLength = m_GeometryParameter->get_Tubs_Length(SolidIndex)/2;
			StartAng = m_GeometryParameter->get_Tubs_StartAng(SolidIndex);
			SpanAng = m_GeometryParameter->get_Tubs_SpanAng(SolidIndex);
			G4Tubs* sol_Tubs=new G4Tubs(name,RMin,RMax,halfLength,StartAng,SpanAng);
			log_Vol = new G4LogicalVolume(sol_Tubs, pttoMaterial, name,0,0,0);
		}
		else if ( SolidType == "Sphere" ){
			G4double RMax, RMin, StartPhi, SpanPhi, StartTheta, SpanTheta;
			RMax = m_GeometryParameter->get_Sphere_RMax(SolidIndex);
			RMin = m_GeometryParameter->get_Sphere_RMin(SolidIndex);
			StartPhi = m_GeometryParameter->get_Sphere_StartPhi(SolidIndex);
			SpanPhi = m_GeometryParameter->get_Sphere_SpanPhi(SolidIndex);
			StartTheta = m_GeometryParameter->get_Sphere_StartTheta(SolidIndex);
			SpanTheta = m_GeometryParameter->get_Sphere_SpanTheta(SolidIndex);
			G4Sphere* sol_Sphere=new G4Sphere(name,RMin,RMax,StartPhi,SpanPhi,StartTheta,SpanTheta);
			log_Vol = new G4LogicalVolume(sol_Sphere, pttoMaterial, name,0,0,0);
		}
		else if ( SolidType == "Hype" ){
			G4double innerRadius, outerRadius, halfLength, innerStereo, outerStereo;
			innerRadius = m_GeometryParameter->get_Hype_innerRadius(SolidIndex);
			outerRadius = m_GeometryParameter->get_Hype_outerRadius(SolidIndex);
			halfLength = m_GeometryParameter->get_Hype_Length(SolidIndex)/2;
			innerStereo = m_GeometryParameter->get_Hype_innerStereo(SolidIndex);
			outerStereo = m_GeometryParameter->get_Hype_outerStereo(SolidIndex);
			G4Hype* sol_Hype=new G4Hype(name,innerRadius,outerRadius,innerStereo,outerStereo,halfLength);
			log_Vol = new G4LogicalVolume(sol_Hype, pttoMaterial, name,0,0,0);
		}
		else if ( SolidType == "TwistedTubs" ){
			G4double endinnerrad, endouterrad, halfLength, twistedangle, dphi;
			twistedangle = m_GeometryParameter->get_TwistedTubs_twistedangle(SolidIndex);
			endinnerrad = m_GeometryParameter->get_TwistedTubs_endinnerrad(SolidIndex);
			endouterrad = m_GeometryParameter->get_TwistedTubs_endouterrad(SolidIndex);
			halfLength = m_GeometryParameter->get_TwistedTubs_Length(SolidIndex)/2;
			dphi = m_GeometryParameter->get_TwistedTubs_dphi(SolidIndex);
			G4TwistedTubs* sol_TwistedTubs=new G4TwistedTubs(name,twistedangle,endinnerrad,endouterrad,halfLength,dphi);
			log_Vol = new G4LogicalVolume(sol_TwistedTubs, pttoMaterial, name,0,0,0);
		}
		else if ( SolidType == "Cons" ){
			G4double RMax1, RMin1, RMax2, RMin2, halfLength, StartAng, SpanAng;
			RMax1 = m_GeometryParameter->get_Cons_RMax1(SolidIndex);
			RMin1 = m_GeometryParameter->get_Cons_RMin1(SolidIndex);
			RMax2 = m_GeometryParameter->get_Cons_RMax2(SolidIndex);
			RMin2 = m_GeometryParameter->get_Cons_RMin2(SolidIndex);
			halfLength = m_GeometryParameter->get_Cons_Length(SolidIndex)/2;
			StartAng = m_GeometryParameter->get_Cons_StartAng(SolidIndex);
			SpanAng = m_GeometryParameter->get_Cons_SpanAng(SolidIndex);
			G4Cons* sol_Cons=new G4Cons(name,RMin1,RMax1,RMin2,RMax2,halfLength,StartAng,SpanAng);
			log_Vol = new G4LogicalVolume(sol_Cons, pttoMaterial, name,0,0,0);
		}
		else {
			std::cout<<"SolidType "<<SolidType<<" is not supported yet!"<<std::endl;
			G4Exception("SimpleGeometrySvc::ConstructVolumes()",
					"InvalidSetup", FatalException,
					"unknown SolidType");
		}
		G4VSensitiveDetector* aSD = MyDetectorManager::GetMyDetectorManager()->GetSD(name, SDName, const_cast<SimpleGeometrySvc*>(this) );
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
			G4VisAttributes* visAttributes = new G4VisAttributes;
			visAttributes->SetVisibility(true);
			visAttributes->SetColour(r,g,b);
			log_Vol->SetVisAttributes(visAttributes);
		}
	}
}

//=>Place Volumes
G4VPhysicalVolume* SimpleGeometrySvc::PlaceVolumes(){
	G4VPhysicalVolume* world_pvol = 0;
	bool checkOverlap = m_GeometryParameter->get_checkoverlap();
	int nVol = m_GeometryParameter->get_VolNo();
	for ( int i_Vol = 0; i_Vol < nVol; i_Vol++ ){
		G4double PosX, PosY, PosZ;
		G4String name, motVolName; 
		G4double Space, DirTheta, DirPhi;
		G4int SRepNo;
		G4int RepNo;
		PosX  = m_GeometryParameter->get_PosX(i_Vol);
		PosY  = m_GeometryParameter->get_PosY(i_Vol);
		PosZ  = m_GeometryParameter->get_PosZ(i_Vol);
		name = m_GeometryParameter->get_name(i_Vol);
		motVolName = m_GeometryParameter->get_MotherName(i_Vol);
		SRepNo = m_GeometryParameter->get_SRepNo(i_Vol);
		RepNo = m_GeometryParameter->get_RepNo(i_Vol);
		Space = m_GeometryParameter->get_Space(i_Vol);
		DirTheta = m_GeometryParameter->get_DirTheta(i_Vol);
		DirPhi = m_GeometryParameter->get_DirPhi(i_Vol);
		G4LogicalVolume* log_Vol = get_logicalVolume(name);
		G4LogicalVolume* log_container;
		if ( name == "World" ) log_container = 0;
		else log_container = get_logicalVolume(motVolName);
		for ( int i = 0; i < RepNo; i++ ){
			G4ThreeVector pos(PosX ,PosY ,PosZ);
			G4ThreeVector dir(1, 0, 0);
			dir.setTheta(DirTheta);
			dir.setPhi(DirPhi);
			dir.setMag(Space);
			pos += dir * (2*i - RepNo + 1)/2.;
			G4double Ephi = m_GeometryParameter->get_Ephi(i_Vol);
			G4double Etheta = m_GeometryParameter->get_Etheta(i_Vol);
			G4double Epsi = m_GeometryParameter->get_Epsi(i_Vol);
			G4RotationMatrix* rotateMatrix=new G4RotationMatrix(Ephi,Etheta,Epsi);
			if ( name == "World" ) rotateMatrix = 0;
			G4VPhysicalVolume* phy_Vol =
				new G4PVPlacement(rotateMatrix,                //rotation
					pos,                              //position
					log_Vol,                          //its logical volume
					name,                             //its name
					log_container,                    //its mother volume
					false,                            //no boolean operation
					i+SRepNo,                                //copy number
					checkOverlap);                    //overlaps checking
			if ( name == "World" ) world_pvol = phy_Vol;
		}
	}
	return world_pvol;
}
