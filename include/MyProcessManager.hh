#ifndef MyProcessManager_h
#define MyProcessManager_h 1

#include <sstream>
#include <fstream>

class MyProcessManager{
	public:
		MyProcessManager();
		virtual ~MyProcessManager();

		static MyProcessManager* GetMyProcessManager();

		int OpenFile();
		void CloseFile();

		int GetMemorySize();

	private:
		static MyProcessManager* fMyProcessManager;
		int MemoryConsumption;
		std::ifstream *fin_card;
		std::stringstream buff;
		bool fileOpened;
};

#endif
