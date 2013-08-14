//-*-C++-*-
// $Id: MyStackingAction.cc 80 2007-12-09 10:01:09Z comet $

static const char* MyStackingAction_cc =
"@(#) $Id: MyStackingAction.cc 80 2007-12-09 10:01:09Z comet $";

#include "MyStackingAction.hh"
#include "MyStackingActionMessenger.hh"

#include "G4UserStackingAction.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4Track.hh"
#include "G4ios.hh"
#include "G4VProcess.hh"

MyStackingAction::MyStackingAction()
: fTrackIDMap(NULL), fEleCut(0), fPosCut(0), fGamCut(0) {
    fStackingActionMessenger = new MyStackingActionMessenger(this);
    m_white_list.clear();
    m_black_list.clear();
    m_no_sec=false;
    m_no_MC=false;
    m_no_PC=false;
}


MyStackingAction::~MyStackingAction() {
    delete fTrackIDMap;
}


G4ClassificationOfNewTrack
MyStackingAction::ClassifyNewTrack(const G4Track* aTrack) {
    G4ParticleDefinition* aPart = aTrack->GetDefinition();
    G4int aPDGEncoding = aPart->GetPDGEncoding();
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

    //
    // Store the parent-daughter relation.
    //
    G4int aTrackID = aTrack->GetTrackID();
    G4int aParentTrackID = aTrack->GetParentID();
    if ( (fTrackIDMap->find(aTrackID)) == fTrackIDMap->end() ) {
        fTrackIDMap->insert( TTrackIDMapValue(aTrackID, aParentTrackID));
    }

    //
    // Assign the tracking priorities
    //
    G4ClassificationOfNewTrack aClassification = fWaiting;

    if ( aPDGEncoding ==
    particleTable->FindParticle("e-")->GetPDGEncoding() ) {
        if ( aTrack->GetTotalEnergy() >= fEleCut ) {
            aClassification = fUrgent;
        }
        else {
            aClassification = fKill;
        }

    } else if ( aPDGEncoding ==
    particleTable->FindParticle("e+")->GetPDGEncoding() ) {
        if ( aTrack->GetTotalEnergy() >= fPosCut ) {
            aClassification = fUrgent;
        }
        else {
            aClassification = fKill;
        }
    } else if ( aPDGEncoding ==
    particleTable->FindParticle("gamma")->GetPDGEncoding() ) {
        if ( aTrack->GetTotalEnergy() >= fGamCut ) {
            aClassification = fUrgent;
        }
        else {
            aClassification = fKill;
        }

    }

	if (m_no_MC||m_no_PC||m_no_sec){
		G4String processName;
		const G4VProcess* process = aTrack->GetCreatorProcess();
		if (process) {
			processName = process->GetProcessName();
		}
		else{
			processName = "NULL";
		}
		if (processName=="muMinusCaptureAtRest"&&m_no_MC) aClassification = fKill;
		if (processName=="hBertiniCaptureAtRest"&&m_no_PC) aClassification = fKill;
		if (processName!="NULL"&&m_no_sec) aClassification = fKill;
	}

	if (aTrackID!=1){
		bool foundit=false;
		if (m_white_list.size()!=0){
			foundit=false;
			for (int i = 0; i< m_white_list.size(); i++){
				if (aPDGEncoding==m_white_list[i]){
					foundit=true;
					break;
				}
			}
			if (!foundit){
				aClassification = fKill;
			}
		}
		foundit=false;
		for (int i = 0; i< m_black_list.size(); i++){
			if (aPDGEncoding==m_black_list[i]){
				foundit=true;
				break;
			}
		}
		if (foundit){
			aClassification = fKill;
		}
	}

    return aClassification;
}


void MyStackingAction::NewStage()
{;}

void MyStackingAction::PrepareNewEvent() {
    delete fTrackIDMap;
    fTrackIDMap = new TTrackIDMap;
}


G4int MyStackingAction::GetDepthFromParent(G4int aTrackID, G4int aParentID) {
    G4int trackID = aTrackID;
    G4int depth = 0;
    G4bool found = false;

    if ( aParentID <= 0 || aTrackID <= 0 ) {
        return -1;
    }
    else if ( aTrackID == aParentID ) {
        return 0;
    }
    else {
        depth = 1;
        while ( trackID != 0 ) {
            //      G4cout << "(" << (*(fTrackIDMap->find(trackID))).first
            //	     << "," << (*(fTrackIDMap->find(trackID))).second << ")"
            //	     << G4endl;
            if ( (*(fTrackIDMap->find(trackID))).second == aParentID ) {
                found = true;
                break;
            }
            trackID = (*(fTrackIDMap->find(trackID))).second;
            ++depth;
        }
        if ( found ) {
            return depth;
        }
        else {
            return -1;
        }
    }

    return found;
}
