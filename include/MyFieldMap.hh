// AK20110616 Class to implement a 3D field map
//            Modified from BLCMDfieldmap.cc from G4beamline v2.03
//
#ifndef MyFieldMap_hh
#define MyFieldMap_hh 1

#include "G4LogicalVolume.hh"

#include "MyElementField.hh"
#include "MyGlobalField.hh"
#include "MyBLFieldMap.hh"

class MyFieldMap : public MyElementField {
    public:
        //Constructor
        MyFieldMap(G4String fieldMapFile, G4double current, G4double gradient,
            G4double timeOffset, G4LogicalVolume* fieldVolume);

        //Destructor
        ~MyFieldMap();

        // addFieldValue() adds the field for this map to the values in field[].
        // point[] is in global coordinates.
        void addFieldValue(const G4double point[4], G4double field[6]) const;

        // Set rotation matrix of placed instance
        void setRot(G4RotationMatrix * aRotMat);

        ///  return the length used for the bounding box
        virtual G4double getMaxLength() { return fMaxLength; }

        ///  return the width used for the bounding box
        virtual G4double getMaxWidth() { return fMaxWidth; }

        ///  return the height used for the bounding box
        virtual G4double getMaxHeight() { return fMaxHeight; }

    private:
        G4RotationMatrix *fRotation;
        G4double fCurrent;
        G4double fGradient;
        G4double fTimeOffset;
        MyBLFieldMap *fMap;

        G4double fMaxLength;
        G4double fMaxWidth;
        G4double fMaxHeight;

};
#endif
