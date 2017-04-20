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
    int year = 10*(possibleDate[4] - '0') + possibleDate[5] - '0';
	if (month < 1 || month > 12) {return false;}
	if (day < 1 || day > 31) {return false;}
	if (((month == 4) || (month == 6) || (month == 9) || (month == 11)) && day > 30) {return false;}
	if (month == 2 && day > 28) {return false;}
    std::cout << "Expiration Date: " <<std::to_string(year + 2000) <<"-"<<std::to_string(month)<<"-"<< std::to_string(day)<< std::endl;
    
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    std::cout << (now->tm_year + 1900) << '-'
    << (now->tm_mon + 1) << '-'
    <<  now->tm_mday
    << std::endl;
    
    if(year + 2000 <= now->tm_year){
        if (month <= now->tm_mon ){
            if (day < now->tm_mday){
                std::cout << "This is expired!" << std::endl;
            }
            else{
                std::cout <<"This you can consume! Don't even worry"<<std::endl;
            }

        }
        else{
            std::cout <<"This you can consume! Don't even worry"<<std::endl;
        }

    }
    else{
        std::cout <<"This you can consume! Don't even worry"<<std::endl;
    }

    std::cout<<possibleDate<<" is valid "<<std::endl;
    
	return true;
}
