//---------------------------------------------------------------------------//
//Description: 1. Setup Cdc geometry and manage associated sensitive detectors
//             2. Communicate with associated sensitive detectors
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 26 Aug, 2014
//Comment: The units are "mm"(default), "um"(for wire radius) and "rad". 
//Comment: Datum plain is downstream end plate of CDC
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

	//*******************Construct Layers and wires***********************
	//get general info
	bool checkOverlap = m_GeometryParameter->get_checkoverlap();
	G4String MVol_name = m_GeometryParameter->get_MotherLogicalVolume();
	G4String SD_name = m_GeometryParameter->get_SensitiveDetector();
	G4String SDVolumeName = m_GeometryParameter->get_SDVolumeName();
	G4String LayerMatName=m_GeometryParameter->get_LayerMaterial();
	G4LogicalVolume* log_CdcContainer = get_logicalVolume(MVol_name);
	G4VSensitiveDetector* aSD = MyDetectorManager::GetMyDetectorManager()->GetSD(SDVolumeName, SD_name, const_cast<CdcGeometrySvc*>(this) );
	G4VSensitiveDetector* bSD = MyDetectorManager::GetMyDetectorManager()->GetSD("wire", "MonitorSD", const_cast<CdcGeometrySvc*>(this) );
	G4UserLimits* stepLimit = new G4UserLimits(m_GeometryParameter->get_MaxStepLength());
	//get wire info
	G4double SignalWireR=m_GeometryParameter->get_SignalWireRadius();
	G4double FieldWireR=m_GeometryParameter->get_FieldWireRadius();
	G4String signalWireMatName=m_GeometryParameter->get_SignalWireMaterial();
	G4String fieldWireMatName=m_GeometryParameter->get_FieldWireMaterial();
	G4Material *SignalWireMat, *FieldWireMat, *LayerMat;
	SignalWireMat = G4Material::GetMaterial(signalWireMatName);
	FieldWireMat = G4Material::GetMaterial(fieldWireMatName);
	LayerMat = G4Material::GetMaterial(LayerMatName);
	if (!SignalWireMat || !FieldWireMat || !LayerMat ){
		std::cout<<"In CdcGeometrySvc, Material is not defined!"<<std::endl;
		G4Exception("CdcGeometrySvc::SetGeometry()",
				"InvalidSetup", FatalException,
				"unknown material name");
	}
	//visual option
	bool svis, fvis, lvis;
	double sr, sg, sb, st, fr, fg, fb, ft, lr, lg, lb, lt;
	svis = m_GeometryParameter->get_vis_SignalWire();
	fvis = m_GeometryParameter->get_vis_FieldWire();
	lvis = m_GeometryParameter->get_vis_layer();
	sr = m_GeometryParameter->get_r_SignalWire();
	sg = m_GeometryParameter->get_g_SignalWire();
	sb = m_GeometryParameter->get_b_SignalWire();
	st = m_GeometryParameter->get_t_SignalWire();
	fr = m_GeometryParameter->get_r_FieldWire();
	fg = m_GeometryParameter->get_g_FieldWire();
	fb = m_GeometryParameter->get_b_FieldWire();
	ft = m_GeometryParameter->get_t_FieldWire();
	lr = m_GeometryParameter->get_r_layer();
	lg = m_GeometryParameter->get_g_layer();
	lb = m_GeometryParameter->get_b_layer();
	lt = m_GeometryParameter->get_t_layer();

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
		double layer_length = m_GeometryParameter->get_layer_length(ilayer) - 0.1*mm; // added 1 um safty margine 
		halfzlen = layer_length/2/cos(angle4stereo) - FieldWireR;
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
		G4Tubs* sol_FieldWire=new G4Tubs("CdcFieldWire",innerR,FieldWireR,halfzlen,startAngle,spanAngle);
		log_FieldWire = new G4LogicalVolume(sol_FieldWire, FieldWireMat,"CdcFieldWire",0,0,0);
		//visual option
		if (!fvis){
			log_FieldWire->SetVisAttributes(G4VisAttributes::GetInvisible());
		}
		else{
			G4VisAttributes* visAttributes = new G4VisAttributes;
			visAttributes->SetVisibility(true);
			visAttributes->SetColour(fr,fg,fb,ft);
			log_FieldWire->SetVisAttributes(visAttributes);
		}
		if (layer_type == 1 || layer_type == 2){
			G4Tubs* sol_SignalWire=new G4Tubs("CdcSenseWire",innerR,SignalWireR,halfzlen,startAngle,spanAngle);
			log_SignalWire = new G4LogicalVolume(sol_SignalWire, SignalWireMat,"CdcSenseWire",0,0,0);
			//visual option
			if (!svis){
				log_SignalWire->SetVisAttributes(G4VisAttributes::GetInvisible());
			}
			else{
				G4VisAttributes* visAttributes = new G4VisAttributes;
				visAttributes->SetVisibility(true);
				visAttributes->SetColour(sr,sg,sb,st);
				log_SignalWire->SetVisAttributes(visAttributes);
			}
		}

		//====>Get logical volumes for layer
		innerR = m_GeometryParameter->get_layer_Rc(ilayer)-2*FieldWireR;
		if (ilayer==layerNo-1){
			outR = 2*m_GeometryParameter->get_layer_Rc(ilayer)-m_GeometryParameter->get_layer_Rc(ilayer-1)-2*FieldWireR;
		}
		else{
			outR = m_GeometryParameter->get_layer_Rc(ilayer+1)-2*FieldWireR;
		}
		double innerStereo = fabs(m_GeometryParameter->get_layer_angle4stereo(ilayer));
		double outerStereo = 0;
		if (ilayer==layerNo-1){
			outerStereo = fabs(m_GeometryParameter->get_layer_angle4stereo(ilayer));
		}
		else{
			outerStereo = fabs(m_GeometryParameter->get_layer_angle4stereo(ilayer+1));
		}
		halfzlen = layer_length/2;
//		std::cout<<ilayer<<": G4Hype("<<innerR/cm<<","<<outR/cm<<","<<innerStereo<<","<<outerStereo<<","<<halfzlen/cm<<")"<<std::endl;
		G4Hype * sol_layer = new G4Hype("CdcLayer",innerR,outR,innerStereo,outerStereo,halfzlen);
		G4LogicalVolume * log_layer = new G4LogicalVolume(sol_layer,LayerMat,"CdcLayer",0,0,0);
		if (layer_type == 1 || (layer_type == 0 && m_GeometryParameter->get_layer_type(ilayer+1) ==1) ){
			log_layer->SetSensitiveDetector( aSD );
			// FIXME add limit to layers
			log_layer->SetUserLimits(stepLimit);
		}
		//visual option
		if (!lvis){
			log_layer->SetVisAttributes(G4VisAttributes::GetInvisible());
		}
		else{
			G4VisAttributes* visAttributes = new G4VisAttributes;
			visAttributes->SetVisibility(true);
			visAttributes->SetColour(lr,lg,lb,lt);
			log_layer->SetVisAttributes(visAttributes);
		}

		//====>Place wires in this layer
		int HoleNo = m_GeometryParameter->get_layer_HoleNo(ilayer);
		G4ThreeVector centerVec(1.,1.,1.);
		double layer_Rc = m_GeometryParameter->get_layer_Rc(ilayer);
		double layer_Re = m_GeometryParameter->get_layer_Re(ilayer);
		//FIXME: print out the position of wires on the downstream side
		//printf("%d: %lf, %lf, %lf; %lf\n",ilayer,m_GeometryParameter->get_layer_phi0z(ilayer,-layer_length/2),m_GeometryParameter->get_layer_phi0z(ilayer,0),m_GeometryParameter->get_layer_phi0z(ilayer,layer_length/2),m_GeometryParameter->get_layer_SPhi(ilayer));
		for ( int holeId = 0; holeId < HoleNo; holeId++ ){
			//place wire
			G4double phi0m = m_GeometryParameter->get_layer_phi0z(ilayer,0);
			G4double phiim = phi0m + holeId*holeDphi;
			G4LogicalVolume* log_wire;
			if ( (layer_type == 1 || layer_type == 2) && holeId%2 == 1 ){
				log_wire = log_SignalWire;
				Name = "CdcSenseWire";
			}
			else{
				log_wire = log_FieldWire;
				Name = "CdcFieldWire";
			}
			// FIXME add SD to wires
			//log_wire->SetSensitiveDetector( bSD );
			centerVec.setZ(0);
			centerVec.setPhi(phiim);
			centerVec.setPerp(layer_Rc);
			G4RotationMatrix* rotMatrix=new G4RotationMatrix();
			rotMatrix->rotateZ(-phiim);
			rotMatrix->rotateX(-angle4stereo);
			G4ThreeVector up(1.,1.,1.);
//			up.setZ(-layer_length/2);
//			up.setPhi(m_GeometryParameter->get_layer_phi0z(ilayer,up.z())+holeId*holeDphi);
//			up.setPerp(layer_Re);
//			G4ThreeVector down(1.,1.,1.);
//			down.setZ(layer_length/2);
//			down.setPhi(m_GeometryParameter->get_layer_phi0z(ilayer,down.z())+holeId*holeDphi);
//			down.setPerp(layer_Re);
//			if (layer_type==1&&holeId%2==1) std::cout<<"wire["<<m_GeometryParameter->get_layer_ID(ilayer)<<","<<holeId/2<<"] @ up:"<<up/cm<<", down:"<<down/cm<<std::endl;
		//FIXME
			new G4PVPlacement(rotMatrix,centerVec,log_wire,Name,log_layer,false,0,checkOverlap);
			//FIXME: print out the position of wires in the center
//			G4ThreeVector testVec(1.,1.,1.);
//			layer_length = 599.17*mm;
//			double testZ = -layer_length/2.;
//			testVec.setZ(0);
//			G4double testPhi = m_GeometryParameter->get_layer_phi0z(ilayer,testZ) + holeId*holeDphi;
//			testVec.setPhi(testPhi);
//			testVec.setPerp(m_GeometryParameter->get_layer_Rz(ilayer,testZ));
//			printf("%d %d %lf %lf",ilayer,holeId,testVec.x(),testVec.y());
//			printf(" %lf %lf",centerVec.x(),centerVec.y());
//			testZ = layer_length/2.;
//			testPhi = m_GeometryParameter->get_layer_phi0z(ilayer,testZ) + holeId*holeDphi;
//			testVec.setPhi(testPhi);
//			testVec.setPerp(m_GeometryParameter->get_layer_Rz(ilayer,testZ));
//			printf(" %lf %lf\n",testVec.x(),testVec.y());
		}

		//====>Place layer
		// FIXME should read this position parameter later
		new G4PVPlacement(0,G4ThreeVector(0,0,-122.5*mm),log_layer,"CdcLayer",log_CdcContainer,false,ilayer,checkOverlap);
	}//end loop in layers
}

//=>Place Volumes
G4VPhysicalVolume* CdcGeometrySvc::PlaceVolumes(){
	G4VPhysicalVolume* world_pvol = 0;
	G4VPhysicalVolume *former = SimpleGeometrySvc::PlaceVolumes();
	if (!world_pvol) world_pvol=former;
	return world_pvol;
}
