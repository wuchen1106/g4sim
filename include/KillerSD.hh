//---------------------------------------------------------------------------
//Description: Sensitive detector definition for Monitor
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#ifndef KillerSD_h
#define KillerSD_h 1

#include "myglobals.hh"

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

		void Initialize(G4HCofThisEvent*);
		G4bool ProcessHits(G4Step*, G4TouchableHistory*);
		void EndOfEvent(G4HCofThisEvent*);

		//inherit from MySD
		void SetBranch();

		void ReadOutputCard(G4String filename);

	private:

		void ReSet();
		void ShowOutCard();

	private:

		//for filter
		bool Switch;
		bool neutralCut;
		G4int maxn;
		G4double minp;
		G4double mine;
		G4double tres;
		G4double mint;
		G4double maxt;
		G4double minedep;
		std::vector<int> white_list;
		std::vector<int> black_list;
};

#endif

