#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>


#include<iostream>
#include<conio.h>           // may have to modify this line if not using Windows

#include "parameters.h"
#include "Classification.h"
#include "DetectChars.h"
#include "DetectText.h"
#include "PossibleDate.h"
#include "PostProcess.h"
void drawRedRectangleAroundDate(cv::Mat &imgOriginalScene, PossibleDate &likelyDate);
void writeLicenseDateCharsOnImage(cv::Mat &imgOriginalScene, PossibleDate &likelyDate);


int main() {
   
	bool blnKNNTrainingSuccessful = loadKNNDataAndTrainKNN();           // attempt KNN training

	if (blnKNNTrainingSuccessful == false) {                            // if KNN training was not successful
																		// show error message
		std::cout << std::endl << std::endl << "error: error: KNN traning was not successful" << std::endl << std::endl;
		return(0);                                                      // and exit program
	}

	cv::Mat imgOriginalScene;           // input image
	cv::Mat src;

	double factor = 0;

	src = cv::imread("Images/Dato_melk.jpg", CV_LOAD_IMAGE_COLOR);   // Read the file
	if (src.size().height < src.size().width) {
		factor = (1000 / ((double)src.size().width));
		//cout << "width: " << factor << endl;
		resize(src, imgOriginalScene, cv::Size(1000, src.size().height*factor), 0, 0, cv::INTER_LINEAR);
	}
	else {
		factor = (1000 / ((double)src.size().height));
		//cout << "height: " << factor;
		resize(src, imgOriginalScene, cv::Size(src.size().width*factor, 1000), 0, 0, cv::INTER_LINEAR);
	}

	if (imgOriginalScene.empty()) {                             // if unable to open image
		std::cout << "error: image not read from file\n\n";     // show error message on command line
		_getch();                                               // may have to modify this line if not using Windows
		return(0);                                              // and exit program
	}
	std::vector<PossibleDate> vectorOfPossibleDates = detectTextInScene(imgOriginalScene);          // detect Dates

	vectorOfPossibleDates = detectCharsInDates(vectorOfPossibleDates);                               // detect chars in Dates

	cv::imshow("imgOriginalScene", imgOriginalScene);           // show scene image

	if (vectorOfPossibleDates.empty()) {                                               // if no Dates were found
		std::cout << std::endl << "no Dates were detected" << std::endl;       // inform user no Dates were found
	}
	else {                                                                            // else
																					  // if we get in here vector of possible Dates has at leat one Date

																					  // sort the vector of possible Dates in DESCENDING order (most number of chars to least number of chars)
		std::sort(vectorOfPossibleDates.begin(), vectorOfPossibleDates.end(), PossibleDate::sortDescendingByNumberOfChars);
		std::reverse(vectorOfPossibleDates.begin(), vectorOfPossibleDates.end());
		for (auto it = vectorOfPossibleDates.begin(); it != vectorOfPossibleDates.end(); ++it) {
			it->strChars = convertIsToOnes(it->strChars);
		}
		// suppose the Date with the most recognized chars (the first Date in sorted by string length descending order) is the actual Date
		PossibleDate likelyDate;
		bool dateValid = false;
		while (!vectorOfPossibleDates.empty() && !dateValid) {
			likelyDate = vectorOfPossibleDates.back();
			dateValid = checkLength(likelyDate.strChars) && checkIfValidDate(likelyDate.strChars);
			likelyDate = vectorOfPossibleDates.back();
			vectorOfPossibleDates.pop_back();
		}

		cv::imshow("imgDate", likelyDate.imgDate);            // show crop of Date and threshold of Date
		cv::imshow("imgThresh", likelyDate.imgThresh);
		if (likelyDate.strChars.length() == 0) {                                                     // if no chars were found in the Date
			std::cout << std::endl << "no characters were detected" << std::endl << std::endl;      // show message
			cv::waitKey(0);
			return(0);                                                                              // and exit program
		}
		if (dateValid) {
			drawRedRectangleAroundDate(imgOriginalScene, likelyDate);                // draw red rectangle around Date

			std::cout << std::endl << "Date read from image = " << likelyDate.strChars << std::endl;     // write license Date text to std out
			std::cout << std::endl << "-----------------------------------------" << std::endl;

			writeLicenseDateCharsOnImage(imgOriginalScene, likelyDate);              // write license Date text on the image

			cv::imshow("imgOriginalScene", imgOriginalScene);                       // re-show scene image

			cv::imwrite("imgOriginalScene.png", imgOriginalScene);                  // write image out to file
		}
		else {
			std::cout << "No valid date found" << std::endl;
		}
	}

	cv::waitKey(0);                 // hold windows open until user presses a key

	return(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void drawRedRectangleAroundDate(cv::Mat &imgOriginalScene, PossibleDate &likelyDate) {
	cv::Point2f p2fRectPoints[4];

	likelyDate.rrLocationOfDateInScene.points(p2fRectPoints);            // get 4 vertices of rotated rect

	for (int i = 0; i < 4; i++) {                                       // draw 4 red lines
		cv::line(imgOriginalScene, p2fRectPoints[i], p2fRectPoints[(i + 1) % 4], SCALAR_RED, 2);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void writeLicenseDateCharsOnImage(cv::Mat &imgOriginalScene, PossibleDate &likelyDate) {
	cv::Point ptCenterOfTextArea;                   // this will be the center of the area the text will be written to
	cv::Point ptLowerLeftTextOrigin;                // this will be the bottom left of the area that the text will be written to

	int intFontFace = CV_FONT_HERSHEY_SIMPLEX;                              // choose a plain jane font
	double dblFontScale = (double)likelyDate.imgDate.rows / 30.0;            // base font scale on height of Date area
	int intFontThickness = (int)std::round(dblFontScale * 1.5);             // base font thickness on font scale
	int intBaseline = 0;

	cv::Size textSize = cv::getTextSize(likelyDate.strChars, intFontFace, dblFontScale, intFontThickness, &intBaseline);      // call getTextSize

	ptCenterOfTextArea.x = (int)likelyDate.rrLocationOfDateInScene.center.x;         // the horizontal location of the text area is the same as the Date

	if (likelyDate.rrLocationOfDateInScene.center.y < (imgOriginalScene.rows * 0.75)) {      // if the license Date is in the upper 3/4 of the image
																							// write the chars in below the Date
		ptCenterOfTextArea.y = (int)std::round(likelyDate.rrLocationOfDateInScene.center.y) + (int)std::round((double)likelyDate.imgDate.rows * 1.6);
	}
	else {                                                                                // else if the license Date is in the lower 1/4 of the image
																						  // write the chars in above the Date
		ptCenterOfTextArea.y = (int)std::round(likelyDate.rrLocationOfDateInScene.center.y) - (int)std::round((double)likelyDate.imgDate.rows * 1.6);
	}

	ptLowerLeftTextOrigin.x = (int)(ptCenterOfTextArea.x - (textSize.width / 2));           // calculate the lower left origin of the text area
	ptLowerLeftTextOrigin.y = (int)(ptCenterOfTextArea.y + (textSize.height / 2));          // based on the text area center, width, and height

																							// write the text on the image
	cv::putText(imgOriginalScene, likelyDate.strChars, ptLowerLeftTextOrigin, intFontFace, dblFontScale, SCALAR_YELLOW, intFontThickness);
}
