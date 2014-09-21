//---------------------------------------------------------------------------//
//Description: Handle database I/O and user interface 
//             for CDC geometry parameters
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 26 Aug, 2014
//Comment: The units are "mm"(default), "um"(for wire radius) and "rad". 
//Comment: Datum plain is upstream end plate of CDC
//Comment: fVerboseLevel: 0:    Just Warning Messages and Error Messages
//                        1:    + Dump()
//                        >= 5: All informations
//---------------------------------------------------------------------------//

#ifndef CdcGeometryParameter_H
#define CdcGeometryParameter_H 

#include "myglobals.hh"

#include "SimpleGeometryParameter.hh"

#include <vector>
#include <iostream>

class CdcGeometryParameter : public SimpleGeometryParameter
{

	public:
		CdcGeometryParameter(G4String name, G4String opt = "");
		virtual ~CdcGeometryParameter(void);

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
		G4double get_MaxStepLength() {return MaxStepLength;}

		//wire info
		G4String get_SignalWireMaterial() {return SignalWireMaterial;}
		G4String get_FieldWireMaterial() {return FieldWireMaterial;}
		G4String get_LayerMaterial() {return LayerMaterial;}
		G4double get_SignalWireRadius() {return SignalWireRadius;}
		G4double get_FieldWireRadius() {return FieldWireRadius;}

		//layer info
		G4int get_LayerNo() {return LayerNo;}
		G4double get_layer_length(G4int layerId) {
			if( check_layerId(layerId) ) return layer_length[layerId];
			else return 0;
		}
		G4double get_layer_Re(G4int layerId) { // radius on the end plate
			if( check_layerId(layerId) ) return layer_Re[layerId];
			else return 0;
		}
		G4double get_layer_Rc(G4int layerId) { // radius in the center
			if( check_layerId(layerId) ) return layer_Rc[layerId];
			else return 0;
		}
		G4double get_layer_Rz(G4int layerId, G4double deltaZ) { // radius on z plane
			double Rc = layer_Rc[layerId];
			double Rc2 = Rc*Rc;
			double hLength = layer_length[layerId]/2;
			double tanalpha = tan(layer_angle4rotate[layerId]/2);
			if( check_layerId(layerId) ){
				return sqrt(Rc2+deltaZ*deltaZ*Rc2*tanalpha*tanalpha/hLength/hLength);
			}
			else return 0;
		}
		G4int get_layer_firstWire(G4int layerId) { // in which hole to place the first field wire
			if( check_layerId(layerId) ) return layer_firstWire[layerId];
			else return 0;
		}
		G4int get_layer_ID(G4int layerId) { // in which hole to place the first field wire
			if( check_layerId(layerId) ) return layer_ID[layerId];
			else return 0;
		}
		G4double get_layer_SPhi(G4int layerId) { // phi position of the first field wire
			if( check_layerId(layerId) ) return layer_SPhi[layerId];
			else return 0;
		}
		G4double get_layer_phi0z(G4int layerId, G4double deltaZ) { // phi position of the first field wire at z plane
			if( check_layerId(layerId) ){
				return layer_SPhi[layerId]+layer_angle4stereo[layerId]/2-asin(sin(layer_angle4rotate[layerId])*(deltaZ/layer_length[layerId]));
			}
			else return 0;
		}
		G4int get_layer_type(G4int layerId) {
			if( check_layerId(layerId) ) return layer_type[layerId];
			else return 0;
		}
		G4double get_layer_SkipHoles(G4int layerId) {
			if( check_layerId(layerId) ) return layer_SkipHoles[layerId];
			else return 0;
		}
		G4double get_layer_angle4rotate( G4int layerId ){
			if( check_layerId(layerId) ) return layer_angle4rotate[layerId];
			else return 0;
		}
		G4double get_layer_angle4stereo( G4int layerId ){
			if( check_layerId(layerId) ) return layer_angle4stereo[layerId];
			else return 0;
		}
		G4int get_layer_HoleNo(G4int layerId) {
			if( check_layerId(layerId) ) return layer_HoleNo[layerId];
			else return 0;
		}
		G4double get_layer_holeDphi( G4int layerId ){
			if( check_layerId(layerId) ) return layer_holeDphi[layerId];
			else return 0;
		}

		//other settings
		bool get_vis_layer(){return vis_layer;}
		G4double get_r_layer(){return r_layer;}
		G4double get_g_layer(){return g_layer;}
		G4double get_b_layer(){return b_layer;}
		G4double get_t_layer(){return t_layer;}
		bool get_vis_FieldWire(){return vis_FieldWire;}
		bool get_vis_SignalWire(){return vis_SignalWire;}
		G4double get_r_FieldWire(){return r_FieldWire;}
		G4double get_g_FieldWire(){return g_FieldWire;}
		G4double get_b_FieldWire(){return b_FieldWire;}
		G4double get_t_FieldWire(){return t_FieldWire;}
		G4double get_r_SignalWire(){return r_SignalWire;}
		G4double get_g_SignalWire(){return g_SignalWire;}
		G4double get_b_SignalWire(){return b_SignalWire;}
		G4double get_t_SignalWire(){return t_SignalWire;}

	private:

		//=> check
		bool check_layerId(int layerId){
			if ( layerId < LayerNo ) return true;
			else{
				std::cout<<"In CdcGeometryParameter, layerId is out of range! layerId = "<<layerId<<", LayerNo = "<<LayerNo<<std::endl;
				return false;
			}
		}
		bool check_cellId(int layerId, int cellId){
			if ( cellId < layer_HoleNo[layerId] ) return true;
			else{
				std::cout<<"In CdcGeometryParameter, cellId is out of range! cellId = "<<cellId<<", cellNo= "<<layer_HoleNo[layerId]<<std::endl;
				return false;
			}
		}
		bool check_LayerNo( int No ){
			if( No != LayerNo ){
				std::cout<<"In CdcGeometryParameter::InitFromFile(), insufficient layer info! LayerNo = "<<LayerNo<<", while only "<<No<<" layers were found."<<std::endl;
				return false;
			}
			else return true;
		}

	private:
		//general info
		G4String MotherLogicalVolume;
		G4String SensitiveDetector;
		G4String SDVolumeName;
		G4double MaxStepLength;

		//wire info
		G4String SignalWireMaterial;
		G4String FieldWireMaterial;
		G4String LayerMaterial;
		G4double SignalWireRadius; //in um
		G4double FieldWireRadius; //in um

		//layer info
		G4int LayerNo;
		std::vector<G4double> layer_length;
		std::vector<G4double> layer_Rc;
		std::vector<G4double> layer_Re;
		std::vector<G4int> layer_firstWire;
		std::vector<G4int> layer_ID;
		std::vector<G4double> layer_SPhi;
		std::vector<G4double> layer_SkipHoles;
		std::vector<G4double> layer_angle4rotate;
		std::vector<G4double> layer_angle4stereo;
		std::vector<G4int> layer_HoleNo;
		std::vector<G4int> layer_type;
		std::vector<G4double> layer_holeDphi;

		//other settings
		bool vis_layer;
		G4double r_layer, g_layer, b_layer, t_layer;
		bool vis_FieldWire;
		bool vis_SignalWire;
		G4double r_SignalWire, g_SignalWire, b_SignalWire, t_SignalWire;
		G4double r_FieldWire, g_FieldWire, b_FieldWire, t_FieldWire;

};

#endif
