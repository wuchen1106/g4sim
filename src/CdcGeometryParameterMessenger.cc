//---------------------------------------------------------------------------//
//Description: Messenger class
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "CdcGeometryParameterMessenger.hh"

#include "globals.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithABool.hh"

#include "MyString2Anything.hh"
#include "CdcGeometryParameter.hh"
#include "MyVGeometryParameter.hh"

CdcGeometryParameterMessenger::CdcGeometryParameterMessenger(MyVGeometryParameter* pGeometryParameter, G4String dirName)
:SimpleGeometryParameterMessenger(pGeometryParameter, dirName)
{

	m_GeometryParameter = dynamic_cast<CdcGeometryParameter*> (pGeometryParameter);
	if (!m_GeometryParameter){
		std::cout<<"In CdcGeometryParameterMessenger, Cannot convert pointer from MyVGeometryParameter* to CdcGeometryParameter!!!"<<std::endl;
    G4Exception("CdcGeometryParameterMessenger::CdcGeometryParameterMessenger()",
        "InvalidSetup", FatalException,
        "Cannot convert pointer from MyVGeometryParameter* to CdcGeometryParameter!!!");
	}

	G4String fGeoDirName = get_GeoDirName();
	m_SubDirName = fGeoDirName+"CdcGeometry/";

	SubDir = new G4UIdirectory(m_SubDirName);
	SubDir->SetGuidance("geometry control");

}

CdcGeometryParameterMessenger::~CdcGeometryParameterMessenger()
{
	delete SubDir;
}

void CdcGeometryParameterMessenger::SetNewValue(G4UIcommand* command,G4String newValue){
	MySetNewValue(command, newValue);
}

//----------------------------Special functions-----------------------------------------

int CdcGeometryParameterMessenger::MySetNewValue(G4UIcommand* command,G4String newValue)
{
	int status = 0; //0 means found the value
	status = MyVGeometryParameterMessenger::MySetNewValue(command, newValue); //check Parent Class first
	if ( !status ) return status;

	return status;
}

