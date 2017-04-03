#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>


#include<iostream>
#include<conio.h>           // may have to modify this line if not using Windows

#include "Classification.h"

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

	src = cv::imread("Dato_Toro.jpg", CV_LOAD_IMAGE_COLOR);   // Read the file
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
	return 0;
}