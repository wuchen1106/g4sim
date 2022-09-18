//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
//

#include "G4GeometryManager.hh"

#include "MyElementField.hh"

//#include "F04GlobalField.hh"
#include "MyGlobalField.hh"

G4Navigator* MyElementField::aNavigator;

MyElementField::MyElementField(G4ThreeVector c, G4LogicalVolume* lv) {
    center = c;

    // AK20100812 Add access to bounding box points
    boundingBox=NULL;

    //G4cout << "-AK- MyElementField::MyElementField, center is " << c << G4endl;
    //G4cout << "-AK- MyElementField::MyElementField, volume name is "
    //<< lv->GetName() << G4endl;

    minX = minY = minZ = -DBL_MAX;
    maxX = maxY = maxZ =  DBL_MAX;

    MyGlobalField::getObject()->addElementField(this);
    //F04GlobalField::getObject()->addElementField(this);

    color = "1,1,1";

    userLimits = new G4UserLimits();

    lvolume = lv;
    //lvolume->SetVisAttributes(getVisAttribute(color));

    maxStep = 1*mm;

    userLimits->SetMaxAllowedStep(maxStep);

    userLimits->SetUserMaxTrackLength(500.*m);
    userLimits->SetUserMaxTime(10*ms);
    userLimits->SetUserMinEkine(0.1*MeV);
    //  userLimits->SetUserMinRange(1*mm);

    lvolume->SetUserLimits(userLimits);
}


void MyElementField::construct() {
    G4Navigator* theNavigator =
        G4TransportationManager::GetTransportationManager()->
        GetNavigatorForTracking();

//    G4cout << "-AK- In MyElementField::construct, lvolume is "
//        << lvolume->GetName() << G4endl;

    if (!aNavigator) {
        aNavigator = new G4Navigator();
        if ( theNavigator->GetWorldVolume() )
            aNavigator->SetWorldVolume(theNavigator->GetWorldVolume());
    }

    G4GeometryManager* geomManager = G4GeometryManager::GetInstance();

    if (!geomManager->IsGeometryClosed()) {
        geomManager->OpenGeometry();
        geomManager->CloseGeometry(true);
    }

    aNavigator->LocateGlobalPointAndSetup(center,0,false);
//    G4cout << "-AK- MyElementField::construct, center is " << center << G4endl;

    G4TouchableHistoryHandle fTouchable = aNavigator->
        CreateTouchableHistoryHandle();

    G4int depth = fTouchable->GetHistoryDepth();
//    G4cout << "-AK- fTouchable->GetHistoryDepth is " << depth << G4endl;

    for (G4int i = 0; i<depth; ++i) {
//        G4cout << "-AK- In MyElementField::construct, volume is "
 //           << fTouchable->GetVolume()->GetLogicalVolume()->GetName() << G4endl;
        if(fTouchable->GetVolume()->GetLogicalVolume() == lvolume) {
//            G4cout << "-AK- In MyElementField::construct, found lvolume "
//                << lvolume->GetName() << G4endl;
            break;
        }

        fTouchable->MoveUpHistory();
    }

    // set global2local transform
    global2local = fTouchable->GetHistory()->GetTopTransform();

    // set global bounding box
    G4double local[4], global[4];

    G4ThreeVector globalPosition;
    local[3] = 0.0;
    for (int i=0; i<2; ++i) {
        local[0] = (i==0 ? -1.0 : 1.0) * getMaxWidth()/2.;
        for (int j=0; j<2; ++j) {
            local[1] = (j==0 ? -1.0 : 1.0) * getMaxHeight()/2.;
            for (int k=0; k<2; ++k) {
                local[2] = (k==0 ? -1.0 : 1.0) * getMaxLength()/2.;
                G4ThreeVector localPosition(local[0],local[1],local[2]);
                globalPosition =
                    global2local.Inverse().TransformPoint(localPosition);
                global[0] = globalPosition.x();
                global[1] = globalPosition.y();
                global[2] = globalPosition.z();
                setGlobalPoint(global);
                //G4cout << "-AK- MyElementField::construct, bounding box global pos is"
                //     << globalPosition << G4endl;
            }
        }
    }
}


G4VisAttributes* MyElementField::getVisAttribute(G4String color) {
    G4VisAttributes* p = NULL;
    if(color.size() > 0 &&
    (isdigit(color.c_str()[0]) || color.c_str()[0] == '.')) {
        G4double red=0.0, green=0.0, blue=0.0;
        if (sscanf(color.c_str(),"%lf,%lf,%lf",&red,&green,&blue) == 3) {
            p = new G4VisAttributes(true,G4Color(red,green,blue));
        }
        else {
            G4cout << " Invalid color " << color << G4endl;
        }
    }

    if (!p) p = new G4VisAttributes(G4VisAttributes::GetInvisible());
    p->SetDaughtersInvisible(false);

    return p;
}


G4double const * MyElementField::getBoundingBox() {
    if(boundingBox==NULL) {
        boundingBox= new G4double[6];
    }
    boundingBox[0] = minX;
    boundingBox[1] = maxX;
    boundingBox[2] = minY;
    boundingBox[3] = maxY;
    boundingBox[4] = minZ;
    boundingBox[5] = maxZ;
    return boundingBox;
}


void MyElementField::setCenter(G4ThreeVector c) {
    center = c;
}
