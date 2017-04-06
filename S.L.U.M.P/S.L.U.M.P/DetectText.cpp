// DetectText.cpp

#include "DetectText.h"
#include "parameters.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<PossibleDate> detectTextInScene(cv::Mat &imgOriginalScene) {
	std::vector<PossibleDate> vectorOfPossibleDates;			// this will be the return value

	cv::Mat imgGrayscaleScene;
	cv::Mat imgThreshScene;
	cv::Mat imgContours(imgOriginalScene.size(), CV_16UC3, SCALAR_BLACK);

	cv::RNG rng;

	cv::destroyAllWindows();

#ifdef SHOW_STEPS
	cv::imshow("0", imgOriginalScene);
#endif	// SHOW_STEPS

	preprocess(imgOriginalScene, imgGrayscaleScene, imgThreshScene);        // preprocess to get grayscale and threshold images

#ifdef SHOW_STEPS
	cv::imshow("1a", imgGrayscaleScene);
	cv::imshow("1b", imgThreshScene);
#endif	// SHOW_STEPS

	// find all possible chars in the scene,
	// this function first finds all contours, then only includes contours that could be chars (without comparison to other chars yet)
	std::vector<PossibleChar> vectorOfPossibleCharsInScene = findPossibleCharsInScene(imgThreshScene);

#ifdef SHOW_STEPS
	std::cout << "step 2 - vectorOfPossibleCharsInScene.Count = " << vectorOfPossibleCharsInScene.size() << std::endl;        // 131 with MCLRNF1 image

	imgContours = cv::Mat(imgOriginalScene.size(), CV_8UC3, SCALAR_BLACK);
	std::vector<std::vector<cv::Point> > contours;

	for (auto &possibleChar : vectorOfPossibleCharsInScene) {
		contours.push_back(possibleChar.contour);
	}
	cv::drawContours(imgContours, contours, -1, SCALAR_WHITE);
	cv::imshow("2b", imgContours);
#endif	// SHOW_STEPS

	// given a vector of all possible chars, find groups of matching chars
	// in the next steps each group of matching chars will attempt to be recognized as a date
	std::vector<std::vector<PossibleChar> > vectorOfVectorsOfMatchingCharsInScene = findVectorOfVectorsOfMatchingChars(vectorOfPossibleCharsInScene);

#ifdef SHOW_STEPS
	std::cout << "step 3 - vectorOfVectorsOfMatchingCharsInScene.size() = " << vectorOfVectorsOfMatchingCharsInScene.size() << std::endl;        // 13 with MCLRNF1 image

	imgContours = cv::Mat(imgOriginalScene.size(), CV_8UC3, SCALAR_BLACK);

	for (auto &vectorOfMatchingChars : vectorOfVectorsOfMatchingCharsInScene) {
		int intRandomBlue = rng.uniform(0, 256);
		int intRandomGreen = rng.uniform(0, 256);
		int intRandomRed = rng.uniform(0, 256);

		std::vector<std::vector<cv::Point> > contours;

		for (auto &matchingChar : vectorOfMatchingChars) {
			contours.push_back(matchingChar.contour);
		}
		cv::drawContours(imgContours, contours, -1, cv::Scalar((double)intRandomBlue, (double)intRandomGreen, (double)intRandomRed));
	}
	cv::imshow("3", imgContours);
#endif	// SHOW_STEPS

	for (auto &vectorOfMatchingChars : vectorOfVectorsOfMatchingCharsInScene) {                     // for each group of matching chars
		PossibleDate PossibleDate = extractDate(imgOriginalScene, vectorOfMatchingChars);        // attempt to extract date

		if (PossibleDate.imgDate.empty() == false) {                                              // if date was found
			vectorOfPossibleDates.push_back(PossibleDate);                                        // add to vector of possible dates
		}
	}

	std::cout << std::endl << vectorOfPossibleDates.size() << " possible dates found" << std::endl;       // 13 with MCLRNF1 image

#ifdef SHOW_STEPS
	std::cout << std::endl;
	cv::imshow("4a", imgContours);

	for (unsigned int i = 0; i < vectorOfPossibleDates.size(); i++) {
		cv::Point2f p2fRectPoints[4];

		vectorOfPossibleDates[i].rrLocationOfDateInScene.points(p2fRectPoints);

		for (int j = 0; j < 4; j++) {
			cv::line(imgContours, p2fRectPoints[j], p2fRectPoints[(j + 1) % 4], SCALAR_RED, 2);
		}
		cv::imshow("4a", imgContours);

		std::cout << "possible date " << i << ", click on any image and press a key to continue . . ." << std::endl;

		cv::imshow("4b", vectorOfPossibleDates[i].imgDate);
		cv::waitKey(0);
	}
	std::cout << std::endl << "date detection complete, click on any image and press a key to begin char recognition . . ." << std::endl << std::endl;
	cv::waitKey(0);
#endif	// SHOW_STEPS

	return vectorOfPossibleDates;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<PossibleChar> findPossibleCharsInScene(cv::Mat &imgThresh) {
	std::vector<PossibleChar> vectorOfPossibleChars;            // this will be the return value

	cv::Mat imgContours(imgThresh.size(), CV_8UC3, SCALAR_BLACK);
	int intCountOfPossibleChars = 0;

	cv::Mat imgThreshCopy = imgThresh.clone();

	std::vector<std::vector<cv::Point> > contours;

	cv::findContours(imgThreshCopy, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);        // find all contours

	for (unsigned int i = 0; i < contours.size(); i++) {                // for each contour
#ifdef SHOW_STEPS
		cv::drawContours(imgContours, contours, i, SCALAR_WHITE);
#endif	// SHOW_STEPS
		PossibleChar possibleChar(contours[i]);

		if (checkIfPossibleChar(possibleChar)) {                // if contour is a possible char, note this does not compare to other chars (yet) . . .
			intCountOfPossibleChars++;                          // increment count of possible chars
			vectorOfPossibleChars.push_back(possibleChar);      // and add to vector of possible chars
		}
	}

#ifdef SHOW_STEPS
	std::cout << std::endl << "contours.size() = " << contours.size() << std::endl;                         // 2362 with MCLRNF1 image
	std::cout << "step 2 - intCountOfValidPossibleChars = " << intCountOfPossibleChars << std::endl;        // 131 with MCLRNF1 image
	cv::imshow("2a", imgContours);
#endif	// SHOW_STEPS

	return(vectorOfPossibleChars);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PossibleDate extractDate(cv::Mat &imgOriginal, std::vector<PossibleChar> &vectorOfMatchingChars) {
	PossibleDate PossibleDate;            // this will be the return value

											// sort chars from left to right based on x position
	std::sort(vectorOfMatchingChars.begin(), vectorOfMatchingChars.end(), PossibleChar::sortCharsLeftToRight);

	// calculate the center point of the date
	double dblDateCenterX = (double)(vectorOfMatchingChars[0].intCenterX + vectorOfMatchingChars[vectorOfMatchingChars.size() - 1].intCenterX) / 2.0;
	double dblDateCenterY = (double)(vectorOfMatchingChars[0].intCenterY + vectorOfMatchingChars[vectorOfMatchingChars.size() - 1].intCenterY) / 2.0;
	cv::Point2d p2dDateCenter(dblDateCenterX, dblDateCenterY);

	// calculate date width and height
	int intDateWidth = (int)((vectorOfMatchingChars[vectorOfMatchingChars.size() - 1].boundingRect.x + vectorOfMatchingChars[vectorOfMatchingChars.size() - 1].boundingRect.width - vectorOfMatchingChars[0].boundingRect.x) * DATE_WIDTH_PADDING_FACTOR);

	double intTotalOfCharHeights = 0;

	for (auto &matchingChar : vectorOfMatchingChars) {
		intTotalOfCharHeights = intTotalOfCharHeights + matchingChar.boundingRect.height;
	}

	double dblAverageCharHeight = (double)intTotalOfCharHeights / vectorOfMatchingChars.size();

	int intDateHeight = (int)(dblAverageCharHeight * DATE_HEIGHT_PADDING_FACTOR);

	// calculate correction angle of date region
	double dblOpposite = vectorOfMatchingChars[vectorOfMatchingChars.size() - 1].intCenterY - vectorOfMatchingChars[0].intCenterY;
	double dblHypotenuse = distanceBetweenChars(vectorOfMatchingChars[0], vectorOfMatchingChars[vectorOfMatchingChars.size() - 1]);
	double dblCorrectionAngleInRad = asin(dblOpposite / dblHypotenuse);
	double dblCorrectionAngleInDeg = dblCorrectionAngleInRad * (180.0 / CV_PI);

	// assign rotated rect member variable of possible date
	PossibleDate.rrLocationOfDateInScene = cv::RotatedRect(p2dDateCenter, cv::Size2f((float)intDateWidth, (float)intDateHeight), (float)dblCorrectionAngleInDeg);

	cv::Mat rotationMatrix;             // final steps are to perform the actual rotation
	cv::Mat imgRotated;
	cv::Mat imgCropped;

	rotationMatrix = cv::getRotationMatrix2D(p2dDateCenter, dblCorrectionAngleInDeg, 1.0);         // get the rotation matrix for our calculated correction angle

	cv::warpAffine(imgOriginal, imgRotated, rotationMatrix, imgOriginal.size());            // rotate the entire image

																							// crop out the actual date portion of the rotated image
	cv::getRectSubPix(imgRotated, PossibleDate.rrLocationOfDateInScene.size, PossibleDate.rrLocationOfDateInScene.center, imgCropped);

	PossibleDate.imgDate = imgCropped;            // copy the cropped date image into the applicable member variable of the possible date

	return(PossibleDate);
}

