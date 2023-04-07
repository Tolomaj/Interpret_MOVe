#pragma once
#include <string>

//	  [dataX]   [dataY]	  [dataC]
// [0][1]    [0][1]    [0][1]


class txtDisplay {       // The class
public:             // Access specifier
	bool onHigh = 0;
	std::string text = "___________________";


	void rstBuff() {
		text = "___________________";
	}

	void ini() {}

	std::string get_text() {
		return text;
	};

	void simpleExternalTextDisplay(char x, bool clk) {

		if (clk == true) {
			if (onHigh == false) {
				text = x + text;
				text.pop_back();
			}
			onHigh = true;
		} else {
			onHigh = false;
		}
	}
};