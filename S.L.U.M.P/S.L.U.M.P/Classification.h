#pragma once
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>

extern cv::Ptr<cv::ml::KNearest>  kNearest;

bool loadKNNDataAndTrainKNN(void);