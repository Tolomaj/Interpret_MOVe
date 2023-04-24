#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <conio.h>
#include <Windows.h>
#include <cstdlib>

#include "procesor.h"
#include "txtDisplay.h"
#include "consoleDisplay.h"
#include "Display.h"


using namespace std;

#define FILE_NAME "code.code"

bool totalRST = false;
int keyboardSbitTimer = 0;

Procesor procesor;
consDisplay cDis;
Display displayPer;
txtDisplay txtDisplayPer;

struct simulationRules {
	int upSignalDuration = 4;
	bool run = 0;
	int tickDuration = 10;
	bool conectKeyboardToOutput = false;
}simRules;

void simulateProcesorTick() {
	procesor.countRutine();
	procesor.interuptRutine();

	int err = procesor.proceedInstruction();
	if (err == -1) {
		cDis.print("PROG_POS out of memory!", CLR_RED);
		simRules.run = 0;
	}else if (err == -2) {
		cDis.print("DEST is reedOnly!", CLR_RED); // not implemented yet
	}
}

void executeComand(string comandBuffer) {

	auto pos = comandBuffer.find_first_not_of(' '); // remove first spcaes if there is
	comandBuffer = comandBuffer.substr(pos != std::string::npos ? pos : 0);

	pos = comandBuffer.find_last_not_of(' ') + 1;     // remove last spaces
	comandBuffer = comandBuffer.substr(0, pos != std::string::npos ? pos : 0);

	if (comandBuffer == "run") {
		simRules.run = true;
		cDis.print("Program runing.");
	}
	else if (comandBuffer.substr(0, 3) == "key") {
		if (comandBuffer.length() > 4) {
			
			bool value = (*procesor.INPUT_1 && 1);

			*procesor.INPUT_1 = *procesor.INPUT_1 | 1; // turn up last bit

			*procesor.INPUT_2 = comandBuffer[4];	//set char

			keyboardSbitTimer = simRules.upSignalDuration * simRules.tickDuration;

			cDis.print("Pressing char:" + to_string(comandBuffer[4]) + ".");

			procesor.countRutine();
			procesor.interuptRutine();
		}
		else {
			simRules.conectKeyboardToOutput = true;
			cDis.print("KeyboardConected.");
			cDis.print("Press TAB to disconect.", CLR_YELOW);
		}
	}
	else if (comandBuffer == "load") {
		system("cls");
		int rtrn = system("py compiler.py");
		if (rtrn == 0) {
			cDis.print("Loading new program :)!",CLR_BLUE);
			totalRST = true;
		} else {
			cout << "Press any key to continue.";
			_getch();
			cDis.print("Program not valid!", CLR_RED);
		}
	}
	else if (comandBuffer == "open") {
		// system("notepad code.base");
		ShellExecute(NULL, L"open", L"code.base", NULL, NULL, SW_SHOWNORMAL); // open file in Windows default text editor

	}
	else if (comandBuffer == "stop") {
		simRules.run = false;
		cDis.print("Program stoped.", CLR_YELOW);
	}
	else if (comandBuffer == "hvar") {
		cDis.settings.colorVariables = !cDis.settings.colorVariables;
	}
	else if (comandBuffer == "hvare") {
		cDis.settings.showVarEnd = !cDis.settings.showVarEnd;
	}
	else if (comandBuffer == "hjmp") {
		cDis.settings.colorJumps = !cDis.settings.colorJumps;
	}
	else if (comandBuffer == "hinv") {
		cDis.settings.invalidLines = !cDis.settings.invalidLines;
	}
	else if (comandBuffer == "hall") {
		bool isAnyOff = cDis.settings.colorJumps && cDis.settings.showVarEnd && cDis.settings.colorVariables && cDis.settings.invalidLines;
		cDis.settings.colorJumps = !isAnyOff;
		cDis.settings.showVarEnd = !isAnyOff;
		cDis.settings.colorVariables = !isAnyOff;
		cDis.settings.invalidLines = !isAnyOff;
	}
	else if (comandBuffer == "step") {
		simulateProcesorTick();
	}
	else if (comandBuffer == "ndis") {
		txtDisplayPer.rstBuff();
	}
	else if (comandBuffer.substr(0, 5) == "clock" && comandBuffer.length() > 5) {
		int clk;
		try {
			clk = stoi(comandBuffer.substr(5, comandBuffer.length()));
		} catch (exception& err) {
			cDis.print("Invalid parameters", CLR_RED);
			return;
		}

		simRules.tickDuration = clk;
		cDis.print("Tick pen instruct: " + to_string(simRules.tickDuration), CLR_YELOW);

	}
	else if (comandBuffer.substr(0, 4) == "peek") {
		int id;
		try {
			id = stoi(comandBuffer.substr(4, comandBuffer.length()));
		} catch (exception& err) {
			cDis.print("Invalid parameters", CLR_RED);
			return;
		}
		
		cDis.print("constent of [" + to_string(id) + "] is " + to_string(procesor.DBG_getMemoryBlock()[id]), CLR_YELOW);
	}
	else if (comandBuffer.substr(0, 3) == "mv ") {
		int a;
		int b;

		try {
			string nums = comandBuffer.substr(3, comandBuffer.length());
			int finds = nums.find(' ');
			a = stoi(nums.substr(0, finds)); // do vith space mv xxx xxx
			b = stoi(nums.substr(finds, nums.length()));
		} catch (exception& err) {
			cDis.print("Invalid parameters",CLR_RED);
			return;
		}
		procesor.DBG_manulaMove(a, b);
		cDis.print("Move [" + to_string(a) + "](" + to_string(procesor.DBG_getMemoryBlock()[a]) + ") >> [" + to_string(b) + "].", CLR_YELOW);

		procesor.countRutine();
	}
	else if (comandBuffer.substr(0, 5) == "clsr") {
		system("cls");
	}
}


void checkConsole(string * comandBuffer) {
	char a = (char)_getch();
	if (a != 0) {
		if (!simRules.conectKeyboardToOutput) { //keayboard conected to console
			if (a > 0 && a <= 255) { // check if is valid character for other functions
				if (isalpha(a) || isdigit(a) || a == ' ') {	//printable
					*comandBuffer = *comandBuffer + a;
				}
				else if (a == 8) { //back slash removes last char
					if (comandBuffer->size() > 0) {
						*comandBuffer = comandBuffer->substr(0, comandBuffer->size() - 1);
					}

				}
				else if (a == 13) { //entr key
					for (auto& c : *comandBuffer) { c = tolower(c); } // to lower
					executeComand(*comandBuffer); //run comand
					*comandBuffer = "";
				}
				else if (a == 59) { //entr key
					simulateProcesorTick();
				}
				else {
					cDis.print("nonrecognizedChar" + to_string((int)a), CLR_RED);
				}
			}
			else {

				cDis.print("pressed invalid key!", CLR_RED);

			}
		}
		else {
			if (a == 9) { //on tab
				simRules.conectKeyboardToOutput = false;
				cDis.print("KeyboardDisconected.");
			}
			else {

				bool value = (*procesor.INPUT_1 && 1);

				*procesor.INPUT_1 = *procesor.INPUT_1 | 1; // turn up last bit

				*procesor.INPUT_2 = a;	//set char

				keyboardSbitTimer = simRules.upSignalDuration * simRules.tickDuration;


				procesor.countRutine();
				procesor.interuptRutine();
			}
		}
	}

}


int main(int argc, char** argv) {
main:
	totalRST =false;
	string comandBuffer = "";
	int clockDivider = 0;
	keyboardSbitTimer = 0;

	int programLenght = procesor.loadProgramFromFile(FILE_NAME);

	cDis.ini(programLenght, procesor.DBG_getMemoryBlock()[START_ADRESS] - START_ADRESS , &comandBuffer);
	displayPer.ini();
	txtDisplayPer.ini();

	procesor.countRutine();

	

	while (1) {

		if (_kbhit()) { // console handle
			checkConsole(&comandBuffer);
			if (totalRST) {
				goto main;
			}
		}
		if (keyboardSbitTimer == 1) {
			*procesor.INPUT_1 = *procesor.INPUT_1 & ~1; // turn down last bit
		}
		if (keyboardSbitTimer > 0) {
			keyboardSbitTimer--;
		}

		if (simRules.run && clockDivider > simRules.tickDuration) {
			clockDivider = 0;
			simulateProcesorTick();
		}
		clockDivider++;
		
		if (1) {
			txtDisplayPer.simpleExternalTextDisplay(*procesor.OUTPUT_3, *procesor.OUTPUT_2 & 2);
			displayPer.simpleExternalDisplay(*procesor.OUTPUT_1, *procesor.OUTPUT_2 & 1);
			cDis.display(procesor.DBG_getMemoryBlock()[PROG_POS], procesor.DBG_getMemoryBlock(), &displayPer, &txtDisplayPer);
		
			//externalPeriferie zde

		}

		
		Sleep(0.001);
	}

	return 0;
}