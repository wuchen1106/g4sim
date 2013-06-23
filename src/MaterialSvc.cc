//---------------------------------------------------------------------------//
//Description: Setup Material
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "MaterialSvc.hh"

#include "G4Material.hh"
#include "G4UnitsTable.hh"
#include "globals.hh"

#include <sstream>
#include <fstream>
#include <string.h>
#include <iostream>

#include "DEBUG.hh"

MaterialSvc* MaterialSvc::fMaterialSvc = 0;

MaterialSvc::MaterialSvc()
{
	if (fMaterialSvc){
		G4Exception("MaterialSvc::MaterialSvc()","Run0031",
				FatalException, "MaterialSvc constructed twice.");
	}
	fMaterialSvc = this;
}

MaterialSvc::~MaterialSvc()
{
	printf("~MaterialSvc\n");
}

MaterialSvc* MaterialSvc::GetMaterialSvc(){
	if ( !fMaterialSvc ){
		fMaterialSvc = new MaterialSvc;
	}
	return fMaterialSvc;
}

void MaterialSvc::SetMaterial( G4String file_name ){
	size_t sLast = file_name.last('/');
	if(sLast==std::string::npos){ // file name only
		G4String dir_name = getenv("CONFIGUREROOT");
		if (dir_name[dir_name.size()-1] != '/') dir_name.append("/");
		file_name = dir_name + file_name;
	}
	MAT_LINEVAR(file_name) // DEBUG.hh
	std::ifstream fin_card(file_name);
	if ( !fin_card ){
		std::cout<<"material list "<<file_name<<" is not available!!!"<<std::endl;
		G4Exception("MaterialSvc::SetMaterial()","Run0031",
				FatalException, "material list is not available.");
	}
	std::stringstream buf_card;
	std::string s_card;
	while(getline(fin_card,s_card)){
		buf_card.str("");
		buf_card.clear();
		buf_card<<s_card;
		const char* c = s_card.c_str();
		int length = strlen(c);
		int offset = 0;
		for ( ; offset < length; offset++ ){
			if ( c[offset] != ' ' ) break;
		}
		if ( c[offset] == '#' ) continue;
		else if ( c[offset] == '/' && c[offset+1] == '/' ) continue;
		else if ( length - offset == 0 ) continue;
		std::string keyword;
		buf_card>>keyword;
		if ( keyword == "TYPE" ){
			buf_card>>fMode;
			continue;
		}
		AddMaterial(s_card);
	}
	MAT_LINECONT("Finished adding") // DEBUG.hh
	fin_card.close();
	buf_card.str("");
	buf_card.clear();
	// print table
	//
	G4cout << *(G4Element::GetElementTable()) << G4endl;
	G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

void MaterialSvc::AddMaterial( G4String content ){
	G4Material *aMaterial = 0;
	G4String symbol;
	G4String name;
	G4String s_temp;
	double z = 0;
	double a = 0;
	double density = 0;
	int ncomponents = 0;
	int natoms[50];
	double comFrac[50];
	double rel_dens = 0;
	G4String material[50];
	G4String element[50];
	std::stringstream buf_card;
	buf_card.str("");
	buf_card.clear();
	buf_card<<content;
	MAT_LINEVAR(fMode) // DEBUG.hh
	if ( fMode == "Elements"){
		buf_card>>symbol;
		buf_card>>name;
		buf_card>>z;
		buf_card>>a;
		//    std::cout<<"symbol = "<<symbol<<", name = "<<name<<", a = "<<a<<", z = "<<z<<std::endl;
		new G4Element(name.c_str(), symbol.c_str(), z, a*g/mole );
	}
	else if ( fMode == "Simple_Materials" ){
		buf_card>>name;
		buf_card>>z;
		buf_card>>a;
		buf_card>>density;
		density = rel_dens*density*g/cm3;
		buf_card>>rel_dens;
		//    std::cout<<"name = "<<name<<", a = "<<a<<", z = "<<z<<", density = "<<density<<", rel_dens = "<<rel_dens<<std::endl;
		aMaterial = new G4Material(name.c_str(), z, a*g/mole, density);
	}
	else if ( fMode == "Molecule_Materials" ){
		buf_card>>name;
		buf_card>>density;
		density = rel_dens*density*g/cm3;
		buf_card>>rel_dens;
		buf_card>>ncomponents;
		for ( int i = 0; i < ncomponents; i++ ){
			buf_card>>element[i];
			buf_card>>natoms[i];
		}
		//    std::cout<<"name = "<<name<<", density = "<<density<<", rel_dens = "<<rel_dens<<", ncomponents = "<<ncomponents<<std::endl;
		//    for ( int i = 0; i < ncomponents; i++ ){
		//      std::cout<<"  "<<i<<": "<<"element = "<<element[i]<<", natoms = "<<natoms[i]<<std::endl;
		//    }
		aMaterial = new G4Material(name.c_str(), density, ncomponents);
		for ( int i = 0; i < ncomponents; i++ ){
			G4Element* new_ele = G4Element::GetElement(element[i]);
			aMaterial->AddElement(new_ele, natoms[i]);
		}
	}
	else if ( fMode == "MixEle_Materials" ){
		buf_card>>name;
		buf_card>>density;
		density = rel_dens*density*g/cm3;
		buf_card>>rel_dens;
		buf_card>>ncomponents;
		double sum_frac = 0;
		for ( int i = 0; i < ncomponents; i++ ){
			buf_card>>element[i];
			buf_card>>comFrac[i];
			sum_frac = sum_frac + comFrac[i];
		}
		//    std::cout<<"name = "<<name<<", density = "<<density<<", rel_dens = "<<rel_dens<<", ncomponents = "<<ncomponents<<std::endl;
		//    for ( int i = 0; i < ncomponents; i++ ){
		//      std::cout<<"  "<<i<<": "<<"element = "<<element[i]<<", comFrac = "<<comFrac[i]<<std::endl;
		//    }
		if ( sum_frac == 0 ){
			std::cout<<"Please check mass fractions for "<<name<<std::endl;
			G4Exception("MaterialSvc::AddMaterial()","Run0031",
					FatalException, "total fraction is zero.");
		}
		if ( sum_frac != 1 ){
			std::cout<<"the total fraction for "<<name<<" is not 1!!!"<<std::endl;
			std::cout<<"MaterialSvc will normalize it to 1!"<<std::endl;
			for ( int i = 0; i < ncomponents; i++ ){
				comFrac[i] = comFrac[i]/sum_frac;
			}
		}
		aMaterial = new G4Material(name.c_str(), density, ncomponents);
		for ( int i = 0; i < ncomponents; i++ ){
			G4Element* new_ele = G4Element::GetElement(element[i]);
			aMaterial->AddElement(new_ele, comFrac[i]);
		}
	}
	else if ( fMode == "Mixture_Materials" ){
		buf_card>>name;
		buf_card>>rel_dens;
		buf_card>>ncomponents;
		double sum_frac = 0;
		for ( int i = 0; i < ncomponents; i++ ){
			buf_card>>material[i];
			buf_card>>comFrac[i];
			sum_frac += comFrac[i];
		}
		//    std::cout<<"name = "<<name<<", rel_dens = "<<rel_dens<<", ncomponents = "<<ncomponents<<std::endl;
		if ( sum_frac == 0 ){
			std::cout<<"Please check mass fractions for "<<name<<std::endl;
			G4Exception("MaterialSvc::AddMaterial()","Run0031",
					FatalException, "total fraction is zero.");
		}
		if ( sum_frac != 1 ){
			std::cout<<"the total fraction for "<<name<<" is not 1!!!"<<std::endl;
			std::cout<<"MaterialSvc will normalize it to 1!"<<std::endl;
			for ( int i = 0; i < ncomponents; i++ ){
				comFrac[i] = comFrac[i]/sum_frac;
			}
		}
		for ( int i = 0; i < ncomponents; i++ ){
			G4Material* new_mat_com = G4Material::GetMaterial(material[i]);
			G4double i_density = new_mat_com->GetDensity();
			//      std::cout<<"  "<<i<<": "<<"material = "<<material[i]<<", comFrac = "<<comFrac[i]<<", density = "<<(i_density/(g/cm3))<<std::endl;
			density += comFrac[i]/(i_density/(g/cm3));
		}
		if ( density!=0 ){
			density = rel_dens/density*g/cm3;
		}
		aMaterial = new G4Material(name.c_str(), density, ncomponents);
		for ( int i = 0; i < ncomponents; i++ ){
			G4Material* new_mat_com = G4Material::GetMaterial(material[i]);
			aMaterial->AddMaterial(new_mat_com, comFrac[i]);
		}
	}
	else if ( fMode == "VolMix_Materials" ){
		buf_card>>name;
		buf_card>>rel_dens;
		buf_card>>ncomponents;
		double sum_frac = 0;
		for ( int i = 0; i < ncomponents; i++ ){
			buf_card>>material[i];
			buf_card>>comFrac[i];
			sum_frac += comFrac[i];
		}
		//    std::cout<<"name = "<<name<<", rel_dens = "<<rel_dens<<", ncomponents = "<<ncomponents<<std::endl;
		if ( sum_frac == 0 ){
			std::cout<<"Please check mass fractions for "<<name<<std::endl;
			G4Exception("MaterialSvc::AddMaterial()","Run0031",
					FatalException, "total fraction is zero.");
		}
		if ( sum_frac != 1 ){
			std::cout<<"the total fraction for "<<name<<" is not 1!!!"<<std::endl;
			std::cout<<"MaterialSvc will normalize it to 1!"<<std::endl;
			for ( int i = 0; i < ncomponents; i++ ){
				comFrac[i] = comFrac[i]/sum_frac;
			}
		}
		for ( int i = 0; i < ncomponents; i++ ){
			G4Material* new_mat_com = G4Material::GetMaterial(material[i]);
			G4double i_density = new_mat_com->GetDensity();
			//      std::cout<<"  "<<i<<": "<<"material = "<<material[i]<<", comFrac = "<<comFrac[i]<<", density = "<<(i_density/(g/cm3))<<std::endl;
			density += comFrac[i]*(i_density);
		}
		density = rel_dens*density;
		aMaterial = new G4Material(name.c_str(), density, ncomponents);
		for ( int i = 0; i < ncomponents; i++ ){
			G4Material* new_mat_com = G4Material::GetMaterial(material[i]);
			G4double i_density = new_mat_com->GetDensity();
			aMaterial->AddMaterial(new_mat_com, comFrac[i]*i_density/density);
		}
	}
	else{
		std::cout<<"mode "<<fMode<<" is not defined yet!!!"<<std::endl;
		G4Exception("MaterialSvc::AddMaterial()","Run0031",
				FatalException, "unknown mode type.");
	}
	// Display useful information
	if (aMaterial){
		double Z(0.),A(0.),Ionization(0.),Density(0.),Radlen(0.);
		for(int i=0; i<aMaterial->GetElementVector()->size(); i++){
			Z += (aMaterial->GetElement(i)->GetZ())*
				(aMaterial->GetFractionVector()[i]);
			A += (aMaterial->GetElement(i)->GetA())*
				(aMaterial->GetFractionVector()[i]);
		}
		Ionization = aMaterial->GetIonisation()->GetMeanExcitationEnergy();
		Density = aMaterial->GetDensity();
		Radlen = aMaterial->GetRadlen();
		std::cout<<"#In MaterialSvc# New Material: \""<<aMaterial->GetName()<<std::endl;
		std::cout<<"                 Z: "<<Z<<" A: "<<(A/(g/mole))<<"g/mole, Ionization: "<<(Ionization/eV)<<"eV, Density: "<<Density/(g/cm3)<<"g/cm3 Radlen: "<<Radlen/cm<<"cm"<<std::endl;
	}
}
