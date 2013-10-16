//Description: In charge of FormulizedGeometryParameter
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 1 Nov, 2012
//Modified:
//Comment: The units are "mm"(default) and "rad". 
//Comment: fVerboseLevel: 0:   Just Warning Messages and Error Messages
//                        >=5: All informations
//---------------------------------------------------------------------------//

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

#include "globals.hh"

#include <CLHEP/Units/PhysicalConstants.h>

#include "TF1.h"

#include "FormulizedGeometryParameterMessenger.hh"
#include "SimpleGeometryParameter.hh"

#include "FormulizedGeometryParameter.hh"

FormulizedGeometryParameter::FormulizedGeometryParameter(G4String name, G4String opt )
	: SimpleGeometryParameter(name, "S") // Use a not-empty opt to stop inherited class from creating new MyVGeometryParameterMessenger
{
	if ( opt == "" ){
		FormulizedGeometryParameterMessenger *pointer = new FormulizedGeometryParameterMessenger(this, name);
		//		std::cout<<"======>In FormulizedGeometryParameter, new FormulizedGeometryParameterMessenger at ("<<(void*)pointer<<")!"<<std::endl;
		set_GeometryParameterMessenger(pointer);
	}
}

FormulizedGeometryParameter::~FormulizedGeometryParameter(){
	printf("~FormulizedGeometryParameter");
}

//---------------------------InitFromFile---------------------------------
//will be called in FormulizedGeometrySvc::SetFormulizedGeometry
void FormulizedGeometryParameter::InitFromFile( G4String file_name ){
	set_filename(file_name);

	//Remember to preset first!
	Preset();

	std::ifstream fin_card(file_name);
	if(!fin_card){
		DEBUG("FormulizedGeometryParameter::InitFromFile(), cannot find output card");
	}
	std::string s_card;
	while(getline(fin_card,s_card)){
		//eleminate useless lines
		if ( ISEMPTY(s_card) ) continue;
		int status = GetValue(s_card);
		if( status > 0 ){
			G4String content = "FormulizedGeometryParameter: unknown line:" + s_card + "\n  in file " + file_name + "! Will ignore this line!";
			DEBUG(content,0);
		}
	}
	fin_card.close();
	if (CheckInfo()){
		DEBUG("FormulizedGeometryParameter::InitFromFile(), insufficient simple solid info");
	}
	Calculate();
}

void FormulizedGeometryParameter::Dump(){
	DumpInfo();
}

//-----------------------------------Special functions---------------------------------------------
void FormulizedGeometryParameter::Calculate(){
	TString TF1Name;
	TString TF1Form;
	SimpleGeometryParameter::Calculate();
	for ( int VolId = 0; VolId < VolNo; VolId++ ){
		std::vector<G4double> tPosX; tPosX.clear();
		std::vector<G4double> tPosY; tPosY.clear();
		std::vector<G4double> tPosZ; tPosZ.clear();
		std::vector<G4double> tEphi; tEphi.clear();
		std::vector<G4double> tEtheta; tEtheta.clear();
		std::vector<G4double> tEpsi; tEpsi.clear();
		TF1 *formulaPosX = new TF1(TF1Name="PosX", TF1Form=fPosX[VolId]);
		TF1 *formulaPosY = new TF1(TF1Name="PosY", TF1Form=fPosY[VolId]);
		TF1 *formulaPosZ = new TF1(TF1Name="PosZ", TF1Form=fPosZ[VolId]);
		TF1 *formulaEphi = new TF1(TF1Name="Ephi", TF1Form=fEphi[VolId]);
		TF1 *formulaEtheta = new TF1(TF1Name="Etheta", TF1Form=fEtheta[VolId]);
		TF1 *formulaEpsi = new TF1(TF1Name="Epsi", TF1Form=fEpsi[VolId]);
		for ( int RepId = 0; RepId < RepNo[VolId]; RepId++ ){
			G4double valPosX = formulaPosX->Eval(RepId)*mm; tPosX.push_back(valPosX);
			G4double valPosY = formulaPosY->Eval(RepId)*mm; tPosY.push_back(valPosY);
			G4double valPosZ = formulaPosZ->Eval(RepId)*mm; tPosZ.push_back(valPosZ);
			G4double valEphi = formulaEphi->Eval(RepId)*deg; tEphi.push_back(valEphi);
			G4double valEtheta = formulaEtheta->Eval(RepId)*deg; tEtheta.push_back(valEtheta);
			G4double valEpsi = formulaEpsi->Eval(RepId)*deg; tEpsi.push_back(valEpsi);
		}
		delete formulaPosX;
		delete formulaPosY;
		delete formulaPosZ;
		delete formulaEphi;
		delete formulaEtheta;
		delete formulaEpsi;
		PosX.push_back(tPosX);
		PosY.push_back(tPosY);
		PosZ.push_back(tPosZ);
		Ephi.push_back(tEphi);
		Etheta.push_back(tEtheta);
		Epsi.push_back(tEpsi);
		if ( SolidType[VolId] == "Box" ){
			std::vector<G4double> tBox_X; tBox_X.clear();
			std::vector<G4double> tBox_Y; tBox_Y.clear();
			std::vector<G4double> tBox_Z; tBox_Z.clear();
			TF1 *formulaBox_X = new TF1(TF1Name="Box_X", TF1Form=fBox_X[VolId]);
			TF1 *formulaBox_Y = new TF1(TF1Name="Box_Y", TF1Form=fBox_Y[VolId]);
			TF1 *formulaBox_Z = new TF1(TF1Name="Box_Z", TF1Form=fBox_Z[VolId]);
			for ( int RepId = 0; RepId < RepNo[VolId]; RepId++ ){
				G4double valBox_X = formulaBox_X->Eval(RepId)*mm; tBox_X.push_back(valBox_X);
				G4double valBox_Y = formulaBox_Y->Eval(RepId)*mm; tBox_Y.push_back(valBox_Y);
				G4double valBox_Z = formulaBox_Z->Eval(RepId)*mm; tBox_Z.push_back(valBox_Z);
			}
			delete formulaBox_X;
			delete formulaBox_Y;
			delete formulaBox_Z;
			Box_X.push_back(tBox_X);
			Box_Y.push_back(tBox_Y);
			Box_Z.push_back(tBox_Z);
		}
		else if ( SolidType[VolId] == "Tubs" ){
			std::vector<G4double> tTubs_RMax; tTubs_RMax.clear();
			std::vector<G4double> tTubs_RMin; tTubs_RMin.clear();
			std::vector<G4double> tTubs_Length; tTubs_Length.clear();
			std::vector<G4double> tTubs_StartAng; tTubs_StartAng.clear();
			std::vector<G4double> tTubs_SpanAng; tTubs_SpanAng.clear();
			TF1 *formulaTubs_RMax = new TF1(TF1Name="Tubs_RMax", TF1Form=fTubs_RMax[VolId]);
			TF1 *formulaTubs_RMin = new TF1(TF1Name="Tubs_RMin", TF1Form=fTubs_RMin[VolId]);
			TF1 *formulaTubs_Length = new TF1(TF1Name="Tubs_Length", TF1Form=fTubs_Length[VolId]);
			TF1 *formulaTubs_StartAng = new TF1(TF1Name="Tubs_StartAng", TF1Form=fTubs_StartAng[VolId]);
			TF1 *formulaTubs_SpanAng = new TF1(TF1Name="Tubs_SpanAng", TF1Form=fTubs_SpanAng[VolId]);
			for ( int RepId = 0; RepId < RepNo[VolId]; RepId++ ){
				G4double valTubs_RMax = formulaTubs_RMax->Eval(RepId)*mm; tTubs_RMax.push_back(valTubs_RMax);
				G4double valTubs_RMin = formulaTubs_RMin->Eval(RepId)*mm; tTubs_RMin.push_back(valTubs_RMin);
				G4double valTubs_Length = formulaTubs_Length->Eval(RepId)*mm; tTubs_Length.push_back(valTubs_Length);
				G4double valTubs_StartAng = formulaTubs_StartAng->Eval(RepId)*deg; tTubs_StartAng.push_back(valTubs_StartAng);
				G4double valTubs_SpanAng = formulaTubs_SpanAng->Eval(RepId)*deg; tTubs_SpanAng.push_back(valTubs_SpanAng);
			}
			delete formulaTubs_RMax;
			delete formulaTubs_RMin;
			delete formulaTubs_Length;
			delete formulaTubs_StartAng;
			delete formulaTubs_SpanAng;
			Tubs_RMax.push_back(tTubs_RMax);
			Tubs_RMin.push_back(tTubs_RMin);
			Tubs_Length.push_back(tTubs_Length);
			Tubs_StartAng.push_back(tTubs_StartAng);
			Tubs_SpanAng.push_back(tTubs_SpanAng);
		}
		else if ( SolidType[VolId] == "Sphere" ){
			std::vector<G4double> tSphere_RMax; tSphere_RMax.clear();
			std::vector<G4double> tSphere_RMin; tSphere_RMin.clear();
			std::vector<G4double> tSphere_StartPhi; tSphere_StartPhi.clear();
			std::vector<G4double> tSphere_SpanPhi; tSphere_SpanPhi.clear();
			std::vector<G4double> tSphere_StartTheta; tSphere_StartTheta.clear();
			std::vector<G4double> tSphere_SpanTheta; tSphere_SpanTheta.clear();
			TF1 *formulaSphere_RMax = new TF1(TF1Name="Sphere_RMax", TF1Form=fSphere_RMax[VolId]);
			TF1 *formulaSphere_RMin = new TF1(TF1Name="Sphere_RMin", TF1Form=fSphere_RMin[VolId]);
			TF1 *formulaSphere_StartPhi = new TF1(TF1Name="Sphere_StartPhi", TF1Form=fSphere_StartPhi[VolId]);
			TF1 *formulaSphere_SpanPhi = new TF1(TF1Name="Sphere_SpanPhi", TF1Form=fSphere_SpanPhi[VolId]);
			TF1 *formulaSphere_StartTheta = new TF1(TF1Name="Sphere_StartTheta", TF1Form=fSphere_StartTheta[VolId]);
			TF1 *formulaSphere_SpanTheta = new TF1(TF1Name="Sphere_SpanTheta", TF1Form=fSphere_SpanTheta[VolId]);
			for ( int RepId = 0; RepId < RepNo[VolId]; RepId++ ){
				G4double valSphere_RMax = formulaSphere_RMax->Eval(RepId)*mm; tSphere_RMax.push_back(valSphere_RMax);
				G4double valSphere_RMin = formulaSphere_RMin->Eval(RepId)*mm; tSphere_RMin.push_back(valSphere_RMin);
				G4double valSphere_StartPhi = formulaSphere_StartPhi->Eval(RepId)*deg; tSphere_StartPhi.push_back(valSphere_StartPhi);
				G4double valSphere_SpanPhi = formulaSphere_SpanPhi->Eval(RepId)*deg; tSphere_SpanPhi.push_back(valSphere_SpanPhi);
				G4double valSphere_StartTheta = formulaSphere_StartTheta->Eval(RepId)*deg; tSphere_StartTheta.push_back(valSphere_StartTheta);
				G4double valSphere_SpanTheta = formulaSphere_SpanTheta->Eval(RepId)*deg; tSphere_SpanTheta.push_back(valSphere_SpanTheta);
			}
			delete formulaSphere_RMax;
			delete formulaSphere_RMin;
			delete formulaSphere_StartPhi;
			delete formulaSphere_SpanPhi;
			delete formulaSphere_StartTheta;
			delete formulaSphere_SpanTheta;
			Sphere_RMax.push_back(tSphere_RMax);
			Sphere_RMin.push_back(tSphere_RMin);
			Sphere_StartPhi.push_back(tSphere_StartPhi);
			Sphere_SpanPhi.push_back(tSphere_SpanPhi);
			Sphere_StartTheta.push_back(tSphere_StartTheta);
			Sphere_SpanTheta.push_back(tSphere_SpanTheta);
		}
		else{
			DEBUG("In Calculate, unknown Solid type!!!");
		}
	}
}

bool FormulizedGeometryParameter::CheckInfo(){
	bool flag = false;// false means no problem
	flag = SimpleGeometryParameter::CheckInfo();
	if (flag) return flag;// if something is wrong with its Parent Class, then return the status.

	//examine
	return flag;
}

void FormulizedGeometryParameter::Preset(){
	SimpleGeometryParameter::Preset();// Call its Parent class to preset
	notReSetVis = true;
	VolNo = 0;
	BoxNo = 0;
	TubsNo = 0;
	SphereNo = 0;
	//General info about volume
	fPosX.clear();
	fPosY.clear();
	fPosZ.clear();
	fEphi.clear();
	fEtheta.clear();
	fEpsi.clear();
	PosX.clear();
	PosY.clear();
	PosZ.clear();
	Ephi.clear();
	Etheta.clear();
	Epsi.clear();
	Name.clear();
	MotherName.clear();
	SDName.clear();
	Material.clear();
	SRepNo.clear();
	RepNo.clear();
	SolidType.clear();
	SolidIndex.clear();

	//Box info
	fBox_X.clear();
	fBox_Y.clear();
	fBox_Z.clear();
	Box_X.clear();
	Box_Y.clear();
	Box_Z.clear();
	Box_GenIndex.clear();

	//Tubs info
	fTubs_RMax.clear();
	fTubs_RMin.clear();
	fTubs_Length.clear();
	fTubs_StartAng.clear();
	fTubs_SpanAng.clear();
	Tubs_RMax.clear();
	Tubs_RMin.clear();
	Tubs_Length.clear();
	Tubs_StartAng.clear();
	Tubs_SpanAng.clear();
	Tubs_GenIndex.clear();

	//Sphere info
	fSphere_RMax.clear();
	fSphere_RMin.clear();
	fSphere_StartPhi.clear();
	fSphere_SpanPhi.clear();
	fSphere_StartTheta.clear();
	fSphere_SpanTheta.clear();
	Sphere_RMax.clear();
	Sphere_RMin.clear();
	Sphere_StartPhi.clear();
	Sphere_SpanPhi.clear();
	Sphere_StartTheta.clear();
	Sphere_SpanTheta.clear();
	Sphere_GenIndex.clear();

	//visual settings
	vVis.clear();
	vR.clear();
	vG.clear();
	vB.clear();
}

int FormulizedGeometryParameter::GetValue(G4String s_card){
	if (!SimpleGeometryParameter::GetValue(s_card)) return 0; // if found by Parent Class, then return.
	int status = 0;// 0 means found the value
	std::stringstream buf_card;
	buf_card.str("");
	buf_card.clear();
	buf_card<<s_card;
	std::string name;
	buf_card>>name;
	std::string s_para;
	int iVol = VolNo;
	if( name == "fB" || name == "fT" || name == "fSph" ){
		if( name == "fB" ){
			SolidType.push_back("Box");
			SolidIndex.push_back(Box_X.size());
			std::string dump;
			G4String fmlBox_X;
			G4String fmlBox_Y;
			G4String fmlBox_Z;
			buf_card>>dump>>fmlBox_X>>fmlBox_Y>>fmlBox_Z;
			fBox_X.push_back(fmlBox_X);
			fBox_Y.push_back(fmlBox_Y);
			fBox_Z.push_back(fmlBox_Z);
			Box_GenIndex.push_back(iVol);
			BoxNo++;
		}
		else if( name == "fT" ){
			SolidType.push_back("Tubs");
			SolidIndex.push_back(Tubs_RMax.size());
			std::string dump;
			G4String fmlTubs_RMax;
			G4String fmlTubs_RMin;
			G4String fmlTubs_Length;
			G4String fmlTubs_StartAng;
			G4String fmlTubs_SpanAng;
			buf_card>>dump>>fmlTubs_RMin>>fmlTubs_RMax>>fmlTubs_Length>>fmlTubs_StartAng>>fmlTubs_SpanAng;
			fTubs_RMax.push_back(fmlTubs_RMax);
			fTubs_RMin.push_back(fmlTubs_RMin);
			fTubs_Length.push_back(fmlTubs_Length);
			fTubs_StartAng.push_back(fmlTubs_StartAng);
			fTubs_SpanAng.push_back(fmlTubs_SpanAng);
			Tubs_GenIndex.push_back(iVol);
			TubsNo++;
		}
		else if( name == "fSph" ){
			SolidType.push_back("Sphere");
			SolidIndex.push_back(Sphere_RMax.size());
			std::string dump;
			G4String fmlSphere_RMax;
			G4String fmlSphere_RMin;
			G4String fmlSphere_StartPhi;
			G4String fmlSphere_SpanPhi;
			G4String fmlSphere_StartTheta;
			G4String fmlSphere_SpanTheta;
			buf_card>>dump>>fmlSphere_RMin>>fmlSphere_RMax>>fmlSphere_StartPhi>>fmlSphere_SpanPhi>>fmlSphere_StartTheta>>fmlSphere_SpanTheta;
			fSphere_RMax.push_back(fmlSphere_RMax);
			fSphere_RMin.push_back(fmlSphere_RMin);
			fSphere_StartPhi.push_back(fmlSphere_StartPhi);
			fSphere_SpanPhi.push_back(fmlSphere_SpanPhi);
			fSphere_StartTheta.push_back(fmlSphere_StartTheta);
			fSphere_SpanTheta.push_back(fmlSphere_SpanTheta);
			Sphere_GenIndex.push_back(iVol);
			SphereNo++;
		}
		G4String tPosX;
		G4String tPosY;
		G4String tPosZ;
		G4String tEphi;
		G4String tEtheta;
		G4String tEpsi;
		G4String tRepCont;
		G4int tRepNo;
		G4int tSRepNo;
		G4String tSDName;
		std::string tName;
		std::string tMotherName;
		std::string tMaterial;
		buf_card>>tPosX>>tPosY>>tPosZ>>tName>>tMotherName>>tMaterial>>tSDName>>tRepCont>>tEphi>>tEtheta>>tEpsi;
		get_RepCont(tRepCont,tSRepNo,tRepNo);
		fPosX.push_back(tPosX);
		fPosY.push_back(tPosY);
		fPosZ.push_back(tPosZ);
		fEphi.push_back(tEphi);
		fEtheta.push_back(tEtheta);
		fEpsi.push_back(tEpsi);
		Name.push_back(tName);
		MotherName.push_back(tMotherName);
		Material.push_back(tMaterial);
		SRepNo.push_back(tSRepNo);
		RepNo.push_back(tRepNo);
		SDName.push_back(tSDName);
		VolNo++;
	}
	else if ( name == "VISSETTING" ){
		if(notReSetVis) ReSetVis();
		status = -1;
	}
	else{
		bool foundornot = false;
		if ( name.substr(0,4) == "vis_" ){
			if(notReSetVis) ReSetVis();
			std::stringstream buf_temp;
			for ( G4int i = 0; i < VolNo; i++ ){
				buf_temp.str("");
				buf_temp.clear();
				buf_temp<<"vis_"<<Name[i];
				if( name == buf_temp.str() ){
					set_vis(i,true);
					G4double vTr, vTg, vTb;
					G4double vTT = 0;
					buf_card>>vTr>>vTg>>vTb>>vTT;
					if (!vTT) vTT = 1;
					set_r(i,vTr);
					set_g(i,vTg);
					set_b(i,vTb);
					set_t(i,vTT);
					foundornot = true;
					break; // got the parameter, go to next line
				}
			}
			buf_temp.str("");
			buf_temp.clear();
		}
		if (!foundornot){
			status = 1;
		}
	}
	buf_card.str("");
	buf_card.clear();
	return status;
}

//---------------------------ReSetVis---------------------------------
//
void FormulizedGeometryParameter::ReSetVis() {
	std::cout<<"In FormulizedGeometryParameter::ReSetVis(): VolNo = "<<VolNo<<std::endl;
	vVis.resize(VolNo);
	vR.resize(VolNo);
	vG.resize(VolNo);
	vB.resize(VolNo);
	for ( G4int i = 0; i < VolNo; i++ ){
		vVis[i] = false;
	}
	notReSetVis = false;
}

//---------------------------DumpInfo---------------------------------
//output information
void FormulizedGeometryParameter::DumpInfo() {
	SimpleGeometryParameter::DumpInfo();// dump info of its Parent Class first
	std::cout<<"------ FormulizedGeometry info :--------"<<std::endl;
	for( G4int i = 0; i < BoxNo; i++ ){
		if ( i == 0 ){
			std::cout<<"=>Box info:"<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
				<<std::setiosflags(std::ios::left)<<std::setw(6) <<"RepNo"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"x"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"y"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"z"
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<"Name"
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<"MotherName"
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<"Matierial"
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<"SDName"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"PosX"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"PosY"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"PosZ"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"Ephi"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"Etheta"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"Epsi"
				<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(6) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<""
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<""
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<""
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<""
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(6) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
				<<std::endl;
		}
		int index = Box_GenIndex[i];
		int repNo = RepNo[index];
		for ( G4int j = 0; j < repNo; j++ ){
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
				<<std::setiosflags(std::ios::left)<<std::setw(6) <<j+SRepNo[index]
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Box_X[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Box_Y[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Box_Z[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<Name[index]
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<MotherName[index]
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<Material[index]
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<SDName[index]
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<PosX[index][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<PosY[index][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<PosZ[index][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<Ephi[index][j]/deg
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<Etheta[index][j]/deg
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<Epsi[index][j]/deg
				<<std::endl;
		}
	}

	for( G4int i = 0; i < TubsNo; i++ ){
		if ( i == 0 ){
			std::cout<<"=>Tubs info:"<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
				<<std::setiosflags(std::ios::left)<<std::setw(6) <<"RepNo"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"RMin"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"RMax"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"Length"
				<<std::setiosflags(std::ios::left)<<std::setw(9) <<"StartAng"
				<<std::setiosflags(std::ios::left)<<std::setw(8) <<"SpanAng"
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<"Name"
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<"MotherName"
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<"Matierial"
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<"SDName"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"PosX"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"PosY"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"PosZ"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"Ephi"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"Etheta"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"Epsi"
				<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(6) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(9) <<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(8) <<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<""
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<""
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<""
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<""
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
				<<std::endl;
		}
		int index = Tubs_GenIndex[i];
		int repNo = RepNo[index];
		for ( G4int j = 0; j < repNo; j++ ){
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
				<<std::setiosflags(std::ios::left)<<std::setw(6) <<j+SRepNo[index]
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Tubs_RMin[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Tubs_RMax[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Tubs_Length[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(9) <<Tubs_StartAng[i][j]/deg
				<<std::setiosflags(std::ios::left)<<std::setw(8) <<Tubs_SpanAng[i][j]/deg
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<Name[index]
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<MotherName[index]
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<Material[index]
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<SDName[index]
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<PosX[index][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<PosY[index][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<PosZ[index][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<Ephi[index][j]/deg
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<Etheta[index][j]/deg
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<Epsi[index][j]/deg
				<<std::endl;
		}
	}

	for( G4int i = 0; i < SphereNo; i++ ){
		if ( i == 0 ){
			std::cout<<"=>Sphere info:"<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
				<<std::setiosflags(std::ios::left)<<std::setw(6) <<"RepNo"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"RMin"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"RMax"
				<<std::setiosflags(std::ios::left)<<std::setw(9) <<"StartPhi"
				<<std::setiosflags(std::ios::left)<<std::setw(8) <<"SpanPhi"
				<<std::setiosflags(std::ios::left)<<std::setw(11)<<"StartTheta"
				<<std::setiosflags(std::ios::left)<<std::setw(10)<<"SpanTheta"
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<"Name"
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<"MotherName"
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<"Matierial"
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<"SDName"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"PosX"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"PosY"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"PosZ"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"Ephi"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"Etheta"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"Epsi"
				<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(6) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(9) <<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(8) <<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(11)<<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(10)<<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<""
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<""
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<""
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<""
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
				<<std::endl;
		}
		int index = Sphere_GenIndex[i];
		int repNo = RepNo[index];
		for ( G4int j = 0; j < repNo; j++ ){
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
				<<std::setiosflags(std::ios::left)<<std::setw(6) <<j+SRepNo[index]
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Sphere_RMin[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Sphere_RMax[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(9) <<Sphere_StartPhi[i][j]/deg
				<<std::setiosflags(std::ios::left)<<std::setw(8) <<Sphere_SpanPhi[i][j]/deg
				<<std::setiosflags(std::ios::left)<<std::setw(11)<<Sphere_StartTheta[i][j]/deg
				<<std::setiosflags(std::ios::left)<<std::setw(10)<<Sphere_SpanTheta[i][j]/deg
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<Name[index]
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<MotherName[index]
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<Material[index]
				<<std::setiosflags(std::ios::left)<<std::setw(15)<<SDName[index]
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<PosX[index][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<PosY[index][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<PosZ[index][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<Ephi[index][j]/deg
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<Etheta[index][j]/deg
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<Epsi[index][j]/deg
				<<std::endl;
		}
	}
}
