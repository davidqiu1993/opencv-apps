/**
* Feature detector demo application.
*
* Authors:
*    - David Qiu (david.qiu@dji.com)
*
* Copyright (C) 2016, David Qiu. All rights Reserved.
*/

#include "AppFeatureDetector.h"
#include <conio.h>

#define APP_FEATURE_DETECTOR_USE_SIFT_SURF 0

#if APP_FEATURE_DETECTOR_USE_SIFT_SURF
#include <opencv2/nonfree/nonfree.hpp>
#endif


/**
 * @brief Application entry.
 */
int AppFeatureDetector::run(int argc, char* argv[])
{
    if (argc != 2)
    {
        readme(); return -1;
    }


#if APP_FEATURE_DETECTOR_USE_SIFT_SURF
    // Initialize the nonfree module
    initModule_nonfree();
#endif

    // Load target image
    Mat targetImage = imread(argv[1]);

    // Create feature detector
    /*
    "FAST" �C FastFeatureDetector
    "STAR" �C StarFeatureDetector
    "SIFT" �C SIFT (nonfree module)
    "SURF" �C SURF (nonfree module)
    "ORB" �C ORB
    "BRISK" �C BRISK
    "MSER" �C MSER
    "GFTT" �C GoodFeaturesToTrackDetector
    "HARRIS" �C GoodFeaturesToTrackDetector with Harris detector enabled
    "Dense" �C DenseFeatureDetector
    "SimpleBlob" �C SimpleBlobDetector
    */
    Ptr<FeatureDetector> detector = FeatureDetector::create("ORB");
    
    // Detect keypoints
    vector<KeyPoint> keypoints;
    detector->detect(targetImage, keypoints);

    

    while (!_kbhit());

    return EXIT_SUCCESS;
}


void AppFeatureDetector::readme()
{
    std::cout << " Usage: AppFeatureDetector <targetImage>" << std::endl;
}


