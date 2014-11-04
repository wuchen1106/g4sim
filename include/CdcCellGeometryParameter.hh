//---------------------------------------------------------------------------//
//Description: Handle database I/O and user interface 
//             for CDC geometry parameters
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 6 Oct, 2012
//Modified: 21 Oct, 2012
//Comment: The units are "mm"(default), "um"(for wire radius) and "rad". 
//Comment: for SPhi, Datum plain is downstream end plate of CDC
//Comment: about layer_type:
//         phi direction: 
//                0_________2pi
//
//         r   direction:
//                 ^  Cdc_RMax
//                 |
//                 |
//                 |  0
//
//         TYPE 0: Ordinary layer
//                 ________ each cell is a twisted(or not) tube with 
//                 |      | 3 field wires and 1 signal wire inside
//                 |o  x  |
//                 |      |
//                 |o  o  |
//                 --------
//
//         TYPE 1: Out most layer
//                 ________ each cell is a twisted(or not) tube with 
//                 |o  o  | 5 field wires and 1 signal wire inside
//                 |      |
//                 |o  x  |
//                 |      |
//                 |o  o  |
//                 --------
//
//         TYPE 2/3/4/5: Stereo layer upon an axial layer/
//                       Axial layer beneath a stereo layer/
//                       Axial layer upon a stereo layer/
//                       Stereo layer beneath an axial layer/
//                 __________ each cell is a G4TwistedTube subtracted by a G4Hype
//                 |        | with 3 field wire and 1 signal wire inside
//                 |o0  x   | for TYPE 2/4, the innerward two field wires
//                 |        | have the rotate angle as half as the stereo layer
//                 |o1  o2  | 
//                 ---------- 
//Comment: fVerboseLevel: 0:    Just Warning Messages and Error Messages
//                        1:    + Dump()
//                        >= 5: All informations
//---------------------------------------------------------------------------//

#ifndef CdcCellGeometryParameter_H
#define CdcCellGeometryParameter_H 

#include "myglobals.hh"

#include "SimpleGeometryParameter.hh"

#include <vector>
#include <iostream>

class CdcCellGeometryParameter : public SimpleGeometryParameter
{

	public:
		CdcCellGeometryParameter(G4String name, G4String opt = "");
		virtual ~CdcCellGeometryParameter(void);

		virtual void InitFromFile(G4String);  //Get necessary parameters from given file

		virtual void Calculate();

		virtual void Dump();

		//=> Special functions
		int GetValue(G4String);

		void Preset();

		bool CheckInfo();

		void DumpInfo(void);  //Output info

	private:

	public:

		//=>Access
		//general info
		G4String get_MotherLogicalVolume() {return MotherLogicalVolume;}
		G4String get_SensitiveDetector() {return SensitiveDetector;}
		G4String get_SDVolumeName() {return SDVolumeName;}

		//wire info
		G4String get_SignalWireMaterial() {return SignalWireMaterial;}
		G4String get_FieldWireMaterial() {return FieldWireMaterial;}
		G4String get_CellMaterial() {return CellMaterial;}
		G4double get_SignalWireRadius() {return SignalWireRadius;}
		G4double get_FieldWireRadius() {return FieldWireRadius;}

		//layer info
		G4int get_LayerNo() {return LayerNo;}
		G4double get_Cdc_RMax() {return Cdc_RMax;}
		G4double get_layer_length(G4int layerId) {
			if( check_layerId(layerId) ) return layer_length[layerId];
			else return 0;
		}
		G4double get_layer_RMin(G4int layerId) {
			if( check_layerId(layerId) ) return layer_RMin[layerId];
			else return 0;
		}
		G4double get_layer_RMid(G4int layerId) {
			if( check_layerId(layerId) ) return layer_RMid[layerId];
			else return 0;
		}
		G4double get_layer_SPhi(G4int layerId) {
			if( check_layerId(layerId) ) return layer_SPhi[layerId];
			else return 0;
		}
		G4double get_layer_ncells4rotate(G4int layerId) {
			if( check_layerId(layerId) ) return layer_ncells4rotate[layerId];
			else return 0;
		}
		G4double get_layer_angle4rotate( G4int layerId ){
			if( check_layerId(layerId) ) return layer_angle4rotate[layerId];
			else return 0;
		}
		G4int get_layer_cell_num(G4int layerId) {
			if( check_layerId(layerId) ) return layer_cell_num[layerId];
			else return 0;
		}
		G4int get_layer_firstWire(G4int layerId) {
			if( check_layerId(layerId) ) return layer_firstWire[layerId];
			else return 0;
		}
		G4int get_layer_type(G4int layerId) {
			if( check_layerId(layerId) ) return layer_type[layerId];
			else return 0;
		}
		G4double get_layer_cell_dphi( G4int layerId ){
			if( check_layerId(layerId) ) return layer_cell_dphi[layerId];
			else return 0;
		}
		G4int get_ReplicaNo( G4int layerId, G4int cellId ) {
			if( check_layerId(layerId) && check_cellId(layerId, cellId) ) return cellId + layer_cell_startNo[layerId];
			else return 0;
		}
		G4int get_layerIdcellId( G4int ReplicaNo, G4int* layerId, G4int* cellId ){
			for ( G4int i = 0; i <= LayerNo; i++ ){
				if ( ReplicaNo < layer_cell_startNo[i] ){
					*layerId = i - 1;
					*cellId = ReplicaNo - layer_cell_startNo[i-1];
					return 0;//found it
				}
			}
			return 1;//not found!
		}
		//phi at the middle
		G4double get_layer_cell_phi( G4int layerId, G4int cellId ){
			if( !check_layerId(layerId) || !check_cellId(layerId, cellId) ) return 0;
			G4double phi = layer_SPhi[layerId] + layer_angle4rotate[layerId]/2. + (cellId + ( 1. - layer_firstWire[layerId] )/2. )*layer_cell_dphi[layerId];
			return phi;
		}

		//other settings
		bool get_vis_layer(){return vis_layer;}
		G4double get_r_layer(){return r_layer;}
		G4double get_g_layer(){return g_layer;}
		G4double get_b_layer(){return b_layer;}
		bool get_vis_cell(){return vis_cell;}
		G4double get_r_cell(){return r_cell;}
		G4double get_g_cell(){return g_cell;}
		G4double get_b_cell(){return b_cell;}
		bool get_vis_FieldWire(){return vis_FieldWire;}
		bool get_vis_SignalWire(){return vis_SignalWire;}
		G4double get_r_FieldWire(){return r_FieldWire;}
		G4double get_g_FieldWire(){return g_FieldWire;}
		G4double get_b_FieldWire(){return b_FieldWire;}
		G4double get_r_SignalWire(){return r_SignalWire;}
		G4double get_g_SignalWire(){return g_SignalWire;}
		G4double get_b_SignalWire(){return b_SignalWire;}

	private:

		//=> check
		bool check_layerId(int layerId){
			if ( layerId < LayerNo ) return true;
			else{
				std::cout<<"In CdcCellGeometryParameter, layerId is out of range! layerId = "<<layerId<<", LayerNo = "<<LayerNo<<std::endl;
				return false;
			}
		}
		bool check_cellId(int layerId, int cellId){
			if ( cellId < layer_cell_num[layerId] ) return true;
			else{
				std::cout<<"In CdcCellGeometryParameter, cellId is out of range! cellId = "<<cellId<<", cellNo= "<<layer_cell_num[layerId]<<std::endl;
				return false;
			}
		}
		bool check_LayerNo( int No ){
			if( No != LayerNo ){
				std::cout<<"In CdcCellGeometryParameter::InitFromFile(), insufficient layer info! LayerNo = "<<LayerNo<<", whilie only "<<No<<" layers were found."<<std::endl;
				return false;
			}
			else return true;
		}

	private:
		//general info
		G4String MotherLogicalVolume;
		G4String SensitiveDetector;
		G4String SDVolumeName;

		//wire info
		G4String SignalWireMaterial;
		G4String FieldWireMaterial;
		G4String CellMaterial;
		G4double SignalWireRadius; //in um
		G4double FieldWireRadius; //in um

		//layer info
		G4int LayerNo;
		G4double Cdc_RMax;
		std::vector<G4double> layer_length;
		std::vector<G4double> layer_RMin;
		std::vector<G4double> layer_RMid;
		std::vector<G4double> layer_SPhi;
		std::vector<G4double> layer_ncells4rotate;
		std::vector<G4double> layer_angle4rotate;
		std::vector<G4int> layer_cell_num;
		std::vector<G4int> layer_firstWire;
		std::vector<G4int> layer_type;
		std::vector<G4int> layer_cell_startNo;
		std::vector<G4double> layer_cell_dphi;

		//other settings
		bool vis_layer;
		G4double r_layer, g_layer, b_layer;
		bool vis_cell;
		G4double r_cell, g_cell, b_cell;
		bool vis_FieldWire;
		bool vis_SignalWire;
		G4double r_SignalWire, g_SignalWire, b_SignalWire;
		G4double r_FieldWire, g_FieldWire, b_FieldWire;

};

#endif
