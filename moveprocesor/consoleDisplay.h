#pragma once
#include <string>
#include <iostream>

using namespace std;

class consDisplay {       // The class
	void * comandSubscriber = NULL;
	// attached device monitor
	// attached device text promt

public:             // Access specifier
	void ini();

	void display();

	void print(string text,int color);

	void setCallback(/*function pointer*/); //function like cmd(string s);

	void setProgramPosition(int);




};