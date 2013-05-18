//---------------------------------------------------------------------------//
//Description: In charge of FormulizedGeometryParameter
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Modified:
//Comment: The units are "mm"(default) and "rad". 
//Comment: fVerboseLevel: 0:    Just Warning Messages and Error Messages
//                        1:    + Dump()
//                        >= 5: All informations
//---------------------------------------------------------------------------//

#ifndef FormulizedGeometryParameter_H
#define FormulizedGeometryParameter_H 

#include "globals.hh"

#include "SimpleGeometryParameter.hh"

#include <vector>
#include <iostream>

class FormulizedGeometryParameter : public SimpleGeometryParameter
{

public:
  FormulizedGeometryParameter(G4String name, G4String opt = "");
  virtual ~FormulizedGeometryParameter(void);

  virtual void InitFromFile(G4String);  //Get necessary parameters from given file

	virtual void Dump();

	//=> Special functions
	int GetValue(G4String);

	void Preset();

	bool CheckInfo();

  void DumpInfo(void);  //Output info

	void Calculate();

private:

	void ReSetVis(); //Reset parameters to default values

public:

	//=>Access
	//General info for volume
	int get_VolIndex(G4String name) {
		int index = -1;
		for ( int i = 0; i < VolNo; i++ ){
			if ( Name[i] == name ){
				index = i;
				break;
			}
		}
		return index;
	}
  int get_VolNo() { return VolNo; }
  G4double get_PosX(int VolId, int i) {if( check_VolId(VolId) && check_RepId(VolId, i) ) return PosX[VolId][i]; else return 0;}
  G4double get_PosY(int VolId, int i) {if( check_VolId(VolId) && check_RepId(VolId, i) ) return PosY[VolId][i]; else return 0;}
  G4double get_PosZ(int VolId, int i) {if( check_VolId(VolId) && check_RepId(VolId, i) ) return PosZ[VolId][i]; else return 0;}
  G4String get_name(int VolId) {if( check_VolId(VolId) ) return Name[VolId]; else return "";}
  G4String get_SDName(int VolId) {if( check_VolId(VolId) ) return SDName[VolId]; else return "";}
  G4String get_MotherName(int VolId) {if( check_VolId(VolId) ) return MotherName[VolId]; else return "";}
  G4String get_material(int VolId) {if( check_VolId(VolId) ) return Material[VolId]; else return "";}
  int get_SRepNo(int VolId) {if(check_VolId(VolId)) return SRepNo[VolId]; else return 0;}
  int get_RepNo(int VolId) {if(check_VolId(VolId)) return RepNo[VolId]; else return 0;}
  G4String get_SolidType(int VolId) {if(check_VolId(VolId)) return SolidType[VolId]; else return "";}
  int get_SolidIndex(int VolId) {if(check_VolId(VolId)) return SolidIndex[VolId]; else return 0;}
  G4double get_Ephi(int VolId, int i) {if( check_VolId(VolId) && check_RepId(VolId, i) ) return Ephi[VolId][i]; else return 0;}
  G4double get_Etheta(int VolId, int i) {if( check_VolId(VolId) && check_RepId(VolId, i) ) return Etheta[VolId][i]; else return 0;}
  G4double get_Epsi(int VolId, int i) {if( check_VolId(VolId) && check_RepId(VolId, i) ) return Epsi[VolId][i]; else return 0;}

	//Box info
  int get_BoxNo() {return BoxNo;}
	G4double get_Box_X(int BoxId, int i) {int VolId = Box_GenIndex[BoxId]; if( check_BoxId(BoxId) && check_RepId(VolId, i) ) return Box_X[BoxId][i]; else return 0;}
	G4double get_Box_Y(int BoxId, int i) {int VolId = Box_GenIndex[BoxId]; if( check_BoxId(BoxId) && check_RepId(VolId, i) ) return Box_Y[BoxId][i]; else return 0;}
	G4double get_Box_Z(int BoxId, int i) {int VolId = Box_GenIndex[BoxId]; if( check_BoxId(BoxId) && check_RepId(VolId, i) ) return Box_Z[BoxId][i]; else return 0;}

	//Tubs info
  int get_TubsNo() {return TubsNo;}
	G4double get_Tubs_RMax(int TubsId, int i) {if( check_TubsId(TubsId) ) return Tubs_RMax[TubsId][i]; else return 0;}
	G4double get_Tubs_RMin(int TubsId, int i) {if( check_TubsId(TubsId) ) return Tubs_RMin[TubsId][i]; else return 0;}
	G4double get_Tubs_Length(int TubsId, int i) {if( check_TubsId(TubsId) ) return Tubs_Length[TubsId][i]; else return 0;}
	G4double get_Tubs_StartAng(int TubsId, int i) {if( check_TubsId(TubsId) ) return Tubs_StartAng[TubsId][i]; else return 0;}
	G4double get_Tubs_SpanAng(int TubsId, int i) {if( check_TubsId(TubsId) ) return Tubs_SpanAng[TubsId][i]; else return 0;}

	//Sphere info
  int get_SphereNo() {return SphereNo;}
	G4double get_Sphere_RMax(int SphereId, int i) {if( check_SphereId(SphereId) ) return Sphere_RMax[SphereId][i]; else return 0;}
	G4double get_Sphere_RMin(int SphereId, int i) {if( check_SphereId(SphereId) ) return Sphere_RMin[SphereId][i]; else return 0;}
	G4double get_Sphere_StartPhi(int SphereId, int i) {if( check_SphereId(SphereId) ) return Sphere_StartPhi[SphereId][i]; else return 0;}
	G4double get_Sphere_SpanPhi(int SphereId, int i) {if( check_SphereId(SphereId) ) return Sphere_SpanPhi[SphereId][i]; else return 0;}
	G4double get_Sphere_StartTheta(int SphereId, int i) {if( check_SphereId(SphereId) ) return Sphere_StartTheta[SphereId][i]; else return 0;}
	G4double get_Sphere_SpanTheta(int SphereId, int i) {if( check_SphereId(SphereId) ) return Sphere_SpanTheta[SphereId][i]; else return 0;}

	//visual settings
//	bool get_vis(int VolId){if( check_VolId(VolId) ) return vVis[VolId]; else return 0;}
	bool get_vis(int VolId){
		if( check_VolId(VolId) ) return vVis[VolId]; else return 0;
	}
	G4double get_r(int VolId){if( check_VolId(VolId) ) return vR[VolId]; else return 0;}
	G4double get_g(int VolId){if( check_VolId(VolId) ) return vG[VolId]; else return 0;}
	G4double get_b(int VolId){if( check_VolId(VolId) ) return vB[VolId]; else return 0;}

	//=>Modify
	//General info for volume
  void set_fPosX(G4String type, int i, G4String val) { int k = get_index(type, i); fPosX[k] = val;}
  void set_fPosY(G4String type, int i, G4String val) { int k = get_index(type, i); fPosY[k] = val;}
  void set_fPosZ(G4String type, int i, G4String val) { int k = get_index(type, i); fPosZ[k] = val;}
  void set_fEphi(G4String type, int i, G4String val) { int k = get_index(type, i); fEphi[k] = val;}
  void set_fEtheta(G4String type, int i, G4String val) { int k = get_index(type, i); fEtheta[k] = val;}
  void set_fEpsi(G4String type, int i, G4String val) { int k = get_index(type, i); fEpsi[k] = val;}
  void set_PosX(G4String type, int i, int RepId, G4double val) { int k = get_index(type, i); PosX[k][RepId] = val;}
  void set_PosY(G4String type, int i, int RepId, G4double val) { int k = get_index(type, i); PosY[k][RepId] = val;}
  void set_PosZ(G4String type, int i, int RepId, G4double val) { int k = get_index(type, i); PosZ[k][RepId] = val;}
  void set_Ephi(G4String type, int i, int RepId, G4double val) { int k = get_index(type, i); Ephi[k][RepId] = val;}
  void set_Etheta(G4String type, int i, int RepId, G4double val) { int k = get_index(type, i); Etheta[k][RepId] = val;}
  void set_Epsi(G4String type, int i, int RepId, G4double val) { int k = get_index(type, i); Epsi[k][RepId] = val;}
  void set_name(G4String type, int i,G4String val) { int k = get_index(type, i); Name[k] = val;}
  void set_SDName(G4String type, int i,G4String val) { int k = get_index(type, i); SDName[k] = val;}
  void set_MotherName(G4String type, int i,G4String val) { int k = get_index(type, i); MotherName[k] = val;}
  void set_material(G4String type, int i,G4String val) { int k = get_index(type, i); Material[k] = val;}
  void set_SRepNo(G4String type, int i,int val) {int k = get_index(type, i); SRepNo[k] = val;}
  void set_RepNo(G4String type, int i,int val) {int k = get_index(type, i); RepNo[k] = val;}

	//Box info
	void set_fBox_X(int i, G4String val) { fBox_X[i] = val;}
	void set_fBox_Y(int i, G4String val) { fBox_Y[i] = val;}
	void set_fBox_Z(int i, G4String val) { fBox_Z[i] = val;}
	void set_Box_X(int i, int RepId, G4double val) { Box_X[i][RepId] = val;}
	void set_Box_Y(int i, int RepId, G4double val) { Box_Y[i][RepId] = val;}
	void set_Box_Z(int i, int RepId, G4double val) { Box_Z[i][RepId] = val;}

	//Tubs info
	void set_fTubs_RMax(int i,G4String val) { fTubs_RMax[i] = val;}
	void set_fTubs_RMin(int i,G4String val) { fTubs_RMin[i] = val;}
	void set_fTubs_Length(int i,G4String val) { fTubs_Length[i] = val;}
	void set_fTubs_StartAng(int i,G4String val) { fTubs_StartAng[i] = val;}
	void set_fTubs_SpanAng(int i,G4String val) { fTubs_SpanAng[i] = val;}
	void set_Tubs_RMax(int i, int RepId,G4double val) { Tubs_RMax[i][RepId] = val;}
	void set_Tubs_RMin(int i, int RepId,G4double val) { Tubs_RMin[i][RepId] = val;}
	void set_Tubs_Length(int i, int RepId,G4double val) { Tubs_Length[i][RepId] = val;}
	void set_Tubs_StartAng(int i, int RepId,G4double val) { Tubs_StartAng[i][RepId] = val;}
	void set_Tubs_SpanAng(int i, int RepId,G4double val) { Tubs_SpanAng[i][RepId] = val;}

	//Sphere info
	void set_fSphere_RMax(int i,G4String val) { fSphere_RMax[i] = val;}
	void set_fSphere_RMin(int i,G4String val) { fSphere_RMin[i] = val;}
	void set_fSphere_StartPhi(int i,G4String val) { fSphere_StartPhi[i] = val;}
	void set_fSphere_SpanPhi(int i,G4String val) { fSphere_SpanPhi[i] = val;}
	void set_fSphere_StartTheta(int i,G4String val) { fSphere_StartTheta[i] = val;}
	void set_fSphere_SpanTheta(int i,G4String val) { fSphere_SpanTheta[i] = val;}
	void set_Sphere_RMax(int i, int RepId,G4double val) { Sphere_RMax[i][RepId] = val;}
	void set_Sphere_RMin(int i, int RepId,G4double val) { Sphere_RMin[i][RepId] = val;}
	void set_Sphere_StartPhi(int i, int RepId,G4double val) { Sphere_StartPhi[i][RepId] = val;}
	void set_Sphere_SpanPhi(int i, int RepId,G4double val) { Sphere_SpanPhi[i][RepId] = val;}
	void set_Sphere_StartTheta(int i, int RepId,G4double val) { Sphere_StartTheta[i][RepId] = val;}
	void set_Sphere_SpanTheta(int i, int RepId,G4double val) { Sphere_SpanTheta[i][RepId] = val;}

	//visual settings
	void set_vis(G4String type, int i,bool val) { int k = get_index(type, i); vVis[k] = val;}
	void set_r(G4String type, int i,G4double val) { int k = get_index(type, i); vR[k] = val;}
	void set_g(G4String type, int i,G4double val) { int k = get_index(type, i); vG[k] = val;}

	void set_vis(G4int i,bool val)   { vVis[i] = val;}
	void set_r(G4int i,G4double val) { vR[i] = val;}
	void set_g(G4int i,G4double val) { vG[i] = val;}
	void set_b(G4int i,G4double val) { vB[i] = val;}
	void set_b(G4String type, int i,G4double val) { int k = get_index(type, i); vB[k] = val;}

private:

	//Get VolId according to type name and index
	int get_index( G4String type, int i ){
		int k = -1;
		if ( type == "Box" && check_BoxId(i) ) k = Box_GenIndex[i];
		else if ( type == "Tubs" && check_TubsId(i) ) k = Tubs_GenIndex[i];
		else if ( type == "Sphere" && check_SphereId(i) ) k = Sphere_GenIndex[i];
		else DEBUG("Wrong index number or unknown volume type!");
		return k;
	}

	//=> check
	bool check_VolId(int VolId){
		if ( VolId < VolNo ) return true;
		else{
			std::cout<<"In FormulizedGeometryParameter, VolId is out of range! VolId = "<<VolId<<", VolNo = "<<VolNo<<std::endl;
			return false;
		}
	}
	bool check_BoxId(int BoxId){
		if ( BoxId < BoxNo ) return true;
		else{
			std::cout<<"In FormulizedGeometryParameter, BoxId is out of range! BoxId = "<<BoxId<<", BoxNo = "<<BoxNo<<std::endl;
			return false;
		}
	}
	bool check_BoxNo( int No ){
		if( No != BoxNo ){
			std::cout<<"In FormulizedGeometryParameter::InitFromFile(), insufficient Box info! BoxNo = "<<BoxNo<<", while only "<<No<<" Box were found."<<std::endl;
			return false;
		}
		else return true;
	}
	bool check_TubsId(int TubsId){
		if ( TubsId < TubsNo ) return true;
		else{
			std::cout<<"In FormulizedGeometryParameter, TubsId is out of range! TubsId = "<<TubsId<<", TubsNo = "<<TubsNo<<std::endl;
			return false;
		}
	}
	bool check_TubsNo( int No ){
		if( No != TubsNo ){
			std::cout<<"In FormulizedGeometryParameter::InitFromFile(), insufficient Tubs info! TubsNo = "<<TubsNo<<", while only "<<No<<" Tubs were found."<<std::endl;
			return false;
		}
		else return true;
	}
	bool check_SphereId(int SphereId){
		if ( SphereId < SphereNo ) return true;
		else{
			std::cout<<"In FormulizedGeometryParameter, SphereId is out of range! SphereId = "<<SphereId<<", SphereNo = "<<SphereNo<<std::endl;
			return false;
		}
	}
	bool check_SphereNo( int No ){
		if( No != SphereNo ){
			std::cout<<"In FormulizedGeometryParameter::InitFromFile(), insufficient Sphere info! SphereNo = "<<SphereNo<<", while only "<<No<<" Sphere were found."<<std::endl;
			return false;
		}
		else return true;
	}
	bool check_RepId( int VolId, int i ){
		if( i < SRepNo[VolId] || i >= SRepNo[VolId] + RepNo[VolId] ){
			std::cout<<"In FormulizedGeometryParameter::InitFromFile(), Wrong Replica Index! RepId = "<<i<<", while RepNo start from "<<SRepNo[VolId]<<" to "<<(SRepNo[VolId] + RepNo[VolId] -1)<<"."<<std::endl;
			return false;
		}
		else return true;
	}

private:

	//General info about volume
	int VolNo;
  std::vector<G4String> fPosX;
  std::vector<G4String> fPosY;
  std::vector<G4String> fPosZ;
  std::vector<G4String> fEphi;
  std::vector<G4String> fEtheta;
  std::vector<G4String> fEpsi;
  std::vector<std::vector<G4double> > PosX;
  std::vector<std::vector<G4double> > PosY;
  std::vector<std::vector<G4double> > PosZ;
  std::vector<std::vector<G4double> > Ephi;
  std::vector<std::vector<G4double> > Etheta;
  std::vector<std::vector<G4double> > Epsi;
  std::vector<G4String> Name;
  std::vector<G4String> MotherName;
  std::vector<G4String> SDName;
  std::vector<G4String> Material;
	std::vector<int>    SRepNo;
	std::vector<int>    RepNo;
	std::vector<G4String> SolidType;
	std::vector<int>    SolidIndex;

	//Box info
  int BoxNo;
	std::vector<G4String> fBox_X;
	std::vector<G4String> fBox_Y;
	std::vector<G4String> fBox_Z;
	std::vector<std::vector<G4double> > Box_X;
	std::vector<std::vector<G4double> > Box_Y;
	std::vector<std::vector<G4double> > Box_Z;
	std::vector<int>    Box_GenIndex;

	//Tubs info
  int TubsNo;
	std::vector<G4String> fTubs_RMax;
	std::vector<G4String> fTubs_RMin;
	std::vector<G4String> fTubs_Length;
	std::vector<G4String> fTubs_StartAng;
	std::vector<G4String> fTubs_SpanAng;
	std::vector<std::vector<G4double> > Tubs_RMax;
	std::vector<std::vector<G4double> > Tubs_RMin;
	std::vector<std::vector<G4double> > Tubs_Length;
	std::vector<std::vector<G4double> > Tubs_StartAng;
	std::vector<std::vector<G4double> > Tubs_SpanAng;
	std::vector<int>    Tubs_GenIndex;

	//Sphere info
  int SphereNo;
	std::vector<G4String> fSphere_RMax;
	std::vector<G4String> fSphere_RMin;
	std::vector<G4String> fSphere_StartPhi;
	std::vector<G4String> fSphere_SpanPhi;
	std::vector<G4String> fSphere_StartTheta;
	std::vector<G4String> fSphere_SpanTheta;
	std::vector<std::vector<G4double> > Sphere_RMax;
	std::vector<std::vector<G4double> > Sphere_RMin;
	std::vector<std::vector<G4double> > Sphere_StartPhi;
	std::vector<std::vector<G4double> > Sphere_SpanPhi;
	std::vector<std::vector<G4double> > Sphere_StartTheta;
	std::vector<std::vector<G4double> > Sphere_SpanTheta;
	std::vector<int>    Sphere_GenIndex;

	//visual settings
	std::vector<bool> vVis;
	std::vector<G4double> vR, vG, vB;

	//to control
	bool notReSetVis;

};

#endif
