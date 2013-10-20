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

		double CalFormula(G4String formula, int iRep = 0);

		void ReplaceMacro(G4String &formula);

		std::vector<G4String> GetWords(G4String);

		bool FindMacro(G4String, G4String&);

		void Replace(G4String &, G4String , G4String);

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
		G4double get_PosX(G4int VolId, G4int j = 0) {if( check_VolId(VolId) ) return PosX[VolId][j]; else return 0;}
		G4double get_PosY(G4int VolId, G4int j = 0) {if( check_VolId(VolId) ) return PosY[VolId][j]; else return 0;}
		G4double get_PosZ(G4int VolId, G4int j = 0) {if( check_VolId(VolId) ) return PosZ[VolId][j]; else return 0;}
		G4String get_name(G4int VolId) {if( check_VolId(VolId) ) return Name[VolId]; else return "";}
		G4String get_SDName(G4int VolId) {if( check_VolId(VolId) ) return SDName[VolId]; else return "";}
		G4String get_MotherName(G4int VolId) {if( check_VolId(VolId) ) return MotherName[VolId]; else return "";}
		G4String get_material(G4int VolId) {if( check_VolId(VolId) ) return Material[VolId]; else return "";}
		G4int get_SRepNo(G4int VolId) {if(check_VolId(VolId)) return SRepNo[VolId]; else return 0;}
		G4int get_RepNo(G4int VolId) {if(check_VolId(VolId)) return RepNo[VolId]; else return 0;}
		G4String get_SolidType(G4int VolId) {if(check_VolId(VolId)) return SolidType[VolId]; else return "";}
		G4int get_SolidIndex(G4int VolId) {if(check_VolId(VolId)) return SolidIndex[VolId]; else return 0;}
		G4double get_Ephi(G4int VolId, G4int j = 0) {if( check_VolId(VolId) ) return Ephi[VolId][j]; else return 0;}
		G4double get_Etheta(G4int VolId, G4int j = 0) {if( check_VolId(VolId) ) return Etheta[VolId][j]; else return 0;}
		G4double get_Epsi(G4int VolId, G4int j = 0) {if( check_VolId(VolId) ) return Epsi[VolId][j]; else return 0;}
		G4bool get_SolidBoolean(G4int VolId) {if( check_VolId(VolId) ) return SolidBoolean[VolId]; else return false;}

		//Box info
		G4int get_BoxNo() {return BoxNo;}
		G4double get_Box_X(G4int BoxId, G4int j = 0) {if( check_BoxId(BoxId) ) return Box_X[BoxId][j]; else return 0;}
		G4double get_Box_Y(G4int BoxId, G4int j = 0) {if( check_BoxId(BoxId) ) return Box_Y[BoxId][j]; else return 0;}
		G4double get_Box_Z(G4int BoxId, G4int j = 0) {if( check_BoxId(BoxId) ) return Box_Z[BoxId][j]; else return 0;}

		//Tubs info
		G4int get_TubsNo() {return TubsNo;}
		G4double get_Tubs_RMax(G4int TubsId, G4int j = 0) {if( check_TubsId(TubsId) ) return Tubs_RMax[TubsId][j]; else return 0;}
		G4double get_Tubs_RMin(G4int TubsId, G4int j = 0) {if( check_TubsId(TubsId) ) return Tubs_RMin[TubsId][j]; else return 0;}
		G4double get_Tubs_Length(G4int TubsId, G4int j = 0) {if( check_TubsId(TubsId) ) return Tubs_Length[TubsId][j]; else return 0;}
		G4double get_Tubs_StartAng(G4int TubsId, G4int j = 0) {if( check_TubsId(TubsId) ) return Tubs_StartAng[TubsId][j]; else return 0;}
		G4double get_Tubs_SpanAng(G4int TubsId, G4int j = 0) {if( check_TubsId(TubsId) ) return Tubs_SpanAng[TubsId][j]; else return 0;}

		//Torus info
		G4int get_TorusNo() {return TorusNo;}
		G4double get_Torus_RMax(G4int TorusId, G4int j = 0) {if( check_TorusId(TorusId) ) return Torus_RMax[TorusId]; else return 0;}
		G4double get_Torus_RMin(G4int TorusId, G4int j = 0) {if( check_TorusId(TorusId) ) return Torus_RMin[TorusId]; else return 0;}
		G4double get_Torus_Rtor(G4int TorusId, G4int j = 0) {if( check_TorusId(TorusId) ) return Torus_Rtor[TorusId]; else return 0;}
		G4double get_Torus_StartAng(G4int TorusId, G4int j = 0) {if( check_TorusId(TorusId) ) return Torus_StartAng[TorusId]; else return 0;}
		G4double get_Torus_SpanAng(G4int TorusId, G4int j = 0) {if( check_TorusId(TorusId) ) return Torus_SpanAng[TorusId]; else return 0;}

		//Sphere info
		G4int get_SphereNo() {return SphereNo;}
		G4double get_Sphere_RMax(G4int SphereId, G4int j = 0) {if( check_SphereId(SphereId) ) return Sphere_RMax[SphereId]; else return 0;}
		G4double get_Sphere_RMin(G4int SphereId, G4int j = 0) {if( check_SphereId(SphereId) ) return Sphere_RMin[SphereId]; else return 0;}
		G4double get_Sphere_StartPhi(G4int SphereId, G4int j = 0) {if( check_SphereId(SphereId) ) return Sphere_StartPhi[SphereId]; else return 0;}
		G4double get_Sphere_SpanPhi(G4int SphereId, G4int j = 0) {if( check_SphereId(SphereId) ) return Sphere_SpanPhi[SphereId]; else return 0;}
		G4double get_Sphere_StartTheta(G4int SphereId, G4int j = 0) {if( check_SphereId(SphereId) ) return Sphere_StartTheta[SphereId]; else return 0;}
		G4double get_Sphere_SpanTheta(G4int SphereId, G4int j = 0) {if( check_SphereId(SphereId) ) return Sphere_SpanTheta[SphereId]; else return 0;}

		//Hype info
		G4int get_HypeNo() {return HypeNo;}
		G4double get_Hype_innerRadius(G4int HypeId, G4int j = 0) {if( check_HypeId(HypeId) ) return Hype_innerRadius[HypeId]; else return 0;}
		G4double get_Hype_outerRadius(G4int HypeId, G4int j = 0) {if( check_HypeId(HypeId) ) return Hype_outerRadius[HypeId]; else return 0;}
		G4double get_Hype_innerStereo(G4int HypeId, G4int j = 0) {if( check_HypeId(HypeId) ) return Hype_innerStereo[HypeId]; else return 0;}
		G4double get_Hype_outerStereo(G4int HypeId, G4int j = 0) {if( check_HypeId(HypeId) ) return Hype_outerStereo[HypeId]; else return 0;}
		G4double get_Hype_Length(G4int HypeId, G4int j = 0) {if( check_HypeId(HypeId) ) return Hype_Length[HypeId]; else return 0;}

		//TwistedTubs info
		G4int get_TwistedTubsNo() {return TwistedTubsNo;}
		G4double get_TwistedTubs_twistedangle(G4int TwistedTubsId, G4int j = 0) {if( check_TwistedTubsId(TwistedTubsId) ) return TwistedTubs_twistedangle[TwistedTubsId]; else return 0;}
		G4double get_TwistedTubs_endinnerrad(G4int TwistedTubsId, G4int j = 0) {if( check_TwistedTubsId(TwistedTubsId) ) return TwistedTubs_endinnerrad[TwistedTubsId]; else return 0;}
		G4double get_TwistedTubs_endouterrad(G4int TwistedTubsId, G4int j = 0) {if( check_TwistedTubsId(TwistedTubsId) ) return TwistedTubs_endouterrad[TwistedTubsId]; else return 0;}
		G4double get_TwistedTubs_Length(G4int TwistedTubsId, G4int j = 0) {if( check_TwistedTubsId(TwistedTubsId) ) return TwistedTubs_Length[TwistedTubsId]; else return 0;}
		G4double get_TwistedTubs_dphi(G4int TwistedTubsId, G4int j = 0) {if( check_TwistedTubsId(TwistedTubsId) ) return TwistedTubs_dphi[TwistedTubsId]; else return 0;}

		//Cons info
		G4int get_ConsNo() {return ConsNo;}
		G4double get_Cons_RMax1(G4int ConsId, G4int j = 0) {if( check_ConsId(ConsId) ) return Cons_RMax1[ConsId]; else return 0;}
		G4double get_Cons_RMin1(G4int ConsId, G4int j = 0) {if( check_ConsId(ConsId) ) return Cons_RMin1[ConsId]; else return 0;}
		G4double get_Cons_RMax2(G4int ConsId, G4int j = 0) {if( check_ConsId(ConsId) ) return Cons_RMax2[ConsId]; else return 0;}
		G4double get_Cons_RMin2(G4int ConsId, G4int j = 0) {if( check_ConsId(ConsId) ) return Cons_RMin2[ConsId]; else return 0;}
		G4double get_Cons_Length(G4int ConsId, G4int j = 0) {if( check_ConsId(ConsId) ) return Cons_Length[ConsId]; else return 0;}
		G4double get_Cons_StartAng(G4int ConsId, G4int j = 0) {if( check_ConsId(ConsId) ) return Cons_StartAng[ConsId]; else return 0;}
		G4double get_Cons_SpanAng(G4int ConsId, G4int j = 0) {if( check_ConsId(ConsId) ) return Cons_SpanAng[ConsId]; else return 0;}

		//Polycone info
		G4int get_PolyconeNo() {return PolyconeNo;}
		G4int    get_Polycone_numZ(G4int PolyconeId, G4int j = 0) {if( check_PolyconeId(PolyconeId) ) return Polycone_numZ[PolyconeId]; else return 0;}
		G4double get_Polycone_RMax(G4int PolyconeId,G4int i, G4int j = 0) {if( check_PolyconeId(PolyconeId) && i < Polycone_numZ[PolyconeId] ) return Polycone_RMax[PolyconeId][i]; else return 0;}
		G4double get_Polycone_RMin(G4int PolyconeId,G4int i, G4int j = 0) {if( check_PolyconeId(PolyconeId) && i < Polycone_numZ[PolyconeId] ) return Polycone_RMin[PolyconeId][i]; else return 0;}
		G4double get_Polycone_Z(G4int PolyconeId,G4int i, G4int j = 0) {if( check_PolyconeId(PolyconeId) && i < Polycone_numZ[PolyconeId]) return Polycone_Z[PolyconeId][i]; else return 0;}
		G4double get_Polycone_StartAng(G4int PolyconeId, G4int j = 0) {if( check_PolyconeId(PolyconeId) ) return Polycone_StartAng[PolyconeId]; else return 0;}
		G4double get_Polycone_SpanAng(G4int PolyconeId, G4int j = 0) {if( check_PolyconeId(PolyconeId) ) return Polycone_SpanAng[PolyconeId]; else return 0;}

		// BooleanSolid
		G4String get_BooleanSolid_type(G4int BooleanSolidId) {if( check_BooleanSolidId(BooleanSolidId) ) return BooleanSolid_type[BooleanSolidId]; else return "";}
		G4String get_BooleanSolid_sol1(G4int BooleanSolidId) {if( check_BooleanSolidId(BooleanSolidId) ) return BooleanSolid_sol1[BooleanSolidId]; else return "";}
		G4String get_BooleanSolid_sol2(G4int BooleanSolidId) {if( check_BooleanSolidId(BooleanSolidId) ) return BooleanSolid_sol2[BooleanSolidId]; else return "";}
		G4double get_BooleanSolid_Ephi(G4int BooleanSolidId, G4int j = 0) {if( check_BooleanSolidId(BooleanSolidId) ) return BooleanSolid_Ephi[BooleanSolidId][j]; else return 0;}
		G4double get_BooleanSolid_Etheta(G4int BooleanSolidId, G4int j = 0) {if( check_BooleanSolidId(BooleanSolidId) ) return BooleanSolid_Etheta[BooleanSolidId][j]; else return 0;}
		G4double get_BooleanSolid_Epsi(G4int BooleanSolidId, G4int j = 0) {if( check_BooleanSolidId(BooleanSolidId) ) return BooleanSolid_Epsi[BooleanSolidId][j]; else return 0;}
		G4double get_BooleanSolid_PosX(G4int BooleanSolidId, G4int j = 0) {if( check_BooleanSolidId(BooleanSolidId) ) return BooleanSolid_PosX[BooleanSolidId][j]; else return 0;}
		G4double get_BooleanSolid_PosY(G4int BooleanSolidId, G4int j = 0) {if( check_BooleanSolidId(BooleanSolidId) ) return BooleanSolid_PosY[BooleanSolidId][j]; else return 0;}
		G4double get_BooleanSolid_PosZ(G4int BooleanSolidId, G4int j = 0) {if( check_BooleanSolidId(BooleanSolidId) ) return BooleanSolid_PosZ[BooleanSolidId][j]; else return 0;}

		//visual settings
		bool get_vis(G4int VolId){if( check_VolId(VolId) ) return vVis[VolId]; else return 0;}
		G4double get_r(G4int VolId){if( check_VolId(VolId) ) return vR[VolId]; else return 0;}
		G4double get_g(G4int VolId){if( check_VolId(VolId) ) return vG[VolId]; else return 0;}
		G4double get_b(G4int VolId){if( check_VolId(VolId) ) return vB[VolId]; else return 0;}
		G4double get_t(G4int VolId){if( check_VolId(VolId) ) return vT[VolId]; else return 0;}

		//=>Modify
		//General info for volume
		void set_PosX(G4String type, G4int i,G4double val, G4int j = 0) { int k = get_index(type, i); PosX[k][j] = val;}
		void set_PosY(G4String type, G4int i,G4double val, G4int j = 0) { int k = get_index(type, i); PosY[k][j] = val;}
		void set_PosZ(G4String type, G4int i,G4double val, G4int j = 0) { int k = get_index(type, i); PosZ[k][j] = val;}
		void set_name(G4String type, G4int i,G4String val) { int k = get_index(type, i); Name[k] = val;}
		void set_SDName(G4String type, G4int i,G4String val) { int k = get_index(type, i); SDName[k] = val;}
		void set_MotherName(G4String type, G4int i,G4String val) { int k = get_index(type, i); MotherName[k] = val;}
		void set_material(G4String type, G4int i,G4String val) { int k = get_index(type, i); Material[k] = val;}
		void set_SRepNo(G4String type, G4int i,G4int val) { int k = get_index(type, i); SRepNo[k] = val;}
		void set_RepNo(G4String type, G4int i,G4int val) { int k = get_index(type, i); RepNo[k] = val;}
		void set_Ephi(G4String type, G4int i, G4double val, G4int j = 0) { int k = get_index(type, i); Ephi[k][j] = val;}
		void set_Etheta(G4String type, G4int i, G4double val, G4int j = 0) { int k = get_index(type, i); Etheta[k][j] = val;}
		void set_Epsi(G4String type, G4int i, G4double val, G4int j = 0) { int k = get_index(type, i); Epsi[k][j] = val;}
		void set_SolidBoolean(G4String type, G4int i, G4int val) { int k = get_index(type, i); SolidBoolean[k] = val;}

		void set_name(G4String name,G4String val) { int k = get_VolIndex(name); Name[k] = val;}
		void set_SDName(G4String name,G4String val) { int k = get_VolIndex(name); SDName[k] = val;}
		void set_MotherName(G4String name,G4String val) { int k = get_VolIndex(name); MotherName[k] = val;}
		void set_material(G4String name,G4String val) { int k = get_VolIndex(name); Material[k] = val;}
		void set_SRepNo(G4String name,G4int val) { int k = get_VolIndex(name); SRepNo[k] = val;}
		void set_RepNo(G4String name,G4int val) { int k = get_VolIndex(name); RepNo[k] = val;}
		void set_SolidBoolean(G4String name, G4int val) { int k = get_VolIndex(name); SolidBoolean[k] = val;}
		void set_PosX(G4String name,G4double val, G4int j = 0) { int k = get_VolIndex(name); PosX[k][j] = val;}
		void set_PosY(G4String name,G4double val, G4int j = 0) { int k = get_VolIndex(name); PosY[k][j] = val;}
		void set_PosZ(G4String name,G4double val, G4int j = 0) { int k = get_VolIndex(name); PosZ[k][j] = val;}
		void set_Ephi(G4String name, G4double val, G4int j = 0) { int k = get_VolIndex(name); Ephi[k][j] = val;}
		void set_Etheta(G4String name, G4double val, G4int j = 0) { int k = get_VolIndex(name); Etheta[k][j] = val;}
		void set_Epsi(G4String name, G4double val, G4int j = 0) { int k = get_VolIndex(name); Epsi[k][j] = val;}

		//Box info
		void set_Box_X(G4int i,G4double val, G4int j = 0) { Box_X[i][j] = val;}
		void set_Box_Y(G4int i,G4double val, G4int j = 0) { Box_Y[i][j] = val;}
		void set_Box_Z(G4int i,G4double val, G4int j = 0) { Box_Z[i][j] = val;}

		void set_Box_X(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Box_X[i][j] = val;}
		void set_Box_Y(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Box_Y[i][j] = val;}
		void set_Box_Z(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Box_Z[i][j] = val;}

		//Tubs info
		void set_Tubs_RMax(G4int i,G4double val, G4int j = 0) { Tubs_RMax[i][j] = val;}
		void set_Tubs_RMin(G4int i,G4double val, G4int j = 0) { Tubs_RMin[i][j] = val;}
		void set_Tubs_Length(G4int i,G4double val, G4int j = 0) { Tubs_Length[i][j] = val;}
		void set_Tubs_StartAng(G4int i,G4double val, G4int j = 0) { Tubs_StartAng[i][j] = val;}
		void set_Tubs_SpanAng(G4int i,G4double val, G4int j = 0) { Tubs_SpanAng[i][j] = val;}

		void set_Tubs_RMax(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Tubs_RMax[i][j] = val;}
		void set_Tubs_RMin(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Tubs_RMin[i][j] = val;}
		void set_Tubs_Length(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Tubs_Length[i][j] = val;}
		void set_Tubs_StartAng(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Tubs_StartAng[i][j] = val;}
		void set_Tubs_SpanAng(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Tubs_SpanAng[i][j] = val;}

		//Torus info
		void set_Torus_RMax(G4int i,G4double val, G4int j = 0) { Torus_RMax[i] = val;}
		void set_Torus_RMin(G4int i,G4double val, G4int j = 0) { Torus_RMin[i] = val;}
		void set_Torus_Rtor(G4int i,G4double val, G4int j = 0) { Torus_Rtor[i] = val;}
		void set_Torus_StartAng(G4int i,G4double val, G4int j = 0) { Torus_StartAng[i] = val;}
		void set_Torus_SpanAng(G4int i,G4double val, G4int j = 0) { Torus_SpanAng[i] = val;}

		void set_Torus_RMax(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Torus_RMax[i] = val;}
		void set_Torus_RMin(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Torus_RMin[i] = val;}
		void set_Torus_Rtor(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Torus_Rtor[i] = val;}
		void set_Torus_StartAng(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Torus_StartAng[i] = val;}
		void set_Torus_SpanAng(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Torus_SpanAng[i] = val;}

		//Sphere info
		void set_Sphere_RMax(G4int i,G4double val, G4int j = 0) { Sphere_RMax[i] = val;}
		void set_Sphere_RMin(G4int i,G4double val, G4int j = 0) { Sphere_RMin[i] = val;}
		void set_Sphere_StartPhi(G4int i,G4double val, G4int j = 0) { Sphere_StartPhi[i] = val;}
		void set_Sphere_SpanPhi(G4int i,G4double val, G4int j = 0) { Sphere_SpanPhi[i] = val;}
		void set_Sphere_StartTheta(G4int i,G4double val, G4int j = 0) { Sphere_StartTheta[i] = val;}
		void set_Sphere_SpanTheta(G4int i,G4double val, G4int j = 0) { Sphere_SpanTheta[i] = val;}

		void set_Sphere_RMax(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Sphere_RMax[i] = val;}
		void set_Sphere_RMin(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Sphere_RMin[i] = val;}
		void set_Sphere_StartPhi(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Sphere_StartPhi[i] = val;}
		void set_Sphere_SpanPhi(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Sphere_SpanPhi[i] = val;}
		void set_Sphere_StartTheta(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Sphere_StartTheta[i] = val;}
		void set_Sphere_SpanTheta(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Sphere_SpanTheta[i] = val;}

		//Hype info
		void set_Hype_innerRadius(G4int i,G4double val, G4int j = 0) { Hype_innerRadius[i] = val;}
		void set_Hype_outerRadius(G4int i,G4double val, G4int j = 0) { Hype_outerRadius[i] = val;}
		void set_Hype_innerStereo(G4int i,G4double val, G4int j = 0) { Hype_innerStereo[i] = val;}
		void set_Hype_outerStereo(G4int i,G4double val, G4int j = 0) { Hype_outerStereo[i] = val;}
		void set_Hype_Length(G4int i,G4double val, G4int j = 0) { Hype_Length[i] = val;}

		void set_Hype_innerRadius(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Hype_innerRadius[i] = val;}
		void set_Hype_outerRadius(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Hype_outerRadius[i] = val;}
		void set_Hype_innerStereo(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Hype_innerStereo[i] = val;}
		void set_Hype_outerStereo(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Hype_outerStereo[i] = val;}
		void set_Hype_Length(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Hype_Length[i] = val;}

		//TwistedTubs info
		void set_TwistedTubs_twistedangle(G4int i,G4double val, G4int j = 0) { TwistedTubs_twistedangle[i] = val;}
		void set_TwistedTubs_endinnerrad(G4int i,G4double val, G4int j = 0) { TwistedTubs_endinnerrad[i] = val;}
		void set_TwistedTubs_endouterrad(G4int i,G4double val, G4int j = 0) { TwistedTubs_endouterrad[i] = val;}
		void set_TwistedTubs_Length(G4int i,G4double val, G4int j = 0) { TwistedTubs_Length[i] = val;}
		void set_TwistedTubs_dphi(G4int i,G4double val, G4int j = 0) { TwistedTubs_dphi[i] = val;}

		void set_TwistedTubs_twistedangle(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); TwistedTubs_twistedangle[i] = val;}
		void set_TwistedTubs_endinnerrad(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); TwistedTubs_endinnerrad[i] = val;}
		void set_TwistedTubs_endouterrad(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); TwistedTubs_endouterrad[i] = val;}
		void set_TwistedTubs_Length(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); TwistedTubs_Length[i] = val;}
		void set_TwistedTubs_dphi(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); TwistedTubs_dphi[i] = val;}

		//Cons info
		void set_Cons_RMax1(G4int i,G4double val, G4int j = 0) { Cons_RMax1[i] = val;}
		void set_Cons_RMin1(G4int i,G4double val, G4int j = 0) { Cons_RMin1[i] = val;}
		void set_Cons_RMax2(G4int i,G4double val, G4int j = 0) { Cons_RMax2[i] = val;}
		void set_Cons_RMin2(G4int i,G4double val, G4int j = 0) { Cons_RMin2[i] = val;}
		void set_Cons_Length(G4int i,G4double val, G4int j = 0) { Cons_Length[i] = val;}
		void set_Cons_StartAng(G4int i,G4double val, G4int j = 0) { Cons_StartAng[i] = val;}
		void set_Cons_SpanAng(G4int i,G4double val, G4int j = 0) { Cons_SpanAng[i] = val;}

		void set_Cons_RMax1(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Cons_RMax1[i] = val;}
		void set_Cons_RMin1(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Cons_RMin1[i] = val;}
		void set_Cons_RMax2(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Cons_RMax2[i] = val;}
		void set_Cons_RMin2(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Cons_RMin2[i] = val;}
		void set_Cons_Length(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Cons_Length[i] = val;}
		void set_Cons_StartAng(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Cons_StartAng[i] = val;}
		void set_Cons_SpanAng(G4String name,G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name)); Cons_SpanAng[i] = val;}

		//Polycone info
		void set_Polycone_numZ(G4int PolyconeId, G4int val, G4int j = 0) {if( check_PolyconeId(PolyconeId) ) Polycone_numZ[PolyconeId] = val;}
		void set_Polycone_RMax(G4int PolyconeId,G4int i, G4double val, G4int j = 0) {if( check_PolyconeId(PolyconeId) && i < Polycone_numZ[PolyconeId] ) Polycone_RMax[PolyconeId][i]=val;}
		void set_Polycone_RMin(G4int PolyconeId,G4int i, G4double val, G4int j = 0) {if( check_PolyconeId(PolyconeId) && i < Polycone_numZ[PolyconeId] ) Polycone_RMin[PolyconeId][i]=val;}
		void set_Polycone_Z(G4int PolyconeId,G4int i, G4double val, G4int j = 0) {if( check_PolyconeId(PolyconeId) && i < Polycone_numZ[PolyconeId]) Polycone_Z[PolyconeId][i]=val;}
		void set_Polycone_StartAng(G4int PolyconeId, G4double val, G4int j = 0) {if( check_PolyconeId(PolyconeId) ) Polycone_StartAng[PolyconeId]=val;}
		void set_Polycone_SpanAng(G4int PolyconeId, G4double val, G4int j = 0) {if( check_PolyconeId(PolyconeId) ) Polycone_SpanAng[PolyconeId]=val;}

		void set_Polycone_numZ(G4String name, G4int val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name));if( check_PolyconeId(i) ) Polycone_numZ[i] = val;}
		void set_Polycone_RMax(G4String name,G4int iCone, G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name));if( check_PolyconeId(i) && iCone < Polycone_numZ[i] ) Polycone_RMax[i][iCone]=val;}
		void set_Polycone_RMin(G4String name,G4int iCone, G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name));if( check_PolyconeId(i) && iCone < Polycone_numZ[i] ) Polycone_RMin[i][iCone]=val;}
		void set_Polycone_Z(G4String name,G4int iCone, G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name));if( check_PolyconeId(i) && iCone < Polycone_numZ[i]) Polycone_Z[i][iCone]=val;}
		void set_Polycone_StartAng(G4String name, G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name));if( check_PolyconeId(i) ) Polycone_StartAng[i]=val;}
		void set_Polycone_SpanAng(G4String name, G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name));if( check_PolyconeId(i) ) Polycone_SpanAng[i]=val;}

		//BooleanSolid info
		void set_BooleanSolid_type(G4int BooleanSolidId, G4String val) {if( check_BooleanSolidId(BooleanSolidId) ) BooleanSolid_type[BooleanSolidId]=val;}
		void set_BooleanSolid_sol1(G4int BooleanSolidId, G4String val) {if( check_BooleanSolidId(BooleanSolidId) ) BooleanSolid_sol1[BooleanSolidId]=val;}
		void set_BooleanSolid_sol2(G4int BooleanSolidId, G4String val) {if( check_BooleanSolidId(BooleanSolidId) ) BooleanSolid_sol2[BooleanSolidId]=val;}
		void set_BooleanSolid_Ephi(G4int BooleanSolidId, G4double val, G4int j = 0) {if( check_BooleanSolidId(BooleanSolidId) ) BooleanSolid_Ephi[BooleanSolidId][j]=val;}
		void set_BooleanSolid_Etheta(G4int BooleanSolidId, G4double val, G4int j = 0) {if( check_BooleanSolidId(BooleanSolidId) ) BooleanSolid_Etheta[BooleanSolidId][j]=val;}
		void set_BooleanSolid_Epsi(G4int BooleanSolidId, G4double val, G4int j = 0) {if( check_BooleanSolidId(BooleanSolidId) ) BooleanSolid_Epsi[BooleanSolidId][j]=val;}
		void set_BooleanSolid_PosX(G4int BooleanSolidId, G4double val, G4int j = 0) {if( check_BooleanSolidId(BooleanSolidId) ) BooleanSolid_PosX[BooleanSolidId][j]=val;}
		void set_BooleanSolid_PosY(G4int BooleanSolidId, G4double val, G4int j = 0) {if( check_BooleanSolidId(BooleanSolidId) ) BooleanSolid_PosY[BooleanSolidId][j]=val;}
		void set_BooleanSolid_PosZ(G4int BooleanSolidId, G4double val, G4int j = 0) {if( check_BooleanSolidId(BooleanSolidId) ) BooleanSolid_PosZ[BooleanSolidId][j]=val;}

		void set_BooleanSolid_type(G4String name, G4String val) {int i = get_SolidIndex(get_VolIndex(name));if( check_BooleanSolidId(i) ) BooleanSolid_type[i]=val;}
		void set_BooleanSolid_sol1(G4String name, G4String val) {int i = get_SolidIndex(get_VolIndex(name));if( check_BooleanSolidId(i) ) BooleanSolid_sol1[i]=val;}
		void set_BooleanSolid_sol2(G4String name, G4String val) {int i = get_SolidIndex(get_VolIndex(name));if( check_BooleanSolidId(i) ) BooleanSolid_sol2[i]=val;}
		void set_BooleanSolid_Ephi(G4String name, G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name));if( check_BooleanSolidId(i) ) BooleanSolid_Ephi[i][j]=val;}
		void set_BooleanSolid_Etheta(G4String name, G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name));if( check_BooleanSolidId(i) ) BooleanSolid_Etheta[i][j]=val;}
		void set_BooleanSolid_Epsi(G4String name, G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name));if( check_BooleanSolidId(i) ) BooleanSolid_Epsi[i][j]=val;}
		void set_BooleanSolid_PosX(G4String name, G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name));if( check_BooleanSolidId(i) ) BooleanSolid_PosX[i][j]=val;}
		void set_BooleanSolid_PosY(G4String name, G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name));if( check_BooleanSolidId(i) ) BooleanSolid_PosY[i][j]=val;}
		void set_BooleanSolid_PosZ(G4String name, G4double val, G4int j = 0) {int i = get_SolidIndex(get_VolIndex(name));if( check_BooleanSolidId(i) ) BooleanSolid_PosZ[i][j]=val;}

		//visual settings
		void set_vis(G4String type, G4int i,bool val) { int k = get_index(type, i); vVis[k] = val;}
		void set_r(G4String type, G4int i,G4double val) { int k = get_index(type, i); vR[k] = val;}
		void set_g(G4String type, G4int i,G4double val) { int k = get_index(type, i); vG[k] = val;}
		void set_b(G4String type, G4int i,G4double val) { int k = get_index(type, i); vB[k] = val;}
		void set_t(G4String type, G4int i,G4double val) { int k = get_index(type, i); vT[k] = val;}

		void set_vis(G4int i,bool val)   { vVis[i] = val;}
		void set_r(G4int i,G4double val) { vR[i] = val;}
		void set_g(G4int i,G4double val) { vG[i] = val;}
		void set_b(G4int i,G4double val) { vB[i] = val;}
		void set_t(G4int i,G4double val) { vT[i] = val;}

		void set_vis(G4String name,bool val) {int k = get_VolIndex(name); vVis[k] = val;}
		void set_r(G4String name,G4double val) {int k = get_VolIndex(name); vR[k] = val;}
		void set_g(G4String name,G4double val) {int k = get_VolIndex(name); vG[k] = val;}
		void set_b(G4String name,G4double val) {int k = get_VolIndex(name); vB[k] = val;}
		void set_t(G4String name,G4double val) {int k = get_VolIndex(name); vT[k] = val;}

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
			else if ( type == "Polycone" && check_PolyconeId(i) ) k = Polycone_GenIndex[i];
			else if ( type == "BooleanSolid" && check_BooleanSolidId(i) ) k = BooleanSolid_GenIndex[i];
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
		bool check_PolyconeId(int PolyconeId){
			if ( PolyconeId < PolyconeNo ) return true;
			else{
				std::cout<<"In SimpleGeometryParameter, PolyconeId is out of range! PolyconeId = "<<PolyconeId<<", PolyconeNo = "<<PolyconeNo<<std::endl;
				return false;
			}
		}
		bool check_PolyconeNo( int No ){
			if( No != PolyconeNo ){
				std::cout<<"In SimpleGeometryParameter::InitFromFile(), insufficient Polycone info! PolyconeNo = "<<PolyconeNo<<", while only "<<No<<" Polycone were found."<<std::endl;
				return false;
			}
			else return true;
		}
		bool check_BooleanSolidId(int BooleanSolidId){
			if ( BooleanSolidId < BooleanSolidNo ) return true;
			else{
				std::cout<<"In SimpleGeometryParameter, BooleanSolidId is out of range! BooleanSolidId = "<<BooleanSolidId<<", BooleanSolidNo = "<<BooleanSolidNo<<std::endl;
				return false;
			}
		}
		bool check_BooleanSolidNo( int No ){
			if( No != BooleanSolidNo ){
				std::cout<<"In SimpleGeometryParameter::InitFromFile(), insufficient BooleanSolid info! BooleanSolidNo = "<<BooleanSolidNo<<", while only "<<No<<" BooleanSolid were found."<<std::endl;
				return false;
			}
			else return true;
		}

		void dump_general_header();
		void dump_general_note();
		void dump_general_value(G4int index, G4int j = 0);

	private:

		//General info about volume
		G4int VolNo;
		std::vector<std::vector<G4double> > PosX;
		std::vector<std::vector<G4double> > PosY;
		std::vector<std::vector<G4double> > PosZ;
		std::vector<G4String> Name;
		std::vector<G4String> MotherName;
		std::vector<G4String> SDName;
		std::vector<G4String> Material;
		std::vector<G4int>    SRepNo;
		std::vector<G4int>    RepNo;
		std::vector<G4String> SolidType;
		std::vector<G4int>    SolidIndex;
		std::vector<std::vector<G4double> > Ephi;
		std::vector<std::vector<G4double> > Etheta;
		std::vector<std::vector<G4double> > Epsi;
		std::vector<G4bool> SolidBoolean;

		//Box info
		G4int BoxNo;
		std::vector<std::vector<G4double> > Box_X;
		std::vector<std::vector<G4double> > Box_Y;
		std::vector<std::vector<G4double> > Box_Z;
		std::vector<G4int>    Box_GenIndex;

		//Tubs info
		G4int TubsNo;
		std::vector<std::vector<G4double> > Tubs_RMax;
		std::vector<std::vector<G4double> > Tubs_RMin;
		std::vector<std::vector<G4double> > Tubs_Length;
		std::vector<std::vector<G4double> > Tubs_StartAng;
		std::vector<std::vector<G4double> > Tubs_SpanAng;
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

		//Polycone info
		G4int PolyconeNo;
		std::vector<G4int> Polycone_numZ;
		std::vector<std::vector<G4double> > Polycone_RMax;
		std::vector<std::vector<G4double> > Polycone_RMin;
		std::vector<std::vector<G4double> > Polycone_Z;
		std::vector<G4double> Polycone_StartAng;
		std::vector<G4double> Polycone_SpanAng;
		std::vector<G4int>    Polycone_GenIndex;
		G4int waited_Polycone_iVol;
		G4int achieved_componets_Polycone;

		// BooleanSolidId
		G4int BooleanSolidNo;
		std::vector<G4String> BooleanSolid_type;
		std::vector<G4String> BooleanSolid_sol1;
		std::vector<G4String> BooleanSolid_sol2;
		std::vector<G4int>    BooleanSolid_GenIndex;
		std::vector<std::vector<G4double> > BooleanSolid_Ephi;
		std::vector<std::vector<G4double> > BooleanSolid_Etheta;
		std::vector<std::vector<G4double> > BooleanSolid_Epsi;
		std::vector<std::vector<G4double> > BooleanSolid_PosX;
		std::vector<std::vector<G4double> > BooleanSolid_PosY;
		std::vector<std::vector<G4double> > BooleanSolid_PosZ;

		//visual settings
		std::vector<bool> vVis;
		std::vector<G4double> vR, vG, vB, vT;

		//to control
		bool notReSetVis;

		// macros
		std::vector<G4String> knownValueNames;
		std::vector<G4String> knownValues;

};

#endif
