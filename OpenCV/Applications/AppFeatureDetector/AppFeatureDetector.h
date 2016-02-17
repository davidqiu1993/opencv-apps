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
#include <ctime>


class AppFeatureDetector : public App
{
public:
    AppFeatureDetector() :
        FEATURE_TYPE("ORB"),
        MATCHER_TYPE("FlannBased"),
        DO_SHOW_INTERNAL_STEP_IMAGES(true),
        DO_PRINT_MATCHES(false),
        DO_PRINT_GOOD_MATCHES(false) { ; }

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
    /*
    Matcher Type:
    - "BruteForce"            = Brute Force L2 Matcher
    - "BruteForce-L1"         = Brute Force L1 Matcher
    - "BruteForce-Hamming"    = Brute Force Hamming Matcher
    - "BruteForce-Hamming(2)" = Brute Force Hamming 2 Matcher
    - "FlannBased"            = Flann Based Matcher
    */
    const string MATCHER_TYPE;
    const bool DO_SHOW_INTERNAL_STEP_IMAGES;
    const bool DO_PRINT_MATCHES;
    const bool DO_PRINT_GOOD_MATCHES;

private:
    void step_clock_start();
    void step_clock_stop();
    void readme();

private:
    clock_t clock_begin;
    clock_t clock_end;
    clock_t clock_step_begin;
    clock_t clock_step_end;
};


#endif // APP_FEATURE_DETECTOR_H
