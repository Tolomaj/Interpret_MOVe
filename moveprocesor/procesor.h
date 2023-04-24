#pragma once
#include <cstdint>
#include <string>
using namespace std;

#define CELL_TYPE int16_t
#define CELL_MAX 32767

#define MEMORY_BLOCK_SIZE 65536 // 2^16 should be
#define START_ADRESS 100

#define VAL_NULL    0 // 0
#define VAL_ONE     1 // 1
#define VAL_TWO     2 // 2
#define VAL_FOUR    3 // 4
#define VAL_SIX     4 // 6
#define VAL_EIGHT   5 // 8
#define VAL_SIXTEEN 6 // 16 
#define VAL_NEG     7 //-1

#define G_POINTER   8
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

#define OR_A    32
#define OR_B    34
#define OR_OUT  35

#define AND_A    36
#define AND_B    37
#define AND_OUT  38

#define NEG_A    39
#define NEG_OUT  40

#define OUTPUT1   42
#define OUTPUT2   43
#define OUTPUT3   44

#define INPUT1   45
#define INPUT2   46
#define INPUT3   47

#define MONITOR        48 //recrete INTERUP_MODE 
#define INTERUPT_JMP   49 // debug if 1 restart program
#define PRE_INT_ADRESS 50

#define PROG_NEXT       51  //adressa další instrukce
#define PROG_NEXT_TWO   52	//adresa další další instrukce
#define PROG_NEXT_THREE 53  //...
#define PROG_NEXT_FOUR  54  //...

#define SWITCH_A 55
#define SWITCH_B 56
#define SWITCH_S 57
#define SWITCH_OUT 58

#define RAND 59

#define INTERUPT_MODE 60 // 0 no | 1 UP | 2 DOWN | 3 BOTH


#define PROG_START START_ADRESS + 1
#define PROG_POS START_ADRESS



class Procesor {
	CELL_TYPE memoryBlock[MEMORY_BLOCK_SIZE];
	CELL_TYPE interuptBuffer = memoryBlock[INPUT1];
 
	bool onceHigh = true;
	int InteruptProtect = 2;

	CELL_TYPE protectedAdress = 0;

public:

	CELL_TYPE *OUTPUT_1 = &memoryBlock[OUTPUT1];
	CELL_TYPE *OUTPUT_2 = &memoryBlock[OUTPUT2];
	CELL_TYPE *OUTPUT_3 = &memoryBlock[OUTPUT3];

	CELL_TYPE *INPUT_1 = &memoryBlock[INPUT1];
	CELL_TYPE *INPUT_2 = &memoryBlock[INPUT2];
	CELL_TYPE *INPUT_3 = &memoryBlock[INPUT3];


	int loadProgramFromFile(string filename);

	void countRutine();

	void nextInstruction();

	int proceedInstruction();

	void interuptRutine();

	CELL_TYPE* DBG_getMemoryBlock();

	int DBG_getProgPos() {
		return memoryBlock[PROG_POS];
	};
	int DBG_manulaMove(size_t a, size_t b);
};

void Procesor::interuptRutine() {

	if (InteruptProtect == 0) {
		CELL_TYPE changedBytes = interuptBuffer ^ *INPUT_1;

		bool isHIGH = (changedBytes & *INPUT_1) != 0;		//som byte is changed high
		bool isLOW = (changedBytes & ~(*INPUT_1)) != 0;      //som byte is changed low
		interuptBuffer = *INPUT_1; // z nejakeho duvodu musí být jinak to nefunguje ??


		if (isHIGH && (memoryBlock[INTERUPT_MODE] == 1 || memoryBlock[INTERUPT_MODE] == 3)) {
			memoryBlock[PRE_INT_ADRESS] = memoryBlock[PROG_POS];
			memoryBlock[PROG_POS] = memoryBlock[INTERUPT_JMP];
			interuptBuffer = *INPUT_1;
		}else if (isLOW && (memoryBlock[INTERUPT_MODE] == 2 || memoryBlock[INTERUPT_MODE] == 3)) {
				memoryBlock[PRE_INT_ADRESS] = memoryBlock[PROG_POS];
				memoryBlock[PROG_POS] = memoryBlock[INTERUPT_JMP];
				interuptBuffer = *INPUT_1;
		}
	} else {
		interuptBuffer = *INPUT_1;
	}


}


int Procesor::DBG_manulaMove(size_t a, size_t b) {

	if (a > MEMORY_BLOCK_SIZE || b > MEMORY_BLOCK_SIZE) {
		return -1;
	}

	memoryBlock[b] = memoryBlock[a];

	return 0;
}

int Procesor::loadProgramFromFile(string filename) {

	ifstream code(filename);

	string line;

	int writeAdress = START_ADRESS;
	while (getline(code, line)) {
		memoryBlock[writeAdress] = stoi(line);
		writeAdress++;
	}

	// Close the file
	code.close();
	return writeAdress - START_ADRESS;
}

void Procesor::countRutine() {
	
	//protect constants from revriting
	memoryBlock[VAL_NULL] = 0;
	memoryBlock[VAL_ONE] = 1;
	memoryBlock[VAL_TWO] = 2;
	memoryBlock[VAL_FOUR] = 4;
	memoryBlock[VAL_SIX] = 6;
	memoryBlock[VAL_EIGHT] = 8;
	memoryBlock[VAL_SIXTEEN] = 16;
	memoryBlock[VAL_NEG] = -1;
	memoryBlock[MAX_MEM] = MEMORY_BLOCK_SIZE;
	memoryBlock[RAND] = (CELL_TYPE)rand();

	//do math
	memoryBlock[ADD_OUT]     = memoryBlock[ADD_A]     +     memoryBlock[ADD_B];
	memoryBlock[SUB_OUT]     = memoryBlock[SUB_A]     -     memoryBlock[SUB_B];
	memoryBlock[MUL_OUT]     = memoryBlock[MUL_A]     *     memoryBlock[MUL_B];
	memoryBlock[EQ_OUT]      = memoryBlock[EQ_A]      ==    memoryBlock[EQ_B];
	memoryBlock[SHIFT_L_OUT] = memoryBlock[SHIFT_L_A] << 1;
	memoryBlock[SHIFT_R_OUT] = memoryBlock[SHIFT_R_A] >> 1;

	memoryBlock[OR_OUT]      = memoryBlock[OR_A] | memoryBlock[OR_B];
	memoryBlock[AND_OUT]     = memoryBlock[AND_A] & memoryBlock[AND_B];
	memoryBlock[NEG_OUT]       = ~memoryBlock[NEG_A];

	if (memoryBlock[SWITCH_S] == 1) {
		memoryBlock[SWITCH_OUT] = memoryBlock[SWITCH_A];
	} else {
		memoryBlock[SWITCH_OUT] = memoryBlock[SWITCH_B];
	}
		



	if (memoryBlock[DIV_B] == 0) { memoryBlock[DIV_OUT] == 0; } // ošetøení dìlení nulou
	else {
		memoryBlock[DIV_OUT] = memoryBlock[DIV_A] / memoryBlock[DIV_B];
	}
	if (memoryBlock[REM_B] == 0) { memoryBlock[REM_OUT] == 0; } //ošetøení dìlení nulou
	else {
		memoryBlock[REM_OUT] = memoryBlock[REM_A] % memoryBlock[REM_B];
	}

};

void Procesor::nextInstruction() {
	memoryBlock[PROG_POS] = memoryBlock[PROG_POS] + 2;
	memoryBlock[PROG_NEXT] = memoryBlock[PROG_POS] + 2;
	memoryBlock[PROG_NEXT_TWO] = memoryBlock[PROG_POS] + 4;
	memoryBlock[PROG_NEXT_THREE] = memoryBlock[PROG_POS] + 6;
	memoryBlock[PROG_NEXT_FOUR] = memoryBlock[PROG_POS] + 8;
}

int Procesor::proceedInstruction() {

	int progPos = memoryBlock[PROG_POS];

	nextInstruction();

	int source = memoryBlock[progPos];
	int dest = memoryBlock[progPos + 1];

	if (memoryBlock[PROG_POS] > MEMORY_BLOCK_SIZE) {
		return -1;
	}

	if (dest == INTERUPT_JMP && memoryBlock[INTERUPT_JMP] == 0 && memoryBlock[source] != 0) {// after writing to interupt 0 -> XX 4 instructions are protected
		InteruptProtect = 4;
	}

	if (InteruptProtect > 0) {  // decrease protected instructions
		InteruptProtect--;
	}

	memoryBlock[dest] = memoryBlock[source];
	




	return 0;
};

CELL_TYPE* Procesor::DBG_getMemoryBlock() {
	CELL_TYPE* ret = memoryBlock;
	return ret;
}