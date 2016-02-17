/**
* Feature detector demo application.
*
* Authors:
*    - David Qiu (david.qiu@dji.com)
*
* Copyright (C) 2016, David Qiu. All rights Reserved.
*/

#ifndef APP_FEATURE_DETECTOR_H
#define APP_FEATURE_DETECTOR_H

#include "../App.h"


class AppFeatureDetector : public App
{
public:
    AppFeatureDetector() :
        FEATURE_TYPE("ORB"),
        DO_SHOW_INTERNAL_STEP_IMAGES(true) { ; }

    int run(int argc, char* argv[]);

public:
    /*
    Feature Type:
    - "FAST"       = FastFeatureDetector
    - "STAR"       = StarFeatureDetector
    - "SIFT"       = SIFT (nonfree module)
    - "SURF"       = SURF (nonfree module)
    - "ORB"        = ORB
    - "BRISK"      = BRISK
    - "MSER"       = MSER
    - "GFTT"       = GoodFeaturesToTrackDetector
    - "HARRIS"     = GoodFeaturesToTrackDetector with Harris detector enabled
    - "Dense"      = DenseFeatureDetector
    - "SimpleBlob" = SimpleBlobDetector
    */
    const string FEATURE_TYPE;
    const bool DO_SHOW_INTERNAL_STEP_IMAGES;

private:
    void readme();
};


#endif // APP_FEATURE_DETECTOR_H
