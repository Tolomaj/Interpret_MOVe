#pragma once


//	  [dataX]   [dataY]	  [dataC]
// [0][1]    [0][1]    [0][1]


class Display {       // The class
public:             // Access specifier
	bool onHigh = 0;
	int displayMatrix[6][16];

	int dataMATRIX[3];
	int counter = 0;

	bool DBG_RST = false;

	void ini() {
		for (int i = 0; i < 6; i++) {
			for (int j = 0; j < 16; j++) {
				displayMatrix[i][j] = 0;
			}
		}
		counter = 0;
	}

	int getPixel(size_t x, size_t y) {
		return displayMatrix[5 - y][15 - x];
	}


	void simpleExternalDisplay(int x, bool clk) {
		if (x == 20) {
			DBG_RST = true;
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 16; j++) {
					displayMatrix[i][j] = 0;
				}
			}
			counter = 0;
		}
		else {
			DBG_RST = false;
		}

		if (clk == true) {
			if(onHigh == false) {
				//printC("HI" + to_string(x));
				dataMATRIX[counter] = x;

				counter++;
				if (counter == 3) {
					//printC("VV" + to_string(dataMATRIX[0]) + " - " + to_string(dataMATRIX[1]));
					if (dataMATRIX[0] < 16 && dataMATRIX[1] < 16) {
						displayMatrix[dataMATRIX[1]][dataMATRIX[0]] = dataMATRIX[2];
						//printC("¨¡X");
					}
					else {
						//cout << "metrixOwerload!" << endl;
					}

					counter = 0;
				}
				onHigh = true;
			}

		} else {
			onHigh = false;
		}
	}
};