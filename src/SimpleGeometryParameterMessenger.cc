//---------------------------------------------------------------------------//
//Description: Messenger class
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Modified: 11, Jan, 2013
//          Support G4Hype, G4TwistedTube
//Comment:
//---------------------------------------------------------------------------//

#include "SimpleGeometryParameterMessenger.hh"

#include "globals.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithABool.hh"

#include "MyString2Anything.hh"
#include "SimpleGeometryParameter.hh"
#include "MyVGeometryParameterMessenger.hh"
#include "MyVGeometryParameter.hh"

	SimpleGeometryParameterMessenger::SimpleGeometryParameterMessenger(MyVGeometryParameter* pGeometryParameter, G4String dirName)
:MyVGeometryParameterMessenger(pGeometryParameter, dirName)
{

	m_GeometryParameter = dynamic_cast<SimpleGeometryParameter*> (pGeometryParameter);
	if (!m_GeometryParameter){
		std::cout<<"In SimpleGeometryParameterMessenger, Cannot convert pointer from MyVGeometryParameter* to SimpleGeometryParameter!!!"<<std::endl;
		G4Exception("SimpleGeometryParameterMessenger::SimpleGeometryParameterMessenger()",
				"InvalidSetup", FatalException,
				"Cannot convert pointer from MyVGeometryParameter* to SimpleGeometryParameter!!!");
	}

	G4String fGeoDirName = get_GeoDirName();
	m_SubDirName = fGeoDirName+"SimpleGeometry/";

	SubDir = new G4UIdirectory(m_SubDirName);
	SubDir->SetGuidance("geometry control");

	G4String CmdName;
	//General info for volume
	CmdName = m_SubDirName + "PosX";
	set_PosX_cmd = new G4UIcmdWithAString(CmdName, this);
	set_PosX_cmd->SetGuidance("set PosX.");
	set_PosX_cmd->SetGuidance("string, double, string, int");
	set_PosX_cmd->SetGuidance("name, val, unit, RepId");
	set_PosX_cmd->SetParameterName("PosX",false);
	set_PosX_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "PosY";
	set_PosY_cmd = new G4UIcmdWithAString(CmdName, this);
	set_PosY_cmd->SetGuidance("set PosY.");
	set_PosY_cmd->SetGuidance("string, double, string, int");
	set_PosY_cmd->SetGuidance("name, val, unit, RepId");
	set_PosY_cmd->SetParameterName("PosY",false);
	set_PosY_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "PosZ";
	set_PosZ_cmd = new G4UIcmdWithAString(CmdName, this);
	set_PosZ_cmd->SetGuidance("set PosZ.");
	set_PosZ_cmd->SetGuidance("string, double, string, int");
	set_PosZ_cmd->SetGuidance("name, val, unit, RepId");
	set_PosZ_cmd->SetParameterName("PosZ",false);
	set_PosZ_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "name";
	set_name_cmd = new G4UIcmdWithAString(CmdName, this);
	set_name_cmd->SetGuidance("set name.");
	set_name_cmd->SetGuidance("string, string");
	set_name_cmd->SetGuidance("name, val");
	set_name_cmd->SetParameterName("name",false);
	set_name_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "SDName";
	set_SDName_cmd = new G4UIcmdWithAString(CmdName, this);
	set_SDName_cmd->SetGuidance("set SDName.");
	set_SDName_cmd->SetGuidance("string, string");
	set_SDName_cmd->SetGuidance("name, val");
	set_SDName_cmd->SetParameterName("SDName",false);
	set_SDName_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "MotherName";
	set_MotherName_cmd = new G4UIcmdWithAString(CmdName, this);
	set_MotherName_cmd->SetGuidance("set MotherName.");
	set_MotherName_cmd->SetGuidance("string, string");
	set_MotherName_cmd->SetGuidance("name, val");
	set_MotherName_cmd->SetParameterName("MotherName",false);
	set_MotherName_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "material";
	set_material_cmd = new G4UIcmdWithAString(CmdName, this);
	set_material_cmd->SetGuidance("set material.");
	set_material_cmd->SetGuidance("string, string");
	set_material_cmd->SetGuidance("name, val");
	set_material_cmd->SetParameterName("material",false);
	set_material_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "SRepNo";
	set_SRepNo_cmd = new G4UIcmdWithAString(CmdName, this);
	set_SRepNo_cmd->SetGuidance("set SRepNo.");
	set_SRepNo_cmd->SetGuidance("string, int");
	set_SRepNo_cmd->SetGuidance("name, val");
	set_SRepNo_cmd->SetParameterName("SRepNo",false);
	set_SRepNo_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "RepNo";
	set_RepNo_cmd = new G4UIcmdWithAString(CmdName, this);
	set_RepNo_cmd->SetGuidance("set RepNo.");
	set_RepNo_cmd->SetGuidance("string, int");
	set_RepNo_cmd->SetGuidance("name, val");
	set_RepNo_cmd->SetParameterName("RepNo",false);
	set_RepNo_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Ephi";
	set_Ephi_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Ephi_cmd->SetGuidance("set Ephi.");
	set_Ephi_cmd->SetGuidance("string, double, string, int");
	set_Ephi_cmd->SetGuidance("name, val, unit, RepId");
	set_Ephi_cmd->SetParameterName("Ephi",false);
	set_Ephi_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Etheta";
	set_Etheta_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Etheta_cmd->SetGuidance("set Etheta.");
	set_Etheta_cmd->SetGuidance("string, double, string, int");
	set_Etheta_cmd->SetGuidance("name, val, unit, RepId");
	set_Etheta_cmd->SetParameterName("Etheta",false);
	set_Etheta_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Epsi";
	set_Epsi_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Epsi_cmd->SetGuidance("set Epsi.");
	set_Epsi_cmd->SetGuidance("string, double, string, int");
	set_Epsi_cmd->SetGuidance("name, val, unit, RepId");
	set_Epsi_cmd->SetParameterName("Epsi",false);
	set_Epsi_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	//Box info
	CmdName = m_SubDirName + "Box_X";
	set_Box_X_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Box_X_cmd->SetGuidance("set Box_X.");
	set_Box_X_cmd->SetGuidance("string, double, string, int");
	set_Box_X_cmd->SetGuidance("name, val, unit, RepId");
	set_Box_X_cmd->SetParameterName("Box_X",false);
	set_Box_X_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Box_Y";
	set_Box_Y_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Box_Y_cmd->SetGuidance("set Box_Y.");
	set_Box_Y_cmd->SetGuidance("string, double, string, int");
	set_Box_Y_cmd->SetGuidance("name, val, unit, RepId");
	set_Box_Y_cmd->SetParameterName("Box_Y",false);
	set_Box_Y_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Box_Z";
	set_Box_Z_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Box_Z_cmd->SetGuidance("set Box_Z.");
	set_Box_Z_cmd->SetGuidance("string, double, string, int");
	set_Box_Z_cmd->SetGuidance("name, val, unit, RepId");
	set_Box_Z_cmd->SetParameterName("Box_Z",false);
	set_Box_Z_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	//Tubs info
	CmdName = m_SubDirName + "Tubs_RMax";
	set_Tubs_RMax_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Tubs_RMax_cmd->SetGuidance("set Tubs_RMax.");
	set_Tubs_RMax_cmd->SetGuidance("string, double, string, int");
	set_Tubs_RMax_cmd->SetGuidance("name, val, unit, RepId");
	set_Tubs_RMax_cmd->SetParameterName("Tubs_RMax",false);
	set_Tubs_RMax_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Tubs_RMin";
	set_Tubs_RMin_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Tubs_RMin_cmd->SetGuidance("set Tubs_RMin.");
	set_Tubs_RMin_cmd->SetGuidance("string, double, string, int");
	set_Tubs_RMin_cmd->SetGuidance("name, val, unit, RepId");
	set_Tubs_RMin_cmd->SetParameterName("Tubs_RMin",false);
	set_Tubs_RMin_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Tubs_Length";
	set_Tubs_Length_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Tubs_Length_cmd->SetGuidance("set Tubs_Length.");
	set_Tubs_Length_cmd->SetGuidance("string, double, string, int");
	set_Tubs_Length_cmd->SetGuidance("name, val, unit, RepId");
	set_Tubs_Length_cmd->SetParameterName("Tubs_Length",false);
	set_Tubs_Length_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Tubs_StartAng";
	set_Tubs_StartAng_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Tubs_StartAng_cmd->SetGuidance("set Tubs_StartAng.");
	set_Tubs_StartAng_cmd->SetGuidance("string, double, string, int");
	set_Tubs_StartAng_cmd->SetGuidance("name, val, unit, RepId");
	set_Tubs_StartAng_cmd->SetParameterName("Tubs_StartAng",false);
	set_Tubs_StartAng_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Tubs_SpanAng";
	set_Tubs_SpanAng_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Tubs_SpanAng_cmd->SetGuidance("set Tubs_SpanAng.");
	set_Tubs_SpanAng_cmd->SetGuidance("string, double, string, int");
	set_Tubs_SpanAng_cmd->SetGuidance("name, val, unit, RepId");
	set_Tubs_SpanAng_cmd->SetParameterName("Tubs_SpanAng",false);
	set_Tubs_SpanAng_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	//Torus info
	CmdName = m_SubDirName + "Torus_RMax";
	set_Torus_RMax_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Torus_RMax_cmd->SetGuidance("set Torus_RMax.");
	set_Torus_RMax_cmd->SetGuidance("string, double, string, int");
	set_Torus_RMax_cmd->SetGuidance("name, val, unit, RepId");
	set_Torus_RMax_cmd->SetParameterName("Torus_RMax",false);
	set_Torus_RMax_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Torus_RMin";
	set_Torus_RMin_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Torus_RMin_cmd->SetGuidance("set Torus_RMin.");
	set_Torus_RMin_cmd->SetGuidance("string, double, string, int");
	set_Torus_RMin_cmd->SetGuidance("name, val, unit, RepId");
	set_Torus_RMin_cmd->SetParameterName("Torus_RMin",false);
	set_Torus_RMin_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Torus_Rtor";
	set_Torus_Rtor_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Torus_Rtor_cmd->SetGuidance("set Torus_Rtor.");
	set_Torus_Rtor_cmd->SetGuidance("string, double, string, int");
	set_Torus_Rtor_cmd->SetGuidance("name, val, unit, RepId");
	set_Torus_Rtor_cmd->SetParameterName("Torus_Rtor",false);
	set_Torus_Rtor_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Torus_StartAng";
	set_Torus_StartAng_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Torus_StartAng_cmd->SetGuidance("set Torus_StartAng.");
	set_Torus_StartAng_cmd->SetGuidance("string, double, string, int");
	set_Torus_StartAng_cmd->SetGuidance("name, val, unit, RepId");
	set_Torus_StartAng_cmd->SetParameterName("Torus_StartAng",false);
	set_Torus_StartAng_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Torus_SpanAng";
	set_Torus_SpanAng_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Torus_SpanAng_cmd->SetGuidance("set Torus_SpanAng.");
	set_Torus_SpanAng_cmd->SetGuidance("string, double, string, int");
	set_Torus_SpanAng_cmd->SetGuidance("name, val, unit, RepId");
	set_Torus_SpanAng_cmd->SetParameterName("Torus_SpanAng",false);
	set_Torus_SpanAng_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	//Sphere info
	CmdName = m_SubDirName + "Sphere_RMax";
	set_Sphere_RMax_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Sphere_RMax_cmd->SetGuidance("set Sphere_RMax.");
	set_Sphere_RMax_cmd->SetGuidance("string, double, string, int");
	set_Sphere_RMax_cmd->SetGuidance("name, val, unit, RepId");
	set_Sphere_RMax_cmd->SetParameterName("Sphere_RMax",false);
	set_Sphere_RMax_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Sphere_RMin";
	set_Sphere_RMin_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Sphere_RMin_cmd->SetGuidance("set Sphere_RMin.");
	set_Sphere_RMin_cmd->SetGuidance("string, double, string, int");
	set_Sphere_RMin_cmd->SetGuidance("name, val, unit, RepId");
	set_Sphere_RMin_cmd->SetParameterName("Sphere_RMin",false);
	set_Sphere_RMin_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Sphere_StartPhi";
	set_Sphere_StartPhi_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Sphere_StartPhi_cmd->SetGuidance("set Sphere_StartPhi.");
	set_Sphere_StartPhi_cmd->SetGuidance("string, double, string, int");
	set_Sphere_StartPhi_cmd->SetGuidance("name, val, unit, RepId");
	set_Sphere_StartPhi_cmd->SetParameterName("Sphere_StartPhi",false);
	set_Sphere_StartPhi_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Sphere_SpanPhi";
	set_Sphere_SpanPhi_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Sphere_SpanPhi_cmd->SetGuidance("set Sphere_SpanPhi.");
	set_Sphere_SpanPhi_cmd->SetGuidance("string, double, string, int");
	set_Sphere_SpanPhi_cmd->SetGuidance("name, val, unit, RepId");
	set_Sphere_SpanPhi_cmd->SetParameterName("Sphere_SpanPhi",false);
	set_Sphere_SpanPhi_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Sphere_StartTheta";
	set_Sphere_StartTheta_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Sphere_StartTheta_cmd->SetGuidance("set Sphere_StartTheta.");
	set_Sphere_StartTheta_cmd->SetGuidance("string, double, string, int");
	set_Sphere_StartTheta_cmd->SetGuidance("name, val, unit, RepId");
	set_Sphere_StartTheta_cmd->SetParameterName("Sphere_StartTheta",false);
	set_Sphere_StartTheta_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Sphere_SpanTheta";
	set_Sphere_SpanTheta_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Sphere_SpanTheta_cmd->SetGuidance("set Sphere_SpanTheta.");
	set_Sphere_SpanTheta_cmd->SetGuidance("string, double, string, int");
	set_Sphere_SpanTheta_cmd->SetGuidance("name, val, unit, RepId");
	set_Sphere_SpanTheta_cmd->SetParameterName("Sphere_SpanTheta",false);
	set_Sphere_SpanTheta_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	//Hype info
	CmdName = m_SubDirName + "Hype_innerRadius";
	set_Hype_innerRadius_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Hype_innerRadius_cmd->SetGuidance("set Hype_innerRadius.");
	set_Hype_innerRadius_cmd->SetGuidance("string, double, string, int");
	set_Hype_innerRadius_cmd->SetGuidance("name, val, unit, RepId");
	set_Hype_innerRadius_cmd->SetParameterName("Hype_innerRadius",false);
	set_Hype_innerRadius_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Hype_outerRadius";
	set_Hype_outerRadius_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Hype_outerRadius_cmd->SetGuidance("set Hype_outerRadius.");
	set_Hype_outerRadius_cmd->SetGuidance("string, double, string, int");
	set_Hype_outerRadius_cmd->SetGuidance("name, val, unit, RepId");
	set_Hype_outerRadius_cmd->SetParameterName("Hype_outerRadius",false);
	set_Hype_outerRadius_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Hype_Length";
	set_Hype_Length_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Hype_Length_cmd->SetGuidance("set Hype_Length.");
	set_Hype_Length_cmd->SetGuidance("string, double, string, int");
	set_Hype_Length_cmd->SetGuidance("name, val, unit, RepId");
	set_Hype_Length_cmd->SetParameterName("Hype_Length",false);
	set_Hype_Length_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Hype_innerStereo";
	set_Hype_innerStereo_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Hype_innerStereo_cmd->SetGuidance("set Hype_innerStereo.");
	set_Hype_innerStereo_cmd->SetGuidance("string, double, string, int");
	set_Hype_innerStereo_cmd->SetGuidance("name, val, unit, RepId");
	set_Hype_innerStereo_cmd->SetParameterName("Hype_innerStereo",false);
	set_Hype_innerStereo_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Hype_outerStereo";
	set_Hype_outerStereo_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Hype_outerStereo_cmd->SetGuidance("set Hype_outerStereo.");
	set_Hype_outerStereo_cmd->SetGuidance("string, double, string, int");
	set_Hype_outerStereo_cmd->SetGuidance("name, val, unit, RepId");
	set_Hype_outerStereo_cmd->SetParameterName("Hype_outerStereo",false);
	set_Hype_outerStereo_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	//TwistedTubs info
	CmdName = m_SubDirName + "TwistedTubs_twistedangle";
	set_TwistedTubs_twistedangle_cmd = new G4UIcmdWithAString(CmdName, this);
	set_TwistedTubs_twistedangle_cmd->SetGuidance("set TwistedTubs_twistedangle.");
	set_TwistedTubs_twistedangle_cmd->SetGuidance("string, double, string, int");
	set_TwistedTubs_twistedangle_cmd->SetGuidance("name, val, unit, RepId");
	set_TwistedTubs_twistedangle_cmd->SetParameterName("TwistedTubs_twistedangle",false);
	set_TwistedTubs_twistedangle_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "TwistedTubs_endinnerrad";
	set_TwistedTubs_endinnerrad_cmd = new G4UIcmdWithAString(CmdName, this);
	set_TwistedTubs_endinnerrad_cmd->SetGuidance("set TwistedTubs_endinnerrad.");
	set_TwistedTubs_endinnerrad_cmd->SetGuidance("string, double, string, int");
	set_TwistedTubs_endinnerrad_cmd->SetGuidance("name, val, unit, RepId");
	set_TwistedTubs_endinnerrad_cmd->SetParameterName("TwistedTubs_endinnerrad",false);
	set_TwistedTubs_endinnerrad_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "TwistedTubs_endouterrad";
	set_TwistedTubs_endouterrad_cmd = new G4UIcmdWithAString(CmdName, this);
	set_TwistedTubs_endouterrad_cmd->SetGuidance("set TwistedTubs_endouterrad.");
	set_TwistedTubs_endouterrad_cmd->SetGuidance("string, double, string, int");
	set_TwistedTubs_endouterrad_cmd->SetGuidance("name, val, unit, RepId");
	set_TwistedTubs_endouterrad_cmd->SetParameterName("TwistedTubs_endouterrad",false);
	set_TwistedTubs_endouterrad_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "TwistedTubs_Length";
	set_TwistedTubs_Length_cmd = new G4UIcmdWithAString(CmdName, this);
	set_TwistedTubs_Length_cmd->SetGuidance("set TwistedTubs_Length.");
	set_TwistedTubs_Length_cmd->SetGuidance("string, double, string, int");
	set_TwistedTubs_Length_cmd->SetGuidance("name, val, unit, RepId");
	set_TwistedTubs_Length_cmd->SetParameterName("TwistedTubs_Length",false);
	set_TwistedTubs_Length_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "TwistedTubs_dphi";
	set_TwistedTubs_dphi_cmd = new G4UIcmdWithAString(CmdName, this);
	set_TwistedTubs_dphi_cmd->SetGuidance("set TwistedTubs_dphi.");
	set_TwistedTubs_dphi_cmd->SetGuidance("string, double, string, int");
	set_TwistedTubs_dphi_cmd->SetGuidance("name, val, unit, RepId");
	set_TwistedTubs_dphi_cmd->SetParameterName("TwistedTubs_dphi",false);
	set_TwistedTubs_dphi_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	//Cons info
	CmdName = m_SubDirName + "Cons_RMax1";
	set_Cons_RMax1_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Cons_RMax1_cmd->SetGuidance("set Cons_RMax1.");
	set_Cons_RMax1_cmd->SetGuidance("string, double, string, int");
	set_Cons_RMax1_cmd->SetGuidance("name, val, unit, RepId");
	set_Cons_RMax1_cmd->SetParameterName("Cons_RMax1",false);
	set_Cons_RMax1_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Cons_RMin1";
	set_Cons_RMin1_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Cons_RMin1_cmd->SetGuidance("set Cons_RMin1.");
	set_Cons_RMin1_cmd->SetGuidance("string, double, string, int");
	set_Cons_RMin1_cmd->SetGuidance("name, val, unit, RepId");
	set_Cons_RMin1_cmd->SetParameterName("Cons_RMin1",false);
	set_Cons_RMin1_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Cons_RMax2";
	set_Cons_RMax2_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Cons_RMax2_cmd->SetGuidance("set Cons_RMax2.");
	set_Cons_RMax2_cmd->SetGuidance("string, double, string, int");
	set_Cons_RMax2_cmd->SetGuidance("name, val, unit, RepId");
	set_Cons_RMax2_cmd->SetParameterName("Cons_RMax2",false);
	set_Cons_RMax2_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Cons_RMin2";
	set_Cons_RMin2_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Cons_RMin2_cmd->SetGuidance("set Cons_RMin2.");
	set_Cons_RMin2_cmd->SetGuidance("string, double, string, int");
	set_Cons_RMin2_cmd->SetGuidance("name, val, unit, RepId");
	set_Cons_RMin2_cmd->SetParameterName("Cons_RMin2",false);
	set_Cons_RMin2_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Cons_Length";
	set_Cons_Length_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Cons_Length_cmd->SetGuidance("set Cons_Length.");
	set_Cons_Length_cmd->SetGuidance("string, double, string, int");
	set_Cons_Length_cmd->SetGuidance("name, val, unit, RepId");
	set_Cons_Length_cmd->SetParameterName("Cons_Length",false);
	set_Cons_Length_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Cons_StartAng";
	set_Cons_StartAng_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Cons_StartAng_cmd->SetGuidance("set Cons_StartAng.");
	set_Cons_StartAng_cmd->SetGuidance("string, double, string, int");
	set_Cons_StartAng_cmd->SetGuidance("name, val, unit, RepId");
	set_Cons_StartAng_cmd->SetParameterName("Cons_StartAng",false);
	set_Cons_StartAng_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Cons_SpanAng";
	set_Cons_SpanAng_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Cons_SpanAng_cmd->SetGuidance("set Cons_SpanAng.");
	set_Cons_SpanAng_cmd->SetGuidance("string, double, string, int");
	set_Cons_SpanAng_cmd->SetGuidance("name, val, unit, RepId");
	set_Cons_SpanAng_cmd->SetParameterName("Cons_SpanAng",false);
	set_Cons_SpanAng_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	//Polycone info
	CmdName = m_SubDirName + "Polycone_numZ";
	set_Polycone_numZ_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Polycone_numZ_cmd->SetGuidance("set Polycone_numZ.");
	set_Polycone_numZ_cmd->SetGuidance("int, int");
	set_Polycone_numZ_cmd->SetGuidance("index, val");
	set_Polycone_numZ_cmd->SetParameterName("Polycone_numZ",false);
	set_Polycone_numZ_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Polycone_RMax";
	set_Polycone_RMax_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Polycone_RMax_cmd->SetGuidance("set Polycone_RMax.");
	set_Polycone_RMax_cmd->SetGuidance("int, string, double, string");
	set_Polycone_RMax_cmd->SetGuidance("index, name, val, unit");
	set_Polycone_RMax_cmd->SetParameterName("Polycone_RMax",false);
	set_Polycone_RMax_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Polycone_RMin";
	set_Polycone_RMin_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Polycone_RMin_cmd->SetGuidance("set Polycone_RMin.");
	set_Polycone_RMin_cmd->SetGuidance("int, string, double, string");
	set_Polycone_RMin_cmd->SetGuidance("index, name, val, unit");
	set_Polycone_RMin_cmd->SetParameterName("Polycone_RMin",false);
	set_Polycone_RMin_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Polycone_Z";
	set_Polycone_Z_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Polycone_Z_cmd->SetGuidance("set Polycone_Z.");
	set_Polycone_Z_cmd->SetGuidance("int, string, double, string");
	set_Polycone_Z_cmd->SetGuidance("index, name, val, unit");
	set_Polycone_Z_cmd->SetParameterName("Polycone_Z",false);
	set_Polycone_Z_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Polycone_StartAng";
	set_Polycone_StartAng_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Polycone_StartAng_cmd->SetGuidance("set Polycone_StartAng.");
	set_Polycone_StartAng_cmd->SetGuidance("string, double, string, int");
	set_Polycone_StartAng_cmd->SetGuidance("name, val, unit, RepId");
	set_Polycone_StartAng_cmd->SetParameterName("Polycone_StartAng",false);
	set_Polycone_StartAng_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Polycone_SpanAng";
	set_Polycone_SpanAng_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Polycone_SpanAng_cmd->SetGuidance("set Polycone_SpanAng.");
	set_Polycone_SpanAng_cmd->SetGuidance("string, double, string, int");
	set_Polycone_SpanAng_cmd->SetGuidance("name, val, unit, RepId");
	set_Polycone_SpanAng_cmd->SetParameterName("Polycone_SpanAng",false);
	set_Polycone_SpanAng_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	// visual settings

	CmdName = m_SubDirName + "vis";
	set_vis_cmd = new G4UIcmdWithAString(CmdName, this);
	set_vis_cmd->SetGuidance("set vis.");
	set_vis_cmd->SetGuidance("string, bool");
	set_vis_cmd->SetGuidance("name, val");
	set_vis_cmd->SetParameterName("vis",false);
	set_vis_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "r";
	set_r_cmd = new G4UIcmdWithAString(CmdName, this);
	set_r_cmd->SetGuidance("set r.");
	set_r_cmd->SetGuidance("string, double");
	set_r_cmd->SetGuidance("name, val");
	set_r_cmd->SetParameterName("r",false);
	set_r_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "g";
	set_g_cmd = new G4UIcmdWithAString(CmdName, this);
	set_g_cmd->SetGuidance("set g.");
	set_g_cmd->SetGuidance("string, double");
	set_g_cmd->SetGuidance("name, val");
	set_g_cmd->SetParameterName("g",false);
	set_g_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "b";
	set_b_cmd = new G4UIcmdWithAString(CmdName, this);
	set_b_cmd->SetGuidance("set b.");
	set_b_cmd->SetGuidance("string, double");
	set_b_cmd->SetGuidance("name, val");
	set_b_cmd->SetParameterName("b",false);
	set_b_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

SimpleGeometryParameterMessenger::~SimpleGeometryParameterMessenger()
{
	delete SubDir;

	delete set_PosX_cmd;
	delete set_PosY_cmd;
	delete set_PosZ_cmd;
	delete set_name_cmd;
	delete set_SDName_cmd;
	delete set_MotherName_cmd;
	delete set_material_cmd;
	delete set_SRepNo_cmd;
	delete set_RepNo_cmd;
	delete set_Ephi_cmd;
	delete set_Etheta_cmd;
	delete set_Epsi_cmd;

	//Box info
	delete set_Box_X_cmd;
	delete set_Box_Y_cmd;
	delete set_Box_Z_cmd;

	//Tubs info
	delete set_Tubs_RMax_cmd;
	delete set_Tubs_RMin_cmd;
	delete set_Tubs_Length_cmd;
	delete set_Tubs_StartAng_cmd;
	delete set_Tubs_SpanAng_cmd;

	//Torus info
	delete set_Torus_RMax_cmd;
	delete set_Torus_RMin_cmd;
	delete set_Torus_Rtor_cmd;
	delete set_Torus_StartAng_cmd;
	delete set_Torus_SpanAng_cmd;

	//Sphere info
	delete set_Sphere_RMax_cmd;
	delete set_Sphere_RMin_cmd;
	delete set_Sphere_StartPhi_cmd;
	delete set_Sphere_SpanPhi_cmd;
	delete set_Sphere_StartTheta_cmd;
	delete set_Sphere_SpanTheta_cmd;

	//Hype info
	delete set_Hype_innerRadius_cmd;
	delete set_Hype_outerRadius_cmd;
	delete set_Hype_Length_cmd;
	delete set_Hype_innerStereo_cmd;
	delete set_Hype_outerStereo_cmd;

	//TwistedTubs info
	delete set_TwistedTubs_endinnerrad_cmd;
	delete set_TwistedTubs_endouterrad_cmd;
	delete set_TwistedTubs_Length_cmd;
	delete set_TwistedTubs_twistedangle_cmd;
	delete set_TwistedTubs_dphi_cmd;

	//Cons info
	delete set_Cons_RMax1_cmd;
	delete set_Cons_RMin1_cmd;
	delete set_Cons_RMax2_cmd;
	delete set_Cons_RMin2_cmd;
	delete set_Cons_Length_cmd;
	delete set_Cons_StartAng_cmd;
	delete set_Cons_SpanAng_cmd;

	//Polycone info
	delete set_Polycone_RMax_cmd;
	delete set_Polycone_RMin_cmd;
	delete set_Polycone_numZ_cmd;
	delete set_Polycone_Z_cmd;
	delete set_Polycone_StartAng_cmd;
	delete set_Polycone_SpanAng_cmd;

	//other settings
	delete set_vis_cmd;
	delete set_r_cmd;
	delete set_g_cmd;
	delete set_b_cmd;
}

void SimpleGeometryParameterMessenger::SetNewValue(G4UIcommand* command,G4String newValue){
	MySetNewValue(command, newValue);
}

//----------------------------Special functions-----------------------------------------

int SimpleGeometryParameterMessenger::MySetNewValue(G4UIcommand* command,G4String newValue)
{

	int status = 0; //0 means found the value
	status = MyVGeometryParameterMessenger::MySetNewValue(command, newValue); //check Parent Class first
	if ( !status ) return status;

	//General info for volume
	if( command == set_PosX_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_PosX(name,val,iRep);
	}

	else if( command == set_PosY_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_PosY(name,val,iRep);
	}

	else if( command == set_PosZ_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_PosZ(name,val,iRep);
	}

	else if( command == set_SRepNo_cmd ){
		G4String name; G4int val;
		MyString2Anything::get_SI(newValue,name,val);
		m_GeometryParameter->set_SRepNo(name,val);
	}

	else if( command == set_RepNo_cmd ){
		G4String name; G4int val;
		MyString2Anything::get_SI(newValue,name,val);
		m_GeometryParameter->set_RepNo(name,val);
	}

	else if( command == set_name_cmd ){
		G4String name; G4String val;
		MyString2Anything::get_SS(newValue,name,val);
		m_GeometryParameter->set_name(name,val);
	}

	else if( command == set_SDName_cmd ){
		G4String name; G4String val;
		MyString2Anything::get_SS(newValue,name,val);
		m_GeometryParameter->set_SDName(name,val);
	}

	else if( command == set_MotherName_cmd ){
		G4String name; G4String val;
		MyString2Anything::get_SS(newValue,name,val);
		m_GeometryParameter->set_MotherName(name,val);
	}

	else if( command == set_material_cmd ){
		G4String name; G4String val;
		MyString2Anything::get_SS(newValue,name,val);
		m_GeometryParameter->set_material(name,val);
	}

	else if( command == set_Ephi_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Ephi(name,val,iRep);
	}

	else if( command == set_Etheta_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Etheta(name,val,iRep);
	}

	else if( command == set_Epsi_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Epsi(name,val,iRep);
	}

	//Box info
	else if( command == set_Box_X_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Box_X(name,val,iRep);
	}

	else if( command == set_Box_Y_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Box_Y(name,val,iRep);
	}

	else if( command == set_Box_Z_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Box_Z(name,val,iRep);
	}


	//Tubs info
	else if( command == set_Tubs_RMax_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Tubs_RMax(name,val,iRep);
	}

	else if( command == set_Tubs_RMin_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Tubs_RMin(name,val,iRep);
	}

	else if( command == set_Tubs_Length_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Tubs_Length(name,val,iRep);
	}

	else if( command == set_Tubs_StartAng_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Tubs_StartAng(name,val,iRep);
	}

	else if( command == set_Tubs_SpanAng_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Tubs_SpanAng(name,val,iRep);
	}

	//Torus info
	else if( command == set_Torus_RMax_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Torus_RMax(name,val,iRep);
	}

	else if( command == set_Torus_RMin_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Torus_RMin(name,val,iRep);
	}

	else if( command == set_Torus_Rtor_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Torus_Rtor(name,val,iRep);
	}

	else if( command == set_Torus_StartAng_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Torus_StartAng(name,val,iRep);
	}

	else if( command == set_Torus_SpanAng_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Torus_SpanAng(name,val,iRep);
	}

	//Sphere info
	else if( command == set_Sphere_RMax_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Sphere_RMax(name,val,iRep);
	}

	else if( command == set_Sphere_RMin_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Sphere_RMin(name,val,iRep);
	}

	else if( command == set_Sphere_StartPhi_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Sphere_StartPhi(name,val,iRep);
	}

	else if( command == set_Sphere_SpanPhi_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Sphere_SpanPhi(name,val,iRep);
	}

	else if( command == set_Sphere_StartTheta_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Sphere_StartTheta(name,val,iRep);
	}

	else if( command == set_Sphere_SpanTheta_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Sphere_SpanTheta(name,val,iRep);
	}

	//Hype info
	else if( command == set_Hype_innerRadius_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Hype_innerRadius(name,val,iRep);
	}

	else if( command == set_Hype_outerRadius_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Hype_outerRadius(name,val,iRep);
	}

	else if( command == set_Hype_Length_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Hype_Length(name,val,iRep);
	}

	else if( command == set_Hype_innerStereo_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Hype_innerStereo(name,val,iRep);
	}

	else if( command == set_Hype_outerStereo_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Hype_outerStereo(name,val,iRep);
	}

	//TwistedTubs info
	else if( command == set_TwistedTubs_twistedangle_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_TwistedTubs_twistedangle(name,val,iRep);
	}

	else if( command == set_TwistedTubs_endinnerrad_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_TwistedTubs_endinnerrad(name,val,iRep);
	}

	else if( command == set_TwistedTubs_endouterrad_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_TwistedTubs_endouterrad(name,val,iRep);
	}

	else if( command == set_TwistedTubs_Length_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_TwistedTubs_Length(name,val,iRep);
	}

	else if( command == set_TwistedTubs_dphi_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_TwistedTubs_dphi(name,val,iRep);
	}

	//Cons info
	else if( command == set_Cons_RMax1_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Cons_RMax1(name,val,iRep);
	}

	else if( command == set_Cons_RMin1_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Cons_RMin1(name,val,iRep);
	}

	else if( command == set_Cons_RMax2_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Cons_RMax2(name,val,iRep);
	}

	else if( command == set_Cons_RMin2_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Cons_RMin2(name,val,iRep);
	}

	else if( command == set_Cons_Length_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Cons_Length(name,val,iRep);
	}

	else if( command == set_Cons_StartAng_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Cons_StartAng(name,val,iRep);
	}

	else if( command == set_Cons_SpanAng_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Cons_SpanAng(name,val,iRep);
	}

	//Polycone info
	else if( command == set_Polycone_numZ_cmd ){
		G4String name; G4int val; G4int iRep = 0;
		MyString2Anything::get_SII(newValue,name,val,iRep);
		m_GeometryParameter->set_Polycone_numZ(name,val,iRep);
	}

	else if( command == set_Polycone_RMax_cmd ){
		G4String name; G4int iCone; G4double val; G4int iRep = 0;
		MyString2Anything::get_SIDUI(newValue,name,iCone,val,iRep);
		m_GeometryParameter->set_Polycone_RMax(name,iCone,val,iRep);
	}

	else if( command == set_Polycone_RMin_cmd ){
		G4String name; G4int iCone; G4double val; G4int iRep = 0;
		MyString2Anything::get_SIDUI(newValue,name,iCone,val,iRep);
		m_GeometryParameter->set_Polycone_RMin(name,iCone,val,iRep);
	}

	else if( command == set_Polycone_Z_cmd ){
		G4String name; G4int iCone; G4double val; G4int iRep = 0;
		MyString2Anything::get_SIDUI(newValue,name,iCone,val,iRep);
		m_GeometryParameter->set_Polycone_Z(name,iCone,val,iRep);
	}

	else if( command == set_Polycone_StartAng_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Polycone_StartAng(name,val,iRep);
	}

	else if( command == set_Polycone_SpanAng_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SDUI(newValue,name,val,iRep);
		m_GeometryParameter->set_Polycone_SpanAng(name,val,iRep);
	}

	//visual settings
	else if( command == set_vis_cmd ){
		G4String name; bool val;
		MyString2Anything::get_SB(newValue,name,val);
		m_GeometryParameter->set_vis(name,val);
	}

	else if( command == set_r_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SD(newValue,name,val);
		m_GeometryParameter->set_r(name,val);
	}

	else if( command == set_g_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SD(newValue,name,val);
		m_GeometryParameter->set_g(name,val);
	}

	else if( command == set_b_cmd ){
		G4String name; G4double val; G4int iRep = 0;
		MyString2Anything::get_SD(newValue,name,val);
		m_GeometryParameter->set_b(name,val);
	}

	else status = 1;

	return status;

}

