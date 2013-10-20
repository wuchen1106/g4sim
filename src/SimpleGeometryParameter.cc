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

#include "TF1.h"

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
	G4String s_card;
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

double SimpleGeometryParameter::CalFormula(G4String formula, int iRep){
	ReplaceMacro(formula);
	TF1 *f1 = new TF1("f1", formula);
	double value = f1->Eval(iRep);
	delete f1;
	return value;
}

void SimpleGeometryParameter::ReplaceMacro(G4String &formula){
//	std::cout<<"TO replace for: \""<<formula<<"\""<<std::endl; // to be deleted
	std::vector<G4String> words = GetWords(formula);
//	std::cout<<"	"<<words.size()<<" words"<<std::endl; // to be deleted
	for (int iWord = 0; iWord < words.size(); iWord++ ){
//		std::cout<<"		"<<iWord<<std::endl; // to be deleted
		G4String value;
		if (FindMacro(words[iWord],value)){
			Replace(formula,words[iWord],value);
		}
	}
}

std::vector<G4String> SimpleGeometryParameter::GetWords(G4String formula){
	std::vector<G4String> words;
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
				G4String word = temp;
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

bool SimpleGeometryParameter::FindMacro(G4String word, G4String& value){
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

void SimpleGeometryParameter::Replace(G4String &formula, G4String word, G4String value){
//	std::cout<<"-- \""<<formula<<"\""<<std::endl; // to be deleted
	G4String newform = "";
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
					G4String newformtemp = cnewform;
					newform=newform+newformtemp;
				}
				G4String word = temp;
//				std::cout<<"		\""<<word<<"\""<<std::endl; // to be deleted
				G4String newword;
				bool found = FindMacro(word,newword);
				if (found){
					newform=newform+"("+newword+")";
				}
				else{
					newform=newform+word;
				}
//				std::cout<<"		to \""<<newword<<"\""<<std::endl; // to be deleted
			}
			if(!isword){
				cnewform[newformoffset++] = cformula[offset];
			}
			if (offset==length-1){
				cnewform[newformoffset] = '\0';
				G4String newformtemp = cnewform;
				newform=newform+newformtemp;
			}
		}
	}
//	std::cout<<"	-->\""<<newform<<"\""<<std::endl; // to be deleted
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
	G4String name;
	buf_card>>name;
	G4String s_para;
	int iVol = VolNo;
	bool GoOn = true;
//	std::cout<<"==> "<<s_card<<std::endl; // to be deleted
	if (waited_Polycone_iVol>=0){
		GoOn = false;
		if (achieved_componets_Polycone<Polycone_numZ[waited_Polycone_iVol]){
			if (name=="PCI"){
				G4String dump;
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
			G4String dump;
			G4String tBox_X;
			G4String tBox_Y;
			G4String tBox_Z;
			buf_card>>dump>>tBox_X>>tBox_Y>>tBox_Z;
			G4int repNo = 1;
			if (buf_card>>dump){
				buf_card>>dump>>dump>>dump>>dump>>dump>>dump>>dump;
				int TEMP;
				get_RepCont(dump,TEMP,repNo);
			}
			std::vector<double> vBox_X;
			std::vector<double> vBox_Y;
			std::vector<double> vBox_Z;
			for (int iRep = 0; iRep <repNo; iRep++){
				vBox_X.push_back(CalFormula(tBox_X,iRep)*mm);
				vBox_Y.push_back(CalFormula(tBox_Y,iRep)*mm);
				vBox_Z.push_back(CalFormula(tBox_Z,iRep)*mm);
			}
			Box_X.push_back(vBox_X);
			Box_Y.push_back(vBox_Y);
			Box_Z.push_back(vBox_Z);
			Box_GenIndex.push_back(iVol);
			BoxNo++;
			foundVolume = true;
		}
		else if( name == "T" ){
			SolidType.push_back("Tubs");
			SolidIndex.push_back(Tubs_RMax.size());
			G4String dump;
			G4String tTubs_RMax;
			G4String tTubs_RMin;
			G4String tTubs_Length;
			G4String tTubs_StartAng;
			G4String tTubs_SpanAng;
			buf_card>>dump>>tTubs_RMin>>tTubs_RMax>>tTubs_Length>>tTubs_StartAng>>tTubs_SpanAng;
			G4int repNo = 1;
			if (buf_card>>dump){
				buf_card>>dump>>dump>>dump>>dump>>dump>>dump>>dump;
				int TEMP;
				get_RepCont(dump,TEMP,repNo);
			}
			std::vector<double> vTubs_RMax;
			std::vector<double> vTubs_RMin;
			std::vector<double> vTubs_Length;
			std::vector<double> vTubs_StartAng;
			std::vector<double> vTubs_SpanAng;
			for (int iRep = 0; iRep <repNo; iRep++){
				vTubs_RMax.push_back(CalFormula(tTubs_RMax,iRep)*mm);
				vTubs_RMin.push_back(CalFormula(tTubs_RMin,iRep)*mm);
				vTubs_Length.push_back(CalFormula(tTubs_Length,iRep)*mm);
				vTubs_StartAng.push_back(CalFormula(tTubs_StartAng,iRep)*deg);
				vTubs_SpanAng.push_back(CalFormula(tTubs_SpanAng,iRep)*deg);
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
			G4String dump;
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
			G4String dump;
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
			G4String dump;
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
			G4String dump;
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
			G4String dump;
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
			G4String dump;
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
			G4String dump;
			G4String tBooleanSolid_type;
			G4String tBooleanSolid_sol1;
			G4String tBooleanSolid_sol2;
			G4String tBooleanSolid_Ephi;
			G4String tBooleanSolid_Etheta;
			G4String tBooleanSolid_Epsi;
			G4String tBooleanSolid_PosX;
			G4String tBooleanSolid_PosY;
			G4String tBooleanSolid_PosZ;
			buf_card>>dump>>tBooleanSolid_type>>tBooleanSolid_sol1>>tBooleanSolid_sol2>>tBooleanSolid_Ephi>>tBooleanSolid_Etheta>>tBooleanSolid_Epsi>>tBooleanSolid_PosX>>tBooleanSolid_PosY>>tBooleanSolid_PosZ;
			G4int repNo = 1;
			if (buf_card>>dump){
				buf_card>>dump>>dump>>dump>>dump>>dump>>dump>>dump;
				int TEMP;
				get_RepCont(dump,TEMP,repNo);
			}
			std::vector<double> vBooleanSolid_Ephi;
			std::vector<double> vBooleanSolid_Etheta;
			std::vector<double> vBooleanSolid_Epsi;
			std::vector<double> vBooleanSolid_PosX;
			std::vector<double> vBooleanSolid_PosY;
			std::vector<double> vBooleanSolid_PosZ;
			for (int iRep = 0; iRep <repNo; iRep++){
//				std::cout<<"Calculating: \""<<tBooleanSolid_Ephi<<"\""<<std::endl; // to be deleted
				vBooleanSolid_Ephi.push_back(CalFormula(tBooleanSolid_Ephi,iRep)*deg);
//				std::cout<<"Calculating: \""<<tBooleanSolid_Etheta<<"\""<<std::endl; // to be deleted
				vBooleanSolid_Etheta.push_back(CalFormula(tBooleanSolid_Etheta,iRep)*deg);
//				std::cout<<"Calculating: \""<<tBooleanSolid_Epsi<<"\""<<std::endl; // to be deleted
				vBooleanSolid_Epsi.push_back(CalFormula(tBooleanSolid_Epsi,iRep)*deg);
//				std::cout<<"Calculating: \""<<tBooleanSolid_PosX<<"\""<<std::endl; // to be deleted
				vBooleanSolid_PosX.push_back(CalFormula(tBooleanSolid_PosX,iRep)*mm);
//				std::cout<<"Calculating: \""<<tBooleanSolid_PosY<<"\""<<std::endl; // to be deleted
				vBooleanSolid_PosY.push_back(CalFormula(tBooleanSolid_PosY,iRep)*mm);
//				std::cout<<"Calculating: \""<<tBooleanSolid_PosZ<<"\""<<std::endl; // to be deleted
				vBooleanSolid_PosZ.push_back(CalFormula(tBooleanSolid_PosZ,iRep)*mm);
			}
			BooleanSolid_type.push_back(tBooleanSolid_type);
			BooleanSolid_sol1.push_back(tBooleanSolid_sol1);
			BooleanSolid_sol2.push_back(tBooleanSolid_sol2);
			BooleanSolid_Ephi.push_back(vBooleanSolid_Ephi);
			BooleanSolid_Etheta.push_back(vBooleanSolid_Etheta);
			BooleanSolid_Epsi.push_back(vBooleanSolid_Epsi);
			BooleanSolid_PosX.push_back(vBooleanSolid_PosX);
			BooleanSolid_PosY.push_back(vBooleanSolid_PosY);
			BooleanSolid_PosZ.push_back(vBooleanSolid_PosZ);
			BooleanSolid_GenIndex.push_back(iVol);
			BooleanSolidNo++;
			foundVolume = true;
		}
		if (foundVolume){
			G4String tPosX;
			G4String tPosY;
			G4String tPosZ;
			G4String tRepCont;
			G4int tRepNo;
			G4int tSRepNo;
			G4String tSDName;
			G4String tEphi;
			G4String tEtheta;
			G4String tEpsi;
			G4String tName;
			G4String tMotherName;
			G4String tMaterial;
			G4bool   tSolidBoolean = true;
			buf_card>>tName;
			std::vector<double> vPosX;
			std::vector<double> vPosY;
			std::vector<double> vPosZ;
			std::vector<double> vEphi;
			std::vector<double> vEtheta;
			std::vector<double> vEpsi;
			if(buf_card>>tMotherName){
				tSolidBoolean = false;
				buf_card>>tMaterial>>tSDName>>tPosX>>tPosY>>tPosZ>>tRepCont>>tEphi>>tEtheta>>tEpsi;
				get_RepCont(tRepCont,tSRepNo,tRepNo);
				for (int iRep = 0; iRep <tRepNo; iRep++){
					vPosX.push_back(CalFormula(tPosX,iRep)*mm);
					vPosY.push_back(CalFormula(tPosY,iRep)*mm);
					vPosZ.push_back(CalFormula(tPosZ,iRep)*mm);
					vEphi.push_back(CalFormula(tEphi,iRep)*deg);
					vEtheta.push_back(CalFormula(tEtheta,iRep)*deg);
					vEpsi.push_back(CalFormula(tEpsi,iRep)*deg);
				}
			}
			PosX.push_back(vPosX);
			PosY.push_back(vPosY);
			PosZ.push_back(vPosZ);
			Name.push_back(tName);
			MotherName.push_back(tMotherName);
			Material.push_back(tMaterial);
			SDName.push_back(tSDName);
			SRepNo.push_back(tSRepNo);
			RepNo.push_back(tRepNo);
			Ephi.push_back(vEphi);
			Etheta.push_back(vEtheta);
			Epsi.push_back(vEpsi);
			SolidBoolean.push_back(tSolidBoolean);
			VolNo++;
		}
		else if ( name == "DEFINE:" ){
			G4String MacroName;
			G4String MacroValue;
			buf_card>>MacroName>>MacroValue;
//			std::cout<<"found DEFINE:"<<std::endl; // to be deleted
			ReplaceMacro(MacroValue);
			knownValueNames.push_back(MacroName);
			knownValues.push_back(MacroValue);
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
					if( (std::string)name == buf_temp.str() ){
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
		int repNo = RepNo[index];
		for ( G4int j = 0; j < repNo; j++ ){
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<Box_X[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<Box_Y[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<Box_Z[i][j]/mm;
			dump_general_value(index,j);
		}
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
		int repNo = RepNo[index];
		for ( G4int j = 0; j < repNo; j++ ){
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<Tubs_RMin[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<Tubs_RMax[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<Tubs_Length[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(9) <<Tubs_StartAng[i][j]/deg
				<<std::setiosflags(std::ios::left)<<std::setw(8) <<Tubs_SpanAng[i][j]/deg;
			dump_general_value(index,j);
		}
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
			dump_general_header();
			std::cout<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(7) <<""
			         <<std::setiosflags(std::ios::left)<<std::setw(14) <<""
			         <<std::setiosflags(std::ios::left)<<std::setw(14) <<"";
			dump_general_note();
			std::cout<<std::endl;
		} 
		int index = BooleanSolid_GenIndex[i];
		int repNo = RepNo[index];
		for ( G4int j = 0; j < repNo; j++ ){
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
				<<std::setiosflags(std::ios::left)<<std::setw(7) <<BooleanSolid_type[i]
				<<std::setiosflags(std::ios::left)<<std::setw(14)<<BooleanSolid_sol1[i]
				<<std::setiosflags(std::ios::left)<<std::setw(14)<<BooleanSolid_sol2[i];
			dump_general_value(index,j);
		}
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
	         <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
	         <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg"
	         <<std::setiosflags(std::ios::left)<<std::setw(7) <<"deg";
}

void SimpleGeometryParameter::dump_general_value(G4int index, G4int j){
	std::cout<<std::setiosflags(std::ios::left)<<std::setw(15)<<Name[index];
	if (!SolidBoolean[index]){
		std::cout<<std::setiosflags(std::ios::left)<<std::setw(15)<<MotherName[index]
				 <<std::setiosflags(std::ios::left)<<std::setw(15)<<Material[index]
				 <<std::setiosflags(std::ios::left)<<std::setw(15)<<SDName[index]
				 <<std::setiosflags(std::ios::left)<<std::setw(7) <<PosX[index][j]/mm
				 <<std::setiosflags(std::ios::left)<<std::setw(7) <<PosY[index][j]/mm
				 <<std::setiosflags(std::ios::left)<<std::setw(7) <<PosZ[index][j]/mm
				 <<std::setiosflags(std::ios::left)<<std::setw(6) <<RepNo[index]
				 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Ephi[index][j]/deg
				 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Etheta[index][j]/deg
				 <<std::setiosflags(std::ios::left)<<std::setw(7) <<Epsi[index][j]/deg;
	}
	else{
		std::cout<<"This is Boolean Solid Component";
	}
}
