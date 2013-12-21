//Description: In charge of Controling configuration files
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 21 Dec, 2013
//---------------------------------------------------------------------------//

#ifndef MyConfigure_H
#define MyConfigure_H 

#include <vector>

class MyConfigure{

	public:
		MyConfigure(void);
		virtual ~MyConfigure(void);

		std::vector<G4String> GetWords(G4String formula);
		bool FindMacro(G4String word, G4String& value);
		void Replace(G4String &formula, G4String word, G4String value);
		double CalFormula(G4String formula, int iRep = 0);
		G4String ReplaceMacro(G4String formula);

		// macros
		static std::vector<G4String> knownValueNames;
		static std::vector<G4String> knownValues;

};

#endif
