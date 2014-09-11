#include "MyStepLimiter.hh"

#include "G4StepLimiter.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"

#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"

MyStepLimiter::MyStepLimiter(G4int ver)
	:  G4VPhysicsConstructor("MyStepLimiter"), verbose(ver), wasActivated(false)
{
	fStepLimiter = 0;
}

	MyStepLimiter::MyStepLimiter(const G4String& name, G4int ver)
:  G4VPhysicsConstructor(name), verbose(ver), wasActivated(false)
{
	fStepLimiter = 0;
}

MyStepLimiter::~MyStepLimiter()
{
	delete fStepLimiter;
}

void MyStepLimiter::ConstructParticle()
{

	// G4cout << "MyStepLimiter::ConstructParticle" << G4endl;

}

void MyStepLimiter::ConstructProcess()
{
	if(wasActivated) { return; }
	wasActivated = true;

	G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
	G4ParticleTable::G4PTblDicIterator* theParticleIterator
		= G4ParticleTable::GetParticleTable()->GetIterator();

	// add limit process
	theParticleIterator->reset();

	fStepLimiter = new G4StepLimiter();

	while ((*theParticleIterator)()){
		G4ParticleDefinition *particle     = theParticleIterator->value();
		G4ProcessManager     *pmanager     = particle->GetProcessManager();
		G4String              particleName = particle->GetParticleName();
//		if (particleName == "e-" ||
//				particleName == "e+" ||
//				particleName == "mu-" ||
//				particleName == "mu+" ||
//				particleName == "proton"
//		   ) {
		if (particleName != "gamma"
			&& particleName != "geantino"
			&& particleName != "chargedgeantino"
			&& particleName != "nu_e"
			&& particleName != "nu_mu"
			&& particleName != "nu_tau"
			&& particleName != "anti_nu_e"
			&& particleName != "anti_nu_mu"
			&& particleName != "anti_nu_tau"
		   ) {
			pmanager->AddDiscreteProcess(fStepLimiter);
		}   
	} 
}
