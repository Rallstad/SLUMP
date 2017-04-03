#pragma once

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

// global variables ///////////////////////////////////////////////////////////////////////////////
const cv::Size GAUSSIAN_SMOOTH_FILTER_SIZE = cv::Size(11, 15);
const int ADAPTIVE_THRESH_BLOCK_SIZE = 61;
const int ADAPTIVE_THRESH_WEIGHT = 6;

// function prototypes ////////////////////////////////////////////////////////////////////////////

void preprocess(cv::Mat &imgOriginal, cv::Mat &imgGrayscale, cv::Mat &imgThresh);

cv::Mat extractValue(cv::Mat &imgOriginal);

cv::Mat maximizeContrast(cv::Mat &imgGrayscale);