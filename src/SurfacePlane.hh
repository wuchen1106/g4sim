#ifndef SurfacePlane_h
#define SurfacePlane_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4Step.hh"
#include "TH2F.h"
#include "TString.h"
#include <vector>

class SurfacePlane {
public:
  SurfacePlane(const G4String& name,
               const G4ThreeVector& pos,
               const G4ThreeVector& normal,
               const G4ThreeVector& vx,
               const G4ThreeVector& vy,
               G4double xmin, G4double xmax, G4double ymin, G4double ymax,
               G4int nBinX, G4int nBinY)
  : position(pos), normal(normal.unit()),
    ex(vx.unit()), ey(vy.unit()),
    xmin(xmin), xmax(xmax),
    ymin(ymin), ymax(ymax),
    nBinX(nBinX), nBinY(nBinY)
  {
    // Make TH2F
    for (int i = 0; i<nP; i++){
        G4String fullname = Form("%s_%d",name.c_str(),pids[i]);
        hist[pids[i]] = new TH2F(fullname,
                Form("H*(10) per crossing [pSv], pid = %d",pids[i]),
                nBinX, xmin, xmax,
                nBinY, ymin, ymax);
    }
    cm2PerGrid = (xmax-xmin)/nBinX*(ymax-ymin)/nBinY/10*10;
  }

  ~SurfacePlane() {}

  bool IsCrossing(const G4Step* step) const {
    G4ThreeVector p1 = step->GetPreStepPoint()->GetPosition();
    G4ThreeVector p2 = step->GetPostStepPoint()->GetPosition();
    G4double d1 = (p1 - position).dot(normal);
    G4double d2 = (p2 - position).dot(normal);
    return (d1==0||d1 * d2 < 0.0);
  }

  void Fill(const G4Step* step, int pid, G4double dose) {
      G4ThreeVector p1 = step->GetPreStepPoint()->GetPosition();
      G4ThreeVector p2 = step->GetPostStepPoint()->GetPosition();

      G4ThreeVector n = normal;
      G4ThreeVector p0 = position; // plane point

      G4ThreeVector crossPoint;

      G4double denom = (p2 - p1).dot(n);
      if (std::abs(denom) < 1e-9) {
          // Parallel — fallback
          crossPoint = 0.5 * (p1 + p2);
      }

      G4double s = - (p1 - p0).dot(n) / denom;

      G4double costheta = fabs((p1-p0).unit().dot(n));

      crossPoint = p1 + s * (p2 - p1);

      G4ThreeVector rel = crossPoint - position;
      double x = rel.dot(ex);
      double y = rel.dot(ey);
      if (x > xmax || x< xmin || y < ymin || y > ymax) return;

      if (hist[0]) hist[0]->Fill(x, y, dose/cm2PerGrid/costheta);
      if (hist[pid]) hist[pid]->Fill(x, y, dose/cm2PerGrid/costheta);
  }

  void Write() {
      for (int i = 0; i<nP; i++){
          hist[pids[i]]->Write();
      }
  }

  G4ThreeVector position;
  G4ThreeVector normal;
  G4ThreeVector ex, ey;

  G4double xmin, xmax;
  G4double ymin, ymax;
  G4double cm2PerGrid;
  G4int nBinX, nBinY;

  std::map<int, TH2F*> hist;

  static const int nP = 12;
  const int pids[nP] = {0,2112,22,2212,11,-11,13,-13,211,-211,321,-321};
};

#endif
