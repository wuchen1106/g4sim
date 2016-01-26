// AK20110616 Class to implement a 3D field map
//            Modified from BLCMDfieldmap.cc from G4beamline v2.03
//
//

#include <fstream>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "G4Tubs.hh"
#include "G4Box.hh"

#include "G4ElectroMagneticField.hh"
#include "MyFieldMap.hh"

MyFieldMap::MyFieldMap(G4String mapFileName, G4double current,
G4double gradient, G4double timeOffset, G4LogicalVolume* fieldVolume, G4double x, G4double y, G4double z)
: MyElementField(G4ThreeVector(0,0,0),fieldVolume) {
    // This constructor must be called after creating the logical
    // field volume and before defining any detectors in the field
    // volume.

    fMap = new MyBLFieldMap();
    fMap->SetOrigin(x,y,z);
    if (mapFileName.substr(mapFileName.size()-5,5)==".root")
		fMap->readFileROOT(mapFileName);
    else
		fMap->readFile(mapFileName);
	// FIXME: test the field
	//G4double local[4];
	//local[3] = 0;
	//G4double field[6];
	//for (int i = 0; i<100; i++){
	//	local[0] = 5*i/sqrt(2.);
	//	local[1] = 5*i/sqrt(2.);
	//	for (int j = 0; j<100; j++){
	//		local[2] = 5*j+5395;
	//		fMap->getFieldValue(local, field, current, gradient);
	//		std::cout<<local[0]<<" "
	//			     <<local[1]<<" "
	//			     <<local[2]<<" "
	//			     <<field[0]<<" "
	//			     <<field[1]<<" "
	//			     <<field[2]<<std::endl;
	//	}
	//}

    fCurrent = current;
    fGradient = gradient;
    fRotation = NULL;

    // AK20110616: timeOffset doesn't appear to be used for anything
    fTimeOffset = timeOffset;

    // Set dimensions
    if (fieldVolume->GetSolid()->GetEntityType() == "G4Tubs") {
        fMaxLength = 2*((G4Tubs*)fieldVolume->GetSolid())->GetZHalfLength();
        fMaxWidth = 2*((G4Tubs*)fieldVolume->GetSolid())->GetOuterRadius();
        fMaxHeight = fMaxWidth;
    }
    else if (fieldVolume->GetSolid()->GetEntityType()== "G4Box") {
        fMaxLength = 2*((G4Box*)fieldVolume->GetSolid())->GetZHalfLength();
        fMaxWidth = 2*((G4Box*)fieldVolume->GetSolid())->GetXHalfLength();
        fMaxHeight = 2*((G4Box*)fieldVolume->GetSolid())->GetYHalfLength();
    }

}


MyFieldMap::~MyFieldMap() {
    delete fRotation;
    delete fMap;
}


void MyFieldMap::addFieldValue(const G4double point[4], G4double field[6]) const {
    G4ThreeVector global(point[0],point[1],point[2]);
    G4ThreeVector local;

    local = global2local.TransformPoint(global);

    G4double relPoint[4]= {local[0],local[1],local[2],point[3]};
    G4double f[6];
    f[0] = f[1] = f[2] = f[3] = f[4] = f[5] = 0.0;
    fMap->getFieldValue(relPoint,f,fCurrent,fGradient);

    if(fRotation) {
        if (fMap->hasB()) {
            G4ThreeVector B(f[0],f[1],f[2]);
            B = *fRotation * B;
            field[0] += B[0];
            field[1] += B[1];
            field[2] += B[2];
        }
        if (fMap->hasE()) {
            G4ThreeVector E(f[3],f[4],f[5]);
            E = *fRotation * E;
            field[3] += E[0];
            field[4] += E[1];
            field[5] += E[2];
        }
    }
    else {
        if (fMap->hasB()) {
            field[0] += f[0];
            field[1] += f[1];
            field[2] += f[2];
        }
        if (fMap->hasE()) {
            field[3] += f[3];
            field[4] += f[4];
            field[5] += f[5];
        }
    }
}


void MyFieldMap::setRot(G4RotationMatrix * aRotMat) {
    // aRotMat is the rotation matrix used when placing a physical object
    fRotation = new G4RotationMatrix(aRotMat->inverse());
}
