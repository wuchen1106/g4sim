//Description: In charge of MyVGeometryParameter
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Modified:
//Comment: The units are "mm"(default) and "rad". 
//Comment: fVerboseLevel: 0:   Just Warning Messages and Error Messages
//                        >=5: All informations
//---------------------------------------------------------------------------//

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

#include "myglobals.hh"

#include "MyVGeometryParameterMessenger.hh"
#include "MyString2Anything.hh"

#include "MyVGeometryParameter.hh"

MyVGeometryParameter::MyVGeometryParameter(G4String name, G4String opt )
	: MyConfigure()
{
	if ( opt == "" ){
		std::cout<<"MyVGeometryParameter is a virtual class, should be called with opt not empty!"<<std::endl;
    G4Exception("MyVGeometryParameter::MyVGeometryParameter()","Run0031",
        FatalException, "illegal construction.");
	}
	set_Name(name);
}

MyVGeometryParameter::~MyVGeometryParameter(){
	std::cout<<"======>In ~MyVGeometryParameter, delete GeometryParameterMessenger at ("<<(void*)m_GeometryParameterMessenger<<")!"<<std::endl;
  delete m_GeometryParameterMessenger;
}

//--------------------------Special functions----------------------------------
bool MyVGeometryParameter::CheckInfo(){
	bool flag = false;// false means no problem

	//examine
	return flag;
}

//=> Preset
void MyVGeometryParameter::Preset(){
	fVerboseLevel = 0;
	checkoverlap = false;
}

//=> GetValue
int MyVGeometryParameter::GetValue(G4String s_card){
	int status = 0;
	std::stringstream buf_card;
	buf_card.str("");
	buf_card.clear();
	buf_card<<s_card;
	G4String name;
	buf_card>>name;
	G4String s_para;
	if( name == "VerboseLevel:" ) buf_card>>fVerboseLevel;
	else if( name == "checkoverlap" ) checkoverlap = true;
	else status = 1;
	buf_card.str("");
	buf_card.clear();
	return status;
}

//=>DumpInfo
void MyVGeometryParameter::DumpInfo() {
	std::cout<<"*********************************"<<m_Name<<" Geometry Info***********************************"<<std::endl;
	std::cout<<"------General info:--------"<<std::endl;
	std::cout<<" Initialized from \""<<m_filename<<"\""<<std::endl;
	std::cout<<" checkoverlap? "<<(checkoverlap?"yes":"no")<<std::endl;
	std::cout<<" VerboseLevel = "<<fVerboseLevel<<std::endl;
}

//--------------------------functions for common use----------------------------------
//=> get_RepCont
//"29" means replica number count from 0 to 28
//"2,29" means replica number count from 2 to 30
//"1-29" or "1~29" means replica number count from 1 to 29
void MyVGeometryParameter::get_RepCont( G4String RepCont, G4int& SRepNo, G4int& RepNo ){
	size_t sLast = RepCont.rfind(',');
	if(sLast!=G4String::npos){
		G4String part1 = RepCont.substr(0,sLast);
		G4String part2 = RepCont.substr(sLast+1,RepCont.length()-sLast-1);
		MyString2Anything::get_I(part1,SRepNo);
		MyString2Anything::get_I(part2,RepNo);
	}
	else{ // not found
		sLast = RepCont.rfind('-');
		if (sLast==G4String::npos){// not found
			sLast = RepCont.rfind('~');
		}
		if (sLast!=G4String::npos){
			G4String part1 = RepCont.substr(0,sLast);
			G4String part2 = RepCont.substr(sLast+1,RepCont.length()-sLast-1);
			MyString2Anything::get_I(part1,SRepNo);
			G4int endNo;
			MyString2Anything::get_I(part2,endNo);
			RepNo = endNo - SRepNo + 1;
		}
		else{// not found
			SRepNo = 0;
			MyString2Anything::get_I(RepCont,RepNo);
		}
	}
//	std::cout<<"in get_RepCont: \""<<RepCont<<"\" -> ("<<SRepNo<<","<<RepNo<<")"<<std::endl; // to be deleted
}

void MyVGeometryParameter::get_RepCont( G4String RepCont, G4String& SRepNo, G4String& RepNo ){
	size_t sLast = RepCont.rfind(',');
	if(sLast!=G4String::npos){
		SRepNo = RepCont.substr(0,sLast);
		RepNo = RepCont.substr(sLast+1,RepCont.length()-sLast-1);
	}
	else{ // not found
		sLast = RepCont.rfind('-');
		if (sLast==G4String::npos){// not found
			sLast = RepCont.rfind('~');
		}
		if (sLast!=G4String::npos){
			SRepNo = RepCont.substr(0,sLast);
			G4String endNo = RepCont.substr(sLast+1,RepCont.length()-sLast-1);
			RepNo = endNo+"-"+SRepNo+"+1";
		}
		else{// not found
			SRepNo = "0";
			RepNo = RepCont;
		}
	}
//	std::cout<<"in get_RepCont: \""<<RepCont<<"\" -> ("<<SRepNo<<","<<RepNo<<")"<<std::endl; // to be deleted
}

//=> ISEMPTY
//Tell whether this string is empty or commented(by # or //)
bool MyVGeometryParameter::ISEMPTY(G4String s_card){
	bool flag = false;
	const char* c_card = s_card.c_str();
	G4int length = strlen(c_card);
	G4int offset = 0;
	for ( ; offset < length; offset++ ){
		if ( c_card[offset] != ' ' ) break;
	}
	if ( c_card[offset] == '#' || (c_card[offset] == '/' && c_card[offset+1] == '/') || length - offset == 0 ){
		flag = true;
	}
	return flag;
}

