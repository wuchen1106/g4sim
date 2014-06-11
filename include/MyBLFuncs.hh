// AK20100820 Various functions from G4beamline v2.03

#ifndef MYBLFUNCS_HH
#define MYBLFUNCS_HH

#include <vector>
#include <map>
#include <iostream>
#include <fstream>

#include "myglobals.hh"
#include "G4Types.hh"
#include "G4RotationMatrix.hh"
#include "G4VisAttributes.hh"
#include "G4Material.hh"

typedef std::vector<G4String> MyBLArgumentVector;
typedef std::map<G4String,G4String> MyBLArgumentMap;

class MyBLFuncs {
    static std::map<G4String,G4String> *paramMap;
    //static std::map<G4String,G4String> *paramHelpText;
    static void init();
    public:
        MyBLFuncs(){ init();}
        int parseArgs(const G4String &line, MyBLArgumentVector &argv,
            MyBLArgumentMap &namedArgs);
        G4String getString(G4String name);
        void setParam(G4String name, G4String value);

        void setParam(G4String name, G4double value);

        void setParam(G4String name, G4int value);
        G4String expand(G4String str);

    private:
        enum TokenType { NONE, ARGNAME, ARGVALUE };
        static G4String nextToken(const G4String& line, G4String::size_type& place,
            TokenType& type);
};

extern MyBLFuncs BLFuncs;                             /// the global BLFuncs object
#endif                                                // MYBLFUNCS_HH
