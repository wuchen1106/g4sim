//---------------------------------------------------------------------------//
//Description: Messenger class
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Modified: 11, Jan, 2013
//          Support G4Hype, G4TwistedTube
//Comment:
//---------------------------------------------------------------------------//

#ifndef SimpleGeometryParameterMessenger_h
#define SimpleGeometryParameterMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

#include "MyVGeometryParameterMessenger.hh"

class SimpleGeometryParameter;
class MyVGeometryParameter;
class G4UIdirectory;
class G4UIcmdWithAString;

class SimpleGeometryParameterMessenger : public MyVGeometryParameterMessenger
{
	public:
		SimpleGeometryParameterMessenger(MyVGeometryParameter*, G4String);
		virtual ~SimpleGeometryParameterMessenger();

		virtual void SetNewValue(G4UIcommand*, G4String);

		//=> Special functions
		int MySetNewValue(G4UIcommand*, G4String);

		//=> Access
		SimpleGeometryParameter* get_GeometryParameter(){ return m_GeometryParameter; }

		//=> Modify
		void set_GeometryParameter( SimpleGeometryParameter* val ){ m_GeometryParameter = val; }

	private:

		SimpleGeometryParameter*   m_GeometryParameter;

		G4String                   m_SubDirName;
    G4UIdirectory*             SubDir;

		//General info for volume
		G4UIcmdWithAString*        set_PosX_cmd;
		G4UIcmdWithAString*        set_PosY_cmd;
		G4UIcmdWithAString*        set_PosZ_cmd;
		G4UIcmdWithAString*        set_name_cmd;
		G4UIcmdWithAString*        set_SDName_cmd;
		G4UIcmdWithAString*        set_MotherName_cmd;
		G4UIcmdWithAString*        set_material_cmd;
		G4UIcmdWithAString*        set_SRepNo_cmd;
		G4UIcmdWithAString*        set_RepNo_cmd;
		G4UIcmdWithAString*        set_Space_cmd;
		G4UIcmdWithAString*        set_DirTheta_cmd;
		G4UIcmdWithAString*        set_DirPhi_cmd;
		G4UIcmdWithAString*        set_Ephi_cmd;
		G4UIcmdWithAString*        set_Etheta_cmd;
		G4UIcmdWithAString*        set_Epsi_cmd;

		//Box info
		G4UIcmdWithAString*        set_Box_X_cmd;
		G4UIcmdWithAString*        set_Box_Y_cmd;
		G4UIcmdWithAString*        set_Box_Z_cmd;

		//Tubs info
		G4UIcmdWithAString*        set_Tubs_RMax_cmd;
		G4UIcmdWithAString*        set_Tubs_RMin_cmd;
		G4UIcmdWithAString*        set_Tubs_Length_cmd;
		G4UIcmdWithAString*        set_Tubs_StartAng_cmd;
		G4UIcmdWithAString*        set_Tubs_SpanAng_cmd;

		//Sphere info
		G4UIcmdWithAString*        set_Sphere_RMax_cmd;
		G4UIcmdWithAString*        set_Sphere_RMin_cmd;
		G4UIcmdWithAString*        set_Sphere_StartPhi_cmd;
		G4UIcmdWithAString*        set_Sphere_SpanPhi_cmd;
		G4UIcmdWithAString*        set_Sphere_StartTheta_cmd;
		G4UIcmdWithAString*        set_Sphere_SpanTheta_cmd;

		//Hype info
		G4UIcmdWithAString*        set_Hype_innerRadius_cmd;
		G4UIcmdWithAString*        set_Hype_outerRadius_cmd;
		G4UIcmdWithAString*        set_Hype_innerStereo_cmd;
		G4UIcmdWithAString*        set_Hype_outerStereo_cmd;
		G4UIcmdWithAString*        set_Hype_Length_cmd;

		//TwistedTubs info
		G4UIcmdWithAString*        set_TwistedTubs_twistedangle_cmd;
		G4UIcmdWithAString*        set_TwistedTubs_endinnerrad_cmd;
		G4UIcmdWithAString*        set_TwistedTubs_endouterrad_cmd;
		G4UIcmdWithAString*        set_TwistedTubs_Length_cmd;
		G4UIcmdWithAString*        set_TwistedTubs_dphi_cmd;

		//other settings
		G4UIcmdWithAString*        set_vis_cmd;
		G4UIcmdWithAString*        set_r_cmd;
		G4UIcmdWithAString*        set_g_cmd;
		G4UIcmdWithAString*        set_b_cmd;

};

#endif

