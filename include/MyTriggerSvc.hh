//---------------------------------------------------------------------------//
//Description: Setup MyTrigger
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 5 Jan, 2013
//Comment:
//---------------------------------------------------------------------------//

#ifndef MyTriggerSvc_h
#define MyTriggerSvc_h 1

#include "myglobals.hh"

class G4Event;
class MyDetectorManager;
class CdcLayerSD;
class CdcSD;
class CdcSimpleSD;
class MonitorSD;
class KillerSD;
class McTruthSvc;

class MyTriggerSvc
{
  public:
    MyTriggerSvc();
     ~MyTriggerSvc();

    static MyTriggerSvc* GetMyTriggerSvc();

    void SetMyTrigger(G4String name);

		bool TriggerIt( const G4Event*);

  private:

    static MyTriggerSvc* fMyTriggerSvc;

		void ReSet();

		void ShowOutCard();

		int minCdcHits;
		int minM_Hits;
		int minV_Hits;
		int minCorM_Hits;
		double minEleMom;
		int minAntipNum;
		int minTracks;

		MyDetectorManager* pMyDetectorManager;
		CdcLayerSD* myCdcLayerSD;
		CdcSD* myCdcSD;
		CdcSimpleSD* myCdcSimpleSD;
		MonitorSD* myMonitorSD;
		MonitorSD* myMonitorSD2;
		McTruthSvc* myMcTruthSvc;
		KillerSD* myKillerSD;

};

#endif

