//---------------------------------------------------------------------------//
//Description: Messenger class
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#ifndef CdcCellGeometryParameterMessenger_h
#define CdcCellGeometryParameterMessenger_h 1

#include "myglobals.hh"
#include "G4UImessenger.hh"

#include "SimpleGeometryParameterMessenger.hh"

class CdcCellGeometryParameter;
class MyVGeometryParameter;
class G4UIdirectory;

class CdcCellGeometryParameterMessenger : public SimpleGeometryParameterMessenger
{
	public:
		CdcCellGeometryParameterMessenger(MyVGeometryParameter*, G4String);
		virtual ~CdcCellGeometryParameterMessenger();

		virtual void SetNewValue(G4UIcommand*, G4String);

		//=> Special functions
		int MySetNewValue(G4UIcommand*, G4String);

		//=> Access
		CdcCellGeometryParameter* get_GeometryParameter(){ return m_GeometryParameter; }

		//=> Modify
		void set_GeometryParameter( CdcCellGeometryParameter* val ){ m_GeometryParameter = val; }

	private:

		CdcCellGeometryParameter* m_GeometryParameter;

		G4String                   m_SubDirName;
		G4UIdirectory*             SubDir;

};

#endif

