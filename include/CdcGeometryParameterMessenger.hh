//---------------------------------------------------------------------------//
//Description: Messenger class
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#ifndef CdcGeometryParameterMessenger_h
#define CdcGeometryParameterMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

#include "SimpleGeometryParameterMessenger.hh"

class CdcGeometryParameter;
class MyVGeometryParameter;
class G4UIdirectory;

class CdcGeometryParameterMessenger : public SimpleGeometryParameterMessenger
{
	public:
		CdcGeometryParameterMessenger(MyVGeometryParameter*, G4String);
		virtual ~CdcGeometryParameterMessenger();

		virtual void SetNewValue(G4UIcommand*, G4String);

		//=> Special functions
		int MySetNewValue(G4UIcommand*, G4String);

		//=> Access
		CdcGeometryParameter* get_GeometryParameter(){ return m_GeometryParameter; }

		//=> Modify
		void set_GeometryParameter( CdcGeometryParameter* val ){ m_GeometryParameter = val; }

	private:

		CdcGeometryParameter* m_GeometryParameter;

		G4String                   m_SubDirName;
		G4UIdirectory*             SubDir;

};

#endif

