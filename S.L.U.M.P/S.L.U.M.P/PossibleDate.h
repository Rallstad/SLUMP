#pragma once

#include <string>

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
class PossibleDate {
public:
	// member variables ///////////////////////////////////////////////////////////////////////////
	cv::Mat imgDate;
	cv::Mat imgGrayscale;
	cv::Mat imgThresh;

	cv::RotatedRect rrLocationOfDateInScene;

	std::string strChars;

	///////////////////////////////////////////////////////////////////////////////////////////////
	static bool sortDescendingByNumberOfChars(const PossibleDate &ppLeft, const PossibleDate &ppRight) {
		return(ppLeft.strChars.length() > ppRight.strChars.length());
	}

};
