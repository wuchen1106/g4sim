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
#include "G4EllipticalTube.hh"
#include "G4Tubs.hh"
#include "G4Torus.hh"
#include "G4Hype.hh"
#include "G4TwistedTubs.hh"
#include "G4Cons.hh"
#include "G4Polycone.hh"
#include "G4Sphere.hh"
#include "G4IntersectionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4SolidStore.hh"
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
	ConstructVolumes();
	G4VPhysicalVolume *current = PlaceVolumes();
	return current;
}

//-------------------------Special functions-----------------------------
//=> ConstructVolumes
void SimpleGeometrySvc::ConstructVolumes(){
	MyVGeometrySvc::ConstructVolumes();
	bool vis;
	double r, g, b, t;
	std::stringstream buffer;
	G4Material* pttoMaterial;

	int nVol = m_GeometryParameter->get_VolNo();
	for ( int i_Vol = 0; i_Vol < nVol; i_Vol++ ){
		G4String name, mat_name, SDName; 
		G4String SolidType;
		G4int SolidIndex;
		G4int SRepNo;
		G4int RepNo;
		SolidIndex = m_GeometryParameter->get_SolidIndex(i_Vol);
		SolidType = m_GeometryParameter->get_SolidType(i_Vol);
		name = m_GeometryParameter->get_name(i_Vol);
		SRepNo = m_GeometryParameter->get_SRepNo(i_Vol);
		RepNo = m_GeometryParameter->get_RepNo(i_Vol);
		for ( int i = 0; i < RepNo; i++ ){
			G4VSolid* sol_Vol = 0;
			G4String iname = name;
			if (RepNo>1){
				buffer.str("");
				buffer.clear();
				buffer<<name<<"_"<<i;
				iname = buffer.str();
			}
			if ( SolidType == "Box" ){
				G4double halfX, halfY, halfZ;
				halfX = m_GeometryParameter->get_Box_X(SolidIndex,i)/2;
				halfY = m_GeometryParameter->get_Box_Y(SolidIndex,i)/2;
				halfZ = m_GeometryParameter->get_Box_Z(SolidIndex,i)/2;
				sol_Vol=new G4Box(iname,halfX,halfY,halfZ);
			}
			else if ( SolidType == "EllipticalTube" ){
				G4double halfX, halfY, halfZ;
				halfX = m_GeometryParameter->get_EllipticalTube_X(SolidIndex,i)/2;
				halfY = m_GeometryParameter->get_EllipticalTube_Y(SolidIndex,i)/2;
				halfZ = m_GeometryParameter->get_EllipticalTube_Z(SolidIndex,i)/2;
				sol_Vol=new G4EllipticalTube(iname,halfX,halfY,halfZ);
			}
			else if ( SolidType == "Tubs" ){
				G4double RMax, RMin, halfLength, StartAng, SpanAng;
				RMax = m_GeometryParameter->get_Tubs_RMax(SolidIndex,i);
				RMin = m_GeometryParameter->get_Tubs_RMin(SolidIndex,i);
				halfLength = m_GeometryParameter->get_Tubs_Length(SolidIndex,i)/2;
				StartAng = m_GeometryParameter->get_Tubs_StartAng(SolidIndex,i);
				SpanAng = m_GeometryParameter->get_Tubs_SpanAng(SolidIndex,i);
				sol_Vol=new G4Tubs(iname,RMin,RMax,halfLength,StartAng,SpanAng);
			}
			else if ( SolidType == "Torus" ){
				G4double RMax, RMin, Rtor, StartAng, SpanAng;
				RMax = m_GeometryParameter->get_Torus_RMax(SolidIndex,i);
				RMin = m_GeometryParameter->get_Torus_RMin(SolidIndex,i);
				Rtor = m_GeometryParameter->get_Torus_Rtor(SolidIndex,i);
				StartAng = m_GeometryParameter->get_Torus_StartAng(SolidIndex,i);
				SpanAng = m_GeometryParameter->get_Torus_SpanAng(SolidIndex,i);
				sol_Vol=new G4Torus(iname,RMin,RMax,Rtor,StartAng,SpanAng);
			}
			else if ( SolidType == "Sphere" ){
				G4double RMax, RMin, StartPhi, SpanPhi, StartTheta, SpanTheta;
				RMax = m_GeometryParameter->get_Sphere_RMax(SolidIndex,i);
				RMin = m_GeometryParameter->get_Sphere_RMin(SolidIndex,i);
				StartPhi = m_GeometryParameter->get_Sphere_StartPhi(SolidIndex,i);
				SpanPhi = m_GeometryParameter->get_Sphere_SpanPhi(SolidIndex,i);
				StartTheta = m_GeometryParameter->get_Sphere_StartTheta(SolidIndex,i);
				SpanTheta = m_GeometryParameter->get_Sphere_SpanTheta(SolidIndex,i);
				sol_Vol=new G4Sphere(iname,RMin,RMax,StartPhi,SpanPhi,StartTheta,SpanTheta);
			}
			else if ( SolidType == "Hype" ){
				G4double innerRadius, outerRadius, halfLength, innerStereo, outerStereo;
				innerRadius = m_GeometryParameter->get_Hype_innerRadius(SolidIndex,i);
				outerRadius = m_GeometryParameter->get_Hype_outerRadius(SolidIndex,i);
				halfLength = m_GeometryParameter->get_Hype_Length(SolidIndex,i)/2;
				innerStereo = m_GeometryParameter->get_Hype_innerStereo(SolidIndex,i);
				outerStereo = m_GeometryParameter->get_Hype_outerStereo(SolidIndex,i);
				sol_Vol=new G4Hype(iname,innerRadius,outerRadius,innerStereo,outerStereo,halfLength);
			}
			else if ( SolidType == "TwistedTubs" ){
				G4double endinnerrad, endouterrad, halfLength, twistedangle, dphi;
				twistedangle = m_GeometryParameter->get_TwistedTubs_twistedangle(SolidIndex,i);
				endinnerrad = m_GeometryParameter->get_TwistedTubs_endinnerrad(SolidIndex,i);
				endouterrad = m_GeometryParameter->get_TwistedTubs_endouterrad(SolidIndex,i);
				halfLength = m_GeometryParameter->get_TwistedTubs_Length(SolidIndex,i)/2;
				dphi = m_GeometryParameter->get_TwistedTubs_dphi(SolidIndex,i);
				sol_Vol=new G4TwistedTubs(iname,twistedangle,endinnerrad,endouterrad,halfLength,dphi);
			}
			else if ( SolidType == "Cons" ){
				G4double RMax1, RMin1, RMax2, RMin2, halfLength, StartAng, SpanAng;
				RMax1 = m_GeometryParameter->get_Cons_RMax1(SolidIndex,i);
				RMin1 = m_GeometryParameter->get_Cons_RMin1(SolidIndex,i);
				RMax2 = m_GeometryParameter->get_Cons_RMax2(SolidIndex,i);
				RMin2 = m_GeometryParameter->get_Cons_RMin2(SolidIndex,i);
				halfLength = m_GeometryParameter->get_Cons_Length(SolidIndex,i)/2;
				StartAng = m_GeometryParameter->get_Cons_StartAng(SolidIndex,i);
				SpanAng = m_GeometryParameter->get_Cons_SpanAng(SolidIndex,i);
				sol_Vol=new G4Cons(iname,RMin1,RMax1,RMin2,RMax2,halfLength,StartAng,SpanAng);
			}
			else if ( SolidType == "Polycone" ){
				G4double StartAng, SpanAng;
				G4int numZ;
				numZ = m_GeometryParameter->get_Polycone_numZ(SolidIndex,i);
				G4double *RMax = new G4double[numZ];
				G4double *RMin = new G4double[numZ];
				G4double *Z = new G4double[numZ];
				for ( int i = 0; i < numZ; i++ ){
					RMax[i] = m_GeometryParameter->get_Polycone_RMax(SolidIndex,i);
					RMin[i] = m_GeometryParameter->get_Polycone_RMin(SolidIndex,i);
					Z[i] = m_GeometryParameter->get_Polycone_Z(SolidIndex,i);
					std::cout<<i<<": RMax = "<<RMax[i]/mm<<"mm, RMin = "<<RMin[i]/mm<<"mm, "<<Z[i]/mm<<"mm"<<std::endl;
				}
				StartAng = m_GeometryParameter->get_Polycone_StartAng(SolidIndex,i);
				SpanAng = m_GeometryParameter->get_Polycone_SpanAng(SolidIndex,i);
				sol_Vol=new G4Polycone(iname,StartAng,SpanAng,numZ,Z,RMin,RMax);
			}
			else if ( SolidType == "BooleanSolid" ){
				G4double Ephi = m_GeometryParameter->get_BooleanSolid_Ephi(SolidIndex,i);
				G4double Etheta = m_GeometryParameter->get_BooleanSolid_Etheta(SolidIndex,i);
				G4double Epsi = m_GeometryParameter->get_BooleanSolid_Epsi(SolidIndex,i);
				G4double PosX  = m_GeometryParameter->get_BooleanSolid_PosX(SolidIndex,i);
				G4double PosY  = m_GeometryParameter->get_BooleanSolid_PosY(SolidIndex,i);
				G4double PosZ  = m_GeometryParameter->get_BooleanSolid_PosZ(SolidIndex,i);
				G4RotationMatrix* rot =new G4RotationMatrix(Ephi,Etheta,Epsi);
				G4ThreeVector trans(PosX ,PosY ,PosZ);
				G4String type = m_GeometryParameter->get_BooleanSolid_type(SolidIndex);
				G4String sol_name1 = m_GeometryParameter->get_BooleanSolid_sol1(SolidIndex);
				G4String sol_name2 = m_GeometryParameter->get_BooleanSolid_sol2(SolidIndex);
				G4String isol_name1 = sol_name1;
				G4String isol_name2 = sol_name2;
				if (RepNo>1){
					buffer.str("");
					buffer.clear();
					buffer<<sol_name1<<"_"<<i;
					isol_name1 = buffer.str();
					buffer.str("");
					buffer.clear();
					buffer<<sol_name2<<"_"<<i;
					isol_name2 = buffer.str();
				}
				G4SolidStore *pSolidStore = G4SolidStore::GetInstance();
				G4VSolid *sol1 = pSolidStore->GetSolid(isol_name1);
				G4VSolid *sol2 = pSolidStore->GetSolid(isol_name2);
				if (!sol1||!sol2){
					std::cout<<"ERROR: in SimpleGeometrySvc::ConstructVolumes(), cannot find solids for BooleanSolid \""<<iname<<"\" !!!"<<std::endl;
					continue;
				}
				if ( type == "plus" ){
					sol_Vol = new G4UnionSolid(iname,sol1,sol2,rot,trans);
				}
				else if ( type == "minus" ){
					sol_Vol = new G4SubtractionSolid(iname,sol1,sol2,rot,trans);
				}
				else if ( type == "times" ){
					sol_Vol = new G4IntersectionSolid(iname,sol1,sol2,rot,trans);
				}
				else {
					std::cout<<"ERROR: in SimpleGeometrySvc::ConstructVolumes(), cannot recognize BooleanSolid type \""<<type<<"\""<<"for \""<<iname<<"\"!!!"<<std::endl;
					continue;
				}
			}
			else {
				std::cout<<"SolidType "<<SolidType<<" is not supported yet!"<<std::endl;
				G4Exception("SimpleGeometrySvc::ConstructVolumes()",
						"InvalidSetup", FatalException,
						"unknown SolidType");
			}
			if (m_GeometryParameter->get_SolidBoolean(i_Vol)) continue;
			SDName = m_GeometryParameter->get_SDName(i_Vol);
			mat_name = m_GeometryParameter->get_material(i_Vol);
			pttoMaterial = G4Material::GetMaterial(mat_name);
			//		std::cout<<"name = "<<name
			//		         <<", i_Vol = "<<i_Vol
			//		         <<", SolidType = "<<SolidType
			//		         <<", SolidIndex = "<<SolidIndex
			//		         <<std::endl;
			if (!pttoMaterial){
				std::cout<<"Material "<<mat_name<<" is not defined!"<<std::endl;
				G4Exception("SimpleGeometrySvc::ConstructVolumes()",
						"InvalidSetup", FatalException,
						"unknown material name");
			}
			G4LogicalVolume* log_Vol;
			log_Vol = new G4LogicalVolume(sol_Vol, pttoMaterial, iname,0,0,0);
			G4VSensitiveDetector* aSD;
			aSD = MyDetectorManager::GetMyDetectorManager()->GetSD(iname, SDName, const_cast<SimpleGeometrySvc*>(this) );
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
G4VPhysicalVolume* SimpleGeometrySvc::PlaceVolumes(){
	std::stringstream buffer;
	G4VPhysicalVolume* world_pvol = 0;
	bool checkOverlap = m_GeometryParameter->get_checkoverlap();
	int nVol = m_GeometryParameter->get_VolNo();
	for ( int i_Vol = 0; i_Vol < nVol; i_Vol++ ){
		if (m_GeometryParameter->get_SolidBoolean(i_Vol)) continue;
		G4double PosX, PosY, PosZ;
		G4String name, motVolName; 
		G4int SRepNo;
		G4int RepNo;
		name = m_GeometryParameter->get_name(i_Vol);
//		std::cout<<"Place \""<<name<<"\""<<std::endl; // to be deleted
		motVolName = m_GeometryParameter->get_MotherName(i_Vol);
		SRepNo = m_GeometryParameter->get_SRepNo(i_Vol);
		RepNo = m_GeometryParameter->get_RepNo(i_Vol);
		G4LogicalVolume* log_container;
		int motRepNo = 1;
		if ( name == "World" ) motRepNo = 1;
		else{
			SimpleGeometryParameter* motPara = MyDetectorManager::GetMyDetectorManager()->GetParaFromVolume(motVolName);
			if (motPara)
				motRepNo = motPara->get_RepNo(motPara->get_VolIndex(motVolName));
			else{
				std::cout<<"We can't find parameter pointer!!"<<std::endl;
			}
		}
		for ( int im = 0; im < motRepNo; im++ ){
			if ( name == "World" ) log_container = 0;
			else {
				G4String imotVolName = motVolName;
				if (motRepNo>1){
					buffer.str("");
					buffer.clear();
					buffer<<motVolName<<"_"<<im;
					imotVolName = buffer.str();
				}
				log_container = get_logicalVolume(imotVolName);
			}
			for ( int i = 0; i < RepNo; i++ ){
				G4String iname = name;
				if (RepNo>1){
					buffer.str("");
					buffer.clear();
					buffer<<name<<"_"<<i;
					iname = buffer.str();
				}
				G4LogicalVolume* log_Vol = get_logicalVolume(iname);
				PosX  = m_GeometryParameter->get_PosX(i_Vol,i);
				PosY  = m_GeometryParameter->get_PosY(i_Vol,i);
				PosZ  = m_GeometryParameter->get_PosZ(i_Vol,i);
				G4ThreeVector pos(PosX ,PosY ,PosZ);
				G4double Ephi = m_GeometryParameter->get_Ephi(i_Vol,i);
				G4double Etheta = m_GeometryParameter->get_Etheta(i_Vol,i);
				G4double Epsi = m_GeometryParameter->get_Epsi(i_Vol,i);
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
	}
//	std::cout<<"Finished!"<<std::endl; // to be deleted
	G4VPhysicalVolume *former = MyVGeometrySvc::PlaceVolumes();
	if (!world_pvol) world_pvol=former;
	return world_pvol;
}
