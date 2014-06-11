//Description: Handle database I/O and user interface 
//             for CDC geometry parameters
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 6 Oct, 2012
//Modified:
//Comment: The units are "mm"(default), "um"(for wire radius) and "rad". 
//Comment: for SPhi, Datum plain is forward end plate of CDC
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
//Comment: fVerboseLevel: 0:   Just Warning Messages and Error Messages
//                        >=5: All informations
//---------------------------------------------------------------------------//

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

#include "myglobals.hh"

#include "SimpleGeometryParameter.hh"
#include "CdcGeometryParameterMessenger.hh"

#include "CdcGeometryParameter.hh"

CdcGeometryParameter::CdcGeometryParameter(G4String name, G4String opt )
	: SimpleGeometryParameter(name, "S") // Use a not-empty opt to stop inherited class from creating new GeometryParameterMessenger
{
	if ( opt == "" ){
		CdcGeometryParameterMessenger *pointer = new CdcGeometryParameterMessenger(this, name);
		//std::cout<<"======>In CdcGeometryParameter, new CdcGeometryParameterMessenger at ("<<(void*)pointer<<")!"<<std::endl;
		set_GeometryParameterMessenger(pointer);
	}
}

CdcGeometryParameter::~CdcGeometryParameter(){
	printf("~CdcGeometryParameter");
}

//---------------------------InitFromFile---------------------------------
//will be called in CdcGeometrySvc::SetCdc
void CdcGeometryParameter::InitFromFile( G4String file_name ){
	set_filename(file_name);

	//Remember to preset first!
	Preset();

	std::ifstream fin_card(file_name);
	if(!fin_card){
		DEBUG("CdcGeometryParameter::InitFromFile(), cannot find subgeometry card: \""+file_name+"\"");
	}
	std::string s_card;
	while(getline(fin_card,s_card)){
		//eleminate useless lines
		if ( ISEMPTY(s_card) ) continue;
		int status = GetValue(s_card);
		if( status > 0 ){
			G4String content = "CdcGeometryParameter: unknown line:" + s_card + "\n  in file " + file_name + "! Will ignore this line!";
			DEBUG(content,0);
		}
	}
	fin_card.close();
	if (CheckInfo()){
		DEBUG("CdcGeometryParameter::InitFromFile(), insufficient simple solid info");
	}
	Calculate();
}

void CdcGeometryParameter::Dump(){
	DumpInfo();
}

//-----------------------------------Special functions---------------------------------------------

bool CdcGeometryParameter::CheckInfo(){
	bool flag = false;// false means no problem
	flag = SimpleGeometryParameter::CheckInfo();
	if (flag) return flag;// if something is wrong with its Parent Class, then return the status.

	//examine
	if(	!check_LayerNo(layer_RMin.size()) ||
			!check_LayerNo(layer_length.size()) ){
		flag = true;
	}
	return flag;
}

void CdcGeometryParameter::Preset(){
	SimpleGeometryParameter::Preset();// Call its Parent class to preset
	vis_cell = false;
	vis_layer = false;
	vis_SignalWire = false;
	vis_FieldWire = false;
	LayerNo = 0;
}


int CdcGeometryParameter::GetValue(G4String s_card){
	if (!SimpleGeometryParameter::GetValue(s_card)) return 0; // if found by Parent Class, then return.
	int status = 0;// 0 means found the value
	std::stringstream buf_card;
	buf_card.str("");
	buf_card.clear();
	buf_card<<s_card;
	std::string name;
	buf_card>>name;
	std::string s_para;
	if( name == "MotherLogicalVolume:" ) buf_card>>MotherLogicalVolume;
	else if( name == "CellMaterial:" ) buf_card>>CellMaterial;
	else if( name == "SensitiveDetector:" ) buf_card>>SensitiveDetector;
	else if( name == "SDVolumeName:" ) buf_card>>SDVolumeName;
	else if( name == "SignalWireMaterial:" ) buf_card>>SignalWireMaterial;
	else if( name == "FieldWireMaterial:" ) buf_card>>FieldWireMaterial;
	else if( name == "SignalWireRadius:" ) {
		buf_card>>SignalWireRadius;
		SignalWireRadius *= um;
	}
	else if( name == "FieldWireRadius:" ) {
		buf_card>>FieldWireRadius;
		FieldWireRadius *= um;
	}
	else if( name == "LayerNo:" ) buf_card>>LayerNo;
	else if( name == "vis_layer" ){
		vis_layer = true;
		buf_card>>r_layer>>g_layer>>b_layer;
	}
	else if( name == "vis_cell" ){
		vis_cell = true;
		buf_card>>r_cell>>g_cell>>b_cell;
	}
	else if( name == "vis_FieldWire" ){
		vis_FieldWire = true;
		buf_card>>r_FieldWire>>g_FieldWire>>b_FieldWire;
	}
	else if( name == "vis_SignalWire" ){
		vis_SignalWire = true;
		buf_card>>r_SignalWire>>g_SignalWire>>b_SignalWire;
	}
	else{
		std::stringstream buf_temp;
		if( name == "F" ){
			std::string dump;
			G4double tlayer_RMin;
			buf_card>>tlayer_RMin;
			tlayer_RMin *= mm;
			layer_RMin.push_back(tlayer_RMin);
		}
		else if( name == "S" ){
			std::string dump;
			G4int twire_num, tlayer_firstWire;
			G4double tlayer_ncells4rotate;
			G4double tlayer_RMid, tlayer_length, tlayer_SPhi;
			buf_card>>dump>>dump>>twire_num>>tlayer_length>>tlayer_RMid>>tlayer_SPhi>>tlayer_firstWire>>tlayer_ncells4rotate;
			tlayer_length *= mm;
			tlayer_RMid *= mm;
			tlayer_SPhi *= rad;
			layer_cell_num.push_back(twire_num);
			layer_length.push_back(tlayer_length);
			layer_RMid.push_back(tlayer_RMid);
			layer_SPhi.push_back(tlayer_SPhi);
			layer_firstWire.push_back(tlayer_firstWire);
			layer_ncells4rotate.push_back(tlayer_ncells4rotate);
		}
		else if( name == "LF" ){
			std::string dump;
			buf_card>>Cdc_RMax;
		}
		else{
			status = 1;
		}
		buf_temp.str("");
		buf_temp.clear();
	}
	buf_card.str("");
	buf_card.clear();
	return status;
}

//--------------------------Calculate---------------------------------
//
void CdcGeometryParameter::Calculate(){
	SimpleGeometryParameter::Calculate();
	layer_angle4rotate.resize(LayerNo);
	layer_type.resize(LayerNo);
	layer_cell_startNo.resize(LayerNo+1);
	layer_cell_dphi.resize(LayerNo);
	for ( G4int i = 0; i < LayerNo; i++ ){
		//set layer_cell_startNo 
		if ( i == 0 ){
			layer_cell_startNo[i] = 0;
		}
		else{
			layer_cell_startNo[i] = layer_cell_startNo[i-1] + layer_cell_num[i-1];
		}
		if ( i == LayerNo - 1 ){
			layer_cell_startNo[i+1] = layer_cell_startNo[i] + layer_cell_num[i];
		}
		//set layer_angle4rotate
		layer_angle4rotate[i] = 2*pi*layer_ncells4rotate[i]/layer_cell_num[i];
		//set layer_cell_dphi
		layer_cell_dphi[i] = 2*pi/layer_cell_num[i];
		G4double R;
		if (i == LayerNo - 1) R = Cdc_RMax;
		else R = layer_RMin[i+1];
		//set layer_type
		if ( i == LayerNo -1 ){
			layer_type[i] = 1;//TYPE 1: Out most layer
		}
		else if ( fabs(layer_ncells4rotate[i]) != fabs(layer_ncells4rotate[i+1]) ){
			if ( layer_ncells4rotate[i] == 0 ){
				layer_type[i] = 3;//TYPE 3: Axial layer beneath a stereo layer
			}
			else{
				layer_type[i] = 5;//TYPE 5: Stereo layer beneath an axial layer
			}
		}
		else if ( i != 0 ? fabs(layer_ncells4rotate[i]) != fabs(layer_ncells4rotate[i-1]) : 0 ){
			if ( layer_ncells4rotate[i] == 0 ){
				layer_type[i] = 4;//TYPE 4: Axial layer upon a stereo layer
			}
			else{
				layer_type[i] = 2;//TYPE 2: Stereo layer upon an axial layer
			}
		}
		else{
			layer_type[i] = 0;//TYPE 0: Ordinary layer
		}
	}
}

//---------------------------DumpInfo---------------------------------
//output information
void CdcGeometryParameter::DumpInfo() {
	SimpleGeometryParameter::DumpInfo();// dump info of its Parent Class first
	std::cout<<"------CdcGeometry info :--------"<<std::endl;
	std::cout<<"=>General info:"<<std::endl;
	std::cout<<" MotherLogicalVolume:  "<<MotherLogicalVolume<<std::endl;
	std::cout<<" SensitiveDetector:    "<<SensitiveDetector<<std::endl;
	std::cout<<" SDVolumeName:         "<<SDVolumeName<<std::endl;

	std::cout<<"=>Wires info:"<<std::endl;
	std::cout<<" SignalWireRadius:     "<<SignalWireRadius/um<<"um"<<std::endl;
	std::cout<<" FieldWireRadius:      "<<FieldWireRadius/um<<"um"<<std::endl;
	std::cout<<" signalWire material:  "<<SignalWireMaterial<<std::endl;
	std::cout<<" fieldWire material:   "<<FieldWireMaterial<<std::endl;

	std::cout<<"=>Layers info:"<<std::endl;
	std::cout<<" LayerNo: "<<LayerNo<<std::endl;
	std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
		<<std::setiosflags(std::ios::left)<<std::setw(7) <<"StartR"
		<<std::setiosflags(std::ios::left)<<std::setw(8) <<"MiddleR"
		<<std::setiosflags(std::ios::left)<<std::setw(7) <<"length"
		<<std::setiosflags(std::ios::left)<<std::setw(9) <<"StartPhi"
		<<std::setiosflags(std::ios::left)<<std::setw(11)<<"RotateCell"
		<<std::setiosflags(std::ios::left)<<std::setw(17)<<"FirstWire"
		<<std::setiosflags(std::ios::left)<<std::setw(7) <<"CellNo"
		<<std::setiosflags(std::ios::left)<<std::setw(5) <<"type"
		<<std::endl;
	std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
		<<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
		<<std::setiosflags(std::ios::left)<<std::setw(8) <<"mm"
		<<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
		<<std::setiosflags(std::ios::left)<<std::setw(9) <<"rad"
		<<std::setiosflags(std::ios::left)<<std::setw(11)<<""
		<<std::setiosflags(std::ios::left)<<std::setw(17)<<"1:signal/0:field"
		<<std::setiosflags(std::ios::left)<<std::setw(7) <<""
		<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
		<<std::endl;
	for ( G4int i = 0; i < LayerNo; i++ ){
		std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<layer_RMin[i]/mm
			<<std::setiosflags(std::ios::left)<<std::setw(8) <<layer_RMid[i]/mm
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<layer_length[i]/mm
			<<std::setiosflags(std::ios::left)<<std::setw(9) <<layer_SPhi[i]/rad
			<<std::setiosflags(std::ios::left)<<std::setw(11)<<layer_ncells4rotate[i]
			<<std::setiosflags(std::ios::left)<<std::setw(17)<<layer_firstWire[i]
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<layer_cell_num[i]
			<<std::setiosflags(std::ios::left)<<std::setw(5) <<layer_type[i]
			<<std::endl;
	}
	std::cout<<"Out most radius: "<<Cdc_RMax<<"mm"<<std::endl;
}
