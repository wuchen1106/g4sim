//---------------------------------------------------------------------------//
//Description: Hit for Monitor
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#ifndef MonitorHit_h
#define MonitorHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class MonitorHit : public G4VHit
{
public:
  
  MonitorHit();
  ~MonitorHit();
  MonitorHit(const MonitorHit&);
  const MonitorHit& operator=(const MonitorHit&);
  int operator==(const MonitorHit&) const;
  
  inline void* operator new(size_t);
  inline void  operator delete(void*);
  
  void Draw();
  void Print();
  
public:
  void SetTrackID    (G4int track)      { trackID = track; };
  void SetVolID      (G4int val)      { volID = val; };  
  void SetEdep       (G4double de)      { edep = de; };
  void SetPos        (G4ThreeVector xyz){ pos = xyz; };
  void SetGlobalT    (G4double time)    { globalT = time;};

  G4int GetTrackID()       { return trackID; };
  G4int GetVolID()       { return volID; };
  G4double GetEdep()       { return edep; };      
  G4ThreeVector GetPos()   { return pos; };
  G4double GetGlobalT()    { return globalT;};

private:
  G4int         trackID;
  G4int         volID;
  G4double      edep;
  G4ThreeVector pos;
  G4double      globalT;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<MonitorHit> MonitorHitsCollection;

extern G4Allocator<MonitorHit> MonitorHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* MonitorHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) MonitorHitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void MonitorHit::operator delete(void *aHit)
{
  MonitorHitAllocator.FreeSingle((MonitorHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


