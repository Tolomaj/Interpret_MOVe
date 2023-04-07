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

	cout << alo << "    ________________________________" << endl;
	


	for (size_t i = 0; i < alo.length(); i++) {
		cout << "-";
	}
	cout << "   |";
	//display.displayROW(0);
	int lastPrintedRow = 0;

	for (size_t i = 0; i < defProgramSector - START_ADRESS; i = i + 1) {
		string str = "[" + to_string(START_ADRESS + i) + "] : " + to_string(memoryBlock[START_ADRESS + i]);

		cout << str;
		for (size_t c = str.length(); c < alo.length()+3; c++) {
			cout << " ";
		}


		if (i < 8) {
			cout << "|";
			//display.displayROW(i + 1);
			lastPrintedRow = (i + 1);
		} else {
			cout << endl;
		}
	}

	ifstream code(FILE_NAME);
	string l = "";
	int progCLines = 0;
	while (getline(code, l)) {
		progCLines++;
	}
	code.close();

	for (size_t i = 0; i < progCLines - (defProgramSector - START_ADRESS); i = i + 2) {

		string outlin = "[" + to_string(defProgramSector + i) + " - " + to_string(defProgramSector + i +1) + "] : " + to_string(memoryBlock[defProgramSector + i]) + " >> " + to_string(memoryBlock[defProgramSector + i + 1]);
		if ((defProgramSector + i) == memoryBlock[PROG_POS] || (defProgramSector + i + 1) == memoryBlock[PROG_POS]) {
			outlin = outlin + "\033[96m <---\033[0m";
		}

		if ((defProgramSector + i) == memoryBlock[PROG_POS]-1 || (defProgramSector + i + 1) == memoryBlock[PROG_POS]-1 && memoryBlock[RUN] == 0) {
			outlin = outlin + "\033[93m  <STOP\033[0m";
		}

		cout << outlin;
		


		if (i/2 + lastPrintedRow <= 8 && lastPrintedRow != 8) {
			for (size_t c = outlin.length(); c < alo.length(); c++) {
				cout << " ";
			}

			cout << "   |";
			//display.displayROW(i + lastPrintedRow);
			lastPrintedRow = (i + lastPrintedRow);
		}
		else {
			cout << endl;
		}




	}

	for (size_t i = 0; i < (alo.length() - 7)/2; i++) {
		cout << "-";
	}
	cout << "HYSTORY";
	for (size_t i = 0; i < (alo.length() - 7) / 2; i++) {
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