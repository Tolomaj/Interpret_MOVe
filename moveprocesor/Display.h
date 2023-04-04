#pragma once


//	  [dataX]   [dataY]	  [dataC]
// [0][1]    [0][1]    [0][1]


class Display {       // The class
public:             // Access specifier
	bool onHigh = 0;
	bool displayMatrix[8][16];

	int dataMATRIX[3];
	int counter = 0;

	void ini() {
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 16; j++) {
				displayMatrix[i][j] = 0;
			}
		}
		counter = 0;
	}

	void display() {
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 16; j++) {
				cout << displayMatrix[i][j] << displayMatrix[i][j];
			}
			cout << endl;
		}
	}

	void displayROW(int i) {
		if(i != 8) {
			for (int j = 0; j < 16; j++) {
				cout << displayMatrix[i][j] << displayMatrix[i][j];
			}
			cout << "|";
			cout << endl;
		}
		else {		 
			cout << "--------------------------------|" << endl;
		}
	}

	void simpleExternalDisplay(int x, int clk) {
		if (x == 20) {
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 16; j++) {
					displayMatrix[i][j] = 0;
				}
			}
			counter = 0;
		}

		if (clk == 1) {
			if(onHigh == 0) {
				//printC("HI" + to_string(x));
				dataMATRIX[counter] = x;

				counter++;
				if (counter == 3) {
					//printC("VV" + to_string(dataMATRIX[0]) + " - " + to_string(dataMATRIX[1]));
					if (dataMATRIX[0] < 8 && dataMATRIX[1] < 16) {
						displayMatrix[dataMATRIX[0]][dataMATRIX[1]] = dataMATRIX[2] > 0;
						//printC("¨¡X");
					}
					else {
						cout << "metrixOwerload!" << endl;
					}

					counter = 0;
				}
				onHigh = 1;
			}

		} else {
			onHigh = 0;
		}
	}
};