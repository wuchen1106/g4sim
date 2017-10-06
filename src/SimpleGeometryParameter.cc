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

#include "myglobals.hh"

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
	Calculate();
}

void SimpleGeometryParameter::Calculate(){
//	std::cout<<"In SimpleGeometryParameter::Calculate"<<std::endl; // to be deleted
	MyVGeometryParameter::Calculate();
	for ( int VolId = 0; VolId < VolNo; VolId++ ){
		std::vector<G4ThreeVector> tPos; tPos.clear();
		std::vector<G4double> tEphi; tEphi.clear();
		std::vector<G4double> tEtheta; tEtheta.clear();
		std::vector<G4double> tEpsi; tEpsi.clear();
		if (!SolidBoolean[VolId]){
			for ( int RepId = 0; RepId < RepNo[VolId]; RepId++ ){
				tEphi.push_back(CalFormula(fEphi[VolId],RepId)*deg);
				tEtheta.push_back(CalFormula(fEtheta[VolId],RepId)*deg);
				tEpsi.push_back(CalFormula(fEpsi[VolId],RepId)*deg);
				G4ThreeVector vec3temp(1,1,1);
				if (PosType[VolId]=="Cartesian"){
					vec3temp.setX(CalFormula(fPosX[VolId],RepId)*mm);
					vec3temp.setY(CalFormula(fPosY[VolId],RepId)*mm);
					vec3temp.setZ(CalFormula(fPosZ[VolId],RepId)*mm);
				}
				else if (PosType[VolId]=="Spherical"){
					vec3temp.setMag(CalFormula(fPosR[VolId],RepId)*mm);
					vec3temp.setTheta(CalFormula(fPosTheta[VolId],RepId)*deg);
					vec3temp.setPhi(CalFormula(fPosPhi[VolId],RepId)*deg);
				}
				else if (PosType[VolId]=="Polar"){
					vec3temp.setRho(CalFormula(fPosR[VolId],RepId)*mm);
					vec3temp.setZ(CalFormula(fPosZ[VolId],RepId)*mm);
					vec3temp.setPhi(CalFormula(fPosPhi[VolId],RepId)*deg);
				}
				tPos.push_back(vec3temp);
			}
		}
		Pos.push_back(tPos);
		Ephi.push_back(tEphi);
		Etheta.push_back(tEtheta);
		Epsi.push_back(tEpsi);
		int SolId = SolidIndex[VolId];
		if ( SolidType[VolId] == "BooleanSolid" ){
			std::vector<G4double> tBooleanSolid_Ephi;
			std::vector<G4double> tBooleanSolid_Etheta;
			std::vector<G4double> tBooleanSolid_Epsi;
			std::vector<G4ThreeVector> tBooleanSolid_Pos;
			for ( int RepId = 0; RepId < RepNo[VolId]; RepId++ ){
				tBooleanSolid_Ephi.push_back(CalFormula(fBooleanSolid_Ephi[SolId],RepId)*deg);
				tBooleanSolid_Etheta.push_back(CalFormula(fBooleanSolid_Etheta[SolId],RepId)*deg);
				tBooleanSolid_Epsi.push_back(CalFormula(fBooleanSolid_Epsi[SolId],RepId)*deg);
				G4ThreeVector vec3temp(1,1,1);
				if (BooleanSolid_PosType[SolId]=="Cartesian"){
					vec3temp.setX(CalFormula(fBooleanSolid_PosX[SolId],RepId)*mm);
					vec3temp.setY(CalFormula(fBooleanSolid_PosY[SolId],RepId)*mm);
					vec3temp.setZ(CalFormula(fBooleanSolid_PosZ[SolId],RepId)*mm);
				}
				else if (BooleanSolid_PosType[SolId]=="Spherical"){
					vec3temp.setMag(CalFormula(fBooleanSolid_PosR[SolId],RepId)*mm);
					vec3temp.setTheta(CalFormula(fBooleanSolid_PosTheta[SolId],RepId)*deg);
					vec3temp.setPhi(CalFormula(fBooleanSolid_PosPhi[SolId],RepId)*deg);
				}
				else if (BooleanSolid_PosType[SolId]=="Polar"){
					vec3temp.setRho(CalFormula(fBooleanSolid_PosR[SolId],RepId)*mm);
					vec3temp.setZ(CalFormula(fBooleanSolid_PosZ[SolId],RepId)*mm);
					vec3temp.setPhi(CalFormula(fBooleanSolid_PosPhi[SolId],RepId)*deg);
				}
				tBooleanSolid_Pos.push_back(vec3temp);
			}
			BooleanSolid_Ephi.push_back(tBooleanSolid_Ephi);
			BooleanSolid_Etheta.push_back(tBooleanSolid_Etheta);
			BooleanSolid_Epsi.push_back(tBooleanSolid_Epsi);
			BooleanSolid_Pos.push_back(tBooleanSolid_Pos);
		}
		else if ( SolidType[VolId] == "Box" ){
			std::vector<G4double> tBox_X; tBox_X.clear();
			std::vector<G4double> tBox_Y; tBox_Y.clear();
			std::vector<G4double> tBox_Z; tBox_Z.clear();
			for ( int RepId = 0; RepId < RepNo[VolId]; RepId++ ){
				tBox_X.push_back(CalFormula(fBox_X[SolId],RepId)*mm);
				tBox_Y.push_back(CalFormula(fBox_Y[SolId],RepId)*mm);
				tBox_Z.push_back(CalFormula(fBox_Z[SolId],RepId)*mm);
			}
			Box_X.push_back(tBox_X);
			Box_Y.push_back(tBox_Y);
			Box_Z.push_back(tBox_Z);
		}
		else if ( SolidType[VolId] == "EllipticalTube" ){
			std::vector<G4double> tEllipticalTube_X; tEllipticalTube_X.clear();
			std::vector<G4double> tEllipticalTube_Y; tEllipticalTube_Y.clear();
			std::vector<G4double> tEllipticalTube_Z; tEllipticalTube_Z.clear();
			for ( int RepId = 0; RepId < RepNo[VolId]; RepId++ ){
				tEllipticalTube_X.push_back(CalFormula(fEllipticalTube_X[SolId],RepId)*mm);
				tEllipticalTube_Y.push_back(CalFormula(fEllipticalTube_Y[SolId],RepId)*mm);
				tEllipticalTube_Z.push_back(CalFormula(fEllipticalTube_Z[SolId],RepId)*mm);
			}
			EllipticalTube_X.push_back(tEllipticalTube_X);
			EllipticalTube_Y.push_back(tEllipticalTube_Y);
			EllipticalTube_Z.push_back(tEllipticalTube_Z);
		}
		else if ( SolidType[VolId] == "Tubs" ){
			std::vector<G4double> tTubs_RMax;
			std::vector<G4double> tTubs_RMin;
			std::vector<G4double> tTubs_Length;
			std::vector<G4double> tTubs_StartAng;
			std::vector<G4double> tTubs_SpanAng;
			for ( int RepId = 0; RepId < RepNo[VolId]; RepId++ ){
				tTubs_RMax.push_back(CalFormula(fTubs_RMax[SolId],RepId)*mm);
				tTubs_RMin.push_back(CalFormula(fTubs_RMin[SolId],RepId)*mm);
				tTubs_Length.push_back(CalFormula(fTubs_Length[SolId],RepId)*mm);
				tTubs_StartAng.push_back(CalFormula(fTubs_StartAng[SolId],RepId)*deg);
				tTubs_SpanAng.push_back(CalFormula(fTubs_SpanAng[SolId],RepId)*deg);
			}
			Tubs_RMax.push_back(tTubs_RMax);
			Tubs_RMin.push_back(tTubs_RMin);
			Tubs_Length.push_back(tTubs_Length);
			Tubs_StartAng.push_back(tTubs_StartAng);
			Tubs_SpanAng.push_back(tTubs_SpanAng);
		}
		else if( SolidType[VolId] == "Torus" ){
			std::vector<G4double> tTorus_RMax;
			std::vector<G4double> tTorus_RMin;
			std::vector<G4double> tTorus_Rtor;
			std::vector<G4double> tTorus_StartAng;
			std::vector<G4double> tTorus_SpanAng;
			for ( int RepId = 0; RepId < RepNo[VolId]; RepId++ ){
				tTorus_RMax.push_back(CalFormula(fTorus_RMax[SolId],RepId)*mm);
				tTorus_RMin.push_back(CalFormula(fTorus_RMin[SolId],RepId)*mm);
				tTorus_Rtor.push_back(CalFormula(fTorus_Rtor[SolId],RepId)*mm);
				tTorus_StartAng.push_back(CalFormula(fTorus_StartAng[SolId],RepId)*deg);
				tTorus_SpanAng.push_back(CalFormula(fTorus_SpanAng[SolId],RepId)*deg);
			}
			Torus_RMax.push_back(tTorus_RMax);
			Torus_RMin.push_back(tTorus_RMin);
			Torus_Rtor.push_back(tTorus_Rtor);
			Torus_StartAng.push_back(tTorus_StartAng);
			Torus_SpanAng.push_back(tTorus_SpanAng);
		}
		else if( SolidType[VolId] == "Sphere" ){
			std::vector<G4double> tSphere_RMax;
			std::vector<G4double> tSphere_RMin;
			std::vector<G4double> tSphere_StartPhi;
			std::vector<G4double> tSphere_SpanPhi;
			std::vector<G4double> tSphere_StartTheta;
			std::vector<G4double> tSphere_SpanTheta;
			for ( int RepId = 0; RepId < RepNo[VolId]; RepId++ ){
				tSphere_RMax.push_back(CalFormula(fSphere_RMax[SolId],RepId)*mm);
				tSphere_RMin.push_back(CalFormula(fSphere_RMin[SolId],RepId)*mm);
				tSphere_StartPhi.push_back(CalFormula(fSphere_StartPhi[SolId],RepId)*deg);
				tSphere_SpanPhi.push_back(CalFormula(fSphere_SpanPhi[SolId],RepId)*deg);
				tSphere_StartTheta.push_back(CalFormula(fSphere_StartTheta[SolId],RepId)*deg);
				tSphere_SpanTheta.push_back(CalFormula(fSphere_SpanTheta[SolId],RepId)*deg);
			}
			Sphere_RMax.push_back(tSphere_RMax);
			Sphere_RMin.push_back(tSphere_RMin);
			Sphere_StartPhi.push_back(tSphere_StartPhi);
			Sphere_SpanPhi.push_back(tSphere_SpanPhi);
			Sphere_StartTheta.push_back(tSphere_StartTheta);
			Sphere_SpanTheta.push_back(tSphere_SpanTheta);
		}
		else if( SolidType[VolId] == "Hype" ){
			std::vector<G4double> tHype_innerRadius;
			std::vector<G4double> tHype_outerRadius;
			std::vector<G4double> tHype_innerStereo;
			std::vector<G4double> tHype_outerStereo;
			std::vector<G4double> tHype_Length;
			for ( int RepId = 0; RepId < RepNo[VolId]; RepId++ ){
				tHype_innerRadius.push_back(CalFormula(fHype_innerRadius[SolId],RepId)*mm);
				tHype_outerRadius.push_back(CalFormula(fHype_outerRadius[SolId],RepId)*mm);
				tHype_innerStereo.push_back(CalFormula(fHype_innerStereo[SolId],RepId)*deg);
				tHype_outerStereo.push_back(CalFormula(fHype_outerStereo[SolId],RepId)*deg);
				tHype_Length.push_back(CalFormula(fHype_Length[SolId],RepId)*mm);
			}
			Hype_innerRadius.push_back(tHype_innerRadius);
			Hype_outerRadius.push_back(tHype_outerRadius);
			Hype_innerStereo.push_back(tHype_innerStereo);
			Hype_outerStereo.push_back(tHype_outerStereo);
			Hype_Length.push_back(tHype_Length);
		}
		else if( SolidType[VolId] == "TwistedTubs" ){
			std::vector<G4double> tTwistedTubs_twistedangle;
			std::vector<G4double> tTwistedTubs_endinnerrad;
			std::vector<G4double> tTwistedTubs_endouterrad;
			std::vector<G4double> tTwistedTubs_Length;
			std::vector<G4double> tTwistedTubs_dphi;
			for ( int RepId = 0; RepId < RepNo[VolId]; RepId++ ){
				tTwistedTubs_twistedangle.push_back(CalFormula(fTwistedTubs_twistedangle[SolId],RepId)*deg);
				tTwistedTubs_endinnerrad.push_back(CalFormula(fTwistedTubs_endinnerrad[SolId],RepId)*mm);
				tTwistedTubs_endouterrad.push_back(CalFormula(fTwistedTubs_endouterrad[SolId],RepId)*mm);
				tTwistedTubs_Length.push_back(CalFormula(fTwistedTubs_Length[SolId],RepId)*mm);
				tTwistedTubs_dphi.push_back(CalFormula(fTwistedTubs_dphi[SolId],RepId)*deg);
			}
			TwistedTubs_twistedangle.push_back(tTwistedTubs_twistedangle);
			TwistedTubs_endinnerrad.push_back(tTwistedTubs_endinnerrad);
			TwistedTubs_endouterrad.push_back(tTwistedTubs_endouterrad);
			TwistedTubs_Length.push_back(tTwistedTubs_Length);
			TwistedTubs_dphi.push_back(tTwistedTubs_dphi);
		}
		else if( SolidType[VolId] == "Cons" ){
			std::vector<G4double> tCons_RMax1;
			std::vector<G4double> tCons_RMin1;
			std::vector<G4double> tCons_RMax2;
			std::vector<G4double> tCons_RMin2;
			std::vector<G4double> tCons_Length;
			std::vector<G4double> tCons_StartAng;
			std::vector<G4double> tCons_SpanAng;
			for ( int RepId = 0; RepId < RepNo[VolId]; RepId++ ){
				tCons_RMax1.push_back(CalFormula(fCons_RMax1[SolId],RepId)*mm);
				tCons_RMin1.push_back(CalFormula(fCons_RMin1[SolId],RepId)*mm);
				tCons_RMax2.push_back(CalFormula(fCons_RMax2[SolId],RepId)*mm);
				tCons_RMin2.push_back(CalFormula(fCons_RMin2[SolId],RepId)*mm);
				tCons_Length.push_back(CalFormula(fCons_Length[SolId],RepId)*mm);
				tCons_StartAng.push_back(CalFormula(fCons_StartAng[SolId],RepId)*deg);
				tCons_SpanAng.push_back(CalFormula(fCons_SpanAng[SolId],RepId)*deg);
			}
			Cons_RMax1.push_back(tCons_RMax1);
			Cons_RMin1.push_back(tCons_RMin1);
			Cons_RMax2.push_back(tCons_RMax2);
			Cons_RMin2.push_back(tCons_RMin2);
			Cons_Length.push_back(tCons_Length);
			Cons_StartAng.push_back(tCons_StartAng);
			Cons_SpanAng.push_back(tCons_SpanAng);
		}
		else if( SolidType[VolId] == "Polycone" ){
			std::vector<G4double> tPolycone_StartAng;
			std::vector<G4double> tPolycone_SpanAng;
			for ( int RepId = 0; RepId < RepNo[VolId]; RepId++ ){
				tPolycone_StartAng.push_back(CalFormula(fPolycone_StartAng[SolId],RepId)*deg);
				tPolycone_SpanAng.push_back(CalFormula(fPolycone_SpanAng[SolId],RepId)*deg);
			}
			Polycone_StartAng.push_back(tPolycone_StartAng);
			Polycone_SpanAng.push_back(tPolycone_SpanAng);
			std::vector<std::vector<G4double> > iPolycone_RMax;
			std::vector<std::vector<G4double> > iPolycone_RMin;
			std::vector<std::vector<G4double> > iPolycone_Z;
			for ( int ConeId = 0; ConeId < Polycone_numZ[SolId]; ConeId++ ){
				std::vector<G4double> tPolycone_RMax;
				std::vector<G4double> tPolycone_RMin;
				std::vector<G4double> tPolycone_Z;
				for ( int RepId = 0; RepId < RepNo[VolId]; RepId++ ){
					tPolycone_RMax.push_back(CalFormula(fPolycone_RMax[SolId][ConeId],RepId)*mm);
					tPolycone_RMin.push_back(CalFormula(fPolycone_RMin[SolId][ConeId],RepId)*mm);
					tPolycone_Z.push_back(CalFormula(fPolycone_Z[SolId][ConeId],RepId)*mm);
				}
				iPolycone_RMax.push_back(tPolycone_RMax);
				iPolycone_RMin.push_back(tPolycone_RMin);
				iPolycone_Z.push_back(tPolycone_Z);
			}
			Polycone_RMax.push_back(iPolycone_RMax);
			Polycone_RMin.push_back(iPolycone_RMin);
			Polycone_Z.push_back(iPolycone_Z);
		}
		else if( SolidType[VolId] == "ExtrudedSolid" ){
			std::vector<std::vector<G4double> > iExtrudedSolid_X;
			std::vector<std::vector<G4double> > iExtrudedSolid_Y;
			std::vector<std::vector<G4double> > iExtrudedSolid_Z;
			std::vector<std::vector<G4double> > iExtrudedSolid_X0;
			std::vector<std::vector<G4double> > iExtrudedSolid_Y0;
			std::vector<std::vector<G4double> > iExtrudedSolid_Scale;
			for ( int ConeId = 0; ConeId < ExtrudedSolid_numZ[SolId]; ConeId++ ){
				std::vector<G4double> tExtrudedSolid_Z;
				std::vector<G4double> tExtrudedSolid_X0;
				std::vector<G4double> tExtrudedSolid_Y0;
				std::vector<G4double> tExtrudedSolid_Scale;
				for ( int RepId = 0; RepId < RepNo[VolId]; RepId++ ){
					tExtrudedSolid_Z.push_back(CalFormula(fExtrudedSolid_Z[SolId][ConeId],RepId)*mm);
					tExtrudedSolid_X0.push_back(CalFormula(fExtrudedSolid_X0[SolId][ConeId],RepId)*mm);
					tExtrudedSolid_Y0.push_back(CalFormula(fExtrudedSolid_Y0[SolId][ConeId],RepId)*mm);
					tExtrudedSolid_Scale.push_back(CalFormula(fExtrudedSolid_Scale[SolId][ConeId],RepId));
				}
				iExtrudedSolid_Z.push_back(tExtrudedSolid_Z);
				iExtrudedSolid_X0.push_back(tExtrudedSolid_X0);
				iExtrudedSolid_Y0.push_back(tExtrudedSolid_Y0);
				iExtrudedSolid_Scale.push_back(tExtrudedSolid_Scale);
			}
			for ( int ConeId = 0; ConeId < ExtrudedSolid_numP[SolId]; ConeId++ ){
				std::vector<G4double> tExtrudedSolid_X;
				std::vector<G4double> tExtrudedSolid_Y;
				for ( int RepId = 0; RepId < RepNo[VolId]; RepId++ ){
					tExtrudedSolid_X.push_back(CalFormula(fExtrudedSolid_X[SolId][ConeId],RepId)*mm);
					tExtrudedSolid_Y.push_back(CalFormula(fExtrudedSolid_Y[SolId][ConeId],RepId)*mm);
				}
				iExtrudedSolid_X.push_back(tExtrudedSolid_X);
				iExtrudedSolid_Y.push_back(tExtrudedSolid_Y);
			}
			ExtrudedSolid_X.push_back(iExtrudedSolid_X);
			ExtrudedSolid_Y.push_back(iExtrudedSolid_Y);
			ExtrudedSolid_Z.push_back(iExtrudedSolid_Z);
			ExtrudedSolid_X0.push_back(iExtrudedSolid_X0);
			ExtrudedSolid_Y0.push_back(iExtrudedSolid_Y0);
			ExtrudedSolid_Scale.push_back(iExtrudedSolid_Scale);
		}
	}
//	std::cout<<"End of SimpleGeometryParameter::Calculate"<<std::endl; // to be deleted
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
	EllipticalTubeNo = 0;
	TubsNo = 0;
	TorusNo = 0;
	SphereNo = 0;
	HypeNo = 0;
	TwistedTubsNo = 0;
	ConsNo = 0;
	PolyconeNo = 0;
	ExtrudedSolidNo = 0;
	BooleanSolidNo = 0;

	//General info about volume
	Name.clear();
	MotherName.clear();
	SDName.clear();
	Material.clear();
	SRepNo.clear();
	RepNo.clear();
	SolidType.clear();
	SolidIndex.clear();
	SolidBoolean.clear();
	fPosX.clear();
	fPosY.clear();
	fPosZ.clear();
	fPosR.clear();
	fPosPhi.clear();
	fPosTheta.clear();
	PosType.clear();
	fEphi.clear();
	fEtheta.clear();
	fEpsi.clear();
	Pos.clear();
	Ephi.clear();
	Etheta.clear();
	Epsi.clear();

	//Box info
	fBox_X.clear();
	fBox_Y.clear();
	fBox_Z.clear();
	Box_X.clear();
	Box_Y.clear();
	Box_Z.clear();
	Box_GenIndex.clear();

	//EllipticalTube info
	fEllipticalTube_X.clear();
	fEllipticalTube_Y.clear();
	fEllipticalTube_Z.clear();
	EllipticalTube_X.clear();
	EllipticalTube_Y.clear();
	EllipticalTube_Z.clear();
	EllipticalTube_GenIndex.clear();

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

	//Torus info
	fTorus_RMax.clear();
	fTorus_RMin.clear();
	fTorus_Rtor.clear();
	fTorus_StartAng.clear();
	fTorus_SpanAng.clear();
	Torus_RMax.clear();
	Torus_RMin.clear();
	Torus_Rtor.clear();
	Torus_StartAng.clear();
	Torus_SpanAng.clear();
	Torus_GenIndex.clear();

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

	//Hype info
	fHype_innerRadius.clear();
	fHype_outerRadius.clear();
	fHype_innerStereo.clear();
	fHype_outerStereo.clear();
	fHype_Length.clear();
	Hype_innerRadius.clear();
	Hype_outerRadius.clear();
	Hype_innerStereo.clear();
	Hype_outerStereo.clear();
	Hype_Length.clear();
	Hype_GenIndex.clear();

	//TwistedTubs info
	fTwistedTubs_twistedangle.clear();
	fTwistedTubs_endinnerrad.clear();
	fTwistedTubs_endouterrad.clear();
	fTwistedTubs_Length.clear();
	fTwistedTubs_dphi.clear();
	TwistedTubs_twistedangle.clear();
	TwistedTubs_endinnerrad.clear();
	TwistedTubs_endouterrad.clear();
	TwistedTubs_Length.clear();
	TwistedTubs_dphi.clear();
	TwistedTubs_GenIndex.clear();

	//Cons info
	fCons_RMax1.clear();
	fCons_RMin1.clear();
	fCons_RMax2.clear();
	fCons_RMin2.clear();
	fCons_Length.clear();
	fCons_StartAng.clear();
	fCons_SpanAng.clear();
	Cons_RMax1.clear();
	Cons_RMin1.clear();
	Cons_RMax2.clear();
	Cons_RMin2.clear();
	Cons_Length.clear();
	Cons_StartAng.clear();
	Cons_SpanAng.clear();
	Cons_GenIndex.clear();

	//Polycone info
	fPolycone_RMax.clear();
	fPolycone_RMin.clear();
	fPolycone_Z.clear();
	Polycone_RMax.clear();
	Polycone_RMin.clear();
	Polycone_Z.clear();
	Polycone_numZ.clear();
	fPolycone_StartAng.clear();
	fPolycone_SpanAng.clear();
	Polycone_StartAng.clear();
	Polycone_SpanAng.clear();
	Polycone_GenIndex.clear();
	waited_Polycone_iVol = -1;
	achieved_componets_Polycone = 0;

	//ExtrudedSolid info
	fExtrudedSolid_X.clear();
	fExtrudedSolid_Y.clear();
	fExtrudedSolid_Z.clear();
	fExtrudedSolid_X0.clear();
	fExtrudedSolid_Y0.clear();
	fExtrudedSolid_Scale.clear();
	ExtrudedSolid_X.clear();
	ExtrudedSolid_Y.clear();
	ExtrudedSolid_Z.clear();
	ExtrudedSolid_X0.clear();
	ExtrudedSolid_Y0.clear();
	ExtrudedSolid_Scale.clear();
	ExtrudedSolid_numZ.clear();
	ExtrudedSolid_numP.clear();
	ExtrudedSolid_GenIndex.clear();
	waited_ExtrudedSolid_iVol = -1;
	achieved_Z_ExtrudedSolid = 0;
	achieved_P_ExtrudedSolid = 0;

	//BooleanSolid info
	BooleanSolid_type.clear();
	BooleanSolid_sol1.clear();
	BooleanSolid_sol2.clear();
	fBooleanSolid_Ephi.clear();
	fBooleanSolid_Etheta.clear();
	fBooleanSolid_Epsi.clear();
	fBooleanSolid_PosX.clear();
	fBooleanSolid_PosY.clear();
	fBooleanSolid_PosZ.clear();
	fBooleanSolid_PosR.clear();
	fBooleanSolid_PosPhi.clear();
	fBooleanSolid_PosTheta.clear();
	BooleanSolid_PosType.clear();
	BooleanSolid_Ephi.clear();
	BooleanSolid_Etheta.clear();
	BooleanSolid_Epsi.clear();
	BooleanSolid_Pos.clear();
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
				G4String tPolycone_RMax;
				G4String tPolycone_RMin;
				G4String tPolycone_Z;
				buf_card>>tPolycone_Z>>tPolycone_RMin>>tPolycone_RMax;
				fPolycone_RMax[waited_Polycone_iVol][achieved_componets_Polycone] = ReplaceMacro(tPolycone_RMax);
				fPolycone_RMin[waited_Polycone_iVol][achieved_componets_Polycone] = ReplaceMacro(tPolycone_RMin);
				fPolycone_Z[waited_Polycone_iVol][achieved_componets_Polycone] = ReplaceMacro(tPolycone_Z);
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
	else if (waited_ExtrudedSolid_iVol>=0){
		GoOn = false;
		if (achieved_Z_ExtrudedSolid<ExtrudedSolid_numZ[waited_ExtrudedSolid_iVol]){
			if (name=="ESZ"){
				G4String tExtrudedSolid_Z;
				G4String tExtrudedSolid_X0;
				G4String tExtrudedSolid_Y0;
				G4String tExtrudedSolid_Scale;
				buf_card>>tExtrudedSolid_Z>>tExtrudedSolid_X0>>tExtrudedSolid_Y0>>tExtrudedSolid_Scale;
				fExtrudedSolid_Z[waited_ExtrudedSolid_iVol][achieved_Z_ExtrudedSolid] = ReplaceMacro(tExtrudedSolid_Z);
				fExtrudedSolid_X0[waited_ExtrudedSolid_iVol][achieved_Z_ExtrudedSolid] = ReplaceMacro(tExtrudedSolid_X0);
				fExtrudedSolid_Y0[waited_ExtrudedSolid_iVol][achieved_Z_ExtrudedSolid] = ReplaceMacro(tExtrudedSolid_Y0);
				fExtrudedSolid_Scale[waited_ExtrudedSolid_iVol][achieved_Z_ExtrudedSolid] = ReplaceMacro(tExtrudedSolid_Scale);
				achieved_Z_ExtrudedSolid++;
			}
		}
		if (achieved_P_ExtrudedSolid<ExtrudedSolid_numP[waited_ExtrudedSolid_iVol]){
			if (name=="ESP"){
				G4String tExtrudedSolid_X;
				G4String tExtrudedSolid_Y;
				buf_card>>tExtrudedSolid_X>>tExtrudedSolid_Y;
				fExtrudedSolid_X[waited_ExtrudedSolid_iVol][achieved_P_ExtrudedSolid] = ReplaceMacro(tExtrudedSolid_X);
				fExtrudedSolid_Y[waited_ExtrudedSolid_iVol][achieved_P_ExtrudedSolid] = ReplaceMacro(tExtrudedSolid_Y);
				achieved_P_ExtrudedSolid++;
			}
		}
		if (ExtrudedSolid_numP[waited_ExtrudedSolid_iVol]==achieved_P_ExtrudedSolid&&ExtrudedSolid_numZ[waited_ExtrudedSolid_iVol]==achieved_Z_ExtrudedSolid){
			waited_ExtrudedSolid_iVol=-1;
			achieved_P_ExtrudedSolid=0;
			achieved_Z_ExtrudedSolid=0;
		}
	}
	if (GoOn){
		bool foundVolume = false;
		if( name == "B" ){
			SolidType.push_back("Box");
			SolidIndex.push_back(BoxNo);
			G4String tBox_X;
			G4String tBox_Y;
			G4String tBox_Z;
			buf_card>>tBox_X>>tBox_Y>>tBox_Z;
			fBox_X.push_back(ReplaceMacro(tBox_X));
			fBox_Y.push_back(ReplaceMacro(tBox_Y));
			fBox_Z.push_back(ReplaceMacro(tBox_Z));
			Box_GenIndex.push_back(iVol);
			BoxNo++;
			foundVolume = true;
		}
		if( name == "ET" ){
			SolidType.push_back("EllipticalTube");
			SolidIndex.push_back(EllipticalTubeNo);
			G4String tEllipticalTube_X;
			G4String tEllipticalTube_Y;
			G4String tEllipticalTube_Z;
			buf_card>>tEllipticalTube_X>>tEllipticalTube_Y>>tEllipticalTube_Z;
			fEllipticalTube_X.push_back(ReplaceMacro(tEllipticalTube_X));
			fEllipticalTube_Y.push_back(ReplaceMacro(tEllipticalTube_Y));
			fEllipticalTube_Z.push_back(ReplaceMacro(tEllipticalTube_Z));
			EllipticalTube_GenIndex.push_back(iVol);
			EllipticalTubeNo++;
			foundVolume = true;
		}
		else if( name == "T" ){
			SolidType.push_back("Tubs");
			SolidIndex.push_back(TubsNo);
			G4String tTubs_RMax;
			G4String tTubs_RMin;
			G4String tTubs_Length;
			G4String tTubs_StartAng;
			G4String tTubs_SpanAng;
			buf_card>>tTubs_RMin>>tTubs_RMax>>tTubs_Length>>tTubs_StartAng>>tTubs_SpanAng;
			fTubs_RMax.push_back(ReplaceMacro(tTubs_RMax));
			fTubs_RMin.push_back(ReplaceMacro(tTubs_RMin));
			fTubs_Length.push_back(ReplaceMacro(tTubs_Length));
			fTubs_StartAng.push_back(ReplaceMacro(tTubs_StartAng));
			fTubs_SpanAng.push_back(ReplaceMacro(tTubs_SpanAng));
			Tubs_GenIndex.push_back(iVol);
			TubsNo++;
			foundVolume = true;
		}
		else if( name == "Tor" ){
			SolidType.push_back("Torus");
			SolidIndex.push_back(TorusNo);
			G4String tTorus_RMax;
			G4String tTorus_RMin;
			G4String tTorus_Rtor;
			G4String tTorus_StartAng;
			G4String tTorus_SpanAng;
			buf_card>>tTorus_RMin>>tTorus_RMax>>tTorus_Rtor>>tTorus_StartAng>>tTorus_SpanAng;
			fTorus_RMax.push_back(ReplaceMacro(tTorus_RMax));;
			fTorus_RMin.push_back(ReplaceMacro(tTorus_RMin));;
			fTorus_Rtor.push_back(ReplaceMacro(tTorus_Rtor));;
			fTorus_StartAng.push_back(ReplaceMacro(tTorus_StartAng));;
			fTorus_SpanAng.push_back(ReplaceMacro(tTorus_SpanAng));;
			Torus_GenIndex.push_back(iVol);
			TorusNo++;
			foundVolume = true;
		}
		else if( name == "Sph" ){
			SolidType.push_back("Sphere");
			SolidIndex.push_back(SphereNo);
			G4String tSphere_RMax;
			G4String tSphere_RMin;
			G4String tSphere_StartPhi;
			G4String tSphere_SpanPhi;
			G4String tSphere_StartTheta;
			G4String tSphere_SpanTheta;
			buf_card>>tSphere_RMin>>tSphere_RMax>>tSphere_StartPhi>>tSphere_SpanPhi>>tSphere_StartTheta>>tSphere_SpanTheta;
			fSphere_RMax.push_back(ReplaceMacro(tSphere_RMax));;
			fSphere_RMin.push_back(ReplaceMacro(tSphere_RMin));;
			fSphere_StartPhi.push_back(ReplaceMacro(tSphere_StartPhi));;
			fSphere_SpanPhi.push_back(ReplaceMacro(tSphere_SpanPhi));;
			fSphere_StartTheta.push_back(ReplaceMacro(tSphere_StartTheta));;
			fSphere_SpanTheta.push_back(ReplaceMacro(tSphere_SpanTheta));;
			Sphere_GenIndex.push_back(iVol);
			SphereNo++;
			foundVolume = true;
		}
		else if( name == "H" ){
			SolidType.push_back("Hype");
			SolidIndex.push_back(HypeNo);
			G4String tHype_innerRadius;
			G4String tHype_outerRadius;
			G4String tHype_innerStereo;
			G4String tHype_outerStereo;
			G4String tHype_Length;
			buf_card>>tHype_innerRadius>>tHype_outerRadius>>tHype_innerStereo>>tHype_outerStereo>>tHype_Length;
			fHype_innerRadius.push_back(ReplaceMacro(tHype_innerRadius));;
			fHype_outerRadius.push_back(ReplaceMacro(tHype_outerRadius));;
			fHype_innerStereo.push_back(ReplaceMacro(tHype_innerStereo));;
			fHype_outerStereo.push_back(ReplaceMacro(tHype_outerStereo));;
			fHype_Length.push_back(ReplaceMacro(tHype_Length));;
			Hype_GenIndex.push_back(iVol);
			HypeNo++;
			foundVolume = true;
		}
		else if( name == "TT" ){
			SolidType.push_back("TwistedTubs");
			SolidIndex.push_back(TwistedTubsNo);
			G4String tTwistedTubs_twistedangle;
			G4String tTwistedTubs_endinnerrad;
			G4String tTwistedTubs_endouterrad;
			G4String tTwistedTubs_Length;
			G4String tTwistedTubs_dphi;
			buf_card>>tTwistedTubs_twistedangle>>tTwistedTubs_endinnerrad>>tTwistedTubs_endouterrad>>tTwistedTubs_Length>>tTwistedTubs_dphi;
			fTwistedTubs_twistedangle.push_back(ReplaceMacro(tTwistedTubs_twistedangle));;
			fTwistedTubs_endinnerrad.push_back(ReplaceMacro(tTwistedTubs_endinnerrad));;
			fTwistedTubs_endouterrad.push_back(ReplaceMacro(tTwistedTubs_endouterrad));;
			fTwistedTubs_Length.push_back(ReplaceMacro(tTwistedTubs_Length));;
			fTwistedTubs_dphi.push_back(ReplaceMacro(tTwistedTubs_dphi));;
			TwistedTubs_GenIndex.push_back(iVol);
			TwistedTubsNo++;
			foundVolume = true;
		}
		else if( name == "C" ){
			SolidType.push_back("Cons");
			SolidIndex.push_back(ConsNo);
			G4String tCons_RMax1;
			G4String tCons_RMin1;
			G4String tCons_RMax2;
			G4String tCons_RMin2;
			G4String tCons_Length;
			G4String tCons_StartAng;
			G4String tCons_SpanAng;
			buf_card>>tCons_RMin1>>tCons_RMax1>>tCons_RMin2>>tCons_RMax2>>tCons_Length>>tCons_StartAng>>tCons_SpanAng;
			fCons_RMax1.push_back(ReplaceMacro(tCons_RMax1));;
			fCons_RMin1.push_back(ReplaceMacro(tCons_RMin1));;
			fCons_RMax2.push_back(ReplaceMacro(tCons_RMax2));;
			fCons_RMin2.push_back(ReplaceMacro(tCons_RMin2));;
			fCons_Length.push_back(ReplaceMacro(tCons_Length));;
			fCons_StartAng.push_back(ReplaceMacro(tCons_StartAng));;
			fCons_SpanAng.push_back(ReplaceMacro(tCons_SpanAng));;
			Cons_GenIndex.push_back(iVol);
			ConsNo++;
			foundVolume = true;
		}
		else if( name == "PC" ){
			SolidType.push_back("Polycone");
			SolidIndex.push_back(PolyconeNo);
			G4int tPolycone_numZ;
			G4String tPolycone_StartAng;
			G4String tPolycone_SpanAng;
			buf_card>>tPolycone_numZ>>tPolycone_StartAng>>tPolycone_SpanAng;
			Polycone_numZ.push_back(tPolycone_numZ);
			fPolycone_StartAng.push_back(ReplaceMacro(tPolycone_StartAng));;
			fPolycone_SpanAng.push_back(ReplaceMacro(tPolycone_SpanAng));;
			Polycone_GenIndex.push_back(iVol);
			std::vector<G4String> empty_vec;
			empty_vec.resize(tPolycone_numZ);
			fPolycone_RMax.push_back(empty_vec);;
			fPolycone_RMin.push_back(empty_vec);;
			fPolycone_Z.push_back(empty_vec);;
			waited_Polycone_iVol = PolyconeNo;
			achieved_componets_Polycone = 0;
			PolyconeNo++;
			foundVolume = true;
		}
		else if( name == "ES" ){
			SolidType.push_back("ExtrudedSolid");
			SolidIndex.push_back(ExtrudedSolidNo);
			G4int tExtrudedSolid_numZ;
			G4int tExtrudedSolid_numP;
			buf_card>>tExtrudedSolid_numZ>>tExtrudedSolid_numP;
			ExtrudedSolid_numZ.push_back(tExtrudedSolid_numZ);
			ExtrudedSolid_numP.push_back(tExtrudedSolid_numP);
			ExtrudedSolid_GenIndex.push_back(iVol);
			std::vector<G4String> empty_vec;
			empty_vec.resize(tExtrudedSolid_numP);
			fExtrudedSolid_X.push_back(empty_vec);;
			fExtrudedSolid_Y.push_back(empty_vec);;
			empty_vec.resize(tExtrudedSolid_numZ);
			fExtrudedSolid_Z.push_back(empty_vec);;
			fExtrudedSolid_X0.push_back(empty_vec);;
			fExtrudedSolid_Y0.push_back(empty_vec);;
			fExtrudedSolid_Scale.push_back(empty_vec);;
			waited_ExtrudedSolid_iVol = ExtrudedSolidNo;
			achieved_P_ExtrudedSolid = 0;
			achieved_Z_ExtrudedSolid = 0;
			ExtrudedSolidNo++;
			foundVolume = true;
		}
		else if( name == "BL" ){
			SolidType.push_back("BooleanSolid");
			SolidIndex.push_back(BooleanSolidNo);
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
			G4String tBooleanSolid_PosR;
			G4String tBooleanSolid_PosTheta;
			G4String tBooleanSolid_PosPhi;
			G4String tBooleanSolid_PosType;
			buf_card>>tBooleanSolid_type>>tBooleanSolid_sol1>>tBooleanSolid_sol2>>tBooleanSolid_Ephi>>tBooleanSolid_Etheta>>tBooleanSolid_Epsi>>dump;
			if (dump=="P"){ // Need Polar Coordinates
				buf_card>>tBooleanSolid_PosR>>tBooleanSolid_PosPhi>>tBooleanSolid_PosZ;
				tBooleanSolid_PosType = "Polar";
			}
			else if (dump=="S"){ // Need Spherical Coordinates
				buf_card>>tBooleanSolid_PosR>>tBooleanSolid_PosPhi>>tBooleanSolid_PosTheta;
				tBooleanSolid_PosType = "Spherical";
			}
			else if (dump=="C"){ // Need Cartesian Coordinates
				buf_card>>tBooleanSolid_PosX>>tBooleanSolid_PosY>>tBooleanSolid_PosZ;
				tBooleanSolid_PosType = "Cartesian";
			}
			else{ // By default, need Cartesian Coordinates
				tBooleanSolid_PosX=dump;
				buf_card>>tBooleanSolid_PosY>>tBooleanSolid_PosZ;
				tBooleanSolid_PosType = "Cartesian";
			}
			BooleanSolid_type.push_back(tBooleanSolid_type);
			BooleanSolid_sol1.push_back(tBooleanSolid_sol1);
			BooleanSolid_sol2.push_back(tBooleanSolid_sol2);
			fBooleanSolid_Ephi.push_back(ReplaceMacro(tBooleanSolid_Ephi));
			fBooleanSolid_Etheta.push_back(ReplaceMacro(tBooleanSolid_Etheta));
			fBooleanSolid_Epsi.push_back(ReplaceMacro(tBooleanSolid_Epsi));
			fBooleanSolid_PosX.push_back(ReplaceMacro(tBooleanSolid_PosX));
			fBooleanSolid_PosY.push_back(ReplaceMacro(tBooleanSolid_PosY));
			fBooleanSolid_PosZ.push_back(ReplaceMacro(tBooleanSolid_PosZ));
			fBooleanSolid_PosR.push_back(ReplaceMacro(tBooleanSolid_PosR));
			fBooleanSolid_PosTheta.push_back(ReplaceMacro(tBooleanSolid_PosTheta));
			fBooleanSolid_PosPhi.push_back(ReplaceMacro(tBooleanSolid_PosPhi));
			BooleanSolid_PosType.push_back(ReplaceMacro(tBooleanSolid_PosType));
			BooleanSolid_GenIndex.push_back(iVol);
			BooleanSolidNo++;
			foundVolume = true;
		}
		if (foundVolume){
			G4String dump;
			G4String tPosX;
			G4String tPosY;
			G4String tPosZ;
			G4String tPosR;
			G4String tPosTheta;
			G4String tPosPhi;
			G4String tPosType;
			G4String tRepCont;
			G4int tRepNo = 1;
			G4int tSRepNo = 0;
			G4String tSDName;
			G4String tEphi;
			G4String tEtheta;
			G4String tEpsi;
			G4String tName;
			G4String tMotherName;
			G4String tMaterial;
			G4bool   tSolidBoolean = true;
			buf_card>>tName>>tRepCont;
			get_RepCont(tRepCont,tSRepNo,tRepNo);
			if(buf_card>>tMotherName){
				tSolidBoolean = false;
				buf_card>>tMaterial>>tSDName>>dump;
				if (dump=="P"){ // Need Polar Coordinates
					buf_card>>tPosR>>tPosPhi>>tPosZ;
					tPosType = "Polar";
				}
				else if (dump=="S"){ // Need Spherical Coordinates
					buf_card>>tPosR>>tPosPhi>>tPosTheta;
					tPosType = "Spherical";
				}
				else if (dump=="C"){ // Need Cartesian Coordinates
					buf_card>>tPosX>>tPosY>>tPosZ;
					tPosType = "Cartesian";
				}
				else{ // By default, need Cartesian Coordinates
					tPosX=dump;
					buf_card>>tPosY>>tPosZ;
					tPosType = "Cartesian";
				}
				buf_card>>tEphi>>tEtheta>>tEpsi;
			}
			fPosX.push_back(ReplaceMacro(tPosX));;
			fPosY.push_back(ReplaceMacro(tPosY));;
			fPosZ.push_back(ReplaceMacro(tPosZ));;
			fPosR.push_back(ReplaceMacro(tPosR));;
			fPosPhi.push_back(ReplaceMacro(tPosPhi));;
			fPosTheta.push_back(ReplaceMacro(tPosTheta));;
			PosType.push_back(tPosType);
			Name.push_back(tName);
			MotherName.push_back(tMotherName);
			Material.push_back(tMaterial);
			SDName.push_back(tSDName);
			SRepNo.push_back(tSRepNo);
			RepNo.push_back(tRepNo);
			fEphi.push_back(ReplaceMacro(tEphi));;
			fEtheta.push_back(ReplaceMacro(tEtheta));;
			fEpsi.push_back(ReplaceMacro(tEpsi));;
			SolidBoolean.push_back(tSolidBoolean);
			VolNo++;
//			std::cout<<"found Volume:"<<tName<<std::endl; // to be deleted
		}
		else if ( name == "DEFINE:" ){
			G4String MacroName;
			G4String MacroValue;
			buf_card>>MacroName>>MacroValue;
//			std::cout<<"found DEFINE:"<<std::endl; // to be deleted
			MacroValue = ReplaceMacro(MacroValue);
			std::cout<<"Define: "<<MacroName<<":\t"<<MacroValue<<std::endl;
			bool foundName = false;
			for (int i = 0; i < knownValueNames.size(); i++){
				if (knownValueNames[i]==MacroName){ // If this name occurred once, replace the value
					foundName = true;
					knownValues[i]=MacroValue;
					break;
				}
			}
			if (!foundName){
				knownValueNames.push_back(MacroName);
				knownValues.push_back(MacroValue);
			}
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
//	std::cout<<std::scientific<<std::setprecision(2);
	for( G4int i = 0; i < BoxNo; i++ ){
		if ( i == 0 ){
			std::cout<<"=>Box info:"<<BoxNo<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
			         <<std::setiosflags(std::ios::left)<<std::setw(10) <<"x"
			         <<std::setiosflags(std::ios::left)<<std::setw(10) <<"y"
			         <<std::setiosflags(std::ios::left)<<std::setw(10) <<"z";
			dump_general_header();
			std::cout<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
			         <<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
			         <<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
			         <<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm";
			dump_general_note();
			std::cout<<std::endl;
		} 
		int index = Box_GenIndex[i];
		int repNo = RepNo[index];
		for ( G4int j = 0; j < repNo; j++ ){
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Box_X[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Box_Y[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Box_Z[i][j]/mm;
			dump_general_value(index,j);
			std::cout<<std::endl;
		}
	}
	for( G4int i = 0; i < EllipticalTubeNo; i++ ){
		if ( i == 0 ){
			std::cout<<"=>EllipticalTube info:"<<EllipticalTubeNo<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
			         <<std::setiosflags(std::ios::left)<<std::setw(10) <<"Dx"
			         <<std::setiosflags(std::ios::left)<<std::setw(10) <<"Dy"
			         <<std::setiosflags(std::ios::left)<<std::setw(10) <<"Dz";
			dump_general_header();
			std::cout<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
			         <<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
			         <<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
			         <<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm";
			dump_general_note();
			std::cout<<std::endl;
		} 
		int index = EllipticalTube_GenIndex[i];
		int repNo = RepNo[index];
		for ( G4int j = 0; j < repNo; j++ ){
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<EllipticalTube_X[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<EllipticalTube_Y[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<EllipticalTube_Z[i][j]/mm;
			dump_general_value(index,j);
			std::cout<<std::endl;
		}
	}

	for( G4int i = 0; i < TubsNo; i++ ){
		if ( i == 0 ){
			std::cout<<"=>Tubs info:"<<TubsNo<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"RMin"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"RMax"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"Length"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"StartAng"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"SpanAng";
			dump_general_header();
			std::cout<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"deg";
			dump_general_note();
			std::cout<<std::endl;
		}
		int index = Tubs_GenIndex[i];
		int repNo = RepNo[index];
		for ( G4int j = 0; j < repNo; j++ ){
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Tubs_RMin[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Tubs_RMax[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Tubs_Length[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Tubs_StartAng[i][j]/deg
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Tubs_SpanAng[i][j]/deg;
			dump_general_value(index,j);
			std::cout<<std::endl;
		}
	}

	for( G4int i = 0; i < TorusNo; i++ ){
		if ( i == 0 ){
			std::cout<<"=>Torus info:"<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"RMin"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"RMax"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"Rtor"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"StartAng"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"SpanAng";
			dump_general_header();
			std::cout<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"deg";
			dump_general_note();
			std::cout<<std::endl;
		}
		int index = Torus_GenIndex[i];
		int repNo = RepNo[index];
		for ( G4int j = 0; j < repNo; j++ ){
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Torus_RMin[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Torus_RMax[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Torus_Rtor[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Torus_StartAng[i][j]/deg
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Torus_SpanAng[i][j]/deg;
			dump_general_value(index,j);
			std::cout<<std::endl;
		}
	}

	for( G4int i = 0; i < SphereNo; i++ ){
		if ( i == 0 ){
			std::cout<<"=>Sphere info:"<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"RMin"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"RMax"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"StartPhi"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"SpanPhi"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"StartThe"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"SpanThe";
			dump_general_header();
			std::cout<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"deg";
			dump_general_note();
			std::cout<<std::endl;
		}
		int index = Sphere_GenIndex[i];
		int repNo = RepNo[index];
		for ( G4int j = 0; j < repNo; j++ ){
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Sphere_RMin[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Sphere_RMax[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Sphere_StartPhi[i][j]/deg
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Sphere_SpanPhi[i][j]/deg
				<<std::setiosflags(std::ios::left)<<std::setw(10)<<Sphere_StartTheta[i][j]/deg
				<<std::setiosflags(std::ios::left)<<std::setw(10)<<Sphere_SpanTheta[i][j]/deg;
			dump_general_value(index);
			std::cout<<std::endl;
		}
	}

	for( G4int i = 0; i < HypeNo; i++ ){
		if ( i == 0 ){
			std::cout<<"=>Hype info:"<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
				<<std::setiosflags(std::ios::left)<<std::setw(10)<<"iRadius"
				<<std::setiosflags(std::ios::left)<<std::setw(10)<<"oRadius"
				<<std::setiosflags(std::ios::left)<<std::setw(10)<<"iStereo"
				<<std::setiosflags(std::ios::left)<<std::setw(10)<<"oStereo"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"Length";
			dump_general_header();
			std::cout<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(10)<<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10)<<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10)<<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(10)<<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm";
			dump_general_note();
			std::cout<<std::endl;
		}
		int index = Hype_GenIndex[i];
		int repNo = RepNo[index];
		for ( G4int j = 0; j < repNo; j++ ){
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
				<<std::setiosflags(std::ios::left)<<std::setw(10)<<Hype_innerRadius[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10)<<Hype_outerRadius[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10)<<Hype_innerStereo[i][j]/deg
				<<std::setiosflags(std::ios::left)<<std::setw(10)<<Hype_outerStereo[i][j]/deg
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Hype_Length[i][j]/mm;
			dump_general_value(index);
			std::cout<<std::endl;
		}
	}

	for( G4int i = 0; i < TwistedTubsNo; i++ ){
		if ( i == 0 ){
			std::cout<<"=>TwistedTubs info:"<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
				<<std::setiosflags(std::ios::left)<<std::setw(13)<<"TwistedAngle"
				<<std::setiosflags(std::ios::left)<<std::setw(12)<<"EndInnerRad"
				<<std::setiosflags(std::ios::left)<<std::setw(12)<<"EndOuterRad"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"Length"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"dphi";
			dump_general_header();
			std::cout<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(13)<<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(12)<<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(12)<<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"deg";
			dump_general_note();
			std::cout<<std::endl;
		}
		int index = TwistedTubs_GenIndex[i];
		int repNo = RepNo[index];
		for ( G4int j = 0; j < repNo; j++ ){
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
				<<std::setiosflags(std::ios::left)<<std::setw(13)<<TwistedTubs_twistedangle[i][j]/deg
				<<std::setiosflags(std::ios::left)<<std::setw(12)<<TwistedTubs_endinnerrad[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(12)<<TwistedTubs_endouterrad[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<TwistedTubs_Length[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<TwistedTubs_dphi[i][j]/deg;
			dump_general_value(index);
			std::cout<<std::endl;
		}
	}

	for( G4int i = 0; i < ConsNo; i++ ){
		if ( i == 0 ){
			std::cout<<"=>Cons info:"<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"RMin1"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"RMax1"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"RMin2"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"RMax2"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"Length"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"StartAng"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"SpanAng";
			dump_general_header();
			std::cout<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"deg";
			dump_general_note();
			std::cout<<std::endl;
		}
		int index = Cons_GenIndex[i];
		int repNo = RepNo[index];
		for ( G4int j = 0; j < repNo; j++ ){
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Cons_RMin1[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Cons_RMax1[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Cons_RMin2[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Cons_RMax2[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Cons_Length[i][j]/mm
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Cons_StartAng[i][j]/deg
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Cons_SpanAng[i][j]/deg;
			dump_general_value(index);
			std::cout<<std::endl;
		}
	}

	for( G4int i = 0; i < PolyconeNo; i++ ){
		if ( i == 0 ){
			std::cout<<"=>Polycone info:"<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"numZ"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"StartAng"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"SpanAng";
			dump_general_header();
			std::cout<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"deg"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"deg";
			dump_general_note();
			std::cout<<std::endl;
		}
		int index = Polycone_GenIndex[i];
		int repNo = RepNo[index];
		for ( G4int j = 0; j < repNo; j++ ){
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Polycone_numZ[i]
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Polycone_StartAng[i][j]/deg
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<Polycone_SpanAng[i][j]/deg;
			dump_general_value(index);
			std::cout<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"Z"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"RMin"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"RMax"
				<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::endl;
			for (int k = 0; k< Polycone_numZ[i]; k++){
				std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<k
					<<std::setiosflags(std::ios::left)<<std::setw(10) <<Polycone_Z[i][k][j]/mm
					<<std::setiosflags(std::ios::left)<<std::setw(10) <<Polycone_RMin[i][k][j]/mm
					<<std::setiosflags(std::ios::left)<<std::setw(10) <<Polycone_RMax[i][k][j]/mm
					<<std::endl;
			}
		}
	}
	for( G4int i = 0; i < ExtrudedSolidNo; i++ ){
		if ( i == 0 ){
			std::cout<<"=>ExtrudedSolid info:"<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"numZ"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"numP";
			dump_general_header();
			std::cout<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"";
			dump_general_note();
			std::cout<<std::endl;
		}
		int index = ExtrudedSolid_GenIndex[i];
		int repNo = RepNo[index];
		for ( G4int j = 0; j < repNo; j++ ){
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<ExtrudedSolid_numZ[i]
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<ExtrudedSolid_numP[i];
			dump_general_value(index);
			std::cout<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"Z"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"X0"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"Y0"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"Scale"
				<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<""
				<<std::endl;
			for (int k = 0; k< ExtrudedSolid_numZ[i]; k++){
				std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<k
					<<std::setiosflags(std::ios::left)<<std::setw(10) <<ExtrudedSolid_Z[i][k][j]/mm
					<<std::setiosflags(std::ios::left)<<std::setw(10) <<ExtrudedSolid_X0[i][k][j]/mm
					<<std::setiosflags(std::ios::left)<<std::setw(10) <<ExtrudedSolid_Y0[i][k][j]/mm
					<<std::setiosflags(std::ios::left)<<std::setw(10) <<ExtrudedSolid_Scale[i][k][j]
					<<std::endl;
			}
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<"No."
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"X"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"Y"
				<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<""
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
				<<std::endl;
			for (int k = 0; k< ExtrudedSolid_numP[i]; k++){
				std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<k
					<<std::setiosflags(std::ios::left)<<std::setw(10) <<ExtrudedSolid_X[i][k][j]/mm
					<<std::setiosflags(std::ios::left)<<std::setw(10) <<ExtrudedSolid_Y[i][k][j]/mm
					<<std::endl;
			}
		}
	}
	for( G4int i = 0; i < BooleanSolidNo; i++ ){
		if ( i == 0 ){
			std::cout<<"=>BooleanSolid info:"<<BooleanSolidNo<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(10) <<"type."
			         <<std::setiosflags(std::ios::left)<<std::setw(20) <<"solid1"
			         <<std::setiosflags(std::ios::left)<<std::setw(20) <<"solid2"
					 <<std::setiosflags(std::ios::left)<<std::setw(10) <<"PosX"
					 <<std::setiosflags(std::ios::left)<<std::setw(10) <<"PosY"
					 <<std::setiosflags(std::ios::left)<<std::setw(10) <<"PosZ"
					 <<std::setiosflags(std::ios::left)<<std::setw(6) <<"RepNo"
					 <<std::setiosflags(std::ios::left)<<std::setw(10) <<"Ephi"
					 <<std::setiosflags(std::ios::left)<<std::setw(10) <<"Etheta"
					 <<std::setiosflags(std::ios::left)<<std::setw(10) <<"Epsi";
			dump_general_header();
			std::cout<<std::endl;
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(10) <<""
			         <<std::setiosflags(std::ios::left)<<std::setw(20) <<""
			         <<std::setiosflags(std::ios::left)<<std::setw(20) <<""
					 <<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
					 <<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
					 <<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
					 <<std::setiosflags(std::ios::left)<<std::setw(6) <<""
					 <<std::setiosflags(std::ios::left)<<std::setw(10) <<"deg"
					 <<std::setiosflags(std::ios::left)<<std::setw(10) <<"deg"
					 <<std::setiosflags(std::ios::left)<<std::setw(10) <<"deg";
			dump_general_note();
			std::cout<<std::endl;
		} 
		int index = BooleanSolid_GenIndex[i];
		int repNo = RepNo[index];
		for ( G4int j = 0; j < repNo; j++ ){
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(5) <<i
				<<std::setiosflags(std::ios::left)<<std::setw(10) <<BooleanSolid_type[i]
				<<std::setiosflags(std::ios::left)<<std::setw(20)<<BooleanSolid_sol1[i]<<" "
				<<std::setiosflags(std::ios::left)<<std::setw(20)<<BooleanSolid_sol2[i]<<" ";
			if (BooleanSolid_PosType[i]=="Cartesian"){
				std::cout<<std::setiosflags(std::ios::left)<<std::setw(2) <<"C"
						 <<std::setiosflags(std::ios::left)<<std::setw(10) <<BooleanSolid_Pos[i][j].x()/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(10) <<BooleanSolid_Pos[i][j].y()/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(10) <<BooleanSolid_Pos[i][j].z()/mm;
			}
			else if (BooleanSolid_PosType[i]=="Polar"){
				std::cout<<std::setiosflags(std::ios::left)<<std::setw(2) <<"P"
						 <<std::setiosflags(std::ios::left)<<std::setw(10) <<BooleanSolid_Pos[i][j].rho()/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(10) <<BooleanSolid_Pos[i][j].phi()/deg
						 <<std::setiosflags(std::ios::left)<<std::setw(10) <<BooleanSolid_Pos[i][j].z()/mm;
			}
			else if (BooleanSolid_PosType[i]=="Spherical"){
				std::cout<<std::setiosflags(std::ios::left)<<std::setw(2) <<"S"
						 <<std::setiosflags(std::ios::left)<<std::setw(10) <<BooleanSolid_Pos[i][j].mag()/mm
						 <<std::setiosflags(std::ios::left)<<std::setw(10) <<BooleanSolid_Pos[i][j].phi()/deg
						 <<std::setiosflags(std::ios::left)<<std::setw(10) <<BooleanSolid_Pos[i][j].theta()/deg;
			}
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(10) << BooleanSolid_Ephi[i][j]/deg
					 <<std::setiosflags(std::ios::left)<<std::setw(10) << BooleanSolid_Etheta[i][j]/deg
					 <<std::setiosflags(std::ios::left)<<std::setw(10) << BooleanSolid_Epsi[i][j]/deg;
			dump_general_value(index,j);
			std::cout<<std::endl;
		}
	}
}

void SimpleGeometryParameter::dump_general_header(){
	std::cout<<std::setiosflags(std::ios::left)<<std::setw(15)<<"Name"
	         <<std::setiosflags(std::ios::left)<<std::setw(15)<<"MotherName"
	         <<std::setiosflags(std::ios::left)<<std::setw(15)<<"Matierial"
	         <<std::setiosflags(std::ios::left)<<std::setw(15)<<"SDName"
	         <<std::setiosflags(std::ios::left)<<std::setw(10) <<"PosX"
	         <<std::setiosflags(std::ios::left)<<std::setw(10) <<"PosY"
	         <<std::setiosflags(std::ios::left)<<std::setw(10) <<"PosZ"
	         <<std::setiosflags(std::ios::left)<<std::setw(6) <<"RepNo"
	         <<std::setiosflags(std::ios::left)<<std::setw(10) <<"Ephi"
	         <<std::setiosflags(std::ios::left)<<std::setw(10) <<"Etheta"
	         <<std::setiosflags(std::ios::left)<<std::setw(10) <<"Epsi";
}

void SimpleGeometryParameter::dump_general_note(){
	std::cout<<std::setiosflags(std::ios::left)<<std::setw(15)<<""
	         <<std::setiosflags(std::ios::left)<<std::setw(15)<<""
	         <<std::setiosflags(std::ios::left)<<std::setw(15)<<""
	         <<std::setiosflags(std::ios::left)<<std::setw(15)<<""
	         <<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
	         <<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
	         <<std::setiosflags(std::ios::left)<<std::setw(10) <<"mm"
	         <<std::setiosflags(std::ios::left)<<std::setw(6) <<""
	         <<std::setiosflags(std::ios::left)<<std::setw(10) <<"deg"
	         <<std::setiosflags(std::ios::left)<<std::setw(10) <<"deg"
	         <<std::setiosflags(std::ios::left)<<std::setw(10) <<"deg";
}

void SimpleGeometryParameter::dump_general_value(G4int index, G4int j){
	std::cout<<std::setiosflags(std::ios::left)<<std::setw(15)<<Name[index]<<" ";
	if (!SolidBoolean[index]){
		std::cout<<std::setiosflags(std::ios::left)<<std::setw(15)<<MotherName[index]<<" "
				 <<std::setiosflags(std::ios::left)<<std::setw(15)<<Material[index]<<" "
				 <<std::setiosflags(std::ios::left)<<std::setw(15)<<SDName[index]<<" ";
		if (PosType[index]=="Cartesian"){
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(2) <<"C"
					 <<std::setiosflags(std::ios::left)<<std::setw(10) <<Pos[index][j].x()/mm
					 <<std::setiosflags(std::ios::left)<<std::setw(10) <<Pos[index][j].y()/mm
					 <<std::setiosflags(std::ios::left)<<std::setw(10) <<Pos[index][j].z()/mm;
		}
		else if (PosType[index]=="Polar"){
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(2) <<"P"
					 <<std::setiosflags(std::ios::left)<<std::setw(10) <<Pos[index][j].rho()/mm
					 <<std::setiosflags(std::ios::left)<<std::setw(10) <<Pos[index][j].phi()/deg
					 <<std::setiosflags(std::ios::left)<<std::setw(10) <<Pos[index][j].z()/mm;
		}
		else if (PosType[index]=="Spherical"){
			std::cout<<std::setiosflags(std::ios::left)<<std::setw(2) <<"S"
					 <<std::setiosflags(std::ios::left)<<std::setw(10) <<Pos[index][j].mag()/mm
					 <<std::setiosflags(std::ios::left)<<std::setw(10) <<Pos[index][j].phi()/deg
					 <<std::setiosflags(std::ios::left)<<std::setw(10) <<Pos[index][j].theta()/deg;
		}
		std::cout<<std::setiosflags(std::ios::left)<<std::setw(6) <<RepNo[index]
				 <<std::setiosflags(std::ios::left)<<std::setw(10) <<Ephi[index][j]/deg
				 <<std::setiosflags(std::ios::left)<<std::setw(10) <<Etheta[index][j]/deg
				 <<std::setiosflags(std::ios::left)<<std::setw(10) <<Epsi[index][j]/deg;
	}
	else{
		std::cout<<"		[BOOLEANCOMPONENT!]";
	}
}
