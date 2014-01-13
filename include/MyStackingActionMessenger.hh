//-*-C++-*-
// $Id: MyStackingActionMessenger.hh 80 2007-12-09 10:01:09Z comet $

#ifndef MyStackingActionMessenger_HH
#define MyStackingActionMessenger_HH 1

static const char* MyStackingActionMessenger_hh =
"@(#) $Id: MyStackingActionMessenger.hh 80 2007-12-09 10:01:09Z comet $";

class MyStackingAction;

#include "globals.hh"
#include "G4UImessenger.hh"

class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAnInteger;
class G4UIcmdWithoutParameter;

class MyStackingActionMessenger: public G4UImessenger {
    public:
        MyStackingActionMessenger(MyStackingAction* );
        ~MyStackingActionMessenger();

        void SetNewValue(G4UIcommand*, G4String);

    private:
        MyStackingAction* fStackingAction;

        G4UIdirectory*             fDir;
        G4UIcmdWithADoubleAndUnit* fEleCmd;
        G4UIcmdWithADoubleAndUnit* fPosCmd;
        G4UIcmdWithADoubleAndUnit* fGamCmd;

        G4UIcmdWithAnInteger* fset_no_secCmd;
        G4UIcmdWithAnInteger* fset_no_MCCmd;
        G4UIcmdWithAnInteger* fset_no_PCCmd;
        G4UIcmdWithAnInteger* fadd_whiteCmd;
        G4UIcmdWithAnInteger* fadd_blackCmd;
        G4UIcmdWithoutParameter* fclear_whiteCmd;
        G4UIcmdWithoutParameter* fclear_blackCmd;

};
#endif
