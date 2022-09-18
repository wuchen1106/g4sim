#ifndef MyString2Anything_hh
#define MyString2Anything_hh 1

#include "myglobals.hh"
#include "G4UnitsTable.hh"

#include <string>
#include <sstream>

class MyString2Anything
{
  public:
    MyString2Anything();
    ~MyString2Anything();

		static void get_SD( G4String cont, G4String& m_str, G4double& m_dou ){
			std::stringstream buf(cont);
			buf>>m_str>>m_dou;
		}

		static void get_SDDDD( G4String cont, G4String& m_str, G4double& m_dou1, G4double& m_dou2, G4double& m_dou3, G4double& m_dou4){
			std::stringstream buf(cont);
			buf>>m_str>>m_dou1>>m_dou2>>m_dou3>>m_dou4;
		}

		static void get_SDUI( G4String cont, G4String& m_str, G4double& m_dou, G4int& m_int){
			G4String m_unit;
			std::stringstream buf(cont);
			buf>>m_str>>m_dou>>m_unit>>m_int;
			m_dou *= G4UnitDefinition::GetValueOf(m_unit);
		}

		static void get_SIDU( G4String cont, G4String& m_str, G4int& m_int, G4double& m_dou ){
			G4String m_unit;
			std::stringstream buf(cont);
			buf>>m_str>>m_int>>m_dou>>m_unit;
			m_dou *= G4UnitDefinition::GetValueOf(m_unit);
		}

		static void get_SIDUI( G4String cont, G4String& m_str, G4int& m_int, G4double& m_dou, G4int& m_int2 ){
			G4String m_unit;
			std::stringstream buf(cont);
			buf>>m_str>>m_int>>m_dou>>m_unit>>m_int2;
			m_dou *= G4UnitDefinition::GetValueOf(m_unit);
		}

		static void get_SIIDU( G4String cont, G4String& m_str, G4int& m_int1, G4int& m_int2, G4double& m_dou ){
			G4String m_unit;
			std::stringstream buf(cont);
			buf>>m_str>>m_int1>>m_int2>>m_dou>>m_unit;
			m_dou *= G4UnitDefinition::GetValueOf(m_unit);
		}

		static void get_SIB( G4String cont, G4String& m_str, G4int& m_int, bool& m_bool ){
			std::string val;
			std::stringstream buf(cont);
			buf>>m_str>>m_int>>val;
			m_bool = ConvertToBool(val);
		}

		static void get_SID( G4String cont, G4String& m_str, G4int& m_int, G4double& m_dou ){
			std::stringstream buf(cont);
			buf>>m_str>>m_int>>m_dou;
		}

		static void get_SI( G4String cont, G4String& m_str, G4int& m_int ){
			std::stringstream buf(cont);
			buf>>m_str>>m_int;
		}

		static void get_SII( G4String cont, G4String& m_str, G4int& m_int, G4int& m_int2 ){
			std::stringstream buf(cont);
			buf>>m_str>>m_int>>m_int2;
		}

		static void get_SB( G4String cont, G4String& m_str, G4bool& m_bool){
			std::stringstream buf(cont);
			buf>>m_str>>m_bool;
		}

		static void get_SS( G4String cont, G4String& m_str, G4String& m_str2 ){
			std::stringstream buf(cont);
			buf>>m_str>>m_str2;
		}

		static void get_SSS( G4String cont, G4String& m_str, G4String& m_str2, G4String& m_str3 ){
			std::stringstream buf(cont);
			buf>>m_str>>m_str2>>m_str3;
		}

		static void get_SIS( G4String cont, G4String& m_str, G4int& m_int, G4String& m_str2 ){
			std::stringstream buf(cont);
			buf>>m_str>>m_int>>m_str2;
		}

		static void get_IS ( G4String cont, G4int& m_int, G4String& m_str ){
			std::stringstream buf(cont);
			buf>>m_int>>m_str;
		}

		static void get_I ( G4String cont, G4int& m_int){
			std::stringstream buf(cont);
			buf>>m_int;
		}

		static void get_IDU( G4String cont, G4int& m_int, G4double& m_dou ){
			G4String m_unit;
			std::stringstream buf(cont);
			buf>>m_int>>m_dou>>m_unit;
			m_dou *= G4UnitDefinition::GetValueOf(m_unit);
		}

		static void get_DU( G4String cont, G4double& m_dou ){
			G4String m_unit;
			std::stringstream buf(cont);
			buf>>m_dou>>m_unit;
			m_dou *= G4UnitDefinition::GetValueOf(m_unit);
		}

		static void get_IIDU( G4String cont, G4int& m_int1, G4int& m_int2, G4double& m_dou ){
			G4String m_unit;
			std::stringstream buf(cont);
			buf>>m_int1>>m_int2>>m_dou>>m_unit;
			m_dou *= G4UnitDefinition::GetValueOf(m_unit);
		}

		static void get_DDDU( G4String cont, G4double& m_dou1, G4double& m_dou2, G4double& m_dou3 ){
			G4String m_unit;
			std::stringstream buf(cont);
			buf>>m_dou1>>m_dou2>>m_dou3>>m_unit;
			m_dou1 *= G4UnitDefinition::GetValueOf(m_unit);
			m_dou2 *= G4UnitDefinition::GetValueOf(m_unit);
			m_dou3 *= G4UnitDefinition::GetValueOf(m_unit);
		}

		static double get_U( G4String cont ){
			double val = G4UnitDefinition::GetValueOf(cont);
			if (!val){
				std::cout<<"WARNING: value of unit ("<<cont<<") is 0!!! will set it to 1!!!"<<std::endl;
				val = 1;
			}
			return val;
		}

		static void get_ID( G4String cont, G4int& m_int, G4double& m_dou ){
			std::stringstream buf(cont);
			buf>>m_int>>m_dou;
		}

		static void get_IB( G4String cont, G4int& m_int, bool& m_bool){
			G4String t_str;
			std::stringstream buf(cont);
			buf>>m_int>>t_str;
			m_bool = ConvertToBool(t_str);
		}

		static G4bool ConvertToBool(G4String st)
		{
			st.toUpper();
			G4bool vl = false;
			if( st=="Y" || st=="YES" || st=="1" || st=="T" || st=="TRUE" )
			{ vl = true; }
			return vl;
		}

};

#endif
