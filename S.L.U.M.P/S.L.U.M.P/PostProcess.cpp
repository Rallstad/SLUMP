// PostProcess.cpp

#include "PostProcess.h"
#include <iostream>

std::string convertIsToOnes(std::string likelyDate) {
	for (int i = 0; i < likelyDate.length();i++) {
		if (likelyDate[i] == 'I') {
			likelyDate[i] = '1';
		}
	}
	return likelyDate;
}

bool checkLength(std::string possibleDate) {
	if (possibleDate.length() == 6) {
		return true;
	}
	return false;
}

bool checkIfValidDate(std::string possibleDate) {
	int day = 10*(possibleDate[0] - '0') + possibleDate[1] - '0';
	int month = 10 * (possibleDate[2] - '0') + possibleDate[3] - '0';
	if (month < 1 || month > 12) {return false;}
	if (day < 1 || day > 31) {return false;}
	if (((month == 4) || (month == 6) || (month == 9) || (month == 11)) && day > 30) {return false;}
	if (month == 2 && day > 28) {return false;}

	std::cout << "Day: " <<std::to_string(day) << std::endl;
	std::cout << "Month: " << std::to_string(month) << std::endl;
	return true;
}