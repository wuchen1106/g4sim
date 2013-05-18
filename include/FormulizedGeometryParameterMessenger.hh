//---------------------------------------------------------------------------//
//Description: Messenger class
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#ifndef FormulizedGeometryParameterMessenger_h
#define FormulizedGeometryParameterMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

#include "SimpleGeometryParameterMessenger.hh"

class FormulizedGeometryParameter;
class MyVGeometryParameter;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;

class FormulizedGeometryParameterMessenger : public SimpleGeometryParameterMessenger
{
	public:
		FormulizedGeometryParameterMessenger(MyVGeometryParameter*, G4String);
		virtual ~FormulizedGeometryParameterMessenger();

		virtual void SetNewValue(G4UIcommand*, G4String);

		//=> Special functions
		int MySetNewValue(G4UIcommand*, G4String);

		//=> Access
		FormulizedGeometryParameter* get_GeometryParameter(){ return m_GeometryParameter; }

		//=> Modify
		void set_GeometryParameter( FormulizedGeometryParameter* val ){ m_GeometryParameter = val; }

	private:

		FormulizedGeometryParameter* m_GeometryParameter;

		G4String                   m_SubDirName;
    G4UIdirectory*             SubDir;

		//calculate formulas
		G4UIcmdWithoutParameter*   Calculate_cmd;

		//General info for volume
		G4UIcmdWithAString*        set_fPosX_cmd;
		G4UIcmdWithAString*        set_fPosY_cmd;
		G4UIcmdWithAString*        set_fPosZ_cmd;
		G4UIcmdWithAString*        set_fEphi_cmd;
		G4UIcmdWithAString*        set_fEtheta_cmd;
		G4UIcmdWithAString*        set_fEpsi_cmd;
		G4UIcmdWithAString*        set_PosX_cmd;
		G4UIcmdWithAString*        set_PosY_cmd;
		G4UIcmdWithAString*        set_PosZ_cmd;
		G4UIcmdWithAString*        set_Ephi_cmd;
		G4UIcmdWithAString*        set_Etheta_cmd;
		G4UIcmdWithAString*        set_Epsi_cmd;
		G4UIcmdWithAString*        set_name_cmd;
		G4UIcmdWithAString*        set_SDName_cmd;
		G4UIcmdWithAString*        set_MotherName_cmd;
		G4UIcmdWithAString*        set_material_cmd;
		G4UIcmdWithAString*        set_SRepNo_cmd;
		G4UIcmdWithAString*        set_RepNo_cmd;

		//Box info
		G4UIcmdWithAString*        set_fBox_X_cmd;
		G4UIcmdWithAString*        set_fBox_Y_cmd;
		G4UIcmdWithAString*        set_fBox_Z_cmd;
		G4UIcmdWithAString*        set_Box_X_cmd;
		G4UIcmdWithAString*        set_Box_Y_cmd;
		G4UIcmdWithAString*        set_Box_Z_cmd;

		//Tubs info
		G4UIcmdWithAString*        set_fTubs_RMax_cmd;
		G4UIcmdWithAString*        set_fTubs_RMin_cmd;
		G4UIcmdWithAString*        set_fTubs_Length_cmd;
		G4UIcmdWithAString*        set_fTubs_StartAng_cmd;
		G4UIcmdWithAString*        set_fTubs_SpanAng_cmd;
		G4UIcmdWithAString*        set_Tubs_RMax_cmd;
		G4UIcmdWithAString*        set_Tubs_RMin_cmd;
		G4UIcmdWithAString*        set_Tubs_Length_cmd;
		G4UIcmdWithAString*        set_Tubs_StartAng_cmd;
		G4UIcmdWithAString*        set_Tubs_SpanAng_cmd;

		//Sphere info
		G4UIcmdWithAString*        set_fSphere_RMax_cmd;
		G4UIcmdWithAString*        set_fSphere_RMin_cmd;
		G4UIcmdWithAString*        set_fSphere_StartPhi_cmd;
		G4UIcmdWithAString*        set_fSphere_SpanPhi_cmd;
		G4UIcmdWithAString*        set_fSphere_StartTheta_cmd;
		G4UIcmdWithAString*        set_fSphere_SpanTheta_cmd;
		G4UIcmdWithAString*        set_Sphere_RMax_cmd;
		G4UIcmdWithAString*        set_Sphere_RMin_cmd;
		G4UIcmdWithAString*        set_Sphere_StartPhi_cmd;
		G4UIcmdWithAString*        set_Sphere_SpanPhi_cmd;
		G4UIcmdWithAString*        set_Sphere_StartTheta_cmd;
		G4UIcmdWithAString*        set_Sphere_SpanTheta_cmd;

		//other settings
		G4UIcmdWithAString*        set_fvis_cmd;
		G4UIcmdWithAString*        set_fr_cmd;
		G4UIcmdWithAString*        set_fg_cmd;
		G4UIcmdWithAString*        set_fb_cmd;
		G4UIcmdWithAString*        set_vis_cmd;
		G4UIcmdWithAString*        set_r_cmd;
		G4UIcmdWithAString*        set_g_cmd;
		G4UIcmdWithAString*        set_b_cmd;

};

#endif

