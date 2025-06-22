#include "G4RunManager.hh"
#include "G4Material.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4Navigator.hh"
#include "G4TransportationManager.hh"
#include "TTree.h"
#include "TFile.h"
#include <iostream>

class MaterialDensityScanner {
public:
    // Constructor
    MaterialDensityScanner() {
        // Get the navigator instance
        fNavigator = G4TransportationManager::GetTransportationManager()
                    ->GetNavigatorForTracking();
    }

    void SetX(float idx, float ixmin, float ixmax){
        dx = idx; xmin = ixmin; xmax = ixmax;
    }
    void SetY(float idy, float iymin, float iymax){
        dy = idy; ymin = iymin; ymax = iymax;
    }
    void SetZ(float idz, float izmin, float izmax){
        dz = idz; zmin = izmin; zmax = izmax;
    }
    void AddZ4XY(float iz){
        z4XY[iz] = true;
    }
    void AddX4ZY(float ix){
        x4ZY[ix] = true;
    }

    int GridScan(const G4String& outputFilename){
        G4cout << "Grid scan started" << G4endl;
        ready4ZX = false;
        ready4ZY = false;
        ready4XY = false;

        if (ymax>ymin){
            if (zmax>zmin){
                std::cout<<"Axis setting ready for ZY scanning"<<std::endl;
                ready4ZY = true;
            }
            if (xmax>xmin){
                std::cout<<"Axis setting ready for XY scanning"<<std::endl;
                ready4XY = true;
            }
        }
        if (zmax>zmin&&xmax>xmin){
            std::cout<<"Axis setting ready for ZX scanning"<<std::endl;
            ready4ZX = true;
        }

        if (!ready4ZX&&!ready4ZY&&!ready4XY){
            std::cout<<"Should provide valid ranges for at least two axes"<<std::endl;
            return -3;
        }
        
        TFile * ofile = new TFile(outputFilename,"recreate");
        TTree * otree = new TTree("t","t");
        float x = 0;
        float y = 0;
        float z = 0;
        float d = 0;
        otree->Branch("x",&x);
        otree->Branch("y",&y);
        otree->Branch("z",&z);
        otree->Branch("d",&d);

        int nx = (xmax-xmin)/dx+1;
        int ny = (ymax-ymin)/dy+1;
        int nz = (zmax-zmin)/dz+1;

        for (G4int ix = 0; ix < nx; ++ix) {
            x = xmin+ix*dx;
            
            for (G4int iy = 0; iy < ny; ++iy) {
                y = ymin+iy*dy;
                
                for (G4int iz = 0; iz < nz; ++iz) {
                    z = zmin+iz*dz;
                    
                    G4ThreeVector point(x, y, z);
                    G4Material* material = fNavigator->LocateGlobalPointAndSetup(point)->GetLogicalVolume()->GetMaterial();
                    
                    d = material->GetDensity()/(g/cm3);
                    otree->Fill();
                }
            }
        }
        otree->Write();
        ofile->Close();

        G4cout << "Grid scan completed. Results saved to " << outputFilename << G4endl;
        return 0;
    }

private:
    G4Navigator* fNavigator;
    float xmin = 0;
    float xmax = 0;
    float dx = 1;
    float ymin = 0;
    float ymax = 0;
    float dy = 1;
    float zmin = 0;
    float zmax = 0;
    float dz = 1;
    bool ready4ZX = false;
    bool ready4ZY = false;
    bool ready4XY = false;
    std::map<float,bool> z4XY;
    std::map<float,bool> x4ZY;
};
