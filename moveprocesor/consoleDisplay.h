#pragma once
#include <string>
#include <iostream>
#include "consoleLovMonit.h"
#include <vector>
#include "Display.h"

using namespace std;

#define MAX_CMD 40
#define PROG_ROW_LEN 20

struct displaySettings {
	bool colorJumps = true;
	bool colorVariables = true;
	bool showVarEnd = false;
	bool invalidLines = true;
};

class consDisplay {       // The class
private:
	string *consoleCMD = NULL;
	vector<string> consoleLog;
	vector<int> consoleColors;
	int programLenght = 0;
	int variableLenght = 0;
	
	// attached device monitor
	// attached device text promt

	string DecimalToBinaryString(CELL_TYPE a);

public:             // Access specifier
	displaySettings settings;
	consLOWDisplay myDisplay;

	void ini(int ProgramLenght, int VariableLenght, string* buff);

	void stop();

	void loop();

	void display(int prog_pos, CELL_TYPE* memoryBlock, Display* displayPer, txtDisplay *txtDisplayPer);

	void print(string s, char color);

	void setProgramPosition(int);


};

#define FIRSTPRINTED 7 //haw many zeros are printed
string consDisplay::DecimalToBinaryString(CELL_TYPE a){
	string binary = "";
	CELL_TYPE mask = 1;
	for (int i = 0; i < FIRSTPRINTED; i++)
	{
		if ((mask & a) >= 1)
			binary = "1" + binary;
		else
			binary = "0" + binary;
		mask <<= 1;
	}
	return binary;
}

void consDisplay::print(string s,char color = 0) {
	consoleLog.push_back(s);
	consoleColors.push_back(color);
	//add redraw of cmd
}


void consDisplay::display(int prog_pos,CELL_TYPE *memoryBlock, Display *displayPer,txtDisplay *txtDisplayPer) {

	myDisplay.clear();

	int rows, columns;
	myDisplay.getSizes(&columns, &rows);


	for (size_t i = 0; i < MAX_CMD-1; i++){ myDisplay.print(rows - 2, columns - 2 - i, '-'); } // print lines
	for (size_t i = 0; i < 3; i++) { myDisplay.print(0 + i, columns - MAX_CMD / 2 - 1, '|'); }// print lines
	for (size_t i = 0; i < 4; i++) { myDisplay.print(5 + i, columns - (MAX_CMD - 16 * 2) / 2 - 2, '|'); }

	for (size_t i = 0; i < rows; i++) {
		myDisplay.print(rows - 1 - i, columns - MAX_CMD - 1, '|');
		myDisplay.print(rows - 1 - i, columns - 1, '|');
	}

	for (size_t i = 0; i < MAX_CMD-1; i++) {
		myDisplay.print(1, columns - 2 - i, '-');
		myDisplay.print(3, columns - 2 - i, '-');
		myDisplay.print(10, columns - 2 - i, '-');
		myDisplay.print(12, columns - 2 - i, '-');
	}

	for (size_t i = 0; i < 16; i++) {
		for (size_t c = 0; c < 6; c++) {
			int clr = displayPer->getPixel(i, c);

			if (clr == 0) {
				myDisplay.print(4 + c, columns - 4 - i * 2 - (MAX_CMD - 16 * 2) / 2, '\'');
			}
			else {
				if (clr <= 7 && clr >= 1) {
					myDisplay.print(4 + c, columns - 4 - i * 2 - (MAX_CMD - 16 * 2) / 2, '#',(char)(48 + clr));
				}
				else {
					myDisplay.print(4 + c, columns - 4 - i * 2 - (MAX_CMD - 16 * 2) / 2, '?', CLR_RED);
				}
				
			}

			
		}
	}
	myDisplay.print(5, columns - (MAX_CMD - 16 * 2) / 2, '1', CLR_GREEN * (displayPer->counter == 0) + CLR_BLUE * !(displayPer->counter == 0));
	myDisplay.print(6, columns - (MAX_CMD - 16 * 2) / 2, '1', CLR_GREEN * (displayPer->counter == 1) + CLR_BLUE * !(displayPer->counter == 1));
	myDisplay.print(7, columns - (MAX_CMD - 16 * 2) / 2, '1', CLR_GREEN * (displayPer->counter == 2) + CLR_BLUE * !(displayPer->counter == 2));
	myDisplay.print(8, columns - (MAX_CMD - 16 * 2) / 2, 'R', CLR_GREEN*displayPer->DBG_RST + CLR_BLUE * !displayPer->DBG_RST);


	myDisplay.print(0, columns - MAX_CMD, "PROG_POS[" + to_string(PROG_POS) + "]:" + to_string(memoryBlock[PROG_POS]));
	myDisplay.print(0, columns - MAX_CMD / 2, "CLOCK[INSERT]:#");
	myDisplay.print(2, columns - MAX_CMD, "MONITOR[" + to_string(MONITOR) + "]:" + to_string(memoryBlock[MONITOR]));
	myDisplay.print(2, columns - MAX_CMD / 2, "INPUT1[" + to_string(INPUT1) + "]:~" + DecimalToBinaryString(memoryBlock[INPUT1]));
	myDisplay.print(11, columns - MAX_CMD + (MAX_CMD - 35) / 2, "[EXT_DISPLAY]:" + txtDisplayPer->get_text() );

	

	if (consoleCMD == NULL) {
		myDisplay.print(rows-1, columns - MAX_CMD, "BUFF_NOT_SET!", CLR_RED);
	}else {
		myDisplay.print(rows-1, columns - MAX_CMD, *consoleCMD, CLR_CYAN);
	}

	for (size_t i = 0; i < rows-14/*max cmds*/; i++) {
		int disppos = consoleLog.size() - i;
		if (disppos >= 0 && disppos < consoleLog.size()) {
			myDisplay.print(rows - 2 - i, columns - MAX_CMD, consoleLog[disppos],consoleColors[disppos]);
		}
	}

	int printingLine = 0;
	int printingRow = 0;
	for (size_t i = 0; i < variableLenght; i = i + 1) {
		
		if (i == variableLenght-1 && settings.showVarEnd) {
			myDisplay.print(printingLine, printingRow, "#", CLR_BLUE);
		}

		myDisplay.print(printingLine, printingRow+1, "[" + to_string(START_ADRESS + i) + "]: " + to_string(memoryBlock[START_ADRESS + i]), CLR_YELOW*settings.colorVariables);

		printingLine++;
		if (printingLine >= rows) {
			printingLine = 0;
			printingRow += PROG_ROW_LEN;
		}
	}
	
	for (size_t i = 0; i < programLenght - variableLenght; i = i + 2) {
		string outlin = "[" + to_string(variableLenght + START_ADRESS + i) + "]: " + to_string(memoryBlock[variableLenght + START_ADRESS + i]) + " >> " + to_string(memoryBlock[variableLenght + START_ADRESS + i + 1]);
		
		if ((variableLenght + START_ADRESS + i) == prog_pos || (variableLenght + START_ADRESS + i + 1) == prog_pos) {
			myDisplay.print(printingLine, printingRow, ">",CLR_GREEN);
		}else {
			myDisplay.print(printingLine, printingRow, " ", CLR_GREEN);
		}

		bool showJumpsT = (memoryBlock[variableLenght + START_ADRESS + i + 1] == PROG_POS) && settings.colorJumps;
		bool showInvalT = (memoryBlock[variableLenght + START_ADRESS + i + 1] == memoryBlock[variableLenght + START_ADRESS + i]) && settings.invalidLines;

		if (showInvalT) {
			myDisplay.print(printingLine, printingRow + 1, outlin, CLR_RED);
		}
		else {
			myDisplay.print(printingLine, printingRow + 1, outlin, CLR_MAGENTA * showJumpsT);
		}

		printingLine++;
		if (printingLine >= rows) {
			printingLine = 0;
			printingRow += PROG_ROW_LEN;
		}
	}
	






	myDisplay.display();
}

void consDisplay::loop() {
	
}

void consDisplay::ini(int ProgramLenght, int VariableLenght, string* buff) {
	consoleCMD = buff;
	programLenght = ProgramLenght;
	variableLenght = VariableLenght;
	myDisplay.ini();
}

void consDisplay::stop() {
	
}