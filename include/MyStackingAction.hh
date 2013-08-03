//-*-C++-*-
// $Id: MyStackingAction.hh 80 2007-12-09 10:01:09Z comet $

#ifndef MyStackingAction_HH
#define MyStackingAction_HH 1

static const char* MyStackingAction_hh =
"@(#) $Id: MyStackingAction.hh 80 2007-12-09 10:01:09Z comet $";

#include <map>
#include <vector>
#include "globals.hh"

class MyStackingActionMessenger;

#include "G4UserStackingAction.hh"
class G4StackManager;
class G4Track;
#include "G4ClassificationOfNewTrack.hh"

// class description:
//
//  This is the base class of one of the user's optional action classes.
// This class gives the hooks for G4StackManager which controls the stacks
// of G4Track objects.
//

typedef std::map<G4int, G4int> TTrackIDMap;
typedef TTrackIDMap::value_type TTrackIDMapValue;

class MyStackingAction : public G4UserStackingAction {
    public:
        MyStackingAction();
        ~MyStackingAction();

        inline void SetStackManager(G4StackManager * value)
            { stackManager = value; }

    public:                                           // with description
        //---------------------------------------------------------------
        // vitual methods to be implemented by user
        //---------------------------------------------------------------
        //
        G4ClassificationOfNewTrack
            ClassifyNewTrack(const G4Track* aTrack);
        //
        //    Extended to store the (daughter_ID,parent_ID) for
        //    keeping the track_ID history, by M. Aoki.
        //
        //
        //    Reply G4ClassificationOfNewTrack determined by the
        //  newly coming G4Track.
        //
        //    enum G4ClassificationOfNewTrack
        //    {
        //      fUrgent,    // put into the urgent stack
        //      fWaiting,   // put into the waiting stack
        //      fPostpone,  // postpone to the next event
        //      fKill       // kill without stacking
        //    };
        //
        //    The parent_ID of the track indicates the origin of it.
        //
        //    G4int parent_ID = aTrack->get_parentID();
        //
        //      parent_ID = 0 : primary particle
        //                > 0 : secondary particle
        //                < 0 : postponed from the previous event
        //
        //---------------------------------------------------------------
        //

        void NewStage();
        //
        //    This method is called by G4StackManager when the urgentStack
        //  becomes empty and contents in the waitingStack are transtered
        //  to the urgentStack.
        //    Note that this method is not called at the begining of each
        //  event, but "PrepareNewEvent" is called.
        //
        //    In case re-classification of the stacked tracks is needed,
        //  use the following method to request to G4StackManager.
        //
        //    stackManager->ReClassify();
        //
        //  All of the stacked tracks in the waitingStack will be re-classified
        //  by "ClassifyNewTrack" method.
        //    To abort current event, use the following method.
        //
        //    stackManager->clear();
        //
        //  Note that this way is valid and safe only for the case it is called
        //  from this user class. The more global way of event abortion is
        //
        //    G4UImanager * UImanager = G4UImanager::GetUIpointer();
        //    UImanager->ApplyCommand("/event/abort");
        //
        //---------------------------------------------------------------
        //

        void PrepareNewEvent();
        //
        //    This method is called by G4StackManager at the begining of
        //  each event.
        //    Be careful that the urgentStack and the waitingStack of
        //  G4StackManager are empty at this moment, because this method
        //  is called before accepting primary particles. Also, note that
        //  the postponeStack of G4StackManager may have some postponed
        //  tracks.
        //
        //---------------------------------------------------------------

    public:
        void SetEleCut(G4double val) {fEleCut = val;};
        void SetPosCut(G4double val) {fPosCut = val;};
        void SetGamCut(G4double val) {fGamCut = val;};

        G4double GetEleCut() {return fEleCut;};
        G4double GetPosCut() {return fPosCut;};
        G4double GetGamCut() {return fGamCut;};

		void add_white_list(int val) {m_white_list.push_back(val);};
		void add_black_list(int val) {m_black_list.push_back(val);};
		void clear_white_list() {m_white_list.clear();};
		void clear_black_list() {m_black_list.clear();};

    public:

        G4int GetDepthFromParent(G4int aTrack, G4int parentTrack);
        // return 0  -- if aTrack == parentTrack
        // return -1 -- if aTrack <= 0 || parentTrack <= 0 ... error
        // return 1  -- if a parent track of aTrack is parentTrack itself.
        // return 2  -- if a grand-parent track of aTrack is parentTrack.
        // and so on.
        //

    private:

        MyStackingActionMessenger* fStackingActionMessenger;

        TTrackIDMap* fTrackIDMap;

        G4double fEleCut;
        G4double fPosCut;
        G4double fGamCut;

		std::vector<int> m_white_list;
		std::vector<int> m_black_list;

};
#endif
