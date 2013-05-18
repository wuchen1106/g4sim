//---------------------------------------------------------------------------//
//Description: Hit for Monitor
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "MonitorHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4Allocator<MonitorHit> MonitorHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MonitorHit::MonitorHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MonitorHit::~MonitorHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  MonitorHit::MonitorHit(const MonitorHit& right)
: G4VHit()
{
  trackID   = right.trackID;
  volID     = right.volID;
  edep      = right.edep;
  pos       = right.pos;
  globalT   = right.globalT;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const MonitorHit& MonitorHit::operator=(const MonitorHit& right)
{
  trackID   = right.trackID;
  volID     = right.volID;
  edep      = right.edep;
  pos       = right.pos;
  globalT   = right.globalT;
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int MonitorHit::operator==(const MonitorHit& right) const
{
  return (this==&right) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MonitorHit::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager)
  {
    G4Circle circle(pos);
    circle.SetScreenSize(5.);
    circle.SetFillStyle(G4Circle::filled);
    G4Colour colour(1.,0.,0.);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);
    pVVisManager->Draw(circle);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MonitorHit::Print()
{
  G4cout << "  trackID: " << trackID 
    << "  volID: " << volID
    << "  energy deposit: " << G4BestUnit(edep,"Energy")
    << "  position: " << G4BestUnit(pos,"Length") 
    << "  globalT: " << G4BestUnit(globalT,"Time")
    <<G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

