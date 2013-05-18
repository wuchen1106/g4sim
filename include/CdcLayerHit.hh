//---------------------------------------------------------------------------//
//Description: Hit for CdcLayer
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#ifndef CdcLayerHit_h
#define CdcLayerHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class CdcLayerHit : public G4VHit
{
public:
  
  CdcLayerHit();
  ~CdcLayerHit();
  CdcLayerHit(const CdcLayerHit&);
  const CdcLayerHit& operator=(const CdcLayerHit&);
  int operator==(const CdcLayerHit&) const;
  
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

typedef G4THitsCollection<CdcLayerHit> CdcLayerHitsCollection;

extern G4Allocator<CdcLayerHit> CdcLayerHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* CdcLayerHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) CdcLayerHitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void CdcLayerHit::operator delete(void *aHit)
{
  CdcLayerHitAllocator.FreeSingle((CdcLayerHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


