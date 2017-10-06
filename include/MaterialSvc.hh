//---------------------------------------------------------------------------//
//Description: Setup Material
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#ifndef MaterialSvc_h
#define MaterialSvc_h 1

#include "myglobals.hh"

class MaterialSvc
{
  public:
    MaterialSvc();
     ~MaterialSvc();

    static MaterialSvc* GetMaterialSvc();

    void SetMaterial(G4String name);

  private:

    void AddMaterial( G4String content );

    static MaterialSvc* fMaterialSvc;

    G4String fMode;

};

#endif

