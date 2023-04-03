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

	void simpleExternalDisplay(int x, int clk) {
		if (x == 20) {
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 16; j++) {
					displayMatrix[i][j] = 0;
				}
			}
			counter = 0;
		}

		if (clk == 1 && onHigh == 0) {
			dataMATRIX[counter] = x;
			
			counter++;
			if (counter == 3) {
				if (dataMATRIX[0] < 8 && dataMATRIX[1] < 16) {
					displayMatrix[dataMATRIX[0]][dataMATRIX[1]] = dataMATRIX[2] > 0;
				}else {
					cout << "metrixOwerload!" << endl;
				}

				counter = 0;
			}
			onHigh = 1;
		}
		else {
			onHigh = 0;
		}
	}
};