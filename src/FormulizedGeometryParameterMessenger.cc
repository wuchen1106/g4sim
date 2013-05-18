//---------------------------------------------------------------------------//
//Description: Messenger class
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "FormulizedGeometryParameterMessenger.hh"

#include "globals.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithoutParameter.hh"

#include "MyString2Anything.hh"
#include "FormulizedGeometryParameter.hh"
#include "MyVGeometryParameter.hh"

FormulizedGeometryParameterMessenger::FormulizedGeometryParameterMessenger(MyVGeometryParameter* pGeometryParameter, G4String dirName)
:SimpleGeometryParameterMessenger(pGeometryParameter, dirName)
{

	m_GeometryParameter = dynamic_cast<FormulizedGeometryParameter*> (pGeometryParameter);
	if (!m_GeometryParameter){
		std::cout<<"In FormulizedGeometryParameterMessenger, Cannot convert pointer from MyVGeometryParameter* to FormulizedGeometryParameter!!!"<<std::endl;
    G4Exception("FormulizedGeometryParameterMessenger::FormulizedGeometryParameterMessenger()",
        "InvalidSetup", FatalException,
        "Cannot convert pointer from MyVGeometryParameter* to FormulizedGeometryParameter!!!");
	}

	G4String fGeoDirName = get_GeoDirName();
	m_SubDirName = fGeoDirName+"FormulizedGeometry/";

	SubDir = new G4UIdirectory(m_SubDirName);
	SubDir->SetGuidance("geometry control");

	G4String CmdName;
	//calculate formulas
	CmdName = m_SubDirName + "Calculate";
	Calculate_cmd = new G4UIcmdWithoutParameter(CmdName, this);
  Calculate_cmd->SetGuidance("Calculate formulas.");
  Calculate_cmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
  Calculate_cmd->SetGuidance("if you changed formulas.");
  Calculate_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	//General info for volume
  CmdName = m_SubDirName + "fPosX";
  set_PosX_cmd = new G4UIcmdWithAString(CmdName, this);
  set_PosX_cmd->SetGuidance("set formula for PosX.");
  set_PosX_cmd->SetGuidance("string, int, string");
  set_PosX_cmd->SetGuidance("type, index, formula");
  set_PosX_cmd->SetParameterName("fPosX",false);
  set_PosX_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "fPosY";
  set_PosY_cmd = new G4UIcmdWithAString(CmdName, this);
  set_PosY_cmd->SetGuidance("set formula for PosY.");
  set_PosY_cmd->SetGuidance("string, int, string");
  set_PosY_cmd->SetGuidance("type, index, formula");
  set_PosY_cmd->SetParameterName("fPosY",false);
  set_PosY_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "fPosZ";
  set_PosZ_cmd = new G4UIcmdWithAString(CmdName, this);
  set_PosZ_cmd->SetGuidance("set formula for PosZ.");
  set_PosZ_cmd->SetGuidance("string, int, string");
  set_PosZ_cmd->SetGuidance("type, index, formula");
  set_PosZ_cmd->SetParameterName("fPosZ",false);
  set_PosZ_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "fEphi";
  set_Ephi_cmd = new G4UIcmdWithAString(CmdName, this);
  set_Ephi_cmd->SetGuidance("set formula for Ephi.");
  set_Ephi_cmd->SetGuidance("string, int, string");
  set_Ephi_cmd->SetGuidance("type, index, formula");
  set_Ephi_cmd->SetParameterName("fEphi",false);
  set_Ephi_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "fEtheta";
  set_Etheta_cmd = new G4UIcmdWithAString(CmdName, this);
  set_Etheta_cmd->SetGuidance("set formula for Etheta.");
  set_Etheta_cmd->SetGuidance("string, int, string");
  set_Etheta_cmd->SetGuidance("type, index, formula");
  set_Etheta_cmd->SetParameterName("fEtheta",false);
  set_Etheta_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "fEpsi";
  set_Epsi_cmd = new G4UIcmdWithAString(CmdName, this);
  set_Epsi_cmd->SetGuidance("set formula for Epsi.");
  set_Epsi_cmd->SetGuidance("string, int, string");
  set_Epsi_cmd->SetGuidance("type, index, formula");
  set_Epsi_cmd->SetParameterName("fEpsi",false);
  set_Epsi_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "PosX";
  set_PosX_cmd = new G4UIcmdWithAString(CmdName, this);
  set_PosX_cmd->SetGuidance("set PosX.");
  set_PosX_cmd->SetGuidance("string, int, int, double, string");
  set_PosX_cmd->SetGuidance("type, VolId, RepId, val, unit");
  set_PosX_cmd->SetParameterName("PosX",false);
  set_PosX_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "PosY";
  set_PosY_cmd = new G4UIcmdWithAString(CmdName, this);
  set_PosY_cmd->SetGuidance("set PosY.");
  set_PosY_cmd->SetGuidance("string, int, int, double, string");
  set_PosY_cmd->SetGuidance("type, VolId, RepId, val, unit");
  set_PosY_cmd->SetParameterName("PosY",false);
  set_PosY_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "PosZ";
  set_PosZ_cmd = new G4UIcmdWithAString(CmdName, this);
  set_PosZ_cmd->SetGuidance("set PosZ.");
  set_PosZ_cmd->SetGuidance("string, int, int, double, string");
  set_PosZ_cmd->SetGuidance("type, VolId, RepId, val, unit");
  set_PosZ_cmd->SetParameterName("PosZ",false);
  set_PosZ_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "Ephi";
  set_Ephi_cmd = new G4UIcmdWithAString(CmdName, this);
  set_Ephi_cmd->SetGuidance("set Ephi.");
  set_Ephi_cmd->SetGuidance("string, int, int, double, string");
  set_Ephi_cmd->SetGuidance("type, VolId, RepId, val, unit");
  set_Ephi_cmd->SetParameterName("Ephi",false);
  set_Ephi_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "Etheta";
  set_Etheta_cmd = new G4UIcmdWithAString(CmdName, this);
  set_Etheta_cmd->SetGuidance("set Etheta.");
  set_Etheta_cmd->SetGuidance("string, int, int, double, string");
  set_Etheta_cmd->SetGuidance("type, VolId, RepId, val, unit");
  set_Etheta_cmd->SetParameterName("Etheta",false);
  set_Etheta_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "Epsi";
  set_Epsi_cmd = new G4UIcmdWithAString(CmdName, this);
  set_Epsi_cmd->SetGuidance("set Epsi.");
  set_Epsi_cmd->SetGuidance("string, int, int, double, string");
  set_Epsi_cmd->SetGuidance("type, VolId, RepId, val, unit");
  set_Epsi_cmd->SetParameterName("Epsi",false);
  set_Epsi_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

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

		//Box info
  CmdName = m_SubDirName + "fBox_X";
  set_fBox_X_cmd = new G4UIcmdWithAString(CmdName, this);
  set_fBox_X_cmd->SetGuidance("set Box_X.");
  set_fBox_X_cmd->SetGuidance("int, string");
  set_fBox_X_cmd->SetGuidance("index, formula");
  set_fBox_X_cmd->SetParameterName("fBox_X",false);
  set_fBox_X_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "fBox_Y";
  set_fBox_Y_cmd = new G4UIcmdWithAString(CmdName, this);
  set_fBox_Y_cmd->SetGuidance("set Box_Y.");
  set_fBox_Y_cmd->SetGuidance("int, string");
  set_fBox_Y_cmd->SetGuidance("index, formula");
  set_fBox_Y_cmd->SetParameterName("fBox_Y",false);
  set_fBox_Y_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "fBox_Z";
  set_fBox_Z_cmd = new G4UIcmdWithAString(CmdName, this);
  set_fBox_Z_cmd->SetGuidance("set Box_Z.");
  set_fBox_Z_cmd->SetGuidance("int, string");
  set_fBox_Z_cmd->SetGuidance("index, formula");
  set_fBox_Z_cmd->SetParameterName("fBox_Z",false);
  set_fBox_Z_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "Box_X";
  set_Box_X_cmd = new G4UIcmdWithAString(CmdName, this);
  set_Box_X_cmd->SetGuidance("set Box_X.");
  set_Box_X_cmd->SetGuidance("int, int, double, string");
  set_Box_X_cmd->SetGuidance("VolId, RepId, val, unit");
  set_Box_X_cmd->SetParameterName("Box_X",false);
  set_Box_X_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "Box_Y";
  set_Box_Y_cmd = new G4UIcmdWithAString(CmdName, this);
  set_Box_Y_cmd->SetGuidance("set Box_Y.");
  set_Box_Y_cmd->SetGuidance("int, int, double, string");
  set_Box_Y_cmd->SetGuidance("VolId, RepId, val, unit");
  set_Box_Y_cmd->SetParameterName("Box_Y",false);
  set_Box_Y_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "Box_Z";
  set_Box_Z_cmd = new G4UIcmdWithAString(CmdName, this);
  set_Box_Z_cmd->SetGuidance("set Box_Z.");
  set_Box_Z_cmd->SetGuidance("int, int, double, string");
  set_Box_Z_cmd->SetGuidance("VolId, RepId, val, unit");
  set_Box_Z_cmd->SetParameterName("Box_Z",false);
  set_Box_Z_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

		//Tubs info
  CmdName = m_SubDirName + "fTubs_RMax";
  set_fTubs_RMax_cmd = new G4UIcmdWithAString(CmdName, this);
  set_fTubs_RMax_cmd->SetGuidance("set Tubs_RMax.");
  set_fTubs_RMax_cmd->SetGuidance("int, string");
  set_fTubs_RMax_cmd->SetGuidance("index, formula");
  set_fTubs_RMax_cmd->SetParameterName("fTubs_RMax",false);
  set_fTubs_RMax_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "fTubs_RMin";
  set_fTubs_RMin_cmd = new G4UIcmdWithAString(CmdName, this);
  set_fTubs_RMin_cmd->SetGuidance("set Tubs_RMin.");
  set_fTubs_RMin_cmd->SetGuidance("int, string");
  set_fTubs_RMin_cmd->SetGuidance("index, formula");
  set_fTubs_RMin_cmd->SetParameterName("fTubs_RMin",false);
  set_fTubs_RMin_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "fTubs_Length";
  set_fTubs_Length_cmd = new G4UIcmdWithAString(CmdName, this);
  set_fTubs_Length_cmd->SetGuidance("set Tubs_Length.");
  set_fTubs_Length_cmd->SetGuidance("int, string");
  set_fTubs_Length_cmd->SetGuidance("index, formula");
  set_fTubs_Length_cmd->SetParameterName("fTubs_Length",false);
  set_fTubs_Length_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "fTubs_StartAng";
  set_fTubs_StartAng_cmd = new G4UIcmdWithAString(CmdName, this);
  set_fTubs_StartAng_cmd->SetGuidance("set Tubs_StartAng.");
  set_fTubs_StartAng_cmd->SetGuidance("int, string");
  set_fTubs_StartAng_cmd->SetGuidance("index, formula");
  set_fTubs_StartAng_cmd->SetParameterName("fTubs_StartAng",false);
  set_fTubs_StartAng_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "fTubs_SpanAng";
  set_fTubs_SpanAng_cmd = new G4UIcmdWithAString(CmdName, this);
  set_fTubs_SpanAng_cmd->SetGuidance("set Tubs_SpanAng.");
  set_fTubs_SpanAng_cmd->SetGuidance("int, string");
  set_fTubs_SpanAng_cmd->SetGuidance("index, formula");
  set_fTubs_SpanAng_cmd->SetParameterName("fTubs_SpanAng",false);
  set_fTubs_SpanAng_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "Tubs_RMax";
  set_Tubs_RMax_cmd = new G4UIcmdWithAString(CmdName, this);
  set_Tubs_RMax_cmd->SetGuidance("set Tubs_RMax.");
  set_Tubs_RMax_cmd->SetGuidance("int, int, double, string");
  set_Tubs_RMax_cmd->SetGuidance("VolId, RepId, val, unit");
  set_Tubs_RMax_cmd->SetParameterName("Tubs_RMax",false);
  set_Tubs_RMax_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "Tubs_RMin";
  set_Tubs_RMin_cmd = new G4UIcmdWithAString(CmdName, this);
  set_Tubs_RMin_cmd->SetGuidance("set Tubs_RMin.");
  set_Tubs_RMin_cmd->SetGuidance("int, int, double, string");
  set_Tubs_RMin_cmd->SetGuidance("VolId, RepId, val, unit");
  set_Tubs_RMin_cmd->SetParameterName("Tubs_RMin",false);
  set_Tubs_RMin_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "Tubs_Length";
  set_Tubs_Length_cmd = new G4UIcmdWithAString(CmdName, this);
  set_Tubs_Length_cmd->SetGuidance("set Tubs_Length.");
  set_Tubs_Length_cmd->SetGuidance("int, int, double, string");
  set_Tubs_Length_cmd->SetGuidance("VolId, RepId, val, unit");
  set_Tubs_Length_cmd->SetParameterName("Tubs_Length",false);
  set_Tubs_Length_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "Tubs_StartAng";
  set_Tubs_StartAng_cmd = new G4UIcmdWithAString(CmdName, this);
  set_Tubs_StartAng_cmd->SetGuidance("set Tubs_StartAng.");
  set_Tubs_StartAng_cmd->SetGuidance("int, int, double, string");
  set_Tubs_StartAng_cmd->SetGuidance("VolId, RepId, val, unit");
  set_Tubs_StartAng_cmd->SetParameterName("Tubs_StartAng",false);
  set_Tubs_StartAng_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "Tubs_SpanAng";
  set_Tubs_SpanAng_cmd = new G4UIcmdWithAString(CmdName, this);
  set_Tubs_SpanAng_cmd->SetGuidance("set Tubs_SpanAng.");
  set_Tubs_SpanAng_cmd->SetGuidance("int, int, double, string");
  set_Tubs_SpanAng_cmd->SetGuidance("VolId, RepId, val, unit");
  set_Tubs_SpanAng_cmd->SetParameterName("Tubs_SpanAng",false);
  set_Tubs_SpanAng_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

		//Sphere info
  CmdName = m_SubDirName + "fSphere_RMax";
  set_fSphere_RMax_cmd = new G4UIcmdWithAString(CmdName, this);
  set_fSphere_RMax_cmd->SetGuidance("set Sphere_RMax.");
  set_fSphere_RMax_cmd->SetGuidance("int, string");
  set_fSphere_RMax_cmd->SetGuidance("index, formula");
  set_fSphere_RMax_cmd->SetParameterName("fSphere_RMax",false);
  set_fSphere_RMax_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "fSphere_RMin";
  set_fSphere_RMin_cmd = new G4UIcmdWithAString(CmdName, this);
  set_fSphere_RMin_cmd->SetGuidance("set Sphere_RMin.");
  set_fSphere_RMin_cmd->SetGuidance("int, string");
  set_fSphere_RMin_cmd->SetGuidance("index, formula");
  set_fSphere_RMin_cmd->SetParameterName("fSphere_RMin",false);
  set_fSphere_RMin_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "fSphere_StartPhi";
  set_fSphere_StartPhi_cmd = new G4UIcmdWithAString(CmdName, this);
  set_fSphere_StartPhi_cmd->SetGuidance("set Sphere_StartPhi.");
  set_fSphere_StartPhi_cmd->SetGuidance("int, string");
  set_fSphere_StartPhi_cmd->SetGuidance("index, formula");
  set_fSphere_StartPhi_cmd->SetParameterName("fSphere_StartPhi",false);
  set_fSphere_StartPhi_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "fSphere_SpanPhi";
  set_fSphere_SpanPhi_cmd = new G4UIcmdWithAString(CmdName, this);
  set_fSphere_SpanPhi_cmd->SetGuidance("set Sphere_SpanPhi.");
  set_fSphere_SpanPhi_cmd->SetGuidance("int, string");
  set_fSphere_SpanPhi_cmd->SetGuidance("index, formula");
  set_fSphere_SpanPhi_cmd->SetParameterName("fSphere_SpanPhi",false);
  set_fSphere_SpanPhi_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "fSphere_StartTheta";
  set_fSphere_StartTheta_cmd = new G4UIcmdWithAString(CmdName, this);
  set_fSphere_StartTheta_cmd->SetGuidance("set Sphere_StartTheta.");
  set_fSphere_StartTheta_cmd->SetGuidance("int, string");
  set_fSphere_StartTheta_cmd->SetGuidance("index, formula");
  set_fSphere_StartTheta_cmd->SetParameterName("fSphere_StartTheta",false);
  set_fSphere_StartTheta_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "fSphere_SpanTheta";
  set_fSphere_SpanTheta_cmd = new G4UIcmdWithAString(CmdName, this);
  set_fSphere_SpanTheta_cmd->SetGuidance("set Sphere_SpanTheta.");
  set_fSphere_SpanTheta_cmd->SetGuidance("int, string");
  set_fSphere_SpanTheta_cmd->SetGuidance("index, formula");
  set_fSphere_SpanTheta_cmd->SetParameterName("fSphere_SpanTheta",false);
  set_fSphere_SpanTheta_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "Sphere_RMax";
  set_Sphere_RMax_cmd = new G4UIcmdWithAString(CmdName, this);
  set_Sphere_RMax_cmd->SetGuidance("set Sphere_RMax.");
  set_Sphere_RMax_cmd->SetGuidance("int, int, double, string");
  set_Sphere_RMax_cmd->SetGuidance("VolId, RepId, val, unit");
  set_Sphere_RMax_cmd->SetParameterName("Sphere_RMax",false);
  set_Sphere_RMax_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "Sphere_RMin";
  set_Sphere_RMin_cmd = new G4UIcmdWithAString(CmdName, this);
  set_Sphere_RMin_cmd->SetGuidance("set Sphere_RMin.");
  set_Sphere_RMin_cmd->SetGuidance("int, int, double, string");
  set_Sphere_RMin_cmd->SetGuidance("VolId, RepId, val, unit");
  set_Sphere_RMin_cmd->SetParameterName("Sphere_RMin",false);
  set_Sphere_RMin_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "Sphere_StartPhi";
  set_Sphere_StartPhi_cmd = new G4UIcmdWithAString(CmdName, this);
  set_Sphere_StartPhi_cmd->SetGuidance("set Sphere_StartPhi.");
  set_Sphere_StartPhi_cmd->SetGuidance("int, int, double, string");
  set_Sphere_StartPhi_cmd->SetGuidance("VolId, RepId, val, unit");
  set_Sphere_StartPhi_cmd->SetParameterName("Sphere_StartPhi",false);
  set_Sphere_StartPhi_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "Sphere_SpanPhi";
  set_Sphere_SpanPhi_cmd = new G4UIcmdWithAString(CmdName, this);
  set_Sphere_SpanPhi_cmd->SetGuidance("set Sphere_SpanPhi.");
  set_Sphere_SpanPhi_cmd->SetGuidance("int, int, double, string");
  set_Sphere_SpanPhi_cmd->SetGuidance("VolId, RepId, val, unit");
  set_Sphere_SpanPhi_cmd->SetParameterName("Sphere_SpanPhi",false);
  set_Sphere_SpanPhi_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "Sphere_StartTheta";
  set_Sphere_StartTheta_cmd = new G4UIcmdWithAString(CmdName, this);
  set_Sphere_StartTheta_cmd->SetGuidance("set Sphere_StartTheta.");
  set_Sphere_StartTheta_cmd->SetGuidance("int, int, double, string");
  set_Sphere_StartTheta_cmd->SetGuidance("VolId, RepId, val, unit");
  set_Sphere_StartTheta_cmd->SetParameterName("Sphere_StartTheta",false);
  set_Sphere_StartTheta_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  CmdName = m_SubDirName + "Sphere_SpanTheta";
  set_Sphere_SpanTheta_cmd = new G4UIcmdWithAString(CmdName, this);
  set_Sphere_SpanTheta_cmd->SetGuidance("set Sphere_SpanTheta.");
  set_Sphere_SpanTheta_cmd->SetGuidance("int, int, double, string");
  set_Sphere_SpanTheta_cmd->SetGuidance("VolId, RepId, val, unit");
  set_Sphere_SpanTheta_cmd->SetParameterName("Sphere_SpanTheta",false);
  set_Sphere_SpanTheta_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

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

FormulizedGeometryParameterMessenger::~FormulizedGeometryParameterMessenger()
{
	delete SubDir;

	//calculate formulas
  delete Calculate_cmd;

	//General info for volume
  delete set_fPosX_cmd;
  delete set_fPosY_cmd;
  delete set_fPosZ_cmd;
  delete set_fEphi_cmd;
  delete set_fEtheta_cmd;
  delete set_fEpsi_cmd;
  delete set_PosX_cmd;
  delete set_PosY_cmd;
  delete set_PosZ_cmd;
  delete set_Ephi_cmd;
  delete set_Etheta_cmd;
  delete set_Epsi_cmd;
  delete set_name_cmd;
  delete set_SDName_cmd;
  delete set_MotherName_cmd;
  delete set_material_cmd;
  delete set_SRepNo_cmd;
  delete set_RepNo_cmd;

		//Box info
  delete set_fBox_X_cmd;
  delete set_fBox_Y_cmd;
  delete set_fBox_Z_cmd;
  delete set_Box_X_cmd;
  delete set_Box_Y_cmd;
  delete set_Box_Z_cmd;

		//Tubs info
  delete set_fTubs_RMax_cmd;
  delete set_fTubs_RMin_cmd;
  delete set_fTubs_Length_cmd;
  delete set_fTubs_StartAng_cmd;
  delete set_fTubs_SpanAng_cmd;
  delete set_Tubs_RMax_cmd;
  delete set_Tubs_RMin_cmd;
  delete set_Tubs_Length_cmd;
  delete set_Tubs_StartAng_cmd;
  delete set_Tubs_SpanAng_cmd;

		//Sphere info
  delete set_fSphere_RMax_cmd;
  delete set_fSphere_RMin_cmd;
  delete set_fSphere_StartPhi_cmd;
  delete set_fSphere_SpanPhi_cmd;
  delete set_fSphere_StartTheta_cmd;
  delete set_fSphere_SpanTheta_cmd;
  delete set_Sphere_RMax_cmd;
  delete set_Sphere_RMin_cmd;
  delete set_Sphere_StartPhi_cmd;
  delete set_Sphere_SpanPhi_cmd;
  delete set_Sphere_StartTheta_cmd;
  delete set_Sphere_SpanTheta_cmd;

		//other settings
  delete set_vis_cmd;
  delete set_r_cmd;
  delete set_g_cmd;
  delete set_b_cmd;
}

void FormulizedGeometryParameterMessenger::SetNewValue(G4UIcommand* command,G4String newValue){
	MySetNewValue(command, newValue);
}

//----------------------------Special functions-----------------------------------------

int FormulizedGeometryParameterMessenger::MySetNewValue(G4UIcommand* command,G4String newValue)
{

	int status = 0; //0 means found the value
	status = MyVGeometryParameterMessenger::MySetNewValue(command, newValue); //check Parent Class first
	if ( !status ) return status;

  if( command == Calculate_cmd ) { m_GeometryParameter->Calculate(); }

	//General info for volume
  else if( command == set_fPosX_cmd ){
    G4String type; G4int i; G4String val;
    MyString2Anything::get_SIS(newValue,type,i,val);
    m_GeometryParameter->set_fPosX(type,i,val);
  }

	else if( command == set_fPosY_cmd ){
    G4String type; G4int i; G4String val;
    MyString2Anything::get_SIS(newValue,type,i,val);
    m_GeometryParameter->set_fPosY(type,i,val);
  }

  else if( command == set_fPosZ_cmd ){
    G4String type; G4int i; G4String val;
    MyString2Anything::get_SIS(newValue,type,i,val);
    m_GeometryParameter->set_fPosZ(type,i,val);
  }

	else if( command == set_fEphi_cmd ){
    G4String type; G4int i; G4String val;
    MyString2Anything::get_SIS(newValue,type,i,val);
    m_GeometryParameter->set_fEphi(type,i,val);
  }

	else if( command == set_fEtheta_cmd ){
    G4String type; G4int i; G4String val;
    MyString2Anything::get_SIS(newValue,type,i,val);
    m_GeometryParameter->set_fEtheta(type,i,val);
  }

	else if( command == set_fEpsi_cmd ){
    G4String type; G4int i; G4String val;
    MyString2Anything::get_SIS(newValue,type,i,val);
    m_GeometryParameter->set_fEpsi(type,i,val);
  }

  else if( command == set_PosX_cmd ){
    G4String type; G4int VolId, RepId; G4double val;
    MyString2Anything::get_SIIDU(newValue,type,VolId,RepId,val);
    m_GeometryParameter->set_PosX(type,VolId,RepId,val);
  }

	else if( command == set_PosY_cmd ){
    G4String type; G4int VolId, RepId; G4double val;
    MyString2Anything::get_SIIDU(newValue,type,VolId,RepId,val);
    m_GeometryParameter->set_PosY(type,VolId,RepId,val);
  }

  else if( command == set_PosZ_cmd ){
    G4String type; G4int VolId, RepId; G4double val;
    MyString2Anything::get_SIIDU(newValue,type,VolId,RepId,val);
    m_GeometryParameter->set_PosZ(type,VolId,RepId,val);
  }

	else if( command == set_Ephi_cmd ){
    G4String type; G4int VolId, RepId; G4double val;
    MyString2Anything::get_SIIDU(newValue,type,VolId,RepId,val);
    m_GeometryParameter->set_Ephi(type,VolId,RepId,val);
  }

	else if( command == set_Etheta_cmd ){
    G4String type; G4int VolId, RepId; G4double val;
    MyString2Anything::get_SIIDU(newValue,type,VolId,RepId,val);
    m_GeometryParameter->set_Etheta(type,VolId,RepId,val);
  }

	else if( command == set_Epsi_cmd ){
    G4String type; G4int VolId, RepId; G4double val;
    MyString2Anything::get_SIIDU(newValue,type,VolId,RepId,val);
    m_GeometryParameter->set_Epsi(type,VolId,RepId,val);
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

		//Box info
  else if( command == set_fBox_X_cmd ){
  	G4String type;
    G4int i; G4String val;
    MyString2Anything::get_IS(newValue,i,val);
    m_GeometryParameter->set_fBox_X(i,val);
  }

  else if( command == set_fBox_Y_cmd ){
  	G4String type;
    G4int i; G4String val;
    MyString2Anything::get_IS(newValue,i,val);
    m_GeometryParameter->set_fBox_Y(i,val);
  }

  else if( command == set_fBox_Z_cmd ){
  	G4String type;
    G4int i; G4String val;
    MyString2Anything::get_IS(newValue,i,val);
    m_GeometryParameter->set_fBox_Z(i,val);
  }

  else if( command == set_Box_X_cmd ){
  	G4String type;
    G4int VolId, RepId; G4double val;
    MyString2Anything::get_IIDU(newValue,VolId,RepId,val);
    m_GeometryParameter->set_Box_X(VolId, RepId,val);
  }

  else if( command == set_Box_Y_cmd ){
  	G4String type;
    G4int VolId, RepId; G4double val;
    MyString2Anything::get_IIDU(newValue,VolId,RepId,val);
    m_GeometryParameter->set_Box_Y(VolId, RepId,val);
  }

  else if( command == set_Box_Z_cmd ){
  	G4String type;
    G4int VolId, RepId; G4double val;
    MyString2Anything::get_IIDU(newValue,VolId,RepId,val);
    m_GeometryParameter->set_Box_Z(VolId, RepId,val);
  }

		//Tubs info
  else if( command == set_fTubs_RMax_cmd ){
  	G4String type;
    G4int i; G4String val;
    MyString2Anything::get_IS(newValue,i,val);
    m_GeometryParameter->set_fTubs_RMax(i,val);
  }

  else if( command == set_fTubs_RMin_cmd ){
  	G4String type;
    G4int i; G4String val;
    MyString2Anything::get_IS(newValue,i,val);
    m_GeometryParameter->set_fTubs_RMin(i,val);
  }

  else if( command == set_fTubs_Length_cmd ){
  	G4String type;
    G4int i; G4String val;
    MyString2Anything::get_IS(newValue,i,val);
    m_GeometryParameter->set_fTubs_Length(i,val);
  }

  else if( command == set_fTubs_StartAng_cmd ){
  	G4String type;
    G4int i; G4String val;
    MyString2Anything::get_IS(newValue,i,val);
    m_GeometryParameter->set_fTubs_StartAng(i,val);
  }

  else if( command == set_fTubs_SpanAng_cmd ){
  	G4String type;
    G4int i; G4String val;
    MyString2Anything::get_IS(newValue,i,val);
    m_GeometryParameter->set_fTubs_SpanAng(i,val);
  }

  else if( command == set_Tubs_RMax_cmd ){
  	G4String type;
    G4int VolId, RepId; G4double val;
    MyString2Anything::get_IIDU(newValue,VolId,RepId,val);
    m_GeometryParameter->set_Tubs_RMax(VolId, RepId,val);
  }

  else if( command == set_Tubs_RMin_cmd ){
  	G4String type;
    G4int VolId, RepId; G4double val;
    MyString2Anything::get_IIDU(newValue,VolId,RepId,val);
    m_GeometryParameter->set_Tubs_RMin(VolId, RepId,val);
  }

  else if( command == set_Tubs_Length_cmd ){
  	G4String type;
    G4int VolId, RepId; G4double val;
    MyString2Anything::get_IIDU(newValue,VolId,RepId,val);
    m_GeometryParameter->set_Tubs_Length(VolId, RepId,val);
  }

  else if( command == set_Tubs_StartAng_cmd ){
  	G4String type;
    G4int VolId, RepId; G4double val;
    MyString2Anything::get_IIDU(newValue,VolId,RepId,val);
    m_GeometryParameter->set_Tubs_StartAng(VolId, RepId,val);
  }

  else if( command == set_Tubs_SpanAng_cmd ){
  	G4String type;
    G4int VolId, RepId; G4double val;
    MyString2Anything::get_IIDU(newValue,VolId,RepId,val);
    m_GeometryParameter->set_Tubs_SpanAng(VolId, RepId,val);
  }

		//Sphere info
  else if( command == set_fSphere_RMax_cmd ){
  	G4String type;
    G4int i; G4String val;
    MyString2Anything::get_IS(newValue,i,val);
    m_GeometryParameter->set_fSphere_RMax(i,val);
  }

  else if( command == set_fSphere_RMin_cmd ){
  	G4String type;
    G4int i; G4String val;
    MyString2Anything::get_IS(newValue,i,val);
    m_GeometryParameter->set_fSphere_RMin(i,val);
  }

  else if( command == set_fSphere_StartPhi_cmd ){
  	G4String type;
    G4int i; G4String val;
    MyString2Anything::get_IS(newValue,i,val);
    m_GeometryParameter->set_fSphere_StartPhi(i,val);
  }

  else if( command == set_fSphere_SpanPhi_cmd ){
  	G4String type;
    G4int i; G4String val;
    MyString2Anything::get_IS(newValue,i,val);
    m_GeometryParameter->set_fSphere_SpanPhi(i,val);
  }

  else if( command == set_fSphere_StartTheta_cmd ){
  	G4String type;
    G4int i; G4String val;
    MyString2Anything::get_IS(newValue,i,val);
    m_GeometryParameter->set_fSphere_StartTheta(i,val);
  }

  else if( command == set_fSphere_SpanTheta_cmd ){
  	G4String type;
    G4int i; G4String val;
    MyString2Anything::get_IS(newValue,i,val);
    m_GeometryParameter->set_fSphere_SpanTheta(i,val);
  }

  else if( command == set_Sphere_RMax_cmd ){
  	G4String type;
    G4int VolId, RepId; G4double val;
    MyString2Anything::get_IIDU(newValue,VolId,RepId,val);
    m_GeometryParameter->set_Sphere_RMax(VolId, RepId,val);
  }

  else if( command == set_Sphere_RMin_cmd ){
  	G4String type;
    G4int VolId, RepId; G4double val;
    MyString2Anything::get_IIDU(newValue,VolId,RepId,val);
    m_GeometryParameter->set_Sphere_RMin(VolId, RepId,val);
  }

  else if( command == set_Sphere_StartPhi_cmd ){
  	G4String type;
    G4int VolId, RepId; G4double val;
    MyString2Anything::get_IIDU(newValue,VolId,RepId,val);
    m_GeometryParameter->set_Sphere_StartPhi(VolId, RepId,val);
  }

  else if( command == set_Sphere_SpanPhi_cmd ){
  	G4String type;
    G4int VolId, RepId; G4double val;
    MyString2Anything::get_IIDU(newValue,VolId,RepId,val);
    m_GeometryParameter->set_Sphere_SpanPhi(VolId, RepId,val);
  }

  else if( command == set_Sphere_StartTheta_cmd ){
  	G4String type;
    G4int VolId, RepId; G4double val;
    MyString2Anything::get_IIDU(newValue,VolId,RepId,val);
    m_GeometryParameter->set_Sphere_StartTheta(VolId, RepId,val);
  }

  else if( command == set_Sphere_SpanTheta_cmd ){
  	G4String type;
    G4int VolId, RepId; G4double val;
    MyString2Anything::get_IIDU(newValue,VolId,RepId,val);
    m_GeometryParameter->set_Sphere_SpanTheta(VolId, RepId,val);
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

