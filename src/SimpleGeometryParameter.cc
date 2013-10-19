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
	std::vector<std::string> knownValueNames;
	std::vector<std::string> knownValues;
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

double SimpleGeometryParameter::Calculate(std::string formula, int iRep = 0){
	std::vector<std::string> words = GetWords(formula);
	for (int iWord = 0; iWord < words.size(); iWord++ ){
		double value;
		if (GetValue(words[iWord],value)){
			Replace(formula,words[iWord],value);
		}
	}
}

std::vector<std::string> SimpleGeometryParameter::GetWords(std::string formula){
	std::vector<std::string> words;
	words.clear();
	const char* cformula = formula.c_str();
	int length = strlen(cformula);
	char temp[1240];
	int tempoffset = 0;
	for ( int offset = 0; offset < length; offset++ ){
		char c = cformula[offset];
		bool isword = false;
		if (c>='a'&&c<='z'
			||c>='A'&&c<='Z'
			||c>='0'&&c<='9'
			||c=='_'
		   ){
			temp[tempoffset++] = cformula[offset];
			isword = true;
		}
		if (!isword||offset==length-1){
			if (tempoffset>0){
				temp[tempoffset++] = '\0';
				tempoffset=0;
				std::string word = temp;
				bool found = false;
				for(int iWord = 0; iWord<words.size(); iWord++){
					if (words[iWord]==word){
						found = true;
						break;
					}
				}
				if (!found){
					words.push_back(word);
				}
			}
		}
	}
	return words;
}

bool SimpleGeometryParameter::GetValue(std::string word, std::string& value){
	bool found = false;
	for (int i = 0; i< knownValues.size(); i++){
		if (knownValueNames[i]==word){
			value = knownValues[i];
			found = true;
			break;
		}
	}
	return found;
}

void SimpleGeometryParameter::Replace(std::string &formula, std::string word, std::string value){
	std::string newform = "";
	const char* cformula = formula.c_str();
	int length = strlen(cformula);
	char temp[1024];
	int tempoffset = 0;
	char cnewform[1024];
	int newformoffset = 0;
	for ( int offset = 0; offset < length; offset++ ){
		char c = cformula[offset];
		bool isword = false;
		if (c>='a'&&c<='z'
			||c>='A'&&c<='Z'
			||c>='0'&&c<='9'
			||c=='_'
		   ){
			temp[tempoffset++] = cformula[offset];
			isword = true;
		}
		if (!isword||offset==length-1){
			if (tempoffset>0){
				temp[tempoffset] = '\0';
				tempoffset=0;
				if (newformoffset>0){
					cnewform[newformoffset] = '\0';
					newformoffset=0;
					newform=newform+cnewform;
				}
//				std::cout<<"-- \""<<newform<<"\""<<std::endl;
				std::string word = temp;
//				std::cout<<"		\""<<word<<"\""<<std::endl;
				std::string newword;
				bool found = GetValue(word,newword);
				if (found){
					newform=newform+"("+newword+")";
				}
				else{
					newform=newform+word;
				}
//				std::cout<<"		\""<<newword<<"\""<<std::endl;
//				std::cout<<"	-->\""<<newform<<"\""<<std::endl;
			}
			cnewform[newformoffset++] = cformula[offset];
			if (offset==length-1){
				cnewform[newformoffset] = '\0';
				newform=newform+cnewform;
			}
		}
	}
	formula = newform;
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
	knownValueNames.clear();
	knownValues.clear();
	MyVGeometryParameter::Preset();// Call its Parent class to preset
	notReSetVis = true;
	VolNo = 0;
	BoxNo = 0;
	TubsNo = 0;
	TorusNo = 0;
	SphereNo = 0;
	HypeNo = 0;
	TwistedTubsNo = 0;
	ConsNo = 0;
	PolyconeNo = 0;
	BooleanSolidNo = 0;

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
	SolidBoolean.clear();

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

	//Torus info
	Torus_RMax.clear();
	Torus_RMin.clear();
	Torus_Rtor.clear();
	Torus_StartAng.clear();
	Torus_SpanAng.clear();
	Torus_GenIndex.clear();

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

	//Polycone info
	Polycone_RMax.clear();
	Polycone_RMin.clear();
	Polycone_Z.clear();
	Polycone_numZ.clear();
	Polycone_StartAng.clear();
	Polycone_SpanAng.clear();
	Polycone_GenIndex.clear();
	waited_Polycone_iVol = -1;
	achieved_componets_Polycone = 0;

	//BooleanSolid info
	BooleanSolid_type.clear();
	BooleanSolid_sol1.clear();
	BooleanSolid_sol2.clear();
	BooleanSolid_Ephi.clear();
	BooleanSolid_Etheta.clear();
	BooleanSolid_Epsi.clear();
	BooleanSolid_PosX.clear();
	BooleanSolid_PosY.clear();
	BooleanSolid_PosZ.clear();
	BooleanSolid_GenIndex.clear();
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
	bool GoOn = true;
	if (waited_Polycone_iVol>=0){
		GoOn = false;
		if (achieved_componets_Polycone<Polycone_numZ[waited_Polycone_iVol]){
			if (name=="PCI"){
				std::string dump;
				G4double tPolycone_RMax;
				G4double tPolycone_RMin;
				G4double tPolycone_Z;
				buf_card>>dump>>tPolycone_Z>>tPolycone_RMin>>tPolycone_RMax;
				tPolycone_RMax *= mm;
				tPolycone_RMin *= mm;
				tPolycone_Z *= mm;
				Polycone_RMax[waited_Polycone_iVol][achieved_componets_Polycone] = tPolycone_RMax;
				Polycone_RMin[waited_Polycone_iVol][achieved_componets_Polycone] = tPolycone_RMin;
				Polycone_Z[waited_Polycone_iVol][achieved_componets_Polycone] = tPolycone_Z;
				achieved_componets_Polycone++;
			}
			else {
				std::cout<<"Not enough Polycone components! Needed: "<<Polycone_numZ[waited_Polycone_iVol]<<", got: "<<achieved_componets_Polycone+1<<std::endl;
				Polycone_numZ[waited_Polycone_iVol]=achieved_componets_Polycone;
				GoOn = true;
			}
			if (Polycone_numZ[waited_Polycone_iVol]==achieved_componets_Polycone){
				waited_Polycone_iVol=-1;
				achieved_componets_Polycone=0;
			}
		}
	}
	if (GoOn){
		bool foundVolume = false;
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
			foundVolume = true;
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
			if (buf_card>>dump){
				buf_card>>dump>>dump>>dump>>dump>>dump>>repNo;
			}
			for (int iRep = 0; iRep <repNo; iRep++){
				std::vector<double> vTubs_RMax.push_back(Calculate(tTubs_RMax,iRep)*mm);
				std::vector<double> vTubs_RMin.push_back(Calculate(tTubs_RMin,iRep)*mm);
				std::vector<double> vTubs_Length.push_back(Calculate(tTubs_Length,iRep)*mm);
				std::vector<double> vTubs_StartAng.push_back(Calculate(tTubs_StartAng,iRep)*deg);
				std::vector<double> vTubs_SpanAng.push_back(Calculate(tTubs_SpanAng,iRep)*deg);
			}
			Tubs_RMax.push_back(vTubs_RMax);
			Tubs_RMin.push_back(vTubs_RMin);
			Tubs_Length.push_back(vTubs_Length);
			Tubs_StartAng.push_back(vTubs_StartAng);
			Tubs_SpanAng.push_back(vTubs_SpanAng);
			Tubs_GenIndex.push_back(iVol);
			TubsNo++;
			foundVolume = true;
		}
		else if( name == "Tor" ){
			SolidType.push_back("Torus");
			SolidIndex.push_back(Torus_RMax.size());
			std::string dump;
			G4double tTorus_RMax;
			G4double tTorus_RMin;
			G4double tTorus_Rtor;
			G4double tTorus_StartAng;
			G4double tTorus_SpanAng;
			buf_card>>dump>>tTorus_RMin>>tTorus_RMax>>tTorus_Rtor>>tTorus_StartAng>>tTorus_SpanAng;
			tTorus_RMax *= mm;
			tTorus_RMin *= mm;
			tTorus_Rtor *= mm;
			tTorus_StartAng *= deg;
			tTorus_SpanAng *= deg;
			Torus_RMax.push_back(tTorus_RMax);
			Torus_RMin.push_back(tTorus_RMin);
			Torus_Rtor.push_back(tTorus_Rtor);
			Torus_StartAng.push_back(tTorus_StartAng);
			Torus_SpanAng.push_back(tTorus_SpanAng);
			Torus_GenIndex.push_back(iVol);
			TorusNo++;
			foundVolume = true;
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
			foundVolume = true;
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
			foundVolume = true;
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
			foundVolume = true;
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
			foundVolume = true;
		}
		else if( name == "PC" ){
			SolidType.push_back("Polycone");
			SolidIndex.push_back(Polycone_Z.size());
			std::string dump;
			G4double tPolycone_numZ;
			G4double tPolycone_StartAng;
			G4double tPolycone_SpanAng;
			buf_card>>dump>>tPolycone_numZ>>tPolycone_StartAng>>tPolycone_SpanAng;
			tPolycone_StartAng *= deg;
			tPolycone_SpanAng *= deg;
			Polycone_numZ.push_back(tPolycone_numZ);
			Polycone_StartAng.push_back(tPolycone_StartAng);
			Polycone_SpanAng.push_back(tPolycone_SpanAng);
			Polycone_GenIndex.push_back(iVol);
			std::vector<double> empty_vec;
			empty_vec.resize(tPolycone_numZ);
			Polycone_RMax.push_back(empty_vec);
			Polycone_RMin.push_back(empty_vec);
			Polycone_Z.push_back(empty_vec);
			waited_Polycone_iVol = PolyconeNo;
			achieved_componets_Polycone = 0;
			PolyconeNo++;
			foundVolume = true;
		}
		else if( name == "BL" ){
			SolidType.push_back("BooleanSolid");
			SolidIndex.push_back(BooleanSolid_type.size());
			std::string dump;
			G4String tBooleanSolid_type;
			G4String tBooleanSolid_sol1;
			G4String tBooleanSolid_sol2;
			G4double tBooleanSolid_Ephi;
			G4double tBooleanSolid_Etheta;
			G4double tBooleanSolid_Epsi;
			G4double tBooleanSolid_PosX;
			G4double tBooleanSolid_PosY;
			G4double tBooleanSolid_PosZ;
			buf_card>>dump>>tBooleanSolid_type>>tBooleanSolid_sol1>>tBooleanSolid_sol2>>tBooleanSolid_Ephi>>tBooleanSolid_Etheta>>tBooleanSolid_Epsi>>tBooleanSolid_PosX>>tBooleanSolid_PosY>>tBooleanSolid_PosZ;
			tBooleanSolid_Ephi *= deg;
			tBooleanSolid_Etheta *= deg;
			tBooleanSolid_Epsi *= deg;
			tBooleanSolid_PosX *= mm;
			tBooleanSolid_PosY *= mm;
			tBooleanSolid_PosZ *= mm;
			BooleanSolid_type.push_back(tBooleanSolid_type);
			BooleanSolid_sol1.push_back(tBooleanSolid_sol1);
			BooleanSolid_sol2.push_back(tBooleanSolid_sol2);
			BooleanSolid_Ephi.push_back(tBooleanSolid_Ephi);
			BooleanSolid_Etheta.push_back(tBooleanSolid_Etheta);
			BooleanSolid_Epsi.push_back(tBooleanSolid_Epsi);
			BooleanSolid_PosX.push_back(tBooleanSolid_PosX);
			BooleanSolid_PosY.push_back(tBooleanSolid_PosY);
			BooleanSolid_PosZ.push_back(tBooleanSolid_PosZ);
			BooleanSolid_GenIndex.push_back(iVol);
			BooleanSolidNo++;
			foundVolume = true;
		}
		if (foundVolume){
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
			G4bool   tSolidBoolean = true;
			buf_card>>tName;
			if(buf_card>>tMotherName){
				tSolidBoolean = false;
				buf_card>>tMaterial>>tSDName>>tPosX>>tPosY>>tPosZ>>tRepCont>>tSpace>>tDirTheta>>tDirPhi>>tEphi>>tEtheta>>tEpsi;
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
			}
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
			SolidBoolean.push_back(tSolidBoolean);
			VolNo++;
		}
		else if ( name == "VISSETTING" ){
			if(notReSetVis) ReSetVis();
			status = -1;
		}
		else{
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
	vT.resize(VolNo);
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
			         <<std::setiosflags(std::ios::left)<<std::setw(7) <<"z";
			dump_general_header();
			std::cout<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
			         <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
			         <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
			         <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm";
			dump_general_note();
			std::cout<<std::endl;
		} 
		int index = Box_GenIndex[i];
		std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<Box_X[i]/mm
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<Box_Y[i]/mm
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<Box_Z[i]/mm;
		dump_general_value(index);
		std::cout<<std::endl;
	}

	for( G4int i = 0; i < TubsNo; i++ ){
		if ( i == 0 ){
			std::cout<<"=>Tubs info:"<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"RMin"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"RMax"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"Length"
				<<std::setiosflags(std::ios::left)<<std::setw(9) <<"StartAng"
				<<std::setiosflags(std::ios::left)<<std::setw(8) <<"SpanAng";
			dump_general_header();
			std::cout<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(9) <<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(8) <<"deg";
			dump_general_note();
			std::cout<<std::endl;
		}
		int index = Tubs_GenIndex[i];
		std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<Tubs_RMin[i]/mm
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<Tubs_RMax[i]/mm
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<Tubs_Length[i]/mm
			<<std::setiosflags(std::ios::left)<<std::setw(9) <<Tubs_StartAng[i]/deg
			<<std::setiosflags(std::ios::left)<<std::setw(8) <<Tubs_SpanAng[i]/deg;
		dump_general_value(index);
		std::cout<<std::endl;
	}

	for( G4int i = 0; i < TorusNo; i++ ){
		if ( i == 0 ){
			std::cout<<"=>Torus info:"<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"RMin"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"RMax"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"Rtor"
				<<std::setiosflags(std::ios::left)<<std::setw(9) <<"StartAng"
				<<std::setiosflags(std::ios::left)<<std::setw(8) <<"SpanAng";
			dump_general_header();
			std::cout<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(9) <<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(8) <<"deg";
			dump_general_note();
			std::cout<<std::endl;
		}
		int index = Torus_GenIndex[i];
		std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<Torus_RMin[i]/mm
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<Torus_RMax[i]/mm
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<Torus_Rtor[i]/mm
			<<std::setiosflags(std::ios::left)<<std::setw(9) <<Torus_StartAng[i]/deg
			<<std::setiosflags(std::ios::left)<<std::setw(8) <<Torus_SpanAng[i]/deg;
		dump_general_value(index);
		std::cout<<std::endl;
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
				<<std::setiosflags(std::ios::left)<<std::setw(10)<<"SpanTheta";
			dump_general_header();
			std::cout<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(9) <<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(8) <<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(11)<<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(10)<<"deg";
			dump_general_note();
			std::cout<<std::endl;
		}
		int index = Sphere_GenIndex[i];
		std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<Sphere_RMin[i]/mm
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<Sphere_RMax[i]/mm
			<<std::setiosflags(std::ios::left)<<std::setw(9) <<Sphere_StartPhi[i]/deg
			<<std::setiosflags(std::ios::left)<<std::setw(8) <<Sphere_SpanPhi[i]/deg
			<<std::setiosflags(std::ios::left)<<std::setw(11)<<Sphere_StartTheta[i]/deg
			<<std::setiosflags(std::ios::left)<<std::setw(10)<<Sphere_SpanTheta[i]/deg;
		dump_general_value(index);
		std::cout<<std::endl;
	}

	for( G4int i = 0; i < HypeNo; i++ ){
		if ( i == 0 ){
			std::cout<<"=>Hype info:"<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
				<<std::setiosflags(std::ios::left)<<std::setw(12)<<"innerRadius"
				<<std::setiosflags(std::ios::left)<<std::setw(12)<<"outerRadius"
				<<std::setiosflags(std::ios::left)<<std::setw(12)<<"innerStereo"
				<<std::setiosflags(std::ios::left)<<std::setw(12)<<"outerStereo"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"Length";
			dump_general_header();
			std::cout<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(12)<<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(12)<<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(12)<<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(12)<<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm";
			dump_general_note();
			std::cout<<std::endl;
		}
		int index = Hype_GenIndex[i];
		std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
			<<std::setiosflags(std::ios::left)<<std::setw(12)<<Hype_innerRadius[i]/mm
			<<std::setiosflags(std::ios::left)<<std::setw(12)<<Hype_outerRadius[i]/mm
			<<std::setiosflags(std::ios::left)<<std::setw(12)<<Hype_innerStereo[i]/deg
			<<std::setiosflags(std::ios::left)<<std::setw(12)<<Hype_outerStereo[i]/deg
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<Hype_Length[i]/mm;
		dump_general_value(index);
		std::cout<<std::endl;
	}

	for( G4int i = 0; i < TwistedTubsNo; i++ ){
		if ( i == 0 ){
			std::cout<<"=>TwistedTubs info:"<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
				<<std::setiosflags(std::ios::left)<<std::setw(13)<<"twistedangle"
				<<std::setiosflags(std::ios::left)<<std::setw(12)<<"endinnerrad"
				<<std::setiosflags(std::ios::left)<<std::setw(12)<<"endouterrad"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"Length"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"dphi";
			dump_general_header();
			std::cout<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(13)<<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(12)<<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(12)<<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg";
			dump_general_note();
			std::cout<<std::endl;
		}
		int index = TwistedTubs_GenIndex[i];
		std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
			<<std::setiosflags(std::ios::left)<<std::setw(13)<<TwistedTubs_twistedangle[i]/deg
			<<std::setiosflags(std::ios::left)<<std::setw(12)<<TwistedTubs_endinnerrad[i]/mm
			<<std::setiosflags(std::ios::left)<<std::setw(12)<<TwistedTubs_endouterrad[i]/mm
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<TwistedTubs_Length[i]/mm
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<TwistedTubs_dphi[i]/deg;
		dump_general_value(index);
		std::cout<<std::endl;
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
				<<std::setiosflags(std::ios::left)<<std::setw(8) <<"SpanAng";
			dump_general_header();
			std::cout<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(9) <<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(8) <<"deg";
			dump_general_note();
			std::cout<<std::endl;
		}
		int index = Cons_GenIndex[i];
		std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<Cons_RMin1[i]/mm
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<Cons_RMax1[i]/mm
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<Cons_RMin2[i]/mm
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<Cons_RMax2[i]/mm
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<Cons_Length[i]/mm
			<<std::setiosflags(std::ios::left)<<std::setw(9) <<Cons_StartAng[i]/deg
			<<std::setiosflags(std::ios::left)<<std::setw(8) <<Cons_SpanAng[i]/deg;
		dump_general_value(index);
		std::cout<<std::endl;
	}

	for( G4int i = 0; i < PolyconeNo; i++ ){
		if ( i == 0 ){
			std::cout<<"=>Polycone info:"<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<"numZ"
				<<std::setiosflags(std::ios::left)<<std::setw(9) <<"StartAng"
				<<std::setiosflags(std::ios::left)<<std::setw(8) <<"SpanAng";
			dump_general_header();
			std::cout<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(9) <<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(8) <<"deg";
			dump_general_note();
			std::cout<<std::endl;
		}
		int index = Polycone_GenIndex[i];
		std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<Polycone_numZ[i]
			<<std::setiosflags(std::ios::left)<<std::setw(9) <<Polycone_StartAng[i]/deg
			<<std::setiosflags(std::ios::left)<<std::setw(8) <<Polycone_SpanAng[i]/deg;
		dump_general_value(index);
		std::cout<<std::endl;
		std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<"Z"
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<"RMin"
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<"RMax"
			<<std::endl;
		std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
			<<std::endl;
		for (int j = 0; j< Polycone_numZ[i]; j++){
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<j
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<Polycone_Z[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<Polycone_RMin[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<Polycone_RMax[i][j]/mm
				<<std::endl;
		}
	}
	for( G4int i = 0; i < BooleanSolidNo; i++ ){
		if ( i == 0 ){
			std::cout<<"=>BooleanSolid info:"<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(7) <<"type."
			         <<std::setiosflags(std::ios::left)<<std::setw(14) <<"solid1"
			         <<std::setiosflags(std::ios::left)<<std::setw(14) <<"solid2";
//			         <<std::setiosflags(std::ios::left)<<std::setw(7) <<"Ephi"
//			         <<std::setiosflags(std::ios::left)<<std::setw(7) <<"Etheta"
//			         <<std::setiosflags(std::ios::left)<<std::setw(7) <<"Epsi"
//			         <<std::setiosflags(std::ios::left)<<std::setw(7) <<"PosX"
//			         <<std::setiosflags(std::ios::left)<<std::setw(7) <<"PosY"
//			         <<std::setiosflags(std::ios::left)<<std::setw(7) <<"PosZ"
			dump_general_header();
			std::cout<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(7) <<""
			         <<std::setiosflags(std::ios::left)<<std::setw(14) <<""
			         <<std::setiosflags(std::ios::left)<<std::setw(14) <<"";
//			         <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
//			         <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
//			         <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
//			         <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
//			         <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
//			         <<std::setiosflags(std::ios::left)<<std::setw(7) <<"mm"
			dump_general_note();
			std::cout<<std::endl;
		} 
		int index = BooleanSolid_GenIndex[i];
		std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
			<<std::setiosflags(std::ios::left)<<std::setw(7) <<BooleanSolid_type[i]
			<<std::setiosflags(std::ios::left)<<std::setw(14)<<BooleanSolid_sol1[i]
			<<std::setiosflags(std::ios::left)<<std::setw(14)<<BooleanSolid_sol2[i];
//			<<std::setiosflags(std::ios::left)<<std::setw(7) <<BooleanSolid_Ephi[i]/rad
//			<<std::setiosflags(std::ios::left)<<std::setw(7) <<BooleanSolid_Etheta[i]/rad
//			<<std::setiosflags(std::ios::left)<<std::setw(7) <<BooleanSolid_Epsi[i]/rad
//			<<std::setiosflags(std::ios::left)<<std::setw(7) <<BooleanSolid_PosX[i]/mm
//			<<std::setiosflags(std::ios::left)<<std::setw(7) <<BooleanSolid_PosY[i]/mm
//			<<std::setiosflags(std::ios::left)<<std::setw(7) <<BooleanSolid_PosZ[i]/mm
		dump_general_value(index);
		std::cout<<std::endl;
	}


}

void SimpleGeometryParameter::dump_general_header(){
	std::cout<<std::setiosflags(std::ios::left)<<std::setw(15)<<"Name"
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
	         <<std::setiosflags(std::ios::left)<<std::setw(7) <<"Epsi";
}

void SimpleGeometryParameter::dump_general_note(){
	std::cout<<std::setiosflags(std::ios::left)<<std::setw(15)<<""
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
	         <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg";
}

void SimpleGeometryParameter::dump_general_value(G4int index){
	std::cout<<std::setiosflags(std::ios::left)<<std::setw(15)<<Name[index];
	if (!SolidBoolean[index]){
		std::cout<<std::setiosflags(std::ios::left)<<std::setw(15)<<MotherName[index]
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
				 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Epsi[index]/deg;
	}
	else{
		std::cout<<"This is Boolean Solid Component";
	}
}
