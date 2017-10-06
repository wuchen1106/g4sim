//---------------------------------------------------------------------------//
//Description: For MyDetectorManager. Self defined SD classes should inherit from this class.
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#ifndef MySD_hh
#define MySD_hh 1

#include "myglobals.hh"
#include "G4VSensitiveDetector.hh"

class MyVGeometryParameter;

class MySD : public G4VSensitiveDetector
{
 public:
	MySD(G4String, MyVGeometryParameter*);
	virtual ~MySD();
  void SetVerbose( int val ){fVerboseLevel = val;}
  virtual void ReadOutputCard(G4String){};
  virtual void SetBranch(void){};

	//=>Access
	G4String get_VolName(){ return m_VolumeName; }
 	int get_VerboseLevel(){ return fVerboseLevel; }
 	MyVGeometryParameter* get_GeometryParameter(){ return m_GeometryParameter; }

	//=>Modify
 	void set_VerboseLevel(int val){ fVerboseLevel = val; }
 	void set_GeometryParameter(MyVGeometryParameter* val){ m_GeometryParameter = val; }

 private:
 	int fVerboseLevel;
 	MyVGeometryParameter* m_GeometryParameter;
 	G4String m_VolumeName;
};

#endif
