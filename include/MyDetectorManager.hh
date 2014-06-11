//---------------------------------------------------------------------------//
//Description: Manage my SD classes
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#ifndef MyDetectorManager_h
#define MyDetectorManager_h 1

#include "myglobals.hh"

#include <vector>

//supported SD
#include "CdcSD.hh"
#include "CdcSimpleSD.hh"
#include "CdcIonSD.hh"
#include "MonitorSD.hh"
#include "CdcLayerSD.hh"
#include "KillerSD.hh"

//supported Svc
#include "SimpleGeometrySvc.hh"
#include "CdcGeometrySvc.hh"

#include "SimpleGeometryParameter.hh"

class G4SDManager;
class G4VSensitiveDetector;
class G4VPhysicalVolume;
class MyVGeometrySvc;
class MySD;
class MyDetectorManagerMessenger;

class MyDetectorManager
{
	public:
		MyDetectorManager();
		~MyDetectorManager();

		static MyDetectorManager* GetMyDetectorManager();

		//***************************************ReadCard********************************************
		//OBJECTIVE: Call recorded geometry service objects recursively to read input file for geometry settings
		//PROCEDURE:
		//0. delete existing geometry service objects
		//1. read $CONFIGUREROOT/file_name(or file_name), decide how many services are needed and what files they need.
		//2. create(or modify if exists) geometry service objects, push to vector, and call SetGeometry recursively with those file names.
		//NOTICE.1: file_name here should either be a complete file name with path or a pure file name w.r.t $CONFIGUREROOT,
		//  cause MyVGeometry calsses would prepend $CONFIGUREROOT to file_name if not found '/' in it.
		void ReadCard(G4String);
		void AddGeo(G4String, G4String, G4String);
		void ClearGeo();

		//***************************************SetGeometry********************************************
		//OBJECTIVE: Call recorded geometry service objects recursively to setup geometry
		//NOTICE.1: a return value is needed as the pointer to the physical volume of world.
		//  According to conventions, this pointer would be returned by the first geometry service object.
		G4VPhysicalVolume* SetGeometry();

		//**************************************ReadOutputCard********************************************
		//OBJECTIVE: Call recorded SD objects recursively to read the input file for output customising.
		//NOTICE.1: file_name here should either be a complete file name with path or a pure file name w.r.t $CONFIGUREROOT,
		//  cause MySD calsses would prepend $CONFIGUREROOT to file_name if not found '/' in it.
		void ReadOutputCard(G4String);

		//*****************************************SetBranch**********************************************
		//OBJECTIVE: Call recorded SD objects recursively to setup branches.
		void SetBranch();

		//************************GetSD*********************************
		//OBJECTIVE: Get the wanted SD. Create if not exists.
		//VolName: name of logical volume.
		//SDName: type of sensitive detector.
		//        NOTICE: if a '/' is found in SDName, then SDName would be considered as "VolName/SDName"
		//                and the parameter VolName would be ignored.
		//pointer: the pointer to the geometry service object which managed this volume.
		//PROCEDURE:
		//1. Search for (VolName, SDName).
		//2. If this pair exists, then return the corresponding G4VSensitiveDetector*
		//   If not, then record this pair, new a G4VSensitiveDetector according to the type(SDName),
		//     push the pointer to a vector, and return the pointer.
		//NOTICE.1: All sensitive detector classes used here are assumed to inherit from both 
		//  G4VSensitiveDetector and MySD, thus SetParameter is crucial.
		//NOTICE.2: Here I use VolName + SDName as the name of SD object to allow different volumes to use
		//  the same type of SD while keeping different copies( In my case, they would create different branches in output file).
		G4VSensitiveDetector* GetSD(G4String, G4String, MyVGeometrySvc* );
		G4VSensitiveDetector* GetSD(G4String, G4String);

		//*************************GetSvc********************************
		MyVGeometrySvc* GetSvc( G4String name ){
			MyVGeometrySvc* pointer = 0;
			for ( int i = 0; i < fSvcName.size(); i++ ){
				if ( fSvcName[i] == name ){
					pointer = fMyVGeometrySvc[i];
					break;
				}
			}
			return pointer;
		}

		//*************************GetSvc********************************
		SimpleGeometryParameter* GetParaFromVolume( G4String name ){
			SimpleGeometryParameter* pointer = 0;
			for ( int i = 0; i < fMyVGeometrySvc.size(); i++ ){
				SimpleGeometryParameter* ipointer = (SimpleGeometryParameter*) fMyVGeometrySvc[i]->get_GeometryParameter();
				if ( ipointer->get_VolIndex(name) >= 0 ){
					pointer = ipointer;
					break;
				}
			}
			return pointer;
		}

		//**************************************RegistorDM & Digitize************************************************
		//OBJECTIVE: For digitizing control.
		//Not well supported yet...
		void RegistorDM(G4String);

		void Digitize();

		//==> Access
		void SetVerbose(int val){fVerboseLevel = val;};

	private:

		void DEBUG(G4String);

	private:

		static MyDetectorManager* fMyDetectorManager;

		MyDetectorManagerMessenger* m_MyDetectorManagerMessenger;  //pointer to the Messenger

		//For MySD

		G4SDManager* fSDman;

		std::vector<MySD*> fSDList;
		std::vector<G4String> fSDName;
		std::vector<G4String> fVolName;
		std::vector<G4String> fDMName;

		//For MyVGeometrySvc

		int fVerboseLevel;
		std::vector<MyVGeometrySvc*> fMyVGeometrySvc;
		std::vector<G4String>        fSvcName;

};

#endif

