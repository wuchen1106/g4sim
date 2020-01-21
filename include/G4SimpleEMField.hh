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
// $Id: G4SimpleEMField.hh 66356 2012-12-18 09:02:32Z gcosmo $
//
//
// class G4SimpleEMField
//
// Class description:
//
// Electric Field abstract class, implements inquiry function interface.

// History:
// - Created. JA, November 4th, 2003.

#ifndef G4SIMPLEEM_FIELD_DEF
#define G4SIMPLEEM_FIELD_DEF

#include "G4Types.hh"
#include "G4ThreeVector.hh"
#include "G4ElectroMagneticField.hh"

class G4SimpleEMField : public G4ElectroMagneticField
{
  public:  // with description

     G4SimpleEMField();
     G4SimpleEMField(double v);
     virtual ~G4SimpleEMField();
       // Constructor and destructor. No actions.

     G4SimpleEMField(const G4SimpleEMField &r);
     G4SimpleEMField& operator = (const G4SimpleEMField &p);
       // Copy constructor & assignment operator.

     G4bool   DoesFieldChangeEnergy() const { if (fFieldComponents[3]||fFieldComponents[4]||fFieldComponents[5]||fEleCylinderVoltageScaled) return true; else return false; }
       // Since an electric field can change track energy

     void SetMagUniformVector(const G4ThreeVector FieldVector );

     void SetMagUniformVector(G4double vField,
                            G4double vTheta,
                            G4double vPhi     ) ;

     void SetEleUniformVector(const G4ThreeVector FieldVector );

     void SetEleUniformVector(G4double vField,
                            G4double vTheta,
                            G4double vPhi     ) ;

	 void SetEleCylinderVoltageScaled(double v){fEleCylinderVoltageScaled = v;};

     virtual void  GetFieldValue( const G4double Point[4],G4double *field ) const;

  private:
    
     G4double fEleCylinderVoltageScaled;

     G4double fFieldComponents[6] ;
};

#endif /* G4ELECTRIC_FIELD_DEF */
