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
    "FAST" 每 FastFeatureDetector
    "STAR" 每 StarFeatureDetector
    "SIFT" 每 SIFT (nonfree module)
    "SURF" 每 SURF (nonfree module)
    "ORB" 每 ORB
    "BRISK" 每 BRISK
    "MSER" 每 MSER
    "GFTT" 每 GoodFeaturesToTrackDetector
    "HARRIS" 每 GoodFeaturesToTrackDetector with Harris detector enabled
    "Dense" 每 DenseFeatureDetector
    "SimpleBlob" 每 SimpleBlobDetector
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


