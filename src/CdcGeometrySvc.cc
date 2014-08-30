//---------------------------------------------------------------------------//
//Description: 1. Setup Cdc geometry and manage associated sensitive detectors
//             2. Communicate with associated sensitive detectors
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 26 Aug, 2014
//Comment: The units are "mm"(default), "um"(for wire radius) and "rad". 
//Comment: Datum plain is upstream end plate of CDC
//Comment: fVerboseLevel: 0:    Just Warning Messages and Error Messages
//                        1:    + Dump()
//                        >= 5: All informations
//---------------------------------------------------------------------------//
#include "CdcGeometrySvc.hh"

#include "myglobals.hh"
#include "G4UnitsTable.hh"
#include "G4LogicalVolume.hh"
#include "G4VSensitiveDetector.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4Hype.hh"
#include "G4SubtractionSolid.hh"
#include "G4TwistedTubs.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4RotationMatrix.hh"
#include "G4UserLimits.hh"

#include <sstream>
#include <iostream>

#include "SimpleGeometrySvc.hh"
#include "CdcGeometryParameter.hh"
#include "MyDetectorManager.hh"

CdcGeometrySvc::CdcGeometrySvc(G4String name, G4String opt )
	: SimpleGeometrySvc(name, "S") // Use a not-empty opt to stop inherited class from creating new GeometryParameter
{
	if ( opt == "" ){//called directly by user. set up parameter class.
		CdcGeometryParameter *pointer = new CdcGeometryParameter(name);
		//		std::cout<<"======>In CdcGeometrySvc, new CdcGeometryParameter at ("<<(void*)pointer<<")!"<<std::endl;
		set_GeometryParameter(pointer);
	}
}

CdcGeometrySvc::~CdcGeometrySvc(){
	printf("~CdcGeometrySvc");
}

//------------------------Modify-------------------------------
void CdcGeometrySvc::set_GeometryParameter( CdcGeometryParameter* val ){
	m_GeometryParameter = val;
	SimpleGeometrySvc::set_GeometryParameter(m_GeometryParameter);
}

//---------------------------SetGeometry---------------------------------
//
G4VPhysicalVolume* CdcGeometrySvc::SetGeometry(){
	m_GeometryParameter->Dump();
	ConstructVolumes();
	G4VPhysicalVolume *current = PlaceVolumes();
	return current;
}

//-------------------------Special functions-----------------------------
//=> ConstructVolumes
void CdcGeometrySvc::ConstructVolumes(){
	SimpleGeometrySvc::ConstructVolumes();
	bool vis;
	double r, g, b;

	//*******************Construct Cells and wires***********************
	//get general info
	bool checkOverlap = m_GeometryParameter->get_checkoverlap();
	G4String MVol_name = m_GeometryParameter->get_MotherLogicalVolume();
	G4String SD_name = m_GeometryParameter->get_SensitiveDetector();
	G4String SDVolumeName = m_GeometryParameter->get_SDVolumeName();
	G4String CellMatName=m_GeometryParameter->get_CellMaterial();
	G4LogicalVolume* log_CdcContainer = get_logicalVolume(MVol_name);
	G4VSensitiveDetector* aSD = MyDetectorManager::GetMyDetectorManager()->GetSD(SDVolumeName, SD_name, const_cast<CdcGeometrySvc*>(this) );
	//get wire info
	G4double SignalWireR=m_GeometryParameter->get_SignalWireRadius();
	G4double FieldWireR=m_GeometryParameter->get_FieldWireRadius();
	G4String signalWireMatName=m_GeometryParameter->get_SignalWireMaterial();
	G4String fieldWireMatName=m_GeometryParameter->get_FieldWireMaterial();
	G4Material *SignalWireMat, *FieldWireMat, *CellMat;
	SignalWireMat = G4Material::GetMaterial(signalWireMatName);
	FieldWireMat = G4Material::GetMaterial(fieldWireMatName);
	CellMat = G4Material::GetMaterial(CellMatName);
	if (!SignalWireMat || !FieldWireMat || !CellMat ){
		std::cout<<"In CdcGeometrySvc, Material is not defined!"<<std::endl;
		G4Exception("CdcGeometrySvc::SetGeometry()",
				"InvalidSetup", FatalException,
				"unknown material name");
	}
	//visual option
	bool svis, fvis;
	double sr, sg, sb, fr, fg, fb;
	svis = m_GeometryParameter->get_vis_SignalWire();
	fvis = m_GeometryParameter->get_vis_FieldWire();
	sr = m_GeometryParameter->get_r_SignalWire();
	sg = m_GeometryParameter->get_g_SignalWire();
	sb = m_GeometryParameter->get_b_SignalWire();
	fr = m_GeometryParameter->get_r_FieldWire();
	fg = m_GeometryParameter->get_g_FieldWire();
	fb = m_GeometryParameter->get_b_FieldWire();

	//loop in layers
	G4int    layerNo = m_GeometryParameter->get_LayerNo();      
	//	for( int ilayer = 0; ilayer < 1; ilayer++ ){
	for( int ilayer = 0; ilayer < layerNo; ilayer++ ){
		//set useful variables
		G4LogicalVolume* log_SignalWire, *log_FieldWire;
		G4String Name;
		G4double outR, innerR, halfzlen;
		G4double startAngle, spanAngle;
		double holeDphi = m_GeometryParameter->get_layer_holeDphi(ilayer);
		double angle4rotate = m_GeometryParameter->get_layer_angle4rotate(ilayer);
		double angle4stereo = m_GeometryParameter->get_layer_angle4stereo(ilayer);
		//====>Get logical volumes for Wire
		startAngle=0.*deg;
		spanAngle=360.*deg;
		innerR = 0.;
		halfzlen = m_GeometryParameter->get_layer_length(ilayer)/2/cos(angle4stereo) - FieldWireR;
		if ( get_VerboseLevel() >=10 ){
			std::cout<<"sol_Wire: innerR = "<<innerR/mm
				<<"mm, halfzlen = "<<halfzlen/mm
				<<"mm, startAngle = "<<startAngle/rad
				<<"rad, spanAngle = "<<spanAngle/rad
				<<"rad"
				<<std::endl;
		}
		//get layer info
		Name="CdcWire";
		G4int layer_type = m_GeometryParameter->get_layer_type(ilayer);
		G4Tubs* sol_FieldWire=new G4Tubs(Name,innerR,FieldWireR,halfzlen,startAngle,spanAngle);
		log_FieldWire = new G4LogicalVolume(sol_FieldWire, FieldWireMat,Name,0,0,0);
		//visual option
		if (!fvis){
			log_FieldWire->SetVisAttributes(G4VisAttributes::Invisible);
		}
		else{
			G4VisAttributes* visAttributes = new G4VisAttributes;
			visAttributes->SetVisibility(true);
			visAttributes->SetColour(fr,fg,fb);
			log_FieldWire->SetVisAttributes(visAttributes);
		}
		if (layer_type == 1 || layer_type == 2){
			G4Tubs* sol_SignalWire=new G4Tubs(Name,innerR,SignalWireR,halfzlen,startAngle,spanAngle);
			log_SignalWire = new G4LogicalVolume(sol_SignalWire, SignalWireMat,Name,0,0,0);
			//visual option
			if (!svis){
				log_SignalWire->SetVisAttributes(G4VisAttributes::Invisible);
			}
			else{
				G4VisAttributes* visAttributes = new G4VisAttributes;
				visAttributes->SetVisibility(true);
				visAttributes->SetColour(sr,sg,sb);
				log_SignalWire->SetVisAttributes(visAttributes);
			}
		}

		//====>Get logical volumes for layer
		innerR = m_GeometryParameter->get_layer_Re(ilayer)-2*FieldWireR;
		if (ilayer==layerNo-1){
			outR = 2*m_GeometryParameter->get_layer_Re(ilayer)-m_GeometryParameter->get_layer_Re(ilayer-1)-2*FieldWireR;
		}
		else{
			outR = m_GeometryParameter->get_layer_Re(ilayer+1)-2*FieldWireR;
		}
		double innerStereo = m_GeometryParameter->get_layer_angle4stereo(ilayer);
		double outerStereo = 0;
		if (ilayer==layerNo-1){
			outerStereo = m_GeometryParameter->get_layer_angle4stereo(ilayer);
		}
		else{
			outerStereo = m_GeometryParameter->get_layer_angle4stereo(ilayer+1);
		}
		halfzlen = m_GeometryParameter->get_layer_length(ilayer);
		G4Hype * sol_layer = new G4Hype("CdcLayer",innerR,outR,innerStereo,outerStereo,halfzlen);
		G4LogicalVolume * log_layer = new G4LogicalVolume(sol_layer,CellMat,"CdcLayer",0,0,0);
		log_layer->SetSensitiveDetector( aSD );
//		G4UserLimits* stepLimit = new G4UserLimits(m_GeometryParameter->get_MinStepLength());
//		log_layer->SetUserLimits(stepLimit);

		//====>Place wires in this layer
		int HoleNo = m_GeometryParameter->get_layer_HoleNo(ilayer);
		G4ThreeVector centerVec(1.,1.,1.);
		double layer_Rc = m_GeometryParameter->get_layer_Rc(ilayer);
		for ( int holeId = 0; holeId < HoleNo; holeId++ ){
			//place cell
			G4double SPhi = m_GeometryParameter->get_layer_SPhi(ilayer);
			G4double phi = SPhi + holeId*holeDphi + angle4rotate/2;
			G4LogicalVolume* log_wire;
			if ( (layer_type == 1 || layer_type == 2) && holeId%2 == 1 ){
				log_wire = log_SignalWire;
			}
			else{
				log_wire = log_FieldWire;
			}
			centerVec.setMag(layer_Rc);
			centerVec.setTheta(pi/2);
			centerVec.setPhi(phi);
			G4RotationMatrix* rotMatrix=new G4RotationMatrix();
			rotMatrix->rotateZ(-phi);
			rotMatrix->rotateX(angle4stereo);
			// TOFIX:
			if (holeId==0)
				std::cout<<"G4PVPlacement("<<(*rotMatrix)<<","<<centerVec/cm<<")"<<std::endl;
			if (holeId<300){
				new G4PVPlacement(rotMatrix,centerVec,log_wire,Name,log_layer,false,0,checkOverlap);
			}
		}

		//====>Place layer
		new G4PVPlacement(0,G4ThreeVector(0,0,0),log_layer,"CdcLayer",log_CdcContainer,false,ilayer,checkOverlap);
	}//end loop in layers
}

//=>Place Volumes
G4VPhysicalVolume* CdcGeometrySvc::PlaceVolumes(){
	G4VPhysicalVolume* world_pvol = 0;
	G4VPhysicalVolume *former = SimpleGeometrySvc::PlaceVolumes();
	if (!world_pvol) world_pvol=former;
	return world_pvol;
}
