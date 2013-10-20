//---------------------------------------------------------------------------//
//Description: In charge of MyVGeometryParameter
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Modified:
//Comment: The units are "mm"(default) and "rad". 
//Comment: fVerboseLevel: 0:    Just Warning Messages and Error Messages
//                        1:    + Dump()
//                        >= 5: All informations
//---------------------------------------------------------------------------//

#ifndef MyVGeometryParameter_H
#define MyVGeometryParameter_H 

#include "globals.hh"

#include <string>
#include <iostream>

class MyVGeometryParameterMessenger;

class MyVGeometryParameter{

	public:
		MyVGeometryParameter(G4String name, G4String opt = "");
		virtual ~MyVGeometryParameter(void);

		virtual void InitFromFile(G4String){};//Get necessary parameters from given file

		virtual void Calculate(){};

		virtual void Dump(){};

		//=>Special functions
		int GetValue(G4String);

		void Preset();

		bool CheckInfo();

		void DumpInfo();

		//=> functions for common use
		//"29" means replica number count from 0 to 28
		//"2,29" means replica number count from 2 to 30
		//"1-29" or "1~29" means replica number count from 1 to 29
		void get_RepCont(G4String RepCont, G4int& SRepNo, G4int& RepNo);

		bool ISEMPTY(G4String);

		void DEBUG(G4String content, int level = 1){
			if (level >= 1){
				G4Exception("MyVGeometryParameter","Run0031",
						FatalException, content);
			}
			else if (level == 0){
				std::cout<<"In MyVGeometryParameter, "<<content<<std::endl;
			}
		}

		//=>Access
		//general info
		bool get_checkoverlap(){return checkoverlap;}
		int get_VerboseLevel(){return fVerboseLevel;}
		G4String get_Name(){ return m_Name; }
		G4String get_filename(){ return m_filename; }

		//=>Modify
		//general info
		void set_checkoverlap(bool val) { checkoverlap = val;}
		void set_VerboseLevel(int val) { fVerboseLevel = val;}
		void set_GeometryParameterMessenger(MyVGeometryParameterMessenger* val) {m_GeometryParameterMessenger = val;};
		void set_Name(G4String val){ m_Name = val; }
		void set_filename(G4String val){ m_filename = val; }

	private:
		//general info
		bool checkoverlap;
		int fVerboseLevel;

		G4String m_filename;
		G4String m_Name;
		MyVGeometryParameterMessenger* m_GeometryParameterMessenger;  //pointer to the Messenger

};

#endif
