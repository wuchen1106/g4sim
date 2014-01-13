//---------------------------------------------------------------------------//
//Description: Messenger class
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#ifndef MyAnalysisSvcMessenger_h
#define MyAnalysisSvcMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class MyAnalysisSvc;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;

class MyAnalysisSvcMessenger: public G4UImessenger
{
  public:
    MyAnalysisSvcMessenger(MyAnalysisSvc*);
   ~MyAnalysisSvcMessenger();
    
    void SetNewValue(G4UIcommand*, G4String);
    
  private:
    MyAnalysisSvc*             m_myAnalysisSvc;
    
    G4UIdirectory*             myAnalysisSvcDir;

		//about output
    G4UIcmdWithAString*        out_card_cmd;

    //about root file
    G4UIcmdWithAString*        ofile_name_cmd;
    G4UIcmdWithAString*        tree_name_cmd;

	//about cut
	G4UIcmdWithADoubleAndUnit* set_minT_cmd;
	G4UIcmdWithADoubleAndUnit* set_maxT_cmd;

    //about log
    G4UIcmdWithAString*        run_name_cmd;
};

#endif
