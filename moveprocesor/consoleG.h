#pragma once


// gets [8][16][1]
void externDisplay(){}




string consoleContent = "";
bool firstRead = 1;
int defProgramSector = 0;

void initC() {
	defProgramSector = memoryBlock[START_ADRESS];
}

void clearC() {
	system("cls");
	string progPos = "PROG_POS[" + to_string(PROG_POS) + "] : " + to_string(memoryBlock[PROG_POS]);
	string run = "RUN[" + to_string(RUN) + "] : " + to_string(memoryBlock[RUN]);
	string monit = "MONITOR[" + to_string(MONITOR) + "] : " + to_string(memoryBlock[MONITOR]);

	string alo = progPos + " | " + run + " | " + monit;

	cout << alo << endl;
	for (size_t i = 0; i < alo.length(); i++) {
		cout << "-";
	}
	cout << endl;

	for (size_t i = 0; i < defProgramSector - START_ADRESS; i = i + 1) {
		cout << "[" << (START_ADRESS + i) << "] : " << memoryBlock[START_ADRESS + i] << endl;
	}

	ifstream code(FILE_NAME);
	string l = "";
	int progCLines = 0;
	while (getline(code, l)) {
		progCLines++;
	}
	code.close();

	for (size_t i = 0; i < progCLines - (defProgramSector - START_ADRESS); i = i + 2) {
		cout << "[" << (defProgramSector + i) << " - "<<  (defProgramSector + i +1) << "] : ";
		cout << memoryBlock[defProgramSector + i] << " >> " << memoryBlock[defProgramSector + i + 1];
		if ((defProgramSector + i) == memoryBlock[PROG_POS] || (defProgramSector + i + 1) == memoryBlock[PROG_POS]) {
			cout << "<---";
		}
			
		cout << endl;
	}

	for (size_t i = 0; i < alo.length(); i++) {
		cout << "-";
	}
	cout << endl;

	display.display();

	for (size_t i = 0; i < alo.length(); i++) {
		cout << "-";
	}
	cout << endl;

	
	
	//show display




	consoleContent = "";
}

void printC(string s) {
	consoleContent = consoleContent + s + "\n";
	cout << s << endl;
}

void updateC() {
	string s = consoleContent;
	clearC();
	consoleContent = s;
	cout << consoleContent;
}