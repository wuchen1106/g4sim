//---------------------------------------------------------------------------//
//Description: 1. Setup Cdc geometry and manage associated sensitive detectors
//             2. Communicate with associated sensitive detectors
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 21 Oct, 2012
//Modified: 6 Nov, 2012 by Wu Chen(wuchen@mail.ihep.ac.cn)
//Modified: 7 Jan, 2013 by Wu Chen(wuchen@mail.ihep.ac.cn)
//          Added Boolean Volumes
//Comment: about fVerboseLevel:
//         or modifying input card before starting a run.
//         Either way would set verbose of selected sensitive detectors to the same level
//         0:   Only Warnings and Errors
//         1:   Important information
//         >=5: All information
//---------------------------------------------------------------------------//
#include "CdcGeometrySvc.hh"

#include "globals.hh"
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
	//get wire info
	G4double signalWireR=m_GeometryParameter->get_SignalWireRadius();
	G4double fieldWireR=m_GeometryParameter->get_FieldWireRadius();
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

	//loop in layers
	G4int    layerNo = m_GeometryParameter->get_LayerNo();      
	//	for( int layerId = 0; layerId < 1; layerId++ ){
	for( int layerId = 0; layerId < layerNo; layerId++ ){
		//set useful variables
		G4LogicalVolume* log_cell;// would be used as the mother volume of wires
		G4LogicalVolume* log_FieldWire_in;
		G4LogicalVolume* log_FieldWire_mid;
		G4LogicalVolume* log_FieldWire_out;
		G4LogicalVolume* log_SignalWire;
		std::ostringstream osname;
		osname<<"CdcCellofLayer"<<layerId;
		G4String CellName = osname.str();
		osname.str("");
		osname.clear();
		osname<<"CdcSignalWireofLayer"<<layerId;
		G4String SignalWireName = osname.str();
		osname.str("");
		osname.clear();
		osname<<"CdcFieldWireofLayer"<<layerId;
		G4String FieldWireName = osname.str();
		G4double outR, innerR, halfzlen;
		G4double startAngle, spanAngle;
		//get layer info
		G4double cell_dphi = m_GeometryParameter->get_layer_cell_dphi(layerId);
		G4double twisteddangle = m_GeometryParameter->get_layer_angle4rotate(layerId);
		G4double RMin = m_GeometryParameter->get_layer_RMin(layerId);
		G4double RMid = m_GeometryParameter->get_layer_RMid(layerId);
		G4double RMax = layerId == layerNo - 1 ? m_GeometryParameter->get_Cdc_RMax() : m_GeometryParameter->get_layer_RMin(layerId+1);
		G4double layer_halfzlen = m_GeometryParameter->get_layer_length(layerId)/2.; //In Geant4, soild has central-symmetry 
		G4int layer_type = m_GeometryParameter->get_layer_type(layerId);
		G4double alpha_RMin;
		G4double alpha_RMax;
		G4double alpha_RMid = twisteddangle/2;
		if ( layer_type == 2 ){
			alpha_RMin = twisteddangle/4;
		}
		else if ( layer_type == 4 ){
			alpha_RMin = m_GeometryParameter->get_layer_angle4rotate(layerId-1)/4;
		}
		else{
			alpha_RMin = twisteddangle/2;
		}
		if ( layer_type == 3 ){
			alpha_RMax = m_GeometryParameter->get_layer_angle4rotate(layerId+1)/4;
		}
		else if (layer_type == 5 ){
			alpha_RMax = twisteddangle/4;
		}
		else{
			alpha_RMax = twisteddangle/2;
		}
		G4double theta_RMin = atan(RMin*sin(alpha_RMin)/layer_halfzlen);
		G4double theta_RMax = atan(RMax*sin(alpha_RMax)/layer_halfzlen);
		G4double theta_RMid = atan(RMid*sin(alpha_RMid)/layer_halfzlen);
		//    std::cout<<"twisteddangle = "<<twisteddangle/deg<<std::endl;
		//    std::cout<<"alpha_RMin = "<<alpha_RMin/deg<<std::endl;
		//    std::cout<<"alpha_RMid = "<<alpha_RMid/deg<<std::endl;
		//    std::cout<<"alpha_RMax = "<<alpha_RMax/deg<<std::endl;
		//    std::cout<<"theta_RMin = "<<theta_RMin/deg<<std::endl;
		//    std::cout<<"theta_RMid = "<<theta_RMid/deg<<std::endl;
		//    std::cout<<"theta_RMax = "<<theta_RMax/deg<<std::endl;
		//====>Get logical volumes for SignalWire and FieldWire of this layer
		//SignalWire
		startAngle=0.*deg;
		spanAngle=360.*deg;
		outR=signalWireR;
		innerR = 0.;
		halfzlen = layer_halfzlen;
		double halfzlen_sw = layer_halfzlen/cos(theta_RMid) - 2*signalWireR*tan(theta_RMid);
		if ( get_VerboseLevel() >=10 ){
			std::cout<<"sol_SignalWire: innerR = "<<innerR/mm
				<<"mm, outR = "<<outR/mm
				<<"mm, halfzlen = "<<halfzlen_sw/mm
				<<"mm, startAngle = "<<startAngle/rad
				<<"rad, spanAngle = "<<spanAngle/rad
				<<"rad"
				<<std::endl;
		}
		G4Tubs* sol_SignalWire=new G4Tubs(SignalWireName,innerR,outR,halfzlen_sw,startAngle,spanAngle);
		log_SignalWire = new G4LogicalVolume(sol_SignalWire, SignalWireMat,SignalWireName,0,0,0);
		//FieldWire
		outR=fieldWireR;
		//  FieldWire i_row == 1
		double halfzlen_fw_mid = layer_halfzlen/cos(theta_RMid) - 2*fieldWireR*tan(theta_RMid);
		if ( get_VerboseLevel() >=10 ){
			std::cout<<"sol_FieldWire_mid: innerR = "<<innerR/mm
				<<"mm, outR = "<<outR/mm
				<<"mm, halfzlen = "<<halfzlen_fw_mid/mm
				<<"mm, startAngle = "<<startAngle/rad
				<<"rad, spanAngle = "<<spanAngle/rad
				<<"rad"
				<<std::endl;
		}
		G4Tubs* sol_FieldWire_mid =new G4Tubs(FieldWireName,innerR,outR,halfzlen_fw_mid,startAngle,spanAngle);
		log_FieldWire_mid = new G4LogicalVolume(sol_FieldWire_mid, FieldWireMat,FieldWireName,0,0,0);
		//  FieldWire i_row == 0
		double halfzlen_fw_in = layer_halfzlen/cos(theta_RMin) - 2*fieldWireR*tan(theta_RMin);
		if ( get_VerboseLevel() >=10 ){
			std::cout<<"sol_FieldWire_in: innerR = "<<innerR/mm
				<<"mm, outR = "<<outR/mm
				<<"mm, halfzlen = "<<halfzlen_fw_in/mm
				<<"mm, startAngle = "<<startAngle/rad
				<<"rad, spanAngle = "<<spanAngle/rad
				<<"rad"
				<<std::endl;
		}
		G4Tubs* sol_FieldWire_in =new G4Tubs(FieldWireName,innerR,outR,halfzlen_fw_in,startAngle,spanAngle);
		log_FieldWire_in = new G4LogicalVolume(sol_FieldWire_in, FieldWireMat,FieldWireName,0,0,0);
		//  FieldWire i_row == 2
		if ( layer_type == 1 ){
			double halfzlen_fw_out = layer_halfzlen/cos(theta_RMax) - 2*fieldWireR*tan(theta_RMax);
			if ( get_VerboseLevel() >=10 ){
				std::cout<<"sol_FieldWire_out: innerR = "<<innerR/mm
					<<"mm, outR = "<<outR/mm
					<<"mm, halfzlen = "<<halfzlen_fw_out/mm
					<<"mm, startAngle = "<<startAngle/rad
					<<"rad, spanAngle = "<<spanAngle/rad
					<<"rad"
					<<std::endl;
			}
			G4Tubs* sol_FieldWire_out =new G4Tubs(FieldWireName,innerR,outR,halfzlen_fw_out,startAngle,spanAngle);
			log_FieldWire_out = new G4LogicalVolume(sol_FieldWire_out, FieldWireMat,FieldWireName,0,0,0);
		}
		else{
			log_FieldWire_out = log_FieldWire_mid;
		}
		//visual option
		vis = m_GeometryParameter->get_vis_SignalWire();
		if (!vis){
			log_SignalWire->SetVisAttributes(G4VisAttributes::Invisible);
		}
		else{
			r = m_GeometryParameter->get_r_SignalWire();
			g = m_GeometryParameter->get_g_SignalWire();
			b = m_GeometryParameter->get_b_SignalWire();
			G4VisAttributes* visAttributes = new G4VisAttributes;
			visAttributes->SetVisibility(true);
			visAttributes->SetColour(r,g,b);
			log_SignalWire->SetVisAttributes(visAttributes);
		}
		vis = m_GeometryParameter->get_vis_FieldWire();
		if (!vis){
			log_FieldWire_in->SetVisAttributes(G4VisAttributes::Invisible);
			log_FieldWire_mid->SetVisAttributes(G4VisAttributes::Invisible);
			log_FieldWire_out->SetVisAttributes(G4VisAttributes::Invisible);
		}
		else{
			r = m_GeometryParameter->get_r_FieldWire();
			g = m_GeometryParameter->get_g_FieldWire();
			b = m_GeometryParameter->get_b_FieldWire();
			G4VisAttributes* visAttributes = new G4VisAttributes;
			visAttributes->SetVisibility(true);
			visAttributes->SetColour(r,g,b);
			log_FieldWire_in->SetVisAttributes(visAttributes);
			log_FieldWire_mid->SetVisAttributes(visAttributes);
			log_FieldWire_out->SetVisAttributes(visAttributes);
		}
		//====>Get logical volume for Cell of this layer
		//TYPE 0/1: Ordinary layer/
		//          Out most layer
		//    each cell is a twisted(or not) tube
		//TYPE 2/3/4/5: Stereo layer upon an axial layer/
		//              Axial layer beneath a stereo layer/
		//              Axial layer upon a stereo layer/
		//              Stereo layer beneath an axial layer/
		//    each cell is a G4TwistedTube subtracted by a G4Hype
		G4double dr_RMin = alpha_RMin > theta_RMin ? fieldWireR/cos(alpha_RMin) : fieldWireR/cos(theta_RMin);
		G4double dr_RMax = alpha_RMax > theta_RMax ? fieldWireR/cos(alpha_RMax) : fieldWireR/cos(theta_RMax);
		G4double endinnerrad;
		G4double endouterrad;
		if ( layer_type == 4 ){
			endinnerrad = (RMin + dr_RMin)*cos(alpha_RMin);
			//std::cout<<"endinnerrad = ("<<RMin/mm<<" - "<<dr_RMin/mm<<")*cos("<<alpha_RMin<<") = "<<endinnerrad/mm<<std::endl;
		}
		else if ( layer_type == 2 ){
			endinnerrad = RMin + dr_RMin;
		}
		else {
			endinnerrad = RMin - dr_RMin;
		}
		if ( layer_type == 5 ){
			endouterrad = (RMax - dr_RMax)*cos(alpha_RMax)/cos(alpha_RMid);
			//std::cout<<"endouterrad = ("<<RMax/mm<<" - "<<dr_RMax/mm<<")*cos("<<alpha_RMax<<")/cos("<<alpha_RMid/rad<<") = "<<endouterrad/mm<<std::endl;
		}
		else if ( layer_type == 1 ){
			endouterrad = RMax + dr_RMax;
		}
		else{
			endouterrad = RMax - dr_RMax;
		}
		if ( get_VerboseLevel() >=10 ){
			std::cout<<"sol_cell: twisteddangle = "<<twisteddangle/rad
				<<"rad, endinnerrad = "<<endinnerrad/mm
				<<"mm, endouterrad = "<<endouterrad/mm
				<<"mm, halfzlen = "<<halfzlen/mm
				<<"mm, dphi = "<<cell_dphi/rad
				<<"rad"
				<<std::endl;
		}
		G4TwistedTubs* sol_cell = new G4TwistedTubs(CellName, twisteddangle, endinnerrad, endouterrad, halfzlen, cell_dphi);
		G4double twistedangle4subt;
		G4double endinnerrad4subt;
		G4double endouterrad4subt;
		if ( layer_type == 5 ){
			twistedangle4subt = alpha_RMax*2;
			endinnerrad4subt = (RMax - dr_RMax);
			endouterrad4subt = endouterrad;
		}
		else if ( layer_type == 4 ){
			twistedangle4subt = alpha_RMin*2;
			endinnerrad4subt = endinnerrad;
			endouterrad4subt = (RMin + dr_RMin);
		}
		else if ( layer_type == 3 ){
			twistedangle4subt = alpha_RMax*2;
			endinnerrad4subt = endouterrad;
			endouterrad4subt = (RMax - dr_RMax)/cos(alpha_RMax);
		}
		else if ( layer_type == 2 ){
			twistedangle4subt = alpha_RMin*2;
			endinnerrad4subt = (RMin + dr_RMin)*cos(alpha_RMid)/cos(alpha_RMin);
			endouterrad4subt = endinnerrad;
		}

		if ( layer_type != 0 && layer_type != 1 ){
			if ( get_VerboseLevel() >=10 ){
				std::cout<<"sol_Subtraction4Cell: twisteddangle = "<<twistedangle4subt/rad
					<<"rad, endinnerrad = "<<endinnerrad4subt/mm
					<<"mm, endouterrad = "<<endouterrad4subt/mm
					<<"mm, halfzlen = "<<halfzlen/mm
					<<std::endl;
			}
			G4TwistedTubs* sol_Subtraction4Cell = new G4TwistedTubs("Subtraction4Cell", twistedangle4subt, endinnerrad4subt, endouterrad4subt, halfzlen, 180*deg );
			G4SubtractionSolid* sol_SubtractedCell = new G4SubtractionSolid(CellName,sol_cell,sol_Subtraction4Cell);
			//log_cell = new G4LogicalVolume(sol_cell, CellMat, CellName,0,0,0);
			//log_cell = new G4LogicalVolume(sol_Subtraction4Cell, CellMat, CellName,0,0,0);
			log_cell = new G4LogicalVolume(sol_SubtractedCell, CellMat, CellName,0,0,0);
		}
		else{
			log_cell = new G4LogicalVolume(sol_cell, CellMat, CellName,0,0,0);
		}
		G4VSensitiveDetector* aSD = MyDetectorManager::GetMyDetectorManager()->GetSD(SDVolumeName, SD_name, const_cast<CdcGeometrySvc*>(this) );
		log_cell->SetSensitiveDetector( aSD );
		//visual option
		vis = m_GeometryParameter->get_vis_cell();
		if (!vis){
			log_cell->SetVisAttributes(G4VisAttributes::Invisible);
		}
		else{
			r = m_GeometryParameter->get_r_cell();
			g = m_GeometryParameter->get_g_cell();
			b = m_GeometryParameter->get_b_cell();
			G4VisAttributes* visAttributes = new G4VisAttributes;
			visAttributes->SetVisibility(true);
			visAttributes->SetColour(r,g,b);
			log_cell->SetVisAttributes(visAttributes);
		}

		//====>place wires into cell or cdc container w.r.t different layer types
		//TYPE 0: Ordinary layer
		//        __________ each cell is a twisted(or not) tube with 
		//        |        | 3 field wires and 1 signal wire inside
		//        |o0  x   |
		//        |        |
		//        |o1  o2  |
		//        ----------
		//TYPE 1: Out most layer
		//        __________ each cell is a twisted(or not) tube with 
		//        |o3  o4  | 5 field wires and 1 signal wire inside
		//        |        |
		//        |o0  x   |
		//        |        |
		//        |o1  o2  |
		//        ----------
		//TYPE 2/3/4/5: Stereo layer upon an axial layer/
		//              Axial layer beneath a stereo layer/
		//              Axial layer upon a stereo layer/
		//              Stereo layer beneath an axial layer/
		//        __________ each cell is a G4TwistedTube subtracted by a G4Hype
		//        |        | with 3 field wire and 1 signal wire inside
		//        |o0  x   | for TYPE 2/4, the innerward two field wires
		//        |        | have the rotate angle as half as the stereo layer
		//        |o1  o2  | 
		//        ---------- 
		G4double RelPhi, wire_pos_R;
		G4ThreeVector centerVec(1.,1.,1.);
		G4double theta;
		G4double wireR2Phi;
		wireR2Phi = asin( fieldWireR/cos(theta_RMin)/(RMin*cos(alpha_RMid)) );//in z section, wire looks like an ellipse
		for ( int i_row = 0; i_row < 2; i_row++ ){//0, inner; 1, middle
			if ( i_row == 0 && ( layer_type == 4 || layer_type == 2 ) ){
				continue;
			}
			for ( int i_line = 0; i_line < 2; i_line++ ){//0, left; 1, right
				G4LogicalVolume* log_wire;
				G4LogicalVolume* log_mother;
				G4String name;
				log_mother = log_cell;
				if ( i_row == 1 ){
					if ( i_line == 1 ){
						log_wire = log_SignalWire;
						name = SignalWireName;
					}
					else{
						log_wire = log_FieldWire_mid;
						name = FieldWireName;
					}
				}
				else{
					log_wire = log_FieldWire_in;
					name = FieldWireName;
				}
				if ( i_row == 0 ){
					wire_pos_R = RMin*cos(alpha_RMid);
					theta = theta_RMin;
				}
				else{
					wire_pos_R = RMid*cos(alpha_RMid);
					theta = theta_RMid;
				}
				if ( i_line == 0 ) RelPhi = wireR2Phi - cell_dphi/2;
				else RelPhi = wireR2Phi;
				G4RotationMatrix* rotateMatrix=new G4RotationMatrix();
				rotateMatrix->rotateZ(-RelPhi);
				rotateMatrix->rotateX(theta);	
				centerVec.setMag(wire_pos_R);
				centerVec.setTheta(pi/2);
				centerVec.setPhi(RelPhi);
				if ( get_VerboseLevel() >=10 ){
					std::cout<<"Placing ("<<i_row<<", "<<i_line<<"): rotateZ("<<-RelPhi/deg<<"deg), rotateX("<<-theta/deg<<"deg), R = "<<centerVec.mag()/mm<<"mm"<<std::endl;
				}
				new G4PVPlacement(rotateMatrix,centerVec,log_wire,name,log_mother,false,0,checkOverlap);
			}
		}
		if ( layer_type == 1 ){
			for ( int i_line = 0; i_line < 2; i_line++ ){//0, left; 1, right
				G4LogicalVolume* log_wire;
				G4LogicalVolume* log_mother;
				G4String name;
				log_wire = log_FieldWire_out;
				name = FieldWireName;
				log_mother = log_cell;
				wire_pos_R = m_GeometryParameter->get_Cdc_RMax()*cos(alpha_RMid);
				theta = theta_RMax;
				if ( i_line == 0 ) RelPhi = wireR2Phi - cell_dphi/2;
				else RelPhi = wireR2Phi;
				G4RotationMatrix* rotateMatrix=new G4RotationMatrix();
				rotateMatrix->rotateZ(-RelPhi);	
				rotateMatrix->rotateX(theta);
				centerVec.setMag(wire_pos_R);
				centerVec.setTheta(pi/2);
				centerVec.setPhi(RelPhi);
				if ( get_VerboseLevel() >=10 ){
					std::cout<<"Placing (2, "<<i_line<<"): rotateZ("<<-RelPhi/deg<<"deg), rotaeX("<<-theta/deg<<"deg), R = "<<centerVec.mag()/mm<<"mm"<<std::endl;
				}
				new G4PVPlacement(rotateMatrix,centerVec,log_wire,name,log_mother,false,0,checkOverlap);
			}
		}

		//====>Place cells in this layer
		//  together with wires that belong to Cdc container
		G4LogicalVolume* log_CdcContainer = get_logicalVolume(MVol_name);
		int cellNo = m_GeometryParameter->get_layer_cell_num(layerId);
		//		for ( int cellId = 0; cellId < 1; cellId++ ){
		for ( int cellId = 0; cellId < cellNo; cellId++ ){
			//place cell
			G4double SPhi = m_GeometryParameter->get_layer_cell_phi(layerId,cellId);
			G4double phi = SPhi - wireR2Phi;
			G4RotationMatrix* rotateMatrix=new G4RotationMatrix();
			rotateMatrix->rotateZ(-phi);
			int ReplicaNo = m_GeometryParameter->get_ReplicaNo( layerId, cellId );
			if ( get_VerboseLevel() >=10 ){
				std::cout<<"Placing cell: phi = "<<phi/deg<<"deg"<<std::endl;
			}
			new G4PVPlacement(rotateMatrix,G4ThreeVector(0.,0.,0.),log_cell,CellName,log_CdcContainer,false,ReplicaNo,checkOverlap);
			if ( layer_type == 2 || layer_type == 4 ){
				for ( int i_line = 0; i_line < 2; i_line++ ){//0, left; 1, right
					G4LogicalVolume* log_wire;
					G4LogicalVolume* log_mother;
					G4String name;
					log_wire = log_FieldWire_in;
					name = FieldWireName;
					log_mother = log_CdcContainer;
					if ( i_line == 0 ) RelPhi = wireR2Phi - cell_dphi/2;
					else RelPhi = wireR2Phi;
					G4double dPhi;
					if ( layer_type == 2 ) dPhi = alpha_RMin;
					else if ( layer_type == 4 ) dPhi = -alpha_RMin;
					wire_pos_R = RMin*cos(alpha_RMin);
					theta = atan(RMin*sin(alpha_RMin)/layer_halfzlen);
					G4RotationMatrix* rotMatrix=new G4RotationMatrix();
					rotMatrix->rotateZ(-phi-RelPhi-dPhi);	
					rotMatrix->rotateX(theta);
					centerVec.setMag(wire_pos_R);
					centerVec.setTheta(pi/2);
					centerVec.setPhi(phi+RelPhi+dPhi);
					new G4PVPlacement(rotMatrix,centerVec,log_wire,name,log_mother,false,0,checkOverlap);
					//std::cout<<"Placing inner field wire of layer["<<layerId<<"], type["<<layer_type<<"]:"<<std::endl;
					//std::cout<<"  Rc = "<<wire_pos_R/mm<<", theta = "<<theta/rad<<std::endl;
				}
			}
		}
	}//end loop in layers
}

//=>Place Volumes
G4VPhysicalVolume* CdcGeometrySvc::PlaceVolumes(){
	G4VPhysicalVolume* world_pvol = 0;
	G4VPhysicalVolume *former = SimpleGeometrySvc::PlaceVolumes();
	if (!world_pvol) world_pvol=former;
	return world_pvol;
}
