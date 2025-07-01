#include "G4RunManager.hh"
#include "G4Material.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4Navigator.hh"
#include "G4TransportationManager.hh"
#include "TTree.h"
#include "TFile.h"
#include "TH2F.h"
#include <iostream>
#include <map>

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
    void AddY4ZX(float iy){
        y4ZX[iy] = true;
    }
    void AddZ4XY(float iz){
        z4XY[iz] = true;
    }
    void AddX4ZY(float ix){
        x4ZY[ix] = true;
    }
    bool checkZX(float x,float y,float z){
        if (ready4ZX&&y4ZX[y]&&x>=xmin&&x<=xmax&&z>=zmin&&z<=zmax) return true;
        return false;
    };
    bool checkZY(float x,float y,float z){
        if (ready4ZY&&x4ZY[x]&&y>=ymin&&y<=ymax&&z>=zmin&&z<=zmax) return true;
        return false;
    };
    bool checkXY(float x,float y,float z){
        if (ready4XY&&z4XY[z]&&y>=ymin&&y<=ymax&&x>=xmin&&x<=xmax) return true;
        return false;
    };


    int GridScan(const G4String& outputFilename){
        G4cout << "Grid scan started" << G4endl;

        float x = 0;
        float y = 0;
        float z = 0;

        int nx = (xmax-xmin)/dx+1;
        int ny = (ymax-ymin)/dy+1;
        int nz = (zmax-zmin)/dz+1;

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

        std::map<float,TH2F*> hmatZX;
        for (std::map<float,bool>::const_iterator it = y4ZX.begin(); it!=y4ZX.end(); it++){
            if (it->second){
                hmatZX[it->first] = new TH2F(Form("hmatZX%.0f",it->first),Form("Material map on the y = %.0f mm Plane;z [mm];x [mm]",it->first),nz,zmin-dz/2,zmax+dz/2,ny,xmin-dx/2,xmax+dx/2);
            }
        }
        std::map<float,TH2F*> hmatZY;
        for (std::map<float,bool>::const_iterator it = x4ZY.begin(); it!=x4ZY.end(); it++){
            if (it->second){
                hmatZY[it->first] = new TH2F(Form("hmatZY%.0f",it->first),Form("Material map on the x = %.0f mm Plane;z [mm];y [mm]",it->first),nz,zmin-dz/2,zmax+dz/2,ny,ymin-dy/2,ymax+dy/2);
            }
        }
        std::map<float,TH2F*> hmatXY;
        for (std::map<float,bool>::const_iterator it = z4XY.begin(); it!=z4XY.end(); it++){
            if (it->second){
                hmatXY[it->first] = new TH2F(Form("hmatXY%.0f",it->first),Form("Material map on the z = %.0f mm Plane;x [mm];y [mm]",it->first),nx,xmin-dx/2,xmax+dx/2,ny,ymin-dy/2,ymax+dy/2);
            }
        }

        Long64_t count = 0;
        Long64_t totalBins = nx*nz+ny*nz;
        for (G4int ix = 0; ix < nx; ++ix) {
            x = xmin+ix*dx;
            for (G4int iy = 0; iy < ny; ++iy) {
                y = ymin+iy*dy;
                for (G4int iz = 0; iz < nz; ++iz) {
                    z = zmin+iz*dz;

                    // should skip?
                    bool isZX = checkZX(x,y,z);
                    bool isZY = checkZY(x,y,z);
                    bool isXY = checkXY(x,y,z);
                    if (!isZX&&!isZY&&!isXY) continue;
                    count++;
                    if (count%1000000==0) std::cerr<<100.*count/totalBins<<"%"<<std::endl;
                
                    G4ThreeVector point(x, y, z);
                    G4Material* material = fNavigator->LocateGlobalPointAndSetup(point)->GetLogicalVolume()->GetMaterial();
                    float density = material->GetDensity()/(g/cm3);

                    // set output
                    if (isZX){
                        hmatZX[y]->SetBinContent(iz,ix,density);
                    }
                    if (isZY){
                        hmatZY[x]->SetBinContent(iz,iy,density);
                    }
                    if (isXY){
                        hmatXY[z]->SetBinContent(ix,iy,density);
                    }
                }
            }
        }

        for (std::map<float,bool>::const_iterator it = y4ZX.begin(); it!=y4ZX.end(); it++){
            if (it->second){
                hmatZX[it->first]->Write();
            }
        }
        for (std::map<float,bool>::const_iterator it = x4ZY.begin(); it!=x4ZY.end(); it++){
            if (it->second){
                hmatZY[it->first]->Write();
            }
        }
        for (std::map<float,bool>::const_iterator it = z4XY.begin(); it!=z4XY.end(); it++){
            if (it->second){
                hmatXY[it->first]->Write();
            }
        }
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
    std::map<float,bool> y4ZX;
    std::map<float,bool> z4XY;
    std::map<float,bool> x4ZY;
};
