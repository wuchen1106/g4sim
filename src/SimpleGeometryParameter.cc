//Description: In charge of SimpleGeometryParameter
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Modified: 11, Jan, 2013
//          Support G4Hype, G4TwistedTube
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

#include "SimpleGeometryParameterMessenger.hh"
#include "MyVGeometryParameter.hh"

#include "SimpleGeometryParameter.hh"

SimpleGeometryParameter::SimpleGeometryParameter(G4String name, G4String opt )
	: MyVGeometryParameter(name, "S") // Use a not-empty opt to stop inherited class from creating new GeometryParameterMessenger
{
	if ( opt == "" ){
		SimpleGeometryParameterMessenger *pointer = new SimpleGeometryParameterMessenger(this, name);
//		std::cout<<"======>In SimpleGeometryParameter, new SimpleGeometryParameterMessenger at ("<<(void*)pointer<<")!"<<std::endl;
		set_GeometryParameterMessenger(pointer);
	}
}

SimpleGeometryParameter::~SimpleGeometryParameter(){
	printf("~SimpleGeometryParameter");
}

//---------------------------InitFromFile---------------------------------
//will be called in SimpleGeometrySvc::SetSimpleGeometry
void SimpleGeometryParameter::InitFromFile( G4String file_name ){

	set_filename(file_name);

	//Remember to preset first!
	Preset();

	std::ifstream fin_card(file_name);
	if(!fin_card){
		G4String content = "SimpleGeometryParameter::InitFromFile(), cannot find output card \"" + file_name + "\"";
		DEBUG(content);
	}
	std::string s_card;
	while(getline(fin_card,s_card)){
		//eleminate useless lines
		if ( ISEMPTY(s_card) ) continue;
		int status = GetValue(s_card);
		if( status > 0 ){
			G4String content = "SimpleGeometryParameter: unknown line:" + s_card + "\n  in file " + file_name + "! Will ignore this line!";
			DEBUG(content,0);
		}
	}
	fin_card.close();
	if (CheckInfo()){
		DEBUG("SimpleGeometryParameter::InitFromFile(), insufficient simple solid info");
	}
}

void SimpleGeometryParameter::Calculate(){
	MyVGeometryParameter::Calculate();
}

void SimpleGeometryParameter::Dump(){
	DumpInfo();
}

//-----------------------------------Special functions---------------------------------------------

bool SimpleGeometryParameter::CheckInfo(){
	bool flag = false;// false means no problem
	flag = MyVGeometryParameter::CheckInfo();
	if (flag) return flag;// if something is wrong with its Parent Class, then return the status.

	//examine
	return flag;
}

void SimpleGeometryParameter::Preset(){
	MyVGeometryParameter::Preset();// Call its Parent class to preset
	notReSetVis = true;
	VolNo = 0;
	BoxNo = 0;
	TubsNo = 0;
	SphereNo = 0;
	HypeNo = 0;
	TwistedTubsNo = 0;
	ConsNo = 0;

	//General info about volume
  PosX.clear();
  PosY.clear();
  PosZ.clear();
  Name.clear();
  MotherName.clear();
  SDName.clear();
  Material.clear();
  SRepNo.clear();
  RepNo.clear();
  Space.clear();
  DirTheta.clear();
  DirPhi.clear();
  SolidType.clear();
  SolidIndex.clear();
  Ephi.clear();
  Etheta.clear();
  Epsi.clear();

	//Box info
  Box_X.clear();
  Box_Y.clear();
  Box_Z.clear();
  Box_GenIndex.clear();

	//Tubs info
  Tubs_RMax.clear();
  Tubs_RMin.clear();
  Tubs_Length.clear();
  Tubs_StartAng.clear();
  Tubs_SpanAng.clear();
  Tubs_GenIndex.clear();

	//Sphere info
  Sphere_RMax.clear();
  Sphere_RMin.clear();
  Sphere_StartPhi.clear();
  Sphere_SpanPhi.clear();
  Sphere_StartTheta.clear();
  Sphere_SpanTheta.clear();
  Sphere_GenIndex.clear();

	//Hype info
  Hype_innerRadius.clear();
  Hype_outerRadius.clear();
  Hype_innerStereo.clear();
  Hype_outerStereo.clear();
  Hype_Length.clear();
  Hype_GenIndex.clear();

	//TwistedTubs info
  TwistedTubs_twistedangle.clear();
  TwistedTubs_endinnerrad.clear();
  TwistedTubs_endouterrad.clear();
  TwistedTubs_Length.clear();
  TwistedTubs_dphi.clear();
  TwistedTubs_GenIndex.clear();

	//Cons info
  Cons_RMax1.clear();
  Cons_RMin1.clear();
  Cons_RMax2.clear();
  Cons_RMin2.clear();
  Cons_Length.clear();
  Cons_StartAng.clear();
  Cons_SpanAng.clear();
  Cons_GenIndex.clear();
}

int SimpleGeometryParameter::GetValue(G4String s_card){
	if (!MyVGeometryParameter::GetValue(s_card)) return 0;// if found by Parent Class, then return.
	int status = 0;// 0 means found the value
	std::stringstream buf_card;
	buf_card.str("");
	buf_card.clear();
	buf_card<<s_card;
	std::string name;
	buf_card>>name;
	std::string s_para;
	int iVol = VolNo;
	if( name == "B" || name == "T" || name == "Sph" || name == "H" || name == "TT" || name == "C" ){
		if( name == "B" ){
			SolidType.push_back("Box");
			SolidIndex.push_back(Box_X.size());
			std::string dump;
			G4double tBox_X;
			G4double tBox_Y;
			G4double tBox_Z;
			buf_card>>dump>>tBox_X>>tBox_Y>>tBox_Z;
			tBox_X *= mm;
			tBox_Y *= mm;
			tBox_Z *= mm;
			Box_X.push_back(tBox_X);
			Box_Y.push_back(tBox_Y);
			Box_Z.push_back(tBox_Z);
			Box_GenIndex.push_back(iVol);
			BoxNo++;
		}
		else if( name == "T" ){
			SolidType.push_back("Tubs");
			SolidIndex.push_back(Tubs_RMax.size());
			std::string dump;
			G4double tTubs_RMax;
			G4double tTubs_RMin;
			G4double tTubs_Length;
			G4double tTubs_StartAng;
			G4double tTubs_SpanAng;
			buf_card>>dump>>tTubs_RMin>>tTubs_RMax>>tTubs_Length>>tTubs_StartAng>>tTubs_SpanAng;
			tTubs_RMax *= mm;
			tTubs_RMin *= mm;
			tTubs_Length *= mm;
			tTubs_StartAng *= deg;
			tTubs_SpanAng *= deg;
			Tubs_RMax.push_back(tTubs_RMax);
			Tubs_RMin.push_back(tTubs_RMin);
			Tubs_Length.push_back(tTubs_Length);
			Tubs_StartAng.push_back(tTubs_StartAng);
			Tubs_SpanAng.push_back(tTubs_SpanAng);
			Tubs_GenIndex.push_back(iVol);
			TubsNo++;
		}
		else if( name == "Sph" ){
			SolidType.push_back("Sphere");
			SolidIndex.push_back(Sphere_RMax.size());
			std::string dump;
			G4double tSphere_RMax;
			G4double tSphere_RMin;
			G4double tSphere_StartPhi;
			G4double tSphere_SpanPhi;
			G4double tSphere_StartTheta;
			G4double tSphere_SpanTheta;
			buf_card>>dump>>tSphere_RMin>>tSphere_RMax>>tSphere_StartPhi>>tSphere_SpanPhi>>tSphere_StartTheta>>tSphere_SpanTheta;
			tSphere_RMax *= mm;
			tSphere_RMin *= mm;
			tSphere_StartPhi *= deg;
			tSphere_SpanPhi *= deg;
			tSphere_StartTheta *= deg;
			tSphere_SpanTheta *= deg;
			Sphere_RMax.push_back(tSphere_RMax);
			Sphere_RMin.push_back(tSphere_RMin);
			Sphere_StartPhi.push_back(tSphere_StartPhi);
			Sphere_SpanPhi.push_back(tSphere_SpanPhi);
			Sphere_StartTheta.push_back(tSphere_StartTheta);
			Sphere_SpanTheta.push_back(tSphere_SpanTheta);
			Sphere_GenIndex.push_back(iVol);
			SphereNo++;
		}
		else if( name == "H" ){
			SolidType.push_back("Hype");
			SolidIndex.push_back(Hype_innerRadius.size());
			std::string dump;
			G4double tHype_innerRadius;
			G4double tHype_outerRadius;
			G4double tHype_innerStereo;
			G4double tHype_outerStereo;
			G4double tHype_Length;
			buf_card>>dump>>tHype_innerRadius>>tHype_outerRadius>>tHype_innerStereo>>tHype_outerStereo>>tHype_Length;
			tHype_innerRadius *= mm;
			tHype_outerRadius *= mm;
			tHype_innerStereo *= deg;
			tHype_outerStereo *= deg;
			tHype_Length *= mm;
			Hype_innerRadius.push_back(tHype_innerRadius);
			Hype_outerRadius.push_back(tHype_outerRadius);
			Hype_innerStereo.push_back(tHype_innerStereo);
			Hype_outerStereo.push_back(tHype_outerStereo);
			Hype_Length.push_back(tHype_Length);
			Hype_GenIndex.push_back(iVol);
			HypeNo++;
		}
		else if( name == "TT" ){
			SolidType.push_back("TwistedTubs");
			SolidIndex.push_back(TwistedTubs_endinnerrad.size());
			std::string dump;
			G4double tTwistedTubs_twistedangle;
			G4double tTwistedTubs_endinnerrad;
			G4double tTwistedTubs_endouterrad;
			G4double tTwistedTubs_Length;
			G4double tTwistedTubs_dphi;
			buf_card>>dump>>tTwistedTubs_twistedangle>>tTwistedTubs_endinnerrad>>tTwistedTubs_endouterrad>>tTwistedTubs_Length>>tTwistedTubs_dphi;
			tTwistedTubs_twistedangle *= deg;
			tTwistedTubs_endinnerrad *= mm;
			tTwistedTubs_endouterrad *= mm;
			tTwistedTubs_Length *= mm;
			tTwistedTubs_dphi *= deg;
			TwistedTubs_twistedangle.push_back(tTwistedTubs_twistedangle);
			TwistedTubs_endinnerrad.push_back(tTwistedTubs_endinnerrad);
			TwistedTubs_endouterrad.push_back(tTwistedTubs_endouterrad);
			TwistedTubs_Length.push_back(tTwistedTubs_Length);
			TwistedTubs_dphi.push_back(tTwistedTubs_dphi);
			TwistedTubs_GenIndex.push_back(iVol);
			TwistedTubsNo++;
		}
		else if( name == "C" ){
			SolidType.push_back("Cons");
			SolidIndex.push_back(Cons_Length.size());
			std::string dump;
			G4double tCons_RMax1;
			G4double tCons_RMin1;
			G4double tCons_RMax2;
			G4double tCons_RMin2;
			G4double tCons_Length;
			G4double tCons_StartAng;
			G4double tCons_SpanAng;
			buf_card>>dump>>tCons_RMin1>>tCons_RMax1>>tCons_RMin2>>tCons_RMax2>>tCons_Length>>tCons_StartAng>>tCons_SpanAng;
			tCons_RMax1 *= mm;
			tCons_RMin1 *= mm;
			tCons_RMax2 *= mm;
			tCons_RMin2 *= mm;
			tCons_Length *= mm;
			tCons_StartAng *= deg;
			tCons_SpanAng *= deg;
			Cons_RMax1.push_back(tCons_RMax1);
			Cons_RMin1.push_back(tCons_RMin1);
			Cons_RMax2.push_back(tCons_RMax2);
			Cons_RMin2.push_back(tCons_RMin2);
			Cons_Length.push_back(tCons_Length);
			Cons_StartAng.push_back(tCons_StartAng);
			Cons_SpanAng.push_back(tCons_SpanAng);
			Cons_GenIndex.push_back(iVol);
			ConsNo++;
		}
		G4double tPosX;
		G4double tPosY;
		G4double tPosZ;
		G4String tRepCont;
		G4int tRepNo;
		G4int tSRepNo;
		G4double tSpace;
		G4double tDirTheta;
		G4double tDirPhi;
		G4String tSDName;
		G4double tEphi;
		G4double tEtheta;
		G4double tEpsi;
		std::string tName;
		std::string tMotherName;
		std::string tMaterial;
		buf_card>>tPosX>>tPosY>>tPosZ>>tName>>tMotherName>>tMaterial>>tSDName>>tRepCont>>tSpace>>tDirTheta>>tDirPhi>>tEphi>>tEtheta>>tEpsi;
		get_RepCont(tRepCont,tSRepNo,tRepNo);
		tPosX *= mm;
		tPosY *= mm;
		tPosZ *= mm;
		tSpace *= mm;
		tDirTheta *= deg;
		tDirPhi *= deg;
		tEphi *= deg;
		tEtheta *= deg;
		tEpsi *= deg;
		PosX.push_back(tPosX);
		PosY.push_back(tPosY);
		PosZ.push_back(tPosZ);
		Name.push_back(tName);
		MotherName.push_back(tMotherName);
		Material.push_back(tMaterial);
		SRepNo.push_back(tSRepNo);
		RepNo.push_back(tRepNo);
		Space.push_back(tSpace);
		DirTheta.push_back(tDirTheta);
		DirPhi.push_back(tDirPhi);
		SDName.push_back(tSDName);
		Ephi.push_back(tEphi);
		Etheta.push_back(tEtheta);
		Epsi.push_back(tEpsi);
		VolNo++;
	}
	else if ( name == "VISSETTING" ){
		if(notReSetVis) ReSetVis();
		status = -1;
	}
	else{
		std::cout<<"#############In SimpleGeometryParameter::GetValue()"<<std::endl;
		std::cout<<"Is \""<<name<<"\" vis setting?"<<std::endl;
		bool foundornot = false;
		if ( name.substr(0,4) == "vis_" ){
			std::stringstream buf_temp;
			for ( G4int i = 0; i < VolNo; i++ ){
				buf_temp.str("");
				buf_temp.clear();
				buf_temp<<"vis_"<<Name[i];
				if( name == buf_temp.str() ){
					set_vis(i,true);
					G4double vTr, vTg, vTb;
					buf_card>>vTr>>vTg>>vTb;
					set_r(i,vTr);
					set_g(i,vTg);
					set_b(i,vTb);
					foundornot = true;
					break; // got the parameter, go to next line
				}
			}
			buf_temp.str("");
			buf_temp.clear();
		}
		if (!foundornot){
			std::cout<<"No..."<<std::endl;
			status = 1;
		}
		else{
			std::cout<<"Yes!!"<<std::endl;
		}
	}
	buf_card.str("");
	buf_card.clear();
	return status;
}

//---------------------------ReSetVis---------------------------------
//
void SimpleGeometryParameter::ReSetVis() {
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
void SimpleGeometryParameter::DumpInfo() {
	MyVGeometryParameter::DumpInfo();// dump info of its Parent Class first
	std::cout<<"------SimpleGeometry info :--------"<<std::endl;
	for( G4int i = 0; i < BoxNo; i++ ){
		if ( i == 0 ){
			std::cout<<"=>Box info:"<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"x"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"y"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"z"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<"Name"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<"MotherName"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<"Matierial"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<"SDName"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"PosX"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"PosY"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"PosZ"
							 <<std::setiosflags(std::ios::left)<<std::setw(6) <<"RepNo"
							 <<std::setiosflags(std::ios::left)<<std::setw(6) <<"Space"
							 <<std::setiosflags(std::ios::left)<<std::setw(9) <<"DirTheta"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"DirPhi"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"Ephi"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"Etheta"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"Epsi"
							 <<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<""
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<""
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<""
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<""
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(6) <<""
							 <<std::setiosflags(std::ios::left)<<std::setw(6) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(9) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
							 <<std::endl;
		} 
		int index = Box_GenIndex[i];
		std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Box_X[i]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Box_Y[i]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Box_Z[i]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(15)<<Name[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(15)<<MotherName[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(15)<<Material[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(15)<<SDName[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<PosX[index]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<PosY[index]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<PosZ[index]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(6) <<RepNo[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(6) <<Space[index]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(9) <<DirTheta[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<DirPhi[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Ephi[index]/deg
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Etheta[index]/deg
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Epsi[index]/deg
						 <<std::endl;
	}

	for( G4int i = 0; i < TubsNo; i++ ){
		if ( i == 0 ){
			std::cout<<"=>Tubs info:"<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"RMin"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"RMax"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"Length"
							 <<std::setiosflags(std::ios::left)<<std::setw(9) <<"StartAng"
							 <<std::setiosflags(std::ios::left)<<std::setw(8) <<"SpanAng"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<"Name"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<"MotherName"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<"Matierial"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<"SDName"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"PosX"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"PosY"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"PosZ"
							 <<std::setiosflags(std::ios::left)<<std::setw(6) <<"RepNo"
							 <<std::setiosflags(std::ios::left)<<std::setw(6) <<"Space"
							 <<std::setiosflags(std::ios::left)<<std::setw(9) <<"DirTheta"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"DirPhi"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"Ephi"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"Etheta"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"Epsi"
							 <<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(9) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(8) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<""
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<""
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<""
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<""
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(6) <<""
							 <<std::setiosflags(std::ios::left)<<std::setw(6) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(9) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
							 <<std::endl;
		}
		int index = Tubs_GenIndex[i];
		std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Tubs_RMin[i]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Tubs_RMax[i]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Tubs_Length[i]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(9) <<Tubs_StartAng[i]/deg
						 <<std::setiosflags(std::ios::left)<<std::setw(8) <<Tubs_SpanAng[i]/deg
						 <<std::setiosflags(std::ios::left)<<std::setw(15)<<Name[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(15)<<MotherName[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(15)<<Material[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(15)<<SDName[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<PosX[index]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<PosY[index]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<PosZ[index]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(6) <<RepNo[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(6) <<Space[index]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(9) <<DirTheta[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<DirPhi[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Ephi[index]/deg
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Etheta[index]/deg
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Epsi[index]/deg
						 <<std::endl;
	}

	for( G4int i = 0; i < SphereNo; i++ ){
		if ( i == 0 ){
			std::cout<<"=>Sphere info:"<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"RMin"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"RMax"
							 <<std::setiosflags(std::ios::left)<<std::setw(9) <<"StartPhi"
							 <<std::setiosflags(std::ios::left)<<std::setw(8) <<"SpanPhi"
							 <<std::setiosflags(std::ios::left)<<std::setw(11)<<"StartTheta"
							 <<std::setiosflags(std::ios::left)<<std::setw(10)<<"SpanTheta"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<"Name"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<"MotherName"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<"Matierial"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<"SDName"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"PosX"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"PosY"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"PosZ"
							 <<std::setiosflags(std::ios::left)<<std::setw(6) <<"RepNo"
							 <<std::setiosflags(std::ios::left)<<std::setw(6) <<"Space"
							 <<std::setiosflags(std::ios::left)<<std::setw(9) <<"DirTheta"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"DirPhi"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"Ephi"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"Etheta"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"Epsi"
							 <<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(9) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(8) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(11)<<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(10)<<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<""
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<""
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<""
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<""
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(6) <<""
							 <<std::setiosflags(std::ios::left)<<std::setw(6) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(9) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
							 <<std::endl;
		}
		int index = Sphere_GenIndex[i];
		std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Sphere_RMin[i]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Sphere_RMax[i]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(9) <<Sphere_StartPhi[i]/deg
						 <<std::setiosflags(std::ios::left)<<std::setw(8) <<Sphere_SpanPhi[i]/deg
						 <<std::setiosflags(std::ios::left)<<std::setw(11)<<Sphere_StartTheta[i]/deg
						 <<std::setiosflags(std::ios::left)<<std::setw(10)<<Sphere_SpanTheta[i]/deg
						 <<std::setiosflags(std::ios::left)<<std::setw(15)<<Name[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(15)<<MotherName[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(15)<<Material[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(15)<<SDName[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<PosX[index]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<PosY[index]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<PosZ[index]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(6) <<RepNo[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(6) <<Space[index]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(9) <<DirTheta[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<DirPhi[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Ephi[index]/deg
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Etheta[index]/deg
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Epsi[index]/deg
						 <<std::endl;
	}

	for( G4int i = 0; i < HypeNo; i++ ){
		if ( i == 0 ){
			std::cout<<"=>Hype info:"<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
							 <<std::setiosflags(std::ios::left)<<std::setw(12)<<"innerRadius"
							 <<std::setiosflags(std::ios::left)<<std::setw(12)<<"outerRadius"
							 <<std::setiosflags(std::ios::left)<<std::setw(12)<<"innerStereo"
							 <<std::setiosflags(std::ios::left)<<std::setw(12)<<"outerStereo"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"Length"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<"Name"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<"MotherName"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<"Matierial"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<"SDName"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"PosX"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"PosY"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"PosZ"
							 <<std::setiosflags(std::ios::left)<<std::setw(6) <<"RepNo"
							 <<std::setiosflags(std::ios::left)<<std::setw(6) <<"Space"
							 <<std::setiosflags(std::ios::left)<<std::setw(9) <<"DirTheta"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"DirPhi"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"Ephi"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"Etheta"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"Epsi"
							 <<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
							 <<std::setiosflags(std::ios::left)<<std::setw(12)<<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(12)<<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(12)<<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(12)<<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<""
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<""
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<""
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<""
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(6) <<""
							 <<std::setiosflags(std::ios::left)<<std::setw(6) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(9) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
							 <<std::endl;
		}
		int index = Hype_GenIndex[i];
		std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
						 <<std::setiosflags(std::ios::left)<<std::setw(12)<<Hype_innerRadius[i]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(12)<<Hype_outerRadius[i]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(12)<<Hype_innerStereo[i]/deg
						 <<std::setiosflags(std::ios::left)<<std::setw(12)<<Hype_outerStereo[i]/deg
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Hype_Length[i]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(15)<<Name[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(15)<<MotherName[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(15)<<Material[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(15)<<SDName[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<PosX[index]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<PosY[index]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<PosZ[index]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(6) <<RepNo[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(6) <<Space[index]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(9) <<DirTheta[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<DirPhi[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Ephi[index]/deg
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Etheta[index]/deg
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Epsi[index]/deg
						 <<std::endl;
	}

	for( G4int i = 0; i < TwistedTubsNo; i++ ){
		if ( i == 0 ){
			std::cout<<"=>TwistedTubs info:"<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
							 <<std::setiosflags(std::ios::left)<<std::setw(13)<<"twistedangle"
							 <<std::setiosflags(std::ios::left)<<std::setw(12)<<"endinnerrad"
							 <<std::setiosflags(std::ios::left)<<std::setw(12)<<"endouterrad"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"Length"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"dphi"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<"Name"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<"MotherName"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<"Matierial"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<"SDName"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"PosX"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"PosY"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"PosZ"
							 <<std::setiosflags(std::ios::left)<<std::setw(6) <<"RepNo"
							 <<std::setiosflags(std::ios::left)<<std::setw(6) <<"Space"
							 <<std::setiosflags(std::ios::left)<<std::setw(9) <<"DirTheta"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"DirPhi"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"Ephi"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"Etheta"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"Epsi"
							 <<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
							 <<std::setiosflags(std::ios::left)<<std::setw(13)<<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(12)<<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(12)<<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<""
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<""
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<""
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<""
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(6) <<""
							 <<std::setiosflags(std::ios::left)<<std::setw(6) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(9) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
							 <<std::endl;
		}
		int index = TwistedTubs_GenIndex[i];
		std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
						 <<std::setiosflags(std::ios::left)<<std::setw(13)<<TwistedTubs_twistedangle[i]/deg
						 <<std::setiosflags(std::ios::left)<<std::setw(12)<<TwistedTubs_endinnerrad[i]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(12)<<TwistedTubs_endouterrad[i]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<TwistedTubs_Length[i]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<TwistedTubs_dphi[i]/deg
						 <<std::setiosflags(std::ios::left)<<std::setw(15)<<Name[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(15)<<MotherName[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(15)<<Material[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(15)<<SDName[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<PosX[index]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<PosY[index]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<PosZ[index]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(6) <<RepNo[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(6) <<Space[index]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(9) <<DirTheta[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<DirPhi[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Ephi[index]/deg
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Etheta[index]/deg
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Epsi[index]/deg
						 <<std::endl;
	}

	for( G4int i = 0; i < ConsNo; i++ ){
		if ( i == 0 ){
			std::cout<<"=>Cons info:"<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"RMin1"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"RMax1"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"RMin2"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"RMax2"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"Length"
							 <<std::setiosflags(std::ios::left)<<std::setw(9) <<"StartAng"
							 <<std::setiosflags(std::ios::left)<<std::setw(8) <<"SpanAng"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<"Name"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<"MotherName"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<"Matierial"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<"SDName"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"PosX"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"PosY"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"PosZ"
							 <<std::setiosflags(std::ios::left)<<std::setw(6) <<"RepNo"
							 <<std::setiosflags(std::ios::left)<<std::setw(6) <<"Space"
							 <<std::setiosflags(std::ios::left)<<std::setw(9) <<"DirTheta"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"DirPhi"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"Ephi"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"Etheta"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"Epsi"
							 <<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(9) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(8) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<""
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<""
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<""
							 <<std::setiosflags(std::ios::left)<<std::setw(15)<<""
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(6) <<""
							 <<std::setiosflags(std::ios::left)<<std::setw(6) <<"mm"
							 <<std::setiosflags(std::ios::left)<<std::setw(9) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
							 <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
							 <<std::endl;
		}
		int index = Cons_GenIndex[i];
		std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Cons_RMin1[i]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Cons_RMax1[i]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Cons_RMin2[i]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Cons_RMax2[i]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Cons_Length[i]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(9) <<Cons_StartAng[i]/deg
						 <<std::setiosflags(std::ios::left)<<std::setw(8) <<Cons_SpanAng[i]/deg
						 <<std::setiosflags(std::ios::left)<<std::setw(15)<<Name[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(15)<<MotherName[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(15)<<Material[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(15)<<SDName[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<PosX[index]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<PosY[index]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<PosZ[index]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(6) <<RepNo[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(6) <<Space[index]/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(9) <<DirTheta[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<DirPhi[index]
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Ephi[index]/deg
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Etheta[index]/deg
						 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Epsi[index]/deg
						 <<std::endl;
	}

}
