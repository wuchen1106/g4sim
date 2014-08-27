//---------------------------------------------------------------------------//
//Description: Messenger class
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "CdcCellGeometryParameterMessenger.hh"

#include "myglobals.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithABool.hh"

#include "MyString2Anything.hh"
#include "CdcCellGeometryParameter.hh"
#include "MyVGeometryParameter.hh"

CdcCellGeometryParameterMessenger::CdcCellGeometryParameterMessenger(MyVGeometryParameter* pGeometryParameter, G4String dirName)
:SimpleGeometryParameterMessenger(pGeometryParameter, dirName)
{

	m_GeometryParameter = dynamic_cast<CdcCellGeometryParameter*> (pGeometryParameter);
	if (!m_GeometryParameter){
		std::cout<<"In CdcCellGeometryParameterMessenger, Cannot convert pointer from MyVGeometryParameter* to CdcCellGeometryParameter!!!"<<std::endl;
    G4Exception("CdcCellGeometryParameterMessenger::CdcCellGeometryParameterMessenger()",
        "InvalidSetup", FatalException,
        "Cannot convert pointer from MyVGeometryParameter* to CdcCellGeometryParameter!!!");
	}

	G4String fGeoDirName = get_GeoDirName();
	m_SubDirName = fGeoDirName+"CdcCellGeometry/";

	SubDir = new G4UIdirectory(m_SubDirName);
	SubDir->SetGuidance("geometry control");

}

CdcCellGeometryParameterMessenger::~CdcCellGeometryParameterMessenger()
{
	delete SubDir;
}

void CdcCellGeometryParameterMessenger::SetNewValue(G4UIcommand* command,G4String newValue){
	MySetNewValue(command, newValue);
}

//----------------------------Special functions-----------------------------------------

int CdcCellGeometryParameterMessenger::MySetNewValue(G4UIcommand* command,G4String newValue)
{
	int status = 0; //0 means found the value
	status = MyVGeometryParameterMessenger::MySetNewValue(command, newValue); //check Parent Class first
	if ( !status ) return status;

	return status;
}

