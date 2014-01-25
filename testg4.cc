#include "PrimaryGeneratorAction.hh"
#include "G4RunManager.hh"
#include "QGSP_BERT.hh"

int main(int argc, char ** argv){
	G4RunManager * runManager = new G4RunManager;
	runManager->SetUserInitialization(new QGSP_BERT);
	PrimaryGeneratorAction * pga = new PrimaryGeneratorAction();
	runManager->SetUserAction(pga);
	pga->ReadCard("/home/chen/MyWorkArea/g4sim/configure/gen_root");
	return 0;
}
