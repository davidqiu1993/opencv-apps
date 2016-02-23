/**
* AR application.
*
* Authors:
*    - David Qiu (david.qiu@dji.com)
*
* Copyright (C) 2016, David Qiu. All rights Reserved.
*/

#ifndef APP_AR_H
#define APP_AR_H

#include "../App.h"
#include <ctime>


class AppAR: public App
{
public:
    AppAR() :
        FEATURE_TYPE("SIFT"),
        MATCHER_TYPE("FlannBased"),
        DO_SHOW_INTERNAL_STEP_IMAGES(false),
        DO_PRINT_MATCHES(false),
        DO_PRINT_GOOD_MATCHES(false),
        INPUT_FILE_TYPE(1),
        MAX_PROCESSING_IMAGE_WIDTH(480) { ; }

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
    /*
    Input File Type:
    - 0 = Image
    - 1 = Video
    */
    const unsigned int INPUT_FILE_TYPE;
    const unsigned int MAX_PROCESSING_IMAGE_WIDTH;
    const bool DO_SHOW_INTERNAL_STEP_IMAGES;
    const bool DO_PRINT_MATCHES;
    const bool DO_PRINT_GOOD_MATCHES;

private:
    void process(const Mat& img_object, const Mat& img_scene);
    void step_clock_start();
    void step_clock_stop();
    void readme();

private:
    clock_t clock_begin;
    clock_t clock_end;
    clock_t clock_step_begin;
    clock_t clock_step_end;
};


#endif // APP_AR_H
