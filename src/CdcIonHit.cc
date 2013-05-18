//---------------------------------------------------------------------------//
//Description: Track type hit class for COMET CDC
//Author: Yuan Ye(yuany@mail.ihep.ac.cn)
//Created: 22 July, 2012
//Modified:
//Comment: 
//---------------------------------------------------------------------------//

#include "CdcIonHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4Allocator<CdcIonHit> CdcIonHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CdcIonHit::CdcIonHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CdcIonHit::~CdcIonHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  CdcIonHit::CdcIonHit(const CdcIonHit& right)
: G4VHit()
{
  trackID   = right.trackID;
  layerNo   = right.layerNo;
  cellNo    = right.cellNo;
  edep      = right.edep;
  ccol      = right.ccol;
  pos       = right.pos;
  driftD    = right.driftD;
  driftT    = right.driftT;
  globalT   = right.globalT;
  theta     = right.theta;
  enterAngle= right.enterAngle;
  posFlag   = right.posFlag;  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const CdcIonHit& CdcIonHit::operator=(const CdcIonHit& right)
{
  trackID   = right.trackID;
  layerNo   = right.layerNo;
  cellNo    = right.cellNo;
  edep      = right.edep;
  ccol      = right.ccol;
  pos       = right.pos;
  driftD    = right.driftD;
  driftT    = right.driftT;
  globalT   = right.globalT;
  theta     = right.theta;
  enterAngle= right.enterAngle;
  posFlag   = right.posFlag;  
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int CdcIonHit::operator==(const CdcIonHit& right) const
{
  return (this==&right) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CdcIonHit::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager)
  {
    G4Circle circle(pos);
    circle.SetScreenSize(10.);
    circle.SetFillStyle(G4Circle::filled);
    G4Colour colour(1.,1.,0.);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);
    pVVisManager->Draw(circle);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CdcIonHit::Print()
{
  G4cout << "  trackID: " << trackID 
    << "  layerNo: " << layerNo
    << "  cellNo:  " << cellNo
    << "  energy deposit: " << G4BestUnit(edep,"Energy")
    << "  charge collected: " << G4BestUnit(ccol,"Charge")
    << "  position: " << G4BestUnit(pos,"Length") 
    << "  driftD: " << G4BestUnit(driftD,"Length")
    << "  driftT: " << G4BestUnit(driftT,"Time")
    << "  globalT: " << G4BestUnit(globalT,"Time")
    << "  theta: " << G4BestUnit(theta,"Angle")
    << "  enterAngle: " << G4BestUnit(enterAngle,"Angle")
    << "  posFlag: " << posFlag
    <<G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

