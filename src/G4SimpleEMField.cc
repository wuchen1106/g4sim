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
// $Id: G4SimpleEMField.cc 96678 2016-04-29 16:21:01Z gcosmo $
//
// --------------------------------------------------------------------

#include "G4SimpleEMField.hh"

G4SimpleEMField::G4SimpleEMField():
    fEleCylinderVoltageScaled(0)
{
    fFieldComponents[0] = 0;
    fFieldComponents[1] = 0;
    fFieldComponents[2] = 0;
    fFieldComponents[3] = 0;
    fFieldComponents[4] = 0;
    fFieldComponents[5] = 0;
}

G4SimpleEMField::~G4SimpleEMField()
{
}

G4SimpleEMField::G4SimpleEMField(const G4SimpleEMField &p)
  : G4ElectroMagneticField(p)
{
    for (G4int i=0; i<6; i++)
    {
        fFieldComponents[i] = p.fFieldComponents[i];
    }
    fEleCylinderVoltageScaled = p.fEleCylinderVoltageScaled;
}

void G4SimpleEMField::SetEleUniformVector(const G4ThreeVector FieldVector )
{
    fFieldComponents[3] += FieldVector.x();
    fFieldComponents[4] += FieldVector.y();
    fFieldComponents[5] += FieldVector.z();
}

void G4SimpleEMField::SetEleUniformVector(G4double vField,
                                     G4double vTheta,
                                     G4double vPhi    )
{
    fFieldComponents[3] += vField*std::sin(vTheta)*std::cos(vPhi) ;
    fFieldComponents[4] += vField*std::sin(vTheta)*std::sin(vPhi) ;
    fFieldComponents[5] += vField*std::cos(vTheta) ;
}

void G4SimpleEMField::SetMagUniformVector(const G4ThreeVector FieldVector )
{
    fFieldComponents[0] += FieldVector.x();
    fFieldComponents[1] += FieldVector.y();
    fFieldComponents[2] += FieldVector.z();
}
   
void G4SimpleEMField::SetMagUniformVector(G4double vField,
                                     G4double vTheta,
                                     G4double vPhi    )
{
    fFieldComponents[0] += vField*std::sin(vTheta)*std::cos(vPhi) ;
    fFieldComponents[1] += vField*std::sin(vTheta)*std::sin(vPhi) ;
    fFieldComponents[2] += vField*std::cos(vTheta) ;
}

G4SimpleEMField& G4SimpleEMField::operator = (const G4SimpleEMField &p)
{
  if (&p == this) return *this;
  fEleCylinderVoltageScaled = p.fEleCylinderVoltageScaled;
  for (G4int i=0; i<6; i++)
  {
    fFieldComponents[i] = p.fFieldComponents[i];
  }
  return *this;
}

void G4SimpleEMField::GetFieldValue( const G4double Point[4],
        G4double *field ) const{

    // Uniform magnetic field and electric field
    field[0] = fFieldComponents[0] ;
    field[1] = fFieldComponents[1] ;
    field[2] = fFieldComponents[2] ;
    field[3] = fFieldComponents[3] ;
    field[4] = fFieldComponents[4] ;
    field[5] = fFieldComponents[5] ;

    // Cylinder electric field
    double r = sqrt(Point[0]*Point[0]+Point[1]*Point[1]);
    double E = fEleCylinderVoltageScaled/r;
    if (r==0) r = 1e-13;
    field[3] += E*Point[0]/r;
    field[4] += E*Point[1]/r;
}
