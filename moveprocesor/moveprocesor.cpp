#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <conio.h>
#include <Windows.h>
#include <cstdlib>

using namespace std;

void printC(string s);


#include "Display.h"
Display display;
/* run this program using the console pauser or add your own getch, system("pause") or input loop */

#define VAL_NULL    0 // 0
#define VAL_ONE     1 // 1
#define VAL_TWO     2 // 2
#define VAL_FOUR    3 // 4
#define VAL_SIX     4 // 6
#define VAL_EIGHT   5 // 8
#define VAL_SIXTEEN 6 // 16 
#define VAL_NEG     7 //-1

#define MAX_MEM     9
// možná pro všechny operace mít stejné A a B a jen brát hodnoty

#define ADD_A   10
#define ADD_B   11
#define ADD_OUT 12

#define SUB_A   13
#define SUB_B   14
#define SUB_OUT 15

#define MUL_A   16
#define MUL_B   17
#define MUL_OUT 18

#define DIV_A   19
#define DIV_B   20
#define DIV_OUT 21


#define REM_A   22
#define REM_B   23
#define REM_OUT 24

#define EQ_A    25
#define EQ_B    26
#define EQ_OUT  27

#define SHIFT_L_A   28
#define SHIFT_L_OUT 29

#define SHIFT_R_A   30
#define SHIFT_R_OUT 31

#define OUTPUT1   42
#define OUTPUT2   43
#define OUTPUT3   44

#define INPUT1   45
#define INPUT2   46
#define INPUT3   47

#define MONITOR 48
#define EXIT    49 // debug if 1 restart program
#define RUN     50

#define PROG_NEXT       51  //adressa další instrukce
#define PROG_NEXT_TWO   52	//adresa další další instrukce
#define PROG_NEXT_THREE 53  //...
#define PROG_NEXT_FOUR  54  //...




#define START_ADRESS 100
#define PROG_START START_ADRESS + 1
#define PROG_POS START_ADRESS


#define FILE_NAME "code.code"
#define MEMORY_BLOCK_SIZE 500
int memoryBlock[MEMORY_BLOCK_SIZE];

#include "consoleG.h"

void countRutine() {
	memoryBlock[ADD_OUT] = memoryBlock[ADD_A] + memoryBlock[ADD_B];
	memoryBlock[SUB_OUT] = memoryBlock[SUB_A] - memoryBlock[SUB_B];
	memoryBlock[MUL_OUT] = memoryBlock[MUL_A] * memoryBlock[MUL_B];
	memoryBlock[EQ_OUT]  = memoryBlock[EQ_A] == memoryBlock[EQ_B];
	if (memoryBlock[DIV_B] == 0) {
		memoryBlock[DIV_OUT] == 0;
	} else {
		memoryBlock[DIV_OUT] = memoryBlock[DIV_A] / memoryBlock[DIV_B];
	}
	if (memoryBlock[REM_B] == 0) {
		memoryBlock[REM_OUT] == 0;
	}
	else {
		memoryBlock[REM_OUT] = memoryBlock[REM_A] % memoryBlock[REM_B];
	}
	

	memoryBlock[SHIFT_L_OUT] = (memoryBlock[SHIFT_L_A] << 1);
	memoryBlock[SHIFT_R_OUT] = (memoryBlock[SHIFT_R_A] >> 1);
}

void loadProgram() {
	string line;

	// Read from the text file
	ifstream code(FILE_NAME);
	bool run = true;
	// Use a while loop together with the getline() function to read the file line by line
	int i = START_ADRESS;
	while (getline(code, line)) {
		printf("%d >> [%d]\n", stoi(line), i );
		memoryBlock[i] = stoi(line);
		i++;
	}

	// Close the file
	code.close();
}





bool err_rst = 0;

int main(int argc, char** argv) {
	main:
	for (size_t i = 0; i < MEMORY_BLOCK_SIZE; i++){
		memoryBlock[i] = 0;
	}

	memoryBlock[VAL_NULL] = 0;
	memoryBlock[VAL_ONE] = 1;
	memoryBlock[VAL_TWO] = 2;
	memoryBlock[VAL_FOUR] = 4;
	memoryBlock[VAL_SIX] = 6;
	memoryBlock[VAL_EIGHT] = 8;
	memoryBlock[VAL_SIXTEEN] = 16;
	memoryBlock[VAL_NEG] = -1;

	memoryBlock[MAX_MEM] = MEMORY_BLOCK_SIZE;


	loadProgram();

	initC(); // get first info about program loaded

	display.ini();

	if (!err_rst) {
		clearC();
	}

	while (memoryBlock[EXIT] != 1) {
		if (memoryBlock[RUN] == 1) {

			int progPos = memoryBlock[PROG_POS];

			memoryBlock[PROG_POS] = memoryBlock[PROG_POS] + 2;
			memoryBlock[PROG_NEXT] = memoryBlock[PROG_POS] + 2;
			memoryBlock[PROG_NEXT_TWO] = memoryBlock[PROG_POS] + 4;
			memoryBlock[PROG_NEXT_THREE] = memoryBlock[PROG_POS] + 6;
			memoryBlock[PROG_NEXT_FOUR] = memoryBlock[PROG_POS] + 8;

			int source = memoryBlock[progPos];
			int dest = memoryBlock[progPos + 1];





			if (memoryBlock[PROG_POS] > MEMORY_BLOCK_SIZE){
				printf("WRITE FAULT!");
				err_rst = 1;
				goto main;
			}

			memoryBlock[dest] = memoryBlock[source];

			if (dest == PROG_POS && memoryBlock[source] == progPos) { // když se zapisuje do progpos stejná hodnota jako je právě hodí err
				printC("\033[93m PROG_POS IM CICLING : stoping program (dbg feature) \033[0m");
				memoryBlock[RUN] = 0;
			}

			

			countRutine();
			updateC();
			printC("[" + to_string(source) + "](" + to_string(memoryBlock[source]) + ") >> [" + to_string(dest) + "] : " + to_string(memoryBlock[MONITOR]));
			//printf("[%d] >> [%d] : %d\n", source, dest, memoryBlock[MONITOR]);
			Sleep(150);


		} else {
			// user controll debug
			int source, dest;
			cin >> source;
			cin >> dest;

			memoryBlock[dest] = memoryBlock[source];

			countRutine();
			clearC();
			//printf("[%d] >> [%d] : %d\n", source, dest, memoryBlock[MONITOR]);
			printC("\033[1;31m [" + to_string(source) + "] >> [" + to_string(dest) + "] : " + to_string(memoryBlock[MONITOR])+"\033[0m");

		}
		display.simpleExternalDisplay(memoryBlock[OUTPUT1], memoryBlock[OUTPUT2]);
	}

	goto main;


	return 0;
}