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
	if(	!check_LayerNo(layer_Re.size()) ||
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
	G4int tlayer_type, tlayer_ID, tlayer_SkipHoles, tlayer_firstWire, tlayer_HoleNo;
	G4double tlayer_Re, tlayer_length;
	std::string dump;
	if( name == "MotherLogicalVolume:" ) buf_card>>MotherLogicalVolume;
	else if( name == "CellMaterial:" ) buf_card>>CellMaterial;
	else if( name == "SensitiveDetector:" ) buf_card>>SensitiveDetector;
	else if( name == "SDVolumeName:" ) buf_card>>SDVolumeName;
	else if( name == "MinStepLength:" ) {buf_card>>MinStepLength;MinStepLength*=1e-3*mm;}
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
		if( name == "F" || name == "S" || name == "G" ){
			buf_card>>dump>>tlayer_ID>>tlayer_Re>>tlayer_length>>tlayer_HoleNo>>tlayer_SkipHoles>>tlayer_firstWire;
			tlayer_length *= cm;
			tlayer_Re *= cm;
			if (name=="F")
				layer_type.push_back(0);
			else if (name=="S")
				layer_type.push_back(1);
			else if (name=="G")
				layer_type.push_back(2);
			layer_ID.push_back(tlayer_ID);
			layer_Re.push_back(tlayer_Re);
			layer_length.push_back(tlayer_length);
			layer_HoleNo.push_back(tlayer_HoleNo);
			layer_SkipHoles.push_back(tlayer_SkipHoles);
			layer_firstWire.push_back(tlayer_firstWire);
			LayerNo++;
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
	layer_Rc.resize(LayerNo);
	layer_SPhi.resize(LayerNo);
	layer_angle4rotate.resize(LayerNo);
	layer_angle4stereo.resize(LayerNo);
	layer_holeDphi.resize(LayerNo);
	for ( G4int i = 0; i < LayerNo; i++ ){
		layer_holeDphi[i]=2*pi/layer_HoleNo[i];
		layer_angle4rotate[i]=layer_SkipHoles[i]*layer_holeDphi[i];
		layer_angle4stereo[i]=atan(layer_Re[i]*sin(layer_angle4rotate[i]/2)*2/layer_length[i]);
		layer_SPhi[i]=layer_holeDphi[i]*layer_firstWire[i];
		layer_Rc[i]=layer_Re[i]*cos(layer_angle4rotate[i]/2);
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
	std::cout<<" MinStepLength:        "<<MinStepLength/cm<<" cm"<<std::endl;

	std::cout<<"=>Wires info:"<<std::endl;
	std::cout<<" SignalWireRadius:     "<<SignalWireRadius/um<<" um"<<std::endl;
	std::cout<<" FieldWireRadius:      "<<FieldWireRadius/um<<" um"<<std::endl;
	std::cout<<" signalWire material:  "<<SignalWireMaterial<<std::endl;
	std::cout<<" fieldWire material:   "<<FieldWireMaterial<<std::endl;

	std::cout<<"=>Layers info:"<<std::endl;
	std::cout<<" LayerNo: "<<LayerNo<<std::endl;
	std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"T"
		<<std::setiosflags(std::ios::left)<<std::setw(5) <<"Iw"
		<<std::setiosflags(std::ios::left)<<std::setw(5) <<"Ic"
		<<std::setiosflags(std::ios::left)<<std::setw(5) <<"Re"
		<<std::setiosflags(std::ios::left)<<std::setw(5) <<"Rc"
		<<std::setiosflags(std::ios::left)<<std::setw(5) <<"L"
		<<std::setiosflags(std::ios::left)<<std::setw(5) <<"Nh"
		<<std::setiosflags(std::ios::left)<<std::setw(5) <<"skip"
		<<std::setiosflags(std::ios::left)<<std::setw(6) <<"first"
		<<std::setiosflags(std::ios::left)<<std::setw(6) <<"alpha"
		<<std::setiosflags(std::ios::left)<<std::setw(6) <<"beta"
		<<std::endl;
	std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
		<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
		<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
		<<std::setiosflags(std::ios::left)<<std::setw(5) <<"cm"
		<<std::setiosflags(std::ios::left)<<std::setw(5) <<"cm"
		<<std::setiosflags(std::ios::left)<<std::setw(5) <<"cm"
		<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
		<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
		<<std::setiosflags(std::ios::left)<<std::setw(6) <<""
		<<std::setiosflags(std::ios::left)<<std::setw(6) <<"rad"
		<<std::setiosflags(std::ios::left)<<std::setw(6) <<"rad"
		<<std::endl;
	for ( G4int i = 0; i < LayerNo; i++ ){
		std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<layer_type[i]<<" "
			<<std::setiosflags(std::ios::left)<<std::setw(5) <<i<<" "
			<<std::setiosflags(std::ios::left)<<std::setw(5) <<layer_ID[i]<<" "
			<<std::setiosflags(std::ios::left)<<std::setw(5) <<layer_Re[i]/cm<<" "
			<<std::setiosflags(std::ios::left)<<std::setw(5) <<layer_Rc[i]/cm<<" "
			<<std::setiosflags(std::ios::left)<<std::setw(5) <<layer_length[i]/cm<<" "
			<<std::setiosflags(std::ios::left)<<std::setw(5) <<layer_HoleNo[i]<<" "
			<<std::setiosflags(std::ios::left)<<std::setw(5) <<layer_SkipHoles[i]<<" "
			<<std::setiosflags(std::ios::left)<<std::setw(6) <<layer_firstWire[i]<<" "
			<<std::setiosflags(std::ios::left)<<std::setw(6) <<layer_angle4rotate[i]/rad<<" "
			<<std::setiosflags(std::ios::left)<<std::setw(6) <<layer_angle4stereo[i]/rad<<" "
			<<std::endl;
	}
}
