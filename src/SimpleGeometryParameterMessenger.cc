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
	set_PosX_cmd->SetGuidance("string, int, double, string");
	set_PosX_cmd->SetGuidance("type, index, val, unit");
	set_PosX_cmd->SetParameterName("PosX",false);
	set_PosX_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "PosY";
	set_PosY_cmd = new G4UIcmdWithAString(CmdName, this);
	set_PosY_cmd->SetGuidance("set PosY.");
	set_PosY_cmd->SetGuidance("string, int, double, string");
	set_PosY_cmd->SetGuidance("type, index, val, unit");
	set_PosY_cmd->SetParameterName("PosY",false);
	set_PosY_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "PosZ";
	set_PosZ_cmd = new G4UIcmdWithAString(CmdName, this);
	set_PosZ_cmd->SetGuidance("set PosZ.");
	set_PosZ_cmd->SetGuidance("string, int, double, string");
	set_PosZ_cmd->SetGuidance("type, index, val, unit");
	set_PosZ_cmd->SetParameterName("PosZ",false);
	set_PosZ_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "name";
	set_name_cmd = new G4UIcmdWithAString(CmdName, this);
	set_name_cmd->SetGuidance("set name.");
	set_name_cmd->SetGuidance("string, int, string");
	set_name_cmd->SetGuidance("type, index, val");
	set_name_cmd->SetParameterName("name",false);
	set_name_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "SDName";
	set_SDName_cmd = new G4UIcmdWithAString(CmdName, this);
	set_SDName_cmd->SetGuidance("set SDName.");
	set_SDName_cmd->SetGuidance("string, int, string");
	set_SDName_cmd->SetGuidance("type, index, val");
	set_SDName_cmd->SetParameterName("SDName",false);
	set_SDName_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "MotherName";
	set_MotherName_cmd = new G4UIcmdWithAString(CmdName, this);
	set_MotherName_cmd->SetGuidance("set MotherName.");
	set_MotherName_cmd->SetGuidance("string, int, string");
	set_MotherName_cmd->SetGuidance("type, index, val");
	set_MotherName_cmd->SetParameterName("MotherName",false);
	set_MotherName_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "material";
	set_material_cmd = new G4UIcmdWithAString(CmdName, this);
	set_material_cmd->SetGuidance("set material.");
	set_material_cmd->SetGuidance("string, int, string");
	set_material_cmd->SetGuidance("type, index, val");
	set_material_cmd->SetParameterName("material",false);
	set_material_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "SRepNo";
	set_SRepNo_cmd = new G4UIcmdWithAString(CmdName, this);
	set_SRepNo_cmd->SetGuidance("set SRepNo.");
	set_SRepNo_cmd->SetGuidance("string, int, int");
	set_SRepNo_cmd->SetGuidance("type, index, val");
	set_SRepNo_cmd->SetParameterName("SRepNo",false);
	set_SRepNo_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "RepNo";
	set_RepNo_cmd = new G4UIcmdWithAString(CmdName, this);
	set_RepNo_cmd->SetGuidance("set RepNo.");
	set_RepNo_cmd->SetGuidance("string, int, int");
	set_RepNo_cmd->SetGuidance("type, index, val");
	set_RepNo_cmd->SetParameterName("RepNo",false);
	set_RepNo_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Space";
	set_Space_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Space_cmd->SetGuidance("set Space.");
	set_Space_cmd->SetGuidance("string, int, double, string");
	set_Space_cmd->SetGuidance("type, index, val, unit");
	set_Space_cmd->SetParameterName("Space",false);
	set_Space_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "DirTheta";
	set_DirTheta_cmd = new G4UIcmdWithAString(CmdName, this);
	set_DirTheta_cmd->SetGuidance("set DirTheta.");
	set_DirTheta_cmd->SetGuidance("string, int, double, string");
	set_DirTheta_cmd->SetGuidance("type, index, val, unit");
	set_DirTheta_cmd->SetParameterName("DirTheta",false);
	set_DirTheta_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "DirPhi";
	set_DirPhi_cmd = new G4UIcmdWithAString(CmdName, this);
	set_DirPhi_cmd->SetGuidance("set DirPhi.");
	set_DirPhi_cmd->SetGuidance("string, int, double, string");
	set_DirPhi_cmd->SetGuidance("type, index, val, unit");
	set_DirPhi_cmd->SetParameterName("DirPhi",false);
	set_DirPhi_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Ephi";
	set_Ephi_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Ephi_cmd->SetGuidance("set Ephi.");
	set_Ephi_cmd->SetGuidance("string, int, double, string");
	set_Ephi_cmd->SetGuidance("type, index, val, unit");
	set_Ephi_cmd->SetParameterName("Ephi",false);
	set_Ephi_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Etheta";
	set_Etheta_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Etheta_cmd->SetGuidance("set Etheta.");
	set_Etheta_cmd->SetGuidance("string, int, double, string");
	set_Etheta_cmd->SetGuidance("type, index, val, unit");
	set_Etheta_cmd->SetParameterName("Etheta",false);
	set_Etheta_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Epsi";
	set_Epsi_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Epsi_cmd->SetGuidance("set Epsi.");
	set_Epsi_cmd->SetGuidance("string, int, double, string");
	set_Epsi_cmd->SetGuidance("type, index, val, unit");
	set_Epsi_cmd->SetParameterName("Epsi",false);
	set_Epsi_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	//Box info
	CmdName = m_SubDirName + "Box_X";
	set_Box_X_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Box_X_cmd->SetGuidance("set Box_X.");
	set_Box_X_cmd->SetGuidance("int, double, string");
	set_Box_X_cmd->SetGuidance("index, val, unit");
	set_Box_X_cmd->SetParameterName("Box_X",false);
	set_Box_X_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Box_Y";
	set_Box_Y_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Box_Y_cmd->SetGuidance("set Box_Y.");
	set_Box_Y_cmd->SetGuidance("int, double, string");
	set_Box_Y_cmd->SetGuidance("index, val, unit");
	set_Box_Y_cmd->SetParameterName("Box_Y",false);
	set_Box_Y_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Box_Z";
	set_Box_Z_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Box_Z_cmd->SetGuidance("set Box_Z.");
	set_Box_Z_cmd->SetGuidance("int, double, string");
	set_Box_Z_cmd->SetGuidance("index, val, unit");
	set_Box_Z_cmd->SetParameterName("Box_Z",false);
	set_Box_Z_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	//Tubs info
	CmdName = m_SubDirName + "Tubs_RMax";
	set_Tubs_RMax_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Tubs_RMax_cmd->SetGuidance("set Tubs_RMax.");
	set_Tubs_RMax_cmd->SetGuidance("int, double, string");
	set_Tubs_RMax_cmd->SetGuidance("index, val, unit");
	set_Tubs_RMax_cmd->SetParameterName("Tubs_RMax",false);
	set_Tubs_RMax_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Tubs_RMin";
	set_Tubs_RMin_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Tubs_RMin_cmd->SetGuidance("set Tubs_RMin.");
	set_Tubs_RMin_cmd->SetGuidance("int, double, string");
	set_Tubs_RMin_cmd->SetGuidance("index, val, unit");
	set_Tubs_RMin_cmd->SetParameterName("Tubs_RMin",false);
	set_Tubs_RMin_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Tubs_Length";
	set_Tubs_Length_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Tubs_Length_cmd->SetGuidance("set Tubs_Length.");
	set_Tubs_Length_cmd->SetGuidance("int, double, string");
	set_Tubs_Length_cmd->SetGuidance("index, val, unit");
	set_Tubs_Length_cmd->SetParameterName("Tubs_Length",false);
	set_Tubs_Length_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Tubs_StartAng";
	set_Tubs_StartAng_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Tubs_StartAng_cmd->SetGuidance("set Tubs_StartAng.");
	set_Tubs_StartAng_cmd->SetGuidance("int, double, string");
	set_Tubs_StartAng_cmd->SetGuidance("index, val, unit");
	set_Tubs_StartAng_cmd->SetParameterName("Tubs_StartAng",false);
	set_Tubs_StartAng_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Tubs_SpanAng";
	set_Tubs_SpanAng_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Tubs_SpanAng_cmd->SetGuidance("set Tubs_SpanAng.");
	set_Tubs_SpanAng_cmd->SetGuidance("int, double, string");
	set_Tubs_SpanAng_cmd->SetGuidance("index, val, unit");
	set_Tubs_SpanAng_cmd->SetParameterName("Tubs_SpanAng",false);
	set_Tubs_SpanAng_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	//Torus info
	CmdName = m_SubDirName + "Torus_RMax";
	set_Torus_RMax_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Torus_RMax_cmd->SetGuidance("set Torus_RMax.");
	set_Torus_RMax_cmd->SetGuidance("int, double, string");
	set_Torus_RMax_cmd->SetGuidance("index, val, unit");
	set_Torus_RMax_cmd->SetParameterName("Torus_RMax",false);
	set_Torus_RMax_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Torus_RMin";
	set_Torus_RMin_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Torus_RMin_cmd->SetGuidance("set Torus_RMin.");
	set_Torus_RMin_cmd->SetGuidance("int, double, string");
	set_Torus_RMin_cmd->SetGuidance("index, val, unit");
	set_Torus_RMin_cmd->SetParameterName("Torus_RMin",false);
	set_Torus_RMin_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Torus_Rtor";
	set_Torus_Rtor_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Torus_Rtor_cmd->SetGuidance("set Torus_Rtor.");
	set_Torus_Rtor_cmd->SetGuidance("int, double, string");
	set_Torus_Rtor_cmd->SetGuidance("index, val, unit");
	set_Torus_Rtor_cmd->SetParameterName("Torus_Rtor",false);
	set_Torus_Rtor_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Torus_StartAng";
	set_Torus_StartAng_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Torus_StartAng_cmd->SetGuidance("set Torus_StartAng.");
	set_Torus_StartAng_cmd->SetGuidance("int, double, string");
	set_Torus_StartAng_cmd->SetGuidance("index, val, unit");
	set_Torus_StartAng_cmd->SetParameterName("Torus_StartAng",false);
	set_Torus_StartAng_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Torus_SpanAng";
	set_Torus_SpanAng_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Torus_SpanAng_cmd->SetGuidance("set Torus_SpanAng.");
	set_Torus_SpanAng_cmd->SetGuidance("int, double, string");
	set_Torus_SpanAng_cmd->SetGuidance("index, val, unit");
	set_Torus_SpanAng_cmd->SetParameterName("Torus_SpanAng",false);
	set_Torus_SpanAng_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	//Sphere info
	CmdName = m_SubDirName + "Sphere_RMax";
	set_Sphere_RMax_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Sphere_RMax_cmd->SetGuidance("set Sphere_RMax.");
	set_Sphere_RMax_cmd->SetGuidance("int, double, string");
	set_Sphere_RMax_cmd->SetGuidance("index, val, unit");
	set_Sphere_RMax_cmd->SetParameterName("Sphere_RMax",false);
	set_Sphere_RMax_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Sphere_RMin";
	set_Sphere_RMin_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Sphere_RMin_cmd->SetGuidance("set Sphere_RMin.");
	set_Sphere_RMin_cmd->SetGuidance("int, double, string");
	set_Sphere_RMin_cmd->SetGuidance("index, val, unit");
	set_Sphere_RMin_cmd->SetParameterName("Sphere_RMin",false);
	set_Sphere_RMin_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Sphere_StartPhi";
	set_Sphere_StartPhi_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Sphere_StartPhi_cmd->SetGuidance("set Sphere_StartPhi.");
	set_Sphere_StartPhi_cmd->SetGuidance("int, double, string");
	set_Sphere_StartPhi_cmd->SetGuidance("index, val, unit");
	set_Sphere_StartPhi_cmd->SetParameterName("Sphere_StartPhi",false);
	set_Sphere_StartPhi_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Sphere_SpanPhi";
	set_Sphere_SpanPhi_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Sphere_SpanPhi_cmd->SetGuidance("set Sphere_SpanPhi.");
	set_Sphere_SpanPhi_cmd->SetGuidance("int, double, string");
	set_Sphere_SpanPhi_cmd->SetGuidance("index, val, unit");
	set_Sphere_SpanPhi_cmd->SetParameterName("Sphere_SpanPhi",false);
	set_Sphere_SpanPhi_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Sphere_StartTheta";
	set_Sphere_StartTheta_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Sphere_StartTheta_cmd->SetGuidance("set Sphere_StartTheta.");
	set_Sphere_StartTheta_cmd->SetGuidance("int, double, string");
	set_Sphere_StartTheta_cmd->SetGuidance("index, val, unit");
	set_Sphere_StartTheta_cmd->SetParameterName("Sphere_StartTheta",false);
	set_Sphere_StartTheta_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Sphere_SpanTheta";
	set_Sphere_SpanTheta_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Sphere_SpanTheta_cmd->SetGuidance("set Sphere_SpanTheta.");
	set_Sphere_SpanTheta_cmd->SetGuidance("int, double, string");
	set_Sphere_SpanTheta_cmd->SetGuidance("index, val, unit");
	set_Sphere_SpanTheta_cmd->SetParameterName("Sphere_SpanTheta",false);
	set_Sphere_SpanTheta_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	//Hype info
	CmdName = m_SubDirName + "Hype_innerRadius";
	set_Hype_innerRadius_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Hype_innerRadius_cmd->SetGuidance("set Hype_innerRadius.");
	set_Hype_innerRadius_cmd->SetGuidance("int, double, string");
	set_Hype_innerRadius_cmd->SetGuidance("index, val, unit");
	set_Hype_innerRadius_cmd->SetParameterName("Hype_innerRadius",false);
	set_Hype_innerRadius_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Hype_outerRadius";
	set_Hype_outerRadius_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Hype_outerRadius_cmd->SetGuidance("set Hype_outerRadius.");
	set_Hype_outerRadius_cmd->SetGuidance("int, double, string");
	set_Hype_outerRadius_cmd->SetGuidance("index, val, unit");
	set_Hype_outerRadius_cmd->SetParameterName("Hype_outerRadius",false);
	set_Hype_outerRadius_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Hype_Length";
	set_Hype_Length_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Hype_Length_cmd->SetGuidance("set Hype_Length.");
	set_Hype_Length_cmd->SetGuidance("int, double, string");
	set_Hype_Length_cmd->SetGuidance("index, val, unit");
	set_Hype_Length_cmd->SetParameterName("Hype_Length",false);
	set_Hype_Length_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Hype_innerStereo";
	set_Hype_innerStereo_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Hype_innerStereo_cmd->SetGuidance("set Hype_innerStereo.");
	set_Hype_innerStereo_cmd->SetGuidance("int, double, string");
	set_Hype_innerStereo_cmd->SetGuidance("index, val, unit");
	set_Hype_innerStereo_cmd->SetParameterName("Hype_innerStereo",false);
	set_Hype_innerStereo_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Hype_outerStereo";
	set_Hype_outerStereo_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Hype_outerStereo_cmd->SetGuidance("set Hype_outerStereo.");
	set_Hype_outerStereo_cmd->SetGuidance("int, double, string");
	set_Hype_outerStereo_cmd->SetGuidance("index, val, unit");
	set_Hype_outerStereo_cmd->SetParameterName("Hype_outerStereo",false);
	set_Hype_outerStereo_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	//TwistedTubs info
	CmdName = m_SubDirName + "TwistedTubs_twistedangle";
	set_TwistedTubs_twistedangle_cmd = new G4UIcmdWithAString(CmdName, this);
	set_TwistedTubs_twistedangle_cmd->SetGuidance("set TwistedTubs_twistedangle.");
	set_TwistedTubs_twistedangle_cmd->SetGuidance("int, double, string");
	set_TwistedTubs_twistedangle_cmd->SetGuidance("index, val, unit");
	set_TwistedTubs_twistedangle_cmd->SetParameterName("TwistedTubs_twistedangle",false);
	set_TwistedTubs_twistedangle_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "TwistedTubs_endinnerrad";
	set_TwistedTubs_endinnerrad_cmd = new G4UIcmdWithAString(CmdName, this);
	set_TwistedTubs_endinnerrad_cmd->SetGuidance("set TwistedTubs_endinnerrad.");
	set_TwistedTubs_endinnerrad_cmd->SetGuidance("int, double, string");
	set_TwistedTubs_endinnerrad_cmd->SetGuidance("index, val, unit");
	set_TwistedTubs_endinnerrad_cmd->SetParameterName("TwistedTubs_endinnerrad",false);
	set_TwistedTubs_endinnerrad_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "TwistedTubs_endouterrad";
	set_TwistedTubs_endouterrad_cmd = new G4UIcmdWithAString(CmdName, this);
	set_TwistedTubs_endouterrad_cmd->SetGuidance("set TwistedTubs_endouterrad.");
	set_TwistedTubs_endouterrad_cmd->SetGuidance("int, double, string");
	set_TwistedTubs_endouterrad_cmd->SetGuidance("index, val, unit");
	set_TwistedTubs_endouterrad_cmd->SetParameterName("TwistedTubs_endouterrad",false);
	set_TwistedTubs_endouterrad_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "TwistedTubs_Length";
	set_TwistedTubs_Length_cmd = new G4UIcmdWithAString(CmdName, this);
	set_TwistedTubs_Length_cmd->SetGuidance("set TwistedTubs_Length.");
	set_TwistedTubs_Length_cmd->SetGuidance("int, double, string");
	set_TwistedTubs_Length_cmd->SetGuidance("index, val, unit");
	set_TwistedTubs_Length_cmd->SetParameterName("TwistedTubs_Length",false);
	set_TwistedTubs_Length_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "TwistedTubs_dphi";
	set_TwistedTubs_dphi_cmd = new G4UIcmdWithAString(CmdName, this);
	set_TwistedTubs_dphi_cmd->SetGuidance("set TwistedTubs_dphi.");
	set_TwistedTubs_dphi_cmd->SetGuidance("int, double, string");
	set_TwistedTubs_dphi_cmd->SetGuidance("index, val, unit");
	set_TwistedTubs_dphi_cmd->SetParameterName("TwistedTubs_dphi",false);
	set_TwistedTubs_dphi_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	//Cons info
	CmdName = m_SubDirName + "Cons_RMax1";
	set_Cons_RMax1_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Cons_RMax1_cmd->SetGuidance("set Cons_RMax1.");
	set_Cons_RMax1_cmd->SetGuidance("int, double, string");
	set_Cons_RMax1_cmd->SetGuidance("index, val, unit");
	set_Cons_RMax1_cmd->SetParameterName("Cons_RMax1",false);
	set_Cons_RMax1_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Cons_RMin1";
	set_Cons_RMin1_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Cons_RMin1_cmd->SetGuidance("set Cons_RMin1.");
	set_Cons_RMin1_cmd->SetGuidance("int, double, string");
	set_Cons_RMin1_cmd->SetGuidance("index, val, unit");
	set_Cons_RMin1_cmd->SetParameterName("Cons_RMin1",false);
	set_Cons_RMin1_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Cons_RMax2";
	set_Cons_RMax2_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Cons_RMax2_cmd->SetGuidance("set Cons_RMax2.");
	set_Cons_RMax2_cmd->SetGuidance("int, double, string");
	set_Cons_RMax2_cmd->SetGuidance("index, val, unit");
	set_Cons_RMax2_cmd->SetParameterName("Cons_RMax2",false);
	set_Cons_RMax2_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Cons_RMin2";
	set_Cons_RMin2_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Cons_RMin2_cmd->SetGuidance("set Cons_RMin2.");
	set_Cons_RMin2_cmd->SetGuidance("int, double, string");
	set_Cons_RMin2_cmd->SetGuidance("index, val, unit");
	set_Cons_RMin2_cmd->SetParameterName("Cons_RMin2",false);
	set_Cons_RMin2_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Cons_Length";
	set_Cons_Length_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Cons_Length_cmd->SetGuidance("set Cons_Length.");
	set_Cons_Length_cmd->SetGuidance("int, double, string");
	set_Cons_Length_cmd->SetGuidance("index, val, unit");
	set_Cons_Length_cmd->SetParameterName("Cons_Length",false);
	set_Cons_Length_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Cons_StartAng";
	set_Cons_StartAng_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Cons_StartAng_cmd->SetGuidance("set Cons_StartAng.");
	set_Cons_StartAng_cmd->SetGuidance("int, double, string");
	set_Cons_StartAng_cmd->SetGuidance("index, val, unit");
	set_Cons_StartAng_cmd->SetParameterName("Cons_StartAng",false);
	set_Cons_StartAng_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Cons_SpanAng";
	set_Cons_SpanAng_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Cons_SpanAng_cmd->SetGuidance("set Cons_SpanAng.");
	set_Cons_SpanAng_cmd->SetGuidance("int, double, string");
	set_Cons_SpanAng_cmd->SetGuidance("index, val, unit");
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
	set_Polycone_RMax_cmd->SetGuidance("int, int, double, string");
	set_Polycone_RMax_cmd->SetGuidance("index, index, val, unit");
	set_Polycone_RMax_cmd->SetParameterName("Polycone_RMax",false);
	set_Polycone_RMax_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Polycone_RMin";
	set_Polycone_RMin_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Polycone_RMin_cmd->SetGuidance("set Polycone_RMin.");
	set_Polycone_RMin_cmd->SetGuidance("int, int, double, string");
	set_Polycone_RMin_cmd->SetGuidance("index, index, val, unit");
	set_Polycone_RMin_cmd->SetParameterName("Polycone_RMin",false);
	set_Polycone_RMin_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Polycone_Z";
	set_Polycone_Z_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Polycone_Z_cmd->SetGuidance("set Polycone_Z.");
	set_Polycone_Z_cmd->SetGuidance("int, int, double, string");
	set_Polycone_Z_cmd->SetGuidance("index, index, val, unit");
	set_Polycone_Z_cmd->SetParameterName("Polycone_Z",false);
	set_Polycone_Z_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Polycone_StartAng";
	set_Polycone_StartAng_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Polycone_StartAng_cmd->SetGuidance("set Polycone_StartAng.");
	set_Polycone_StartAng_cmd->SetGuidance("int, double, string");
	set_Polycone_StartAng_cmd->SetGuidance("index, val, unit");
	set_Polycone_StartAng_cmd->SetParameterName("Polycone_StartAng",false);
	set_Polycone_StartAng_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "Polycone_SpanAng";
	set_Polycone_SpanAng_cmd = new G4UIcmdWithAString(CmdName, this);
	set_Polycone_SpanAng_cmd->SetGuidance("set Polycone_SpanAng.");
	set_Polycone_SpanAng_cmd->SetGuidance("int, double, string");
	set_Polycone_SpanAng_cmd->SetGuidance("index, val, unit");
	set_Polycone_SpanAng_cmd->SetParameterName("Polycone_SpanAng",false);
	set_Polycone_SpanAng_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	// visual settings

	CmdName = m_SubDirName + "vis";
	set_vis_cmd = new G4UIcmdWithAString(CmdName, this);
	set_vis_cmd->SetGuidance("set vis.");
	set_vis_cmd->SetGuidance("int, bool");
	set_vis_cmd->SetGuidance("index, val");
	set_vis_cmd->SetParameterName("vis",false);
	set_vis_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "r";
	set_r_cmd = new G4UIcmdWithAString(CmdName, this);
	set_r_cmd->SetGuidance("set r.");
	set_r_cmd->SetGuidance("int, double");
	set_r_cmd->SetGuidance("index, val");
	set_r_cmd->SetParameterName("r",false);
	set_r_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "g";
	set_g_cmd = new G4UIcmdWithAString(CmdName, this);
	set_g_cmd->SetGuidance("set g.");
	set_g_cmd->SetGuidance("int, double");
	set_g_cmd->SetGuidance("index, val");
	set_g_cmd->SetParameterName("g",false);
	set_g_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	CmdName = m_SubDirName + "b";
	set_b_cmd = new G4UIcmdWithAString(CmdName, this);
	set_b_cmd->SetGuidance("set b.");
	set_b_cmd->SetGuidance("int, double");
	set_b_cmd->SetGuidance("index, val");
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
	delete set_Space_cmd;
	delete set_DirTheta_cmd;
	delete set_DirPhi_cmd;
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
		G4String type; G4int i; G4double val;
		MyString2Anything::get_SIDU(newValue,type,i,val);
		m_GeometryParameter->set_PosX(type,i,val);
	}

	else if( command == set_PosY_cmd ){
		G4String type; G4int i; G4double val;
		MyString2Anything::get_SIDU(newValue,type,i,val);
		m_GeometryParameter->set_PosY(type,i,val);
	}

	else if( command == set_PosZ_cmd ){
		G4String type; G4int i; G4double val;
		MyString2Anything::get_SIDU(newValue,type,i,val);
		m_GeometryParameter->set_PosZ(type,i,val);
	}

	else if( command == set_Space_cmd ){
		G4String type; G4int i; G4double val;
		MyString2Anything::get_SIDU(newValue,type,i,val);
		m_GeometryParameter->set_Space(type,i,val);
	}

	else if( command == set_DirTheta_cmd ){
		G4String type; G4int i; G4double val;
		MyString2Anything::get_SIDU(newValue,type,i,val);
		m_GeometryParameter->set_DirTheta(type,i,val);
	}

	else if( command == set_DirPhi_cmd ){
		G4String type; G4int i; G4double val;
		MyString2Anything::get_SIDU(newValue,type,i,val);
		m_GeometryParameter->set_DirPhi(type,i,val);
	}

	else if( command == set_SRepNo_cmd ){
		G4String type; G4int i; G4int val;
		MyString2Anything::get_SII(newValue,type,i,val);
		m_GeometryParameter->set_SRepNo(type,i,val);
	}

	else if( command == set_RepNo_cmd ){
		G4String type; G4int i; G4int val;
		MyString2Anything::get_SII(newValue,type,i,val);
		m_GeometryParameter->set_RepNo(type,i,val);
	}

	else if( command == set_name_cmd ){
		G4String type; G4int i; G4String val;
		MyString2Anything::get_SIS(newValue,type,i,val);
		m_GeometryParameter->set_name(type,i,val);
	}

	else if( command == set_SDName_cmd ){
		G4String type; G4int i; G4String val;
		MyString2Anything::get_SIS(newValue,type,i,val);
		m_GeometryParameter->set_SDName(type,i,val);
	}

	else if( command == set_MotherName_cmd ){
		G4String type; G4int i; G4String val;
		MyString2Anything::get_SIS(newValue,type,i,val);
		m_GeometryParameter->set_MotherName(type,i,val);
	}

	else if( command == set_material_cmd ){
		G4String type; G4int i; G4String val;
		MyString2Anything::get_SIS(newValue,type,i,val);
		m_GeometryParameter->set_material(type,i,val);
	}

	else if( command == set_Ephi_cmd ){
		G4String type; G4int i; G4double val;
		MyString2Anything::get_SIDU(newValue,type,i,val);
		m_GeometryParameter->set_Ephi(type,i,val);
	}

	else if( command == set_Etheta_cmd ){
		G4String type; G4int i; G4double val;
		MyString2Anything::get_SIDU(newValue,type,i,val);
		m_GeometryParameter->set_Etheta(type,i,val);
	}

	else if( command == set_Epsi_cmd ){
		G4String type; G4int i; G4double val;
		MyString2Anything::get_SIDU(newValue,type,i,val);
		m_GeometryParameter->set_Epsi(type,i,val);
	}

	//Box info
	else if( command == set_Box_X_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Box_X(i,val);
	}

	else if( command == set_Box_Y_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Box_Y(i,val);
	}

	else if( command == set_Box_Z_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Box_Z(i,val);
	}


	//Tubs info
	else if( command == set_Tubs_RMax_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Tubs_RMax(i,val);
	}

	else if( command == set_Tubs_RMin_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Tubs_RMin(i,val);
	}

	else if( command == set_Tubs_Length_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Tubs_Length(i,val);
	}

	else if( command == set_Tubs_StartAng_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Tubs_StartAng(i,val);
	}

	else if( command == set_Tubs_SpanAng_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Tubs_SpanAng(i,val);
	}

	//Torus info
	else if( command == set_Torus_RMax_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Torus_RMax(i,val);
	}

	else if( command == set_Torus_RMin_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Torus_RMin(i,val);
	}

	else if( command == set_Torus_Rtor_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Torus_Rtor(i,val);
	}

	else if( command == set_Torus_StartAng_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Torus_StartAng(i,val);
	}

	else if( command == set_Torus_SpanAng_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Torus_SpanAng(i,val);
	}

	//Sphere info
	else if( command == set_Sphere_RMax_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Sphere_RMax(i,val);
	}

	else if( command == set_Sphere_RMin_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Sphere_RMin(i,val);
	}

	else if( command == set_Sphere_StartPhi_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Sphere_StartPhi(i,val);
	}

	else if( command == set_Sphere_SpanPhi_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Sphere_SpanPhi(i,val);
	}

	else if( command == set_Sphere_StartTheta_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Sphere_StartTheta(i,val);
	}

	else if( command == set_Sphere_SpanTheta_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Sphere_SpanTheta(i,val);
	}

	//Hype info
	else if( command == set_Hype_innerRadius_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Hype_innerRadius(i,val);
	}

	else if( command == set_Hype_outerRadius_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Hype_outerRadius(i,val);
	}

	else if( command == set_Hype_Length_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Hype_Length(i,val);
	}

	else if( command == set_Hype_innerStereo_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Hype_innerStereo(i,val);
	}

	else if( command == set_Hype_outerStereo_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Hype_outerStereo(i,val);
	}

	//TwistedTubs info
	else if( command == set_TwistedTubs_twistedangle_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_TwistedTubs_twistedangle(i,val);
	}

	else if( command == set_TwistedTubs_endinnerrad_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_TwistedTubs_endinnerrad(i,val);
	}

	else if( command == set_TwistedTubs_endouterrad_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_TwistedTubs_endouterrad(i,val);
	}

	else if( command == set_TwistedTubs_Length_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_TwistedTubs_Length(i,val);
	}

	else if( command == set_TwistedTubs_dphi_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_TwistedTubs_dphi(i,val);
	}

	//Cons info
	else if( command == set_Cons_RMax1_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Cons_RMax1(i,val);
	}

	else if( command == set_Cons_RMin1_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Cons_RMin1(i,val);
	}

	else if( command == set_Cons_RMax2_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Cons_RMax2(i,val);
	}

	else if( command == set_Cons_RMin2_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Cons_RMin2(i,val);
	}

	else if( command == set_Cons_Length_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Cons_Length(i,val);
	}

	else if( command == set_Cons_StartAng_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Cons_StartAng(i,val);
	}

	else if( command == set_Cons_SpanAng_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Cons_SpanAng(i,val);
	}

	//Polycone info
	else if( command == set_Polycone_numZ_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_ID(newValue,i,val);
		m_GeometryParameter->set_Polycone_numZ(i,val);
	}

	else if( command == set_Polycone_RMax_cmd ){
		G4String type;
		G4int i; G4int j; G4double val;
		MyString2Anything::get_IIDU(newValue,i,j,val);
		m_GeometryParameter->set_Polycone_RMax(i,j,val);
	}

	else if( command == set_Polycone_RMin_cmd ){
		G4String type;
		G4int i; G4int j; G4double val;
		MyString2Anything::get_IIDU(newValue,i,j,val);
		m_GeometryParameter->set_Polycone_RMin(i,j,val);
	}

	else if( command == set_Polycone_Z_cmd ){
		G4String type;
		G4int i; G4int j; G4double val;
		MyString2Anything::get_IIDU(newValue,i,j,val);
		m_GeometryParameter->set_Polycone_Z(i,j,val);
	}

	else if( command == set_Polycone_StartAng_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Polycone_StartAng(i,val);
	}

	else if( command == set_Polycone_SpanAng_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_IDU(newValue,i,val);
		m_GeometryParameter->set_Polycone_SpanAng(i,val);
	}

	//visual settings
	else if( command == set_vis_cmd ){
		G4String type;
		G4int i; bool val;
		MyString2Anything::get_SIB(newValue,type,i,val);
		m_GeometryParameter->set_vis(type,i,val);
	}

	else if( command == set_r_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_SID(newValue,type,i,val);
		m_GeometryParameter->set_r(type,i,val);
	}

	else if( command == set_g_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_SID(newValue,type,i,val);
		m_GeometryParameter->set_g(type,i,val);
	}

	else if( command == set_b_cmd ){
		G4String type;
		G4int i; G4double val;
		MyString2Anything::get_SID(newValue,type,i,val);
		m_GeometryParameter->set_b(type,i,val);
	}

	else status = 1;

	return status;

}

