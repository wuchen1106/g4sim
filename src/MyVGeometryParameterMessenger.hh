//---------------------------------------------------------------------------//
//Description: Messenger class
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#ifndef MyVGeometryParameterMessenger_h
#define MyVGeometryParameterMessenger_h 1

#include "myglobals.hh"
#include "G4UImessenger.hh"

class MyVGeometryParameter;
class G4UIdirectory;
class G4UIcmdWithABool;
class G4UIcmdWithAnInteger;
class G4UIcmdWithAString;

class MyVGeometryParameterMessenger : public G4UImessenger
{
  public:
    MyVGeometryParameterMessenger(MyVGeometryParameter*, G4String);
    virtual ~MyVGeometryParameterMessenger();

	 //=> Special functions
    int MySetNewValue(G4UIcommand*, G4String);

		//=> Access
		G4String get_Name(){ return m_Name; }
    MyVGeometryParameter* get_GeometryParameter(){ return m_GeometryParameter; }
		G4String get_GeoDirName(){ return m_GeoDirName; }

		//=> Modify
		void set_Name(G4String val){ m_Name = val; }
    void set_GeometryParameter(MyVGeometryParameter* val){ m_GeometryParameter = val; }
		void set_GeoDirName(G4String val){ m_GeoDirName = val; }
    
	private:

    G4UIdirectory*             g4simDir;
    G4UIdirectory*             detDir;
    G4UIdirectory*             geoDir;

    G4UIcmdWithABool*          set_checkoverlap_cmd;
    G4UIcmdWithAnInteger*      set_VerboseLevel_cmd;
    G4UIcmdWithAString*   ReadCard_cmd;

		G4String                   m_Name;
		G4String                   m_GeoDirName;
    MyVGeometryParameter*      m_GeometryParameter;
    
};

#endif

