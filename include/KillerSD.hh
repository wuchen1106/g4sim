//---------------------------------------------------------------------------
//Description: Sensitive detector definition for CDC  
//Author: Yuan Ye(yuany@mail.ihep.ac.cn)
//Created: 22 July, 2012
//Modified:
//Comment: 
//---------------------------------------------------------------------------//

#ifndef KillerSD_h
#define KillerSD_h 1

#include "globals.hh"

#include <iostream>
#include <vector>

#include "MySD.hh"

class G4Step;
class G4HCofThisEvent;
class MyVGeometryParameter;

class KillerSD : public MySD
{
	public:
		KillerSD(G4String, MyVGeometryParameter*);
		virtual ~KillerSD();

		//=> inherit from G4VSensitiveDetector
		void Initialize(G4HCofThisEvent*);
		G4bool ProcessHits(G4Step*, G4TouchableHistory*);
		void EndOfEvent(G4HCofThisEvent*);

		//=> inherit from MySD
		void SetBranch();
		void ReadOutputCard(G4String filename);

	private:

		void ReSet();
		void ShowOutCard();

	private:

		//For root objects
		// int nHits;
		//for output setting
		// bool flag_nHits;
		//for filter
		// bool Switch;
		// black list
		// white list
		// G4double minp;
		// bool neutralCut;
		// G4double mint;
		// G4double maxt;
		//fTolerance
};

#endif

