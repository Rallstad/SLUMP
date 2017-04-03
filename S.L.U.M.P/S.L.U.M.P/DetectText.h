#pragma once


#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include "PossibleDate.h"
#include "PossibleChar.h"
#include "Preprocess.h"
#include "DetectChars.h"

// global constants ///////////////////////////////////////////////////////////////////////////////
const double DATE_WIDTH_PADDING_FACTOR = 1.3;
const double DATE_HEIGHT_PADDING_FACTOR = 1.5;

// function prototypes ////////////////////////////////////////////////////////////////////////////
std::vector<PossibleDate> detectTextInScene(cv::Mat &imgOriginalScene);

std::vector<PossibleChar> findPossibleCharsInScene(cv::Mat &imgThresh);

PossibleDate extractDate(cv::Mat &imgOriginal, std::vector<PossibleChar> &vectorOfMatchingChars);
