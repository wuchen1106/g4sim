#include "MySD.hh"

MySD::MySD(G4String name, MyVGeometryParameter* pointer)
	:G4VSensitiveDetector(name), m_GeometryParameter(pointer)
{
	fVerboseLevel = 0;
	G4String HCName = name.c_str();
	char* c = const_cast<char*> (HCName.c_str());
	for ( int i = 0; i < HCName.length(); i++ ){
		if ( c[i] == '/' ) c[i] = '_';
	}
  collectionName.insert(c); //Make the HC name unique, so that one can keep many copies of the same SD during a run
	m_VolName = GetPathName();
	char *pFast, *pSlow, *pC;
	pC = pFast = pSlow = const_cast<char*> (m_VolName.c_str());
	while( '\0' != *pFast ){
		if ( *pFast == '/' ) pFast++;
		else{
			*pSlow = *pFast;
			pSlow++;
			pFast++;
		}
	}
	*pSlow = '\0';
	m_VolName = pC;
}

MySD::~MySD(){}
