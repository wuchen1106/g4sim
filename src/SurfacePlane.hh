#ifndef SurfacePlane_h
#define SurfacePlane_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4Step.hh"
#include "TH2F.h"
#include <vector>

class SurfacePlane {
public:
  SurfacePlane(const G4String& name,
               const G4ThreeVector& pos,
               const G4ThreeVector& normal,
               G4double xmin, G4double xmax, G4double ymin, G4double ymax,
               G4int nBinX, G4int nBinY)
  : position(pos), normal(normal.unit()),
    xmin(xmin), xmax(xmax),
    ymin(ymin), ymax(ymax),
    nBinX(nBinX), nBinY(nBinY)
  {
    // Local X/Y axes
    ex = normal.orthogonal().unit();
    ey = normal.cross(ex);

    // Make TH2F
    hist = new TH2F(name,
                    "H*(10) per crossing [pSv]",
                    nBinX, xmin, xmax,
                    nBinY, ymin, ymax);
    cm2PerGrid = (xmax-xmin)/nBinX*(ymax-ymin)/nBinY/10*10;
  }

  ~SurfacePlane() {}

  bool IsCrossing(const G4Step* step) const {
    G4ThreeVector p1 = step->GetPreStepPoint()->GetPosition();
    G4ThreeVector p2 = step->GetPostStepPoint()->GetPosition();
    G4double d1 = (p1 - position).dot(normal);
    G4double d2 = (p2 - position).dot(normal);
    return (d1 * d2 < 0.0);
  }

  void Fill(const G4ThreeVector& hitPoint, G4double dose) {
    G4ThreeVector rel = hitPoint - position;
    double x = rel.dot(ex);
    double y = rel.dot(ey);
    if (x > xmax || x< xmin || y < ymin || y > ymax) return;

    hist->Fill(x, y, dose/cm2PerGrid);
  }

  void Write() {
    hist->Write();
  }

  G4ThreeVector position;
  G4ThreeVector normal;
  G4ThreeVector ex, ey;

  G4double xmin, xmax;
  G4double ymin, ymax;
  G4double cm2PerGrid;
  G4int nBinX, nBinY;

  TH2F* hist;
};

#endif
