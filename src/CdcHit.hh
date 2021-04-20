//---------------------------------------------------------------------------//
//Description: Track type hit class for COMET CDC
//Author: Yuan Ye(yuany@mail.ihep.ac.cn)
//Created: 22 July, 2012
//Modified:
//Comment: 
//---------------------------------------------------------------------------//

#ifndef CdcHit_h
#define CdcHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class CdcHit : public G4VHit
{
public:
  
  CdcHit();
  ~CdcHit();
  CdcHit(const CdcHit&);
  const CdcHit& operator=(const CdcHit&);
  int operator==(const CdcHit&) const;
  
  inline void* operator new(size_t);
  inline void  operator delete(void*);
  
  void Draw();
  void Print();
  
public:
  void SetTrackID    (G4int track)      { trackID = track; };
  void SetLayerNo    (G4int layer)      { layerNo = layer; };  
  void SetCellNo     (G4int cell)       { cellNo =cell;};
  void SetEdep       (G4double de)      { edep = de; };
  void SetPos        (G4ThreeVector xyz){ pos = xyz; };
  void SetDriftD     (G4double distance){ driftD = distance;}; 
  void SetDriftT     (G4double time)    { driftT = time;}; 
  void SetGlobalT    (G4double time)    { globalT = time;};
  void SetTheta      (G4double angle)   { theta = angle;};
  void SetEnterAngle (G4double angle)   { enterAngle = angle;};
  void SetPosFlag    (G4int flag)       { posFlag = flag;};

  G4int GetTrackID()       { return trackID; };
  G4int GetLayerNo()       { return layerNo; };
  G4int GetCellNo()        { return cellNo;};
  G4double GetEdep()       { return edep; };      
  G4ThreeVector GetPos()   { return pos; };
  G4double GetDriftD()     { return driftD;}; 
  G4double GetDriftT()     { return driftT;};      
  G4double GetGlobalT()    { return globalT;};
  G4double GetTheta()      { return theta;};
  G4double GetEnterAngle() { return enterAngle;};
  G4int GetPosFlag()       { return posFlag;};

private:
  G4int         trackID;
  G4int         layerNo;
  G4int         cellNo;
  G4double      edep;
  G4ThreeVector pos;
  G4double      driftD;
  G4double      driftT;
  G4double      globalT;
  G4double      theta;
  G4double      enterAngle;
  G4int         posFlag;      
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<CdcHit> CdcHitsCollection;

extern G4Allocator<CdcHit> CdcHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* CdcHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) CdcHitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void CdcHit::operator delete(void *aHit)
{
  CdcHitAllocator.FreeSingle((CdcHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


