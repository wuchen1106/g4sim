//---------------------------------------------------------------------------//
//Description: In charge of SimpleGeometryParameter
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Modified: 11, Jan, 2013
//          Support G4Hype, G4TwistedTube
//Comment: The units are "mm"(default) and "rad". 
//Comment: fVerboseLevel: 0:    Just Warning Messages and Error Messages
//                        1:    + Dump()
//                        >= 5: All informations
//---------------------------------------------------------------------------//

#ifndef SimpleGeometryParameter_H
#define SimpleGeometryParameter_H 

#include "globals.hh"

#include "MyVGeometryParameter.hh"

#include <vector>
#include <iostream>

class SimpleGeometryParameter : public MyVGeometryParameter
{

	public:
		SimpleGeometryParameter(G4String name, G4String opt = "");
		virtual ~SimpleGeometryParameter(void);

		virtual void InitFromFile(G4String);  //Get necessary parameters from given file

		virtual void Calculate();

		virtual void Dump();

		//=> Special functions
		int GetValue(G4String);

		void Preset();

		bool CheckInfo();

		void DumpInfo(void);  //Output info

	private:

		void ReSetVis(); //Reset parameters to default values

	public:

		//=>Access
		//General info for volume
		G4int get_VolIndex(G4String name) {
			int index = -1;
			for ( int i = 0; i < VolNo; i++ ){
				if ( Name[i] == name ){
					index = i;
					break;
				}
			}
			return index;
		}
		G4int get_VolNo() { return VolNo; }
		G4double get_PosX(G4int VolId) {if( check_VolId(VolId) ) return PosX[VolId]; else return 0;}
		G4double get_PosY(G4int VolId) {if( check_VolId(VolId) ) return PosY[VolId]; else return 0;}
		G4double get_PosZ(G4int VolId) {if( check_VolId(VolId) ) return PosZ[VolId]; else return 0;}
		G4String get_name(G4int VolId) {if( check_VolId(VolId) ) return Name[VolId]; else return "";}
		G4String get_SDName(G4int VolId) {if( check_VolId(VolId) ) return SDName[VolId]; else return "";}
		G4String get_MotherName(G4int VolId) {if( check_VolId(VolId) ) return MotherName[VolId]; else return "";}
		G4String get_material(G4int VolId) {if( check_VolId(VolId) ) return Material[VolId]; else return "";}
		G4int get_SRepNo(G4int VolId) {if(check_VolId(VolId)) return SRepNo[VolId]; else return 0;}
		G4int get_RepNo(G4int VolId) {if(check_VolId(VolId)) return RepNo[VolId]; else return 0;}
		G4double get_Space(G4int VolId) {if(check_VolId(VolId)) return Space[VolId]; else return 0;}
		G4double get_DirTheta(G4int VolId) {if(check_VolId(VolId)) return DirTheta[VolId]; else return 0;}
		G4double get_DirPhi(G4int VolId) {if(check_VolId(VolId)) return DirPhi[VolId]; else return 0;}
		G4String get_SolidType(G4int VolId) {if(check_VolId(VolId)) return SolidType[VolId]; else return "";}
		G4int get_SolidIndex(G4int VolId) {if(check_VolId(VolId)) return SolidIndex[VolId]; else return 0;}
		G4double get_Ephi(G4int VolId) {if( check_VolId(VolId) ) return Ephi[VolId]; else return 0;}
		G4double get_Etheta(G4int VolId) {if( check_VolId(VolId) ) return Etheta[VolId]; else return 0;}
		G4double get_Epsi(G4int VolId) {if( check_VolId(VolId) ) return Epsi[VolId]; else return 0;}

		//Box info
		G4int get_BoxNo() {return BoxNo;}
		G4double get_Box_X(G4int BoxId) {if( check_BoxId(BoxId) ) return Box_X[BoxId]; else return 0;}
		G4double get_Box_Y(G4int BoxId) {if( check_BoxId(BoxId) ) return Box_Y[BoxId]; else return 0;}
		G4double get_Box_Z(G4int BoxId) {if( check_BoxId(BoxId) ) return Box_Z[BoxId]; else return 0;}

		//Tubs info
		G4int get_TubsNo() {return TubsNo;}
		G4double get_Tubs_RMax(G4int TubsId) {if( check_TubsId(TubsId) ) return Tubs_RMax[TubsId]; else return 0;}
		G4double get_Tubs_RMin(G4int TubsId) {if( check_TubsId(TubsId) ) return Tubs_RMin[TubsId]; else return 0;}
		G4double get_Tubs_Length(G4int TubsId) {if( check_TubsId(TubsId) ) return Tubs_Length[TubsId]; else return 0;}
		G4double get_Tubs_StartAng(G4int TubsId) {if( check_TubsId(TubsId) ) return Tubs_StartAng[TubsId]; else return 0;}
		G4double get_Tubs_SpanAng(G4int TubsId) {if( check_TubsId(TubsId) ) return Tubs_SpanAng[TubsId]; else return 0;}

		//Torus info
		G4int get_TorusNo() {return TorusNo;}
		G4double get_Torus_RMax(G4int TorusId) {if( check_TorusId(TorusId) ) return Torus_RMax[TorusId]; else return 0;}
		G4double get_Torus_RMin(G4int TorusId) {if( check_TorusId(TorusId) ) return Torus_RMin[TorusId]; else return 0;}
		G4double get_Torus_Rtor(G4int TorusId) {if( check_TorusId(TorusId) ) return Torus_Rtor[TorusId]; else return 0;}
		G4double get_Torus_StartAng(G4int TorusId) {if( check_TorusId(TorusId) ) return Torus_StartAng[TorusId]; else return 0;}
		G4double get_Torus_SpanAng(G4int TorusId) {if( check_TorusId(TorusId) ) return Torus_SpanAng[TorusId]; else return 0;}

		//Sphere info
		G4int get_SphereNo() {return SphereNo;}
		G4double get_Sphere_RMax(G4int SphereId) {if( check_SphereId(SphereId) ) return Sphere_RMax[SphereId]; else return 0;}
		G4double get_Sphere_RMin(G4int SphereId) {if( check_SphereId(SphereId) ) return Sphere_RMin[SphereId]; else return 0;}
		G4double get_Sphere_StartPhi(G4int SphereId) {if( check_SphereId(SphereId) ) return Sphere_StartPhi[SphereId]; else return 0;}
		G4double get_Sphere_SpanPhi(G4int SphereId) {if( check_SphereId(SphereId) ) return Sphere_SpanPhi[SphereId]; else return 0;}
		G4double get_Sphere_StartTheta(G4int SphereId) {if( check_SphereId(SphereId) ) return Sphere_StartTheta[SphereId]; else return 0;}
		G4double get_Sphere_SpanTheta(G4int SphereId) {if( check_SphereId(SphereId) ) return Sphere_SpanTheta[SphereId]; else return 0;}

		//Hype info
		G4int get_HypeNo() {return HypeNo;}
		G4double get_Hype_innerRadius(G4int HypeId) {if( check_HypeId(HypeId) ) return Hype_innerRadius[HypeId]; else return 0;}
		G4double get_Hype_outerRadius(G4int HypeId) {if( check_HypeId(HypeId) ) return Hype_outerRadius[HypeId]; else return 0;}
		G4double get_Hype_innerStereo(G4int HypeId) {if( check_HypeId(HypeId) ) return Hype_innerStereo[HypeId]; else return 0;}
		G4double get_Hype_outerStereo(G4int HypeId) {if( check_HypeId(HypeId) ) return Hype_outerStereo[HypeId]; else return 0;}
		G4double get_Hype_Length(G4int HypeId) {if( check_HypeId(HypeId) ) return Hype_Length[HypeId]; else return 0;}

		//TwistedTubs info
		G4int get_TwistedTubsNo() {return TwistedTubsNo;}
		G4double get_TwistedTubs_twistedangle(G4int TwistedTubsId) {if( check_TwistedTubsId(TwistedTubsId) ) return TwistedTubs_twistedangle[TwistedTubsId]; else return 0;}
		G4double get_TwistedTubs_endinnerrad(G4int TwistedTubsId) {if( check_TwistedTubsId(TwistedTubsId) ) return TwistedTubs_endinnerrad[TwistedTubsId]; else return 0;}
		G4double get_TwistedTubs_endouterrad(G4int TwistedTubsId) {if( check_TwistedTubsId(TwistedTubsId) ) return TwistedTubs_endouterrad[TwistedTubsId]; else return 0;}
		G4double get_TwistedTubs_Length(G4int TwistedTubsId) {if( check_TwistedTubsId(TwistedTubsId) ) return TwistedTubs_Length[TwistedTubsId]; else return 0;}
		G4double get_TwistedTubs_dphi(G4int TwistedTubsId) {if( check_TwistedTubsId(TwistedTubsId) ) return TwistedTubs_dphi[TwistedTubsId]; else return 0;}

		//Cons info
		G4int get_ConsNo() {return ConsNo;}
		G4double get_Cons_RMax1(G4int ConsId) {if( check_ConsId(ConsId) ) return Cons_RMax1[ConsId]; else return 0;}
		G4double get_Cons_RMin1(G4int ConsId) {if( check_ConsId(ConsId) ) return Cons_RMin1[ConsId]; else return 0;}
		G4double get_Cons_RMax2(G4int ConsId) {if( check_ConsId(ConsId) ) return Cons_RMax2[ConsId]; else return 0;}
		G4double get_Cons_RMin2(G4int ConsId) {if( check_ConsId(ConsId) ) return Cons_RMin2[ConsId]; else return 0;}
		G4double get_Cons_Length(G4int ConsId) {if( check_ConsId(ConsId) ) return Cons_Length[ConsId]; else return 0;}
		G4double get_Cons_StartAng(G4int ConsId) {if( check_ConsId(ConsId) ) return Cons_StartAng[ConsId]; else return 0;}
		G4double get_Cons_SpanAng(G4int ConsId) {if( check_ConsId(ConsId) ) return Cons_SpanAng[ConsId]; else return 0;}

		//visual settings
		bool get_vis(G4int VolId){if( check_VolId(VolId) ) return vVis[VolId]; else return 0;}
		G4double get_r(G4int VolId){if( check_VolId(VolId) ) return vR[VolId]; else return 0;}
		G4double get_g(G4int VolId){if( check_VolId(VolId) ) return vG[VolId]; else return 0;}
		G4double get_b(G4int VolId){if( check_VolId(VolId) ) return vB[VolId]; else return 0;}

		//=>Modify
		//General info for volume
		void set_PosX(G4String type, G4int i,G4double val) { int k = get_index(type, i); PosX[k] = val;}
		void set_PosY(G4String type, G4int i,G4double val) { int k = get_index(type, i); PosY[k] = val;}
		void set_PosZ(G4String type, G4int i,G4double val) { int k = get_index(type, i); PosZ[k] = val;}
		void set_name(G4String type, G4int i,G4String val) { int k = get_index(type, i); Name[k] = val;}
		void set_SDName(G4String type, G4int i,G4String val) { int k = get_index(type, i); SDName[k] = val;}
		void set_MotherName(G4String type, G4int i,G4String val) { int k = get_index(type, i); MotherName[k] = val;}
		void set_material(G4String type, G4int i,G4String val) { int k = get_index(type, i); Material[k] = val;}
		void set_SRepNo(G4String type, G4int i,G4int val) { int k = get_index(type, i); SRepNo[k] = val;}
		void set_RepNo(G4String type, G4int i,G4int val) { int k = get_index(type, i); RepNo[k] = val;}
		void set_Space(G4String type, G4int i,G4double val) { int k = get_index(type, i); Space[k] = val;}
		void set_DirTheta(G4String type, G4int i,G4double val) { int k = get_index(type, i); DirTheta[k] = val;}
		void set_DirPhi(G4String type, G4int i,G4double val) { int k = get_index(type, i); DirPhi[k] = val;}
		void set_Ephi(G4String type, G4int i,G4double val) { int k = get_index(type, i); Ephi[k] = val;}
		void set_Etheta(G4String type, G4int i,G4double val) { int k = get_index(type, i); Etheta[k] = val;}
		void set_Epsi(G4String type, G4int i,G4double val) { int k = get_index(type, i); Epsi[k] = val;}

		//Box info
		void set_Box_X(G4int i,G4double val) { Box_X[i] = val;}
		void set_Box_Y(G4int i,G4double val) { Box_Y[i] = val;}
		void set_Box_Z(G4int i,G4double val) { Box_Z[i] = val;}

		//Tubs info
		void set_Tubs_RMax(G4int i,G4double val) { Tubs_RMax[i] = val;}
		void set_Tubs_RMin(G4int i,G4double val) { Tubs_RMin[i] = val;}
		void set_Tubs_Length(G4int i,G4double val) { Tubs_Length[i] = val;}
		void set_Tubs_StartAng(G4int i,G4double val) { Tubs_StartAng[i] = val;}
		void set_Tubs_SpanAng(G4int i,G4double val) { Tubs_SpanAng[i] = val;}

		//Torus info
		void set_Torus_RMax(G4int i,G4double val) { Torus_RMax[i] = val;}
		void set_Torus_RMin(G4int i,G4double val) { Torus_RMin[i] = val;}
		void set_Torus_Rtor(G4int i,G4double val) { Torus_Rtor[i] = val;}
		void set_Torus_StartAng(G4int i,G4double val) { Torus_StartAng[i] = val;}
		void set_Torus_SpanAng(G4int i,G4double val) { Torus_SpanAng[i] = val;}

		//Sphere info
		void set_Sphere_RMax(G4int i,G4double val) { Sphere_RMax[i] = val;}
		void set_Sphere_RMin(G4int i,G4double val) { Sphere_RMin[i] = val;}
		void set_Sphere_StartPhi(G4int i,G4double val) { Sphere_StartPhi[i] = val;}
		void set_Sphere_SpanPhi(G4int i,G4double val) { Sphere_SpanPhi[i] = val;}
		void set_Sphere_StartTheta(G4int i,G4double val) { Sphere_StartTheta[i] = val;}
		void set_Sphere_SpanTheta(G4int i,G4double val) { Sphere_SpanTheta[i] = val;}

		//Hype info
		void set_Hype_innerRadius(G4int i,G4double val) { Hype_innerRadius[i] = val;}
		void set_Hype_outerRadius(G4int i,G4double val) { Hype_outerRadius[i] = val;}
		void set_Hype_innerStereo(G4int i,G4double val) { Hype_innerStereo[i] = val;}
		void set_Hype_outerStereo(G4int i,G4double val) { Hype_outerStereo[i] = val;}
		void set_Hype_Length(G4int i,G4double val) { Hype_Length[i] = val;}

		//TwistedTubs info
		void set_TwistedTubs_twistedangle(G4int i,G4double val) { TwistedTubs_twistedangle[i] = val;}
		void set_TwistedTubs_endinnerrad(G4int i,G4double val) { TwistedTubs_endinnerrad[i] = val;}
		void set_TwistedTubs_endouterrad(G4int i,G4double val) { TwistedTubs_endouterrad[i] = val;}
		void set_TwistedTubs_Length(G4int i,G4double val) { TwistedTubs_Length[i] = val;}
		void set_TwistedTubs_dphi(G4int i,G4double val) { TwistedTubs_dphi[i] = val;}

		//Cons info
		void set_Cons_RMax1(G4int i,G4double val) { Cons_RMax1[i] = val;}
		void set_Cons_RMin1(G4int i,G4double val) { Cons_RMin1[i] = val;}
		void set_Cons_RMax2(G4int i,G4double val) { Cons_RMax2[i] = val;}
		void set_Cons_RMin2(G4int i,G4double val) { Cons_RMin2[i] = val;}
		void set_Cons_Length(G4int i,G4double val) { Cons_Length[i] = val;}
		void set_Cons_StartAng(G4int i,G4double val) { Cons_StartAng[i] = val;}
		void set_Cons_SpanAng(G4int i,G4double val) { Cons_SpanAng[i] = val;}

		//visual settings
		void set_vis(G4String type, G4int i,bool val) { int k = get_index(type, i); vVis[k] = val;}
		void set_r(G4String type, G4int i,G4double val) { int k = get_index(type, i); vR[k] = val;}
		void set_g(G4String type, G4int i,G4double val) { int k = get_index(type, i); vG[k] = val;}
		void set_b(G4String type, G4int i,G4double val) { int k = get_index(type, i); vB[k] = val;}

		void set_vis(G4int i,bool val)   { vVis[i] = val;}
		void set_r(G4int i,G4double val) { vR[i] = val;}
		void set_g(G4int i,G4double val) { vG[i] = val;}
		void set_b(G4int i,G4double val) { vB[i] = val;}

	private:

		//Get VolId according to type name and index
		int get_index( G4String type, G4int i ){
			int k = -1;
			if ( type == "Box" && check_BoxId(i) ) k = Box_GenIndex[i];
			else if ( type == "Tubs" && check_TubsId(i) ) k = Tubs_GenIndex[i];
			else if ( type == "Torus" && check_TorusId(i) ) k = Torus_GenIndex[i];
			else if ( type == "Sphere" && check_SphereId(i) ) k = Sphere_GenIndex[i];
			else if ( type == "Hype" && check_HypeId(i) ) k = Hype_GenIndex[i];
			else if ( type == "TwistedTubs" && check_TwistedTubsId(i) ) k = TwistedTubs_GenIndex[i];
			else if ( type == "Cons" && check_ConsId(i) ) k = Cons_GenIndex[i];
			else DEBUG("Wrong index number or unknown volume type!");
			return k;
		}

		//=> check
		bool check_VolId(int VolId){
			if ( VolId < VolNo ) return true;
			else{
				std::cout<<"In SimpleGeometryParameter, VolId is out of range! VolId = "<<VolId<<", VolNo = "<<VolNo<<std::endl;
				return false;
			}
		}
		bool check_BoxId(int BoxId){
			if ( BoxId < BoxNo ) return true;
			else{
				std::cout<<"In SimpleGeometryParameter, BoxId is out of range! BoxId = "<<BoxId<<", BoxNo = "<<BoxNo<<std::endl;
				return false;
			}
		}
		bool check_BoxNo( int No ){
			if( No != BoxNo ){
				std::cout<<"In SimpleGeometryParameter::InitFromFile(), insufficient Box info! BoxNo = "<<BoxNo<<", while only "<<No<<" Box were found."<<std::endl;
				return false;
			}
			else return true;
		}
		bool check_TubsId(int TubsId){
			if ( TubsId < TubsNo ) return true;
			else{
				std::cout<<"In SimpleGeometryParameter, TubsId is out of range! TubsId = "<<TubsId<<", TubsNo = "<<TubsNo<<std::endl;
				return false;
			}
		}
		bool check_TubsNo( int No ){
			if( No != TubsNo ){
				std::cout<<"In SimpleGeometryParameter::InitFromFile(), insufficient Tubs info! TubsNo = "<<TubsNo<<", while only "<<No<<" Tubs were found."<<std::endl;
				return false;
			}
			else return true;
		}
		bool check_TorusId(int TorusId){
			if ( TorusId < TorusNo ) return true;
			else{
				std::cout<<"In SimpleGeometryParameter, TorusId is out of range! TorusId = "<<TorusId<<", TorusNo = "<<TorusNo<<std::endl;
				return false;
			}
		}
		bool check_TorusNo( int No ){
			if( No != TorusNo ){
				std::cout<<"In SimpleGeometryParameter::InitFromFile(), insufficient Torus info! TorusNo = "<<TorusNo<<", while only "<<No<<" Torus were found."<<std::endl;
				return false;
			}
			else return true;
		}
		bool check_SphereId(int SphereId){
			if ( SphereId < SphereNo ) return true;
			else{
				std::cout<<"In SimpleGeometryParameter, SphereId is out of range! SphereId = "<<SphereId<<", SphereNo = "<<SphereNo<<std::endl;
				return false;
			}
		}
		bool check_SphereNo( int No ){
			if( No != SphereNo ){
				std::cout<<"In SimpleGeometryParameter::InitFromFile(), insufficient Sphere info! SphereNo = "<<SphereNo<<", while only "<<No<<" Sphere were found."<<std::endl;
				return false;
			}
			else return true;
		}
		bool check_HypeId(int HypeId){
			if ( HypeId < HypeNo ) return true;
			else{
				std::cout<<"In SimpleGeometryParameter, HypeId is out of range! HypeId = "<<HypeId<<", HypeNo = "<<HypeNo<<std::endl;
				return false;
			}
		}
		bool check_HypeNo( int No ){
			if( No != HypeNo ){
				std::cout<<"In SimpleGeometryParameter::InitFromFile(), insufficient Hype info! HypeNo = "<<HypeNo<<", while only "<<No<<" Hype were found."<<std::endl;
				return false;
			}
			else return true;
		}
		bool check_TwistedTubsId(int TwistedTubsId){
			if ( TwistedTubsId < TwistedTubsNo ) return true;
			else{
				std::cout<<"In SimpleGeometryParameter, TwistedTubsId is out of range! TwistedTubsId = "<<TwistedTubsId<<", TwistedTubsNo = "<<TwistedTubsNo<<std::endl;
				return false;
			}
		}
		bool check_TwistedTubsNo( int No ){
			if( No != TwistedTubsNo ){
				std::cout<<"In SimpleGeometryParameter::InitFromFile(), insufficient TwistedTubs info! TwistedTubsNo = "<<TwistedTubsNo<<", while only "<<No<<" TwistedTubs were found."<<std::endl;
				return false;
			}
			else return true;
		}
		bool check_ConsId(int ConsId){
			if ( ConsId < ConsNo ) return true;
			else{
				std::cout<<"In SimpleGeometryParameter, ConsId is out of range! ConsId = "<<ConsId<<", ConsNo = "<<ConsNo<<std::endl;
				return false;
			}
		}
		bool check_ConsNo( int No ){
			if( No != ConsNo ){
				std::cout<<"In SimpleGeometryParameter::InitFromFile(), insufficient Cons info! ConsNo = "<<ConsNo<<", while only "<<No<<" Cons were found."<<std::endl;
				return false;
			}
			else return true;
		}

	private:

		//General info about volume
		G4int VolNo;
		std::vector<G4double> PosX;
		std::vector<G4double> PosY;
		std::vector<G4double> PosZ;
		std::vector<G4String> Name;
		std::vector<G4String> MotherName;
		std::vector<G4String> SDName;
		std::vector<G4String> Material;
		std::vector<G4int>    SRepNo;
		std::vector<G4int>    RepNo;
		std::vector<G4double> Space;
		std::vector<G4double> DirTheta;
		std::vector<G4double> DirPhi;
		std::vector<G4String> SolidType;
		std::vector<G4int>    SolidIndex;
		std::vector<G4double> Ephi;
		std::vector<G4double> Etheta;
		std::vector<G4double> Epsi;

		//Box info
		G4int BoxNo;
		std::vector<G4double> Box_X;
		std::vector<G4double> Box_Y;
		std::vector<G4double> Box_Z;
		std::vector<G4int>    Box_GenIndex;

		//Tubs info
		G4int TubsNo;
		std::vector<G4double> Tubs_RMax;
		std::vector<G4double> Tubs_RMin;
		std::vector<G4double> Tubs_Length;
		std::vector<G4double> Tubs_StartAng;
		std::vector<G4double> Tubs_SpanAng;
		std::vector<G4int>    Tubs_GenIndex;

		//Torus info
		G4int TorusNo;
		std::vector<G4double> Torus_RMax;
		std::vector<G4double> Torus_RMin;
		std::vector<G4double> Torus_Rtor;
		std::vector<G4double> Torus_StartAng;
		std::vector<G4double> Torus_SpanAng;
		std::vector<G4int>    Torus_GenIndex;

		//Sphere info
		G4int SphereNo;
		std::vector<G4double> Sphere_RMax;
		std::vector<G4double> Sphere_RMin;
		std::vector<G4double> Sphere_StartPhi;
		std::vector<G4double> Sphere_SpanPhi;
		std::vector<G4double> Sphere_StartTheta;
		std::vector<G4double> Sphere_SpanTheta;
		std::vector<G4int>    Sphere_GenIndex;

		//Hype info
		G4int HypeNo;
		std::vector<G4double> Hype_innerRadius;
		std::vector<G4double> Hype_outerRadius;
		std::vector<G4double> Hype_innerStereo;
		std::vector<G4double> Hype_outerStereo;
		std::vector<G4double> Hype_Length;
		std::vector<G4int>    Hype_GenIndex;

		//TwistedTubs info
		G4int TwistedTubsNo;
		std::vector<G4double> TwistedTubs_twistedangle;
		std::vector<G4double> TwistedTubs_endinnerrad;
		std::vector<G4double> TwistedTubs_endouterrad;
		std::vector<G4double> TwistedTubs_Length;
		std::vector<G4double> TwistedTubs_dphi;
		std::vector<G4int>    TwistedTubs_GenIndex;

		//Cons info
		G4int ConsNo;
		std::vector<G4double> Cons_RMax1;
		std::vector<G4double> Cons_RMin1;
		std::vector<G4double> Cons_RMax2;
		std::vector<G4double> Cons_RMin2;
		std::vector<G4double> Cons_Length;
		std::vector<G4double> Cons_StartAng;
		std::vector<G4double> Cons_SpanAng;
		std::vector<G4int>    Cons_GenIndex;

		//visual settings
		std::vector<bool> vVis;
		std::vector<G4double> vR, vG, vB;

		//to control
		bool notReSetVis;

};

#endif
