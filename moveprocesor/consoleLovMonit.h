#pragma once
#include <string>
#include <iostream>
#include <windows.h>

using namespace std;


#define CLR_BLACK '0'
#define CLR_RED '1'
#define CLR_GREEN '2'
#define CLR_YELOW '3'
#define CLR_BLUE '4'
#define CLR_MAGENTA '5'
#define CLR_CYAN '6'
#define CLR_WHITE '7'

/*
#define CLR_GRAY '8'
#define CLR_L_BLUE '9'
#define CLR_L_GREEN 'A'
#define CLR_L_AQUA 'B'
#define CLR_L_RED 'C'
#define CLR_L_PURPLE 'D'
#define CLR_L_YELLOW 'E'
#define CLR_B_WHITE 'F'
*/


#define MAX_BOUNDS 1500

class consLOWDisplay {       // The class
public:

private:
	char monitor[MAX_BOUNDS][MAX_BOUNDS][2];

	// attached device monitor
	// attached device text promt

	void clearDisplay();

public:             // Access specifier

	void getSizes(int* columns, int* rows);

	void ini();

	void display();

	void print(int x,int y,string text, char color = 0);

	void print(int x, int y, char c, char color = 0);

	void clear();

};

//fast console clear
void consLOWDisplay::clearDisplay(){
	// Get the Win32 handle representing standard output.
	// This generally only has to be done once, so we make it static.
	static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	COORD topLeft = { 0, 0 };

	// std::cout uses a buffer to batch writes to the underlying console.
	// We need to flush that to the console because we're circumventing
	// std::cout entirely; after we clear the console, we don't want
	// stale buffered text to randomly be written out.
	std::cout.flush();

	// Figure out the current width and height of the console window
	if (!GetConsoleScreenBufferInfo(hOut, &csbi)) {
		// TODO: Handle failure!
		abort();
	}
	DWORD length = csbi.dwSize.X * csbi.dwSize.Y;

	DWORD written;

	// Flood-fill the console with spaces to clear it
	FillConsoleOutputCharacter(hOut, TEXT(' '), length, topLeft, &written);

	// Reset the attributes of every character to the default.
	// This clears all background colour formatting, if any.
	FillConsoleOutputAttribute(hOut, csbi.wAttributes, length, topLeft, &written);

	// Move the cursor back to the top left for the next sequence of writes
	SetConsoleCursorPosition(hOut, topLeft);
}

void consLOWDisplay::getSizes(int* columns, int* rows) {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	*columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	*rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1 - 1;

}

void consLOWDisplay::clear() {
	int rows, columns;
	getSizes(&columns,&rows);
	for (size_t x = 0; x < rows; x++) {
		for (size_t y = 0; y < columns; y++) {
			monitor[x][y][1] = 0;
			monitor[x][y][0] = ' ';
		}
	}
}

void consLOWDisplay::display() {
	string displayBUFFER = "";
	int rows, columns;
	getSizes(&columns, &rows);

	for (size_t x = 0; x < rows; x++) {
		for (size_t y = 0; y < columns; y++) {
			if (monitor[x][y][1] != (char)0) {
				displayBUFFER = displayBUFFER + "\u001b[3" + monitor[x][y][1] + "m";
			}

			displayBUFFER = displayBUFFER + monitor[x][y][0];

			if (monitor[x][y][1] != (char)0) {
				displayBUFFER = displayBUFFER + "\033[0m";
			}
		}
		displayBUFFER = displayBUFFER + '\n';
	}

	clearDisplay();
	cout << displayBUFFER;

}

void consLOWDisplay::print(int x, int y, char character, char color) {
	monitor[x][y][1] = color;
	monitor[x][y][0] = character;
}

void consLOWDisplay::print(int x, int y, string text, char color) {	
	for (size_t i = 0; i < text.length(); i++){
		print(x, y+i, text.at(i), color);
	}
}



void consLOWDisplay::ini() {
	cout.flush();

	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = false; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);

	for (size_t x = 0; x < MAX_BOUNDS; x++) {
		for (size_t y = 0; y < MAX_BOUNDS; y++) {
			monitor[x][y][1] = 0;
			monitor[x][y][0] = ' ';
		}
	}
}
