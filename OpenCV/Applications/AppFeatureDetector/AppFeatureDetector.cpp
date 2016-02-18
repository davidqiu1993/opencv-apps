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
    if (argc != 3)
    {
        readme(); return -1;
    }


#if APP_FEATURE_DETECTOR_USE_SIFT_SURF
    // Initialize the nonfree module
    initModule_nonfree();
#endif

    // Load input images
    Mat objectImage = imread(argv[1]);
    Mat sceneImage = imread(argv[2]);


    // Configurations
    printf("Configurations\n");

    printf("-- feature type: %s\n", FEATURE_TYPE.c_str());
    
    printf("\n");


    // Step 1: Detect keypoints
    printf("Step 1: Detect keypoints\n");
    step_clock_start();

    //-- Create feature detector
    Ptr<FeatureDetector> detector = FeatureDetector::create(FEATURE_TYPE);

    //-- Detect keypoints
    vector<KeyPoint> keypoints_object, keypoints_scene;
    detector->detect(objectImage, keypoints_object);
    printf("-- object keypoints number: %d\n", keypoints_object.size());
    detector->detect(sceneImage, keypoints_scene);
    printf("-- scene keypoints number: %d\n", keypoints_scene.size());

    step_clock_stop();

    if (DO_SHOW_INTERNAL_STEP_IMAGES)
    {
        //-- Draw keypoints
        Mat outputImage_keypoints_object;
        drawKeypoints(objectImage, keypoints_object, outputImage_keypoints_object);
        imshow("outputImage_keypoints_object", outputImage_keypoints_object);

        Mat outputImage_keypoints_scene;
        drawKeypoints(sceneImage, keypoints_scene, outputImage_keypoints_scene);
        imshow("outputImage_keypoints_scene", outputImage_keypoints_scene);
    }

    printf("\n");

    
    // Step 2: Extract keypoint descriptors
    printf("Step 2: Extract keypoint descriptors\n");
    step_clock_start();

    //-- Create keypoint descriptor extractor
    Ptr<DescriptorExtractor> extractor = DescriptorExtractor::create(FEATURE_TYPE);

    //-- Extract keypoint descriptors
    Mat descriptors_object, descriptors_scene;
    extractor->compute(objectImage, keypoints_object, descriptors_object);
    printf("-- object keypoint descriptors matrix size: %d X %d\n", descriptors_object.cols, descriptors_object.rows);
    extractor->compute(sceneImage, keypoints_scene, descriptors_scene);
    printf("-- scene keypoint descriptors matrix size: %d X %d\n", descriptors_scene.cols, descriptors_scene.rows);

    step_clock_stop();

    printf("\n");


    // Step 3: Match the keypoint descriptors
    printf("Step 3: Match the keypoint descriptors\n");
    step_clock_start();

    //-- Create keypoint descriptors matcher
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(MATCHER_TYPE);
    if (matcher.empty())
    {
        printf("-- error: Can not create detector or descriptor exstractor or descriptor matcher of given types\n");
        return -1;
    }

    //-- Convert descriptors type
    if (MATCHER_TYPE == "FlannBased")
    {
        if (descriptors_object.type() != CV_32F)
        {
            descriptors_object.convertTo(descriptors_object, CV_32F);
        }

        if (descriptors_scene.type() != CV_32F)
        {
            descriptors_scene.convertTo(descriptors_scene, CV_32F);
        }
    }

    //-- Match the keypoint descriptors
    vector<DMatch> matches;
    matcher->match(descriptors_object, descriptors_scene, matches);
    printf("-- matches number: %d\n", matches.size());

    step_clock_stop();

    if (DO_PRINT_MATCHES)
    {
        //-- Print the matches
        for (unsigned int i = 0; i < matches.size(); ++i)
        {
            printf("-- match: <object.%d, scene.%d>\n", matches[i].queryIdx, matches[i].trainIdx);
        }
    }

    if (DO_SHOW_INTERNAL_STEP_IMAGES)
    {
        //-- Draw the matches
        Mat outputImage_matches;
        drawMatches(objectImage, keypoints_object, sceneImage, keypoints_scene, matches, outputImage_matches);
        imshow("outputImage_matches", outputImage_matches);
    }

    printf("\n");


    // Step 4: Calculate the minimum and maximum distances between the matches
    printf("Step 4: Calculate the minimum and maximum distances between the matches\n");
    step_clock_start();

    double min_distance = 1000;
    double max_distance = 0;
    for (unsigned int i = 0; i < matches.size(); ++i)
    {
        double distance = matches[i].distance;
        if (distance < min_distance) min_distance = distance;
        if (distance > max_distance) max_distance = distance;
    }
    printf("-- minimum distance: %lf\n", min_distance);
    printf("-- maximum distance: %lf\n", max_distance);

    step_clock_stop();

    printf("\n");


    // Step 5: Find good matches
    printf("Step 5: Find good matches\n");
    step_clock_start();

    vector<DMatch> goodMatches;
    for (unsigned int i = 0; i < matches.size(); ++i)
    {
        if (matches[i].distance < min_distance * 3)
        {
            goodMatches.push_back(matches[i]);
        }
    }
    printf("-- good matches number: %d\n", goodMatches.size());

    step_clock_stop();

    if (DO_PRINT_GOOD_MATCHES)
    {
        //-- Print good matches
        for (unsigned int i = 0; i < goodMatches.size(); ++i)
        {
            printf("-- good match: <object.%d, scene.%d>\n", goodMatches[i].queryIdx, goodMatches[i].trainIdx);
        }
    }

    if (DO_SHOW_INTERNAL_STEP_IMAGES)
    {
        //-- Draw the good matches
        Mat outputImage_goodMatches;
        drawMatches(objectImage, keypoints_object, sceneImage, keypoints_scene, goodMatches, outputImage_goodMatches);
        imshow("outputImage_goodMatches", outputImage_goodMatches);
    }

    printf("\n");


    // Step 6: Calculate the homography matrix
    printf("Step 6: Calculate the homography matrix\n");
    step_clock_start();

    std::vector<Point2f> keypointLocations_object;
    std::vector<Point2f> keypointLocations_scene;

    for (unsigned int i = 0; i < goodMatches.size(); i++)
    {
        //-- Get the keypoints from the good matches
        keypointLocations_object.push_back(keypoints_object[goodMatches[i].queryIdx].pt);
        keypointLocations_scene.push_back(keypoints_scene[goodMatches[i].trainIdx].pt);
    }

    Mat H = findHomography(keypointLocations_object, keypointLocations_scene, CV_RANSAC);
    printf("-- H = \n");
    cout << H << endl;

    step_clock_stop();

    printf("\n");


    // Step 7: Localize the object
    printf("Step 7: Localize the object\n");
    step_clock_start();

    vector<Point2f> corners_object(4);
    corners_object[0] = cvPoint(0, 0);
    corners_object[1] = cvPoint(objectImage.cols, 0);
    corners_object[2] = cvPoint(objectImage.cols, objectImage.rows);
    corners_object[3] = cvPoint(0, objectImage.rows);

    vector<Point2f> corners_scene(4);
    perspectiveTransform(corners_object, corners_scene, H);

    step_clock_stop();

    printf("\n");


    // Step 8: Draw the detected object in the scene
    printf("Step 8: Draw the detected object in the scene\n");
    step_clock_start();

    Mat resultImage;
    sceneImage.copyTo(resultImage);

    line(resultImage, corners_scene[0], corners_scene[1], cvScalar(0, 255, 0), 4);
    line(resultImage, corners_scene[1], corners_scene[2], cvScalar(0, 255, 0), 4);
    line(resultImage, corners_scene[2], corners_scene[3], cvScalar(0, 255, 0), 4);
    line(resultImage, corners_scene[3], corners_scene[0], cvScalar(0, 255, 0), 4);

    step_clock_stop();

    printf("\n");

    imshow("resultImage", resultImage);


    cvWaitKey(0);
    system("pause");
    return EXIT_SUCCESS;
}


void AppFeatureDetector::step_clock_start()
{
    clock_step_begin = clock();
}

void AppFeatureDetector::step_clock_stop()
{
    clock_step_end = clock();
    printf("-- time: %dms\n", clock_step_end - clock_step_begin);
}

void AppFeatureDetector::readme()
{
    std::cout << " Usage: AppFeatureDetector <objectImage> <sceneImage>" << std::endl;
}


