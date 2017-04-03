#include "Classification.h"

// global variables ///////////////////////////////////////////////////////////////////////////////
cv::Ptr<cv::ml::KNearest> kNearest = cv::ml::KNearest::create();

///////////////////////////////////////////////////////////////////////////////////////////////////
bool loadKNNDataAndTrainKNN(void) {

	// read in training classifications ///////////////////////////////////////////////////

	cv::Mat matClassificationInts;              // we will read the classification numbers into this variable as though it is a vector

	cv::FileStorage fsClassifications("classifications.xml", cv::FileStorage::READ);        // open the classifications file

	if (fsClassifications.isOpened() == false) {                                                        // if the file was not opened successfully
		std::cout << "error, unable to open training classifications file, exiting program\n\n";        // show error message
		return(false);                                                                                  // and exit program
	}

	fsClassifications["classifications"] >> matClassificationInts;          // read classifications section into Mat classifications variable
	fsClassifications.release();                                            // close the classifications file

																			// read in training images ////////////////////////////////////////////////////////////

	cv::Mat matTrainingImagesAsFlattenedFloats;         // we will read multiple images into this single image variable as though it is a vector

	cv::FileStorage fsTrainingImages("images.xml", cv::FileStorage::READ);              // open the training images file

	if (fsTrainingImages.isOpened() == false) {                                                 // if the file was not opened successfully
		std::cout << "error, unable to open training images file, exiting program\n\n";         // show error message
		return(false);                                                                          // and exit program
	}

	fsTrainingImages["images"] >> matTrainingImagesAsFlattenedFloats;           // read images section into Mat training images variable
	fsTrainingImages.release();                                                 // close the traning images file

																				// train //////////////////////////////////////////////////////////////////////////////

																				// finally we get to the call to train, note that both parameters have to be of type Mat (a single Mat)
																				// even though in reality they are multiple images / numbers
	kNearest->setDefaultK(1);

	kNearest->train(matTrainingImagesAsFlattenedFloats, cv::ml::ROW_SAMPLE, matClassificationInts);

	return true;
}