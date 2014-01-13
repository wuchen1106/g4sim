//Description: In charge of Controling configuration files
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 21 Dec, 2013
//---------------------------------------------------------------------------//

#include <vector>

#include "globals.hh"

#include "TF1.h"

#include "MyConfigure.hh"

std::vector<G4String> MyConfigure::knownValueNames;
std::vector<G4String> MyConfigure::knownValues;

MyConfigure::MyConfigure()
{}

MyConfigure::~MyConfigure()
{}

double MyConfigure::CalFormula(G4String formula, int iRep){
//	std::cout<<"TO Calculate for: \""<<formula<<"\" "<<iRep<<std::endl; // to be deleted
//	formula = ReplaceMacro(formula);
	TF1 *f1 = new TF1("f1", formula);
	double value = f1->Eval(iRep);
	delete f1;
	return value;
}

G4String MyConfigure::ReplaceMacro(G4String formula){
//	std::cout<<"TO replace for: \""<<formula<<"\""<<std::endl; // to be deleted
	std::vector<G4String> words = GetWords(formula);
//	std::cout<<"	"<<words.size()<<" words"<<std::endl; // to be deleted
	for (int iWord = 0; iWord < words.size(); iWord++ ){
//		std::cout<<"		"<<iWord<<std::endl; // to be deleted
		G4String value;
		if (FindMacro(words[iWord],value)){
			Replace(formula,words[iWord],value);
		}
	}
	return formula;
}

std::vector<G4String> MyConfigure::GetWords(G4String formula){
	std::vector<G4String> words;
	words.clear();
	const char* cformula = formula.c_str();
	int length = strlen(cformula);
	char temp[1240];
	int tempoffset = 0;
	for ( int offset = 0; offset < length; offset++ ){
		char c = cformula[offset];
		bool isword = false;
		if (c>='a'&&c<='z'
			||c>='A'&&c<='Z'
			||c>='0'&&c<='9'
			||c=='_'
		   ){
			temp[tempoffset++] = cformula[offset];
			isword = true;
		}
		if (!isword||offset==length-1){
			if (tempoffset>0){
				temp[tempoffset++] = '\0';
				tempoffset=0;
				G4String word = temp;
				bool found = false;
				for(int iWord = 0; iWord<words.size(); iWord++){
					if (words[iWord]==word){
						found = true;
						break;
					}
				}
				if (!found){
					words.push_back(word);
				}
			}
		}
	}
	return words;
}

bool MyConfigure::FindMacro(G4String word, G4String& value){
	bool found = false;
	for (int i = 0; i< knownValues.size(); i++){
		if (knownValueNames[i]==word){
			value = knownValues[i];
			found = true;
			break;
		}
	}
	return found;
}

void MyConfigure::Replace(G4String &formula, G4String word, G4String value){
//	std::cout<<"-- \""<<formula<<"\""<<std::endl; // to be deleted
	G4String newform = "";
	const char* cformula = formula.c_str();
	int length = strlen(cformula);
	char temp[1024];
	int tempoffset = 0;
	char cnewform[1024];
	int newformoffset = 0;
	for ( int offset = 0; offset < length; offset++ ){
		char c = cformula[offset];
		bool isword = false;
		if (c>='a'&&c<='z'
			||c>='A'&&c<='Z'
			||c>='0'&&c<='9'
			||c=='_'
		   ){
			temp[tempoffset++] = cformula[offset];
			isword = true;
		}
		if (!isword||offset==length-1){
			if (tempoffset>0){
				temp[tempoffset] = '\0';
				tempoffset=0;
				if (newformoffset>0){
					cnewform[newformoffset] = '\0';
					newformoffset=0;
					G4String newformtemp = cnewform;
					newform=newform+newformtemp;
				}
				G4String word = temp;
//				std::cout<<"		\""<<word<<"\""<<std::endl; // to be deleted
				G4String newword;
				bool found = FindMacro(word,newword);
				if (found){
					newform=newform+"("+newword+")";
				}
				else{
					newform=newform+word;
				}
//				std::cout<<"		to \""<<newword<<"\""<<std::endl; // to be deleted
			}
			if(!isword){
				cnewform[newformoffset++] = cformula[offset];
			}
			if (offset==length-1){
				cnewform[newformoffset] = '\0';
				G4String newformtemp = cnewform;
				newform=newform+newformtemp;
			}
		}
	}
//	std::cout<<"	-->\""<<newform<<"\""<<std::endl; // to be deleted
	formula = newform;
}

