#ifndef ThinSlab_h
#define ThinSlab_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4Step.hh"
#include "TH2F.h"
#include "TString.h"
#include <vector>

class ThinSlab {
    public:
        ThinSlab(const G4String& name,
                const G4ThreeVector& pos,
                const G4ThreeVector& normal,
                const G4ThreeVector& vx,
                const G4ThreeVector& vy,
                G4double xmin, G4double xmax, G4double ymin, G4double ymax,
                G4int nBinX, G4int nBinY, G4double thickness)
            : position(pos), normal(normal.unit()),
            ex(vx.unit()), ey(vy.unit()),
            xmin(xmin), xmax(xmax),
            ymin(ymin), ymax(ymax),
            nBinX(nBinX), nBinY(nBinY), halfThickness(thickness/2.)
        {
            // Make TH2F
            for (int i = 0; i<nP; i++){
                G4String fullname = Form("%s_%d",name.c_str(),pids[i]);
                hist[pids[i]] = new TH2F(fullname,
                        Form("H*(10) per crossing [pSv], pid = %d",pids[i]),
                        nBinX, xmin, xmax,
                        nBinY, ymin, ymax);
            }
            cm2PerGrid = (xmax-xmin)/nBinX*(ymax-ymin)/nBinY/10/10;
        }

        ~ThinSlab() {}

        bool Fill(const G4Step* step, int pid, G4double coeff_pSvcm2) {
            G4ThreeVector p1 = step->GetPreStepPoint()->GetPosition();
            G4ThreeVector p2 = step->GetPostStepPoint()->GetPosition();
            G4double d1 = (p1 - position).dot(normal);
            G4double d2 = (p2 - position).dot(normal);
            if ((d1>halfThickness&&d2>halfThickness)||(d1<-halfThickness&&d2<-halfThickness)){
                // not inside the slab
                return false;
            }
            
            G4double stepLength = (p2 - p1).mag();

            G4double loopStepSize = 1.0; // mm

            G4ThreeVector dir = (p2-p1).unit();

            G4int nLoops = static_cast<G4int>(stepLength / loopStepSize);

            G4ThreeVector curPos = p1;

            for (int i = 0; i<=nLoops; i++){ // loop inside the step by moving 1 mm. Maximum step acceptable: 10 meters
                double d = (curPos - position).dot(normal);
                G4ThreeVector rel = curPos - position;
                double x = rel.dot(ex);
                double y = rel.dot(ey);
                curPos+=dir*loopStepSize;
                if (fabs(d)>halfThickness) continue; // not insid
                G4double dL = loopStepSize;
                if (i==nLoops) dL = stepLength-nLoops*loopStepSize;
                if (x > xmax || x< xmin || y < ymin || y > ymax) continue; // out of the sampline plane
                if (hist[0]) hist[0]->Fill(x, y, coeff_pSvcm2*dL/cm2PerGrid/halfThickness/2);// 1 is 1 mm step, cm2PerGrid*halfThickness*2 is volume size
                if (hist[pid]) hist[pid]->Fill(x, y, coeff_pSvcm2*dL/cm2PerGrid/halfThickness/2);
            }
            return true;
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
        G4double halfThickness;

        std::map<int, TH2F*> hist;

        static const int nP = 12;
        const int pids[nP] = {0,2112,22,2212,11,-11,13,-13,211,-211,321,-321};
};

#endif
