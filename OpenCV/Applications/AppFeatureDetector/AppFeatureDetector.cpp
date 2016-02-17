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

    //-- Create feature detector
    Ptr<FeatureDetector> detector = FeatureDetector::create(FEATURE_TYPE);

    //-- Detect keypoints
    vector<KeyPoint> keypoints_object, keypoints_scene;
    detector->detect(objectImage, keypoints_object);
    printf("-- object keypoints number: %d\n", keypoints_object.size());
    detector->detect(sceneImage, keypoints_scene);
    printf("-- scene keypoints number: %d\n", keypoints_scene.size());

    if (DO_SHOW_INTERNAL_STEP_IMAGES)
    {
        //-- Draw keypoints
        Mat outputImage_keypoints_object;
        drawKeypoints(objectImage, keypoints_object, outputImage_keypoints_object);
        imshow("outputImage_keypoints_object", outputImage_keypoints_object);

        Mat outputImage_keypoints_scene;
        drawKeypoints(sceneImage, keypoints_scene, outputImage_keypoints_scene);
        imshow("outputImage_keypoints_scene", outputImage_keypoints_scene);

        cvWaitKey(0);
    }

    printf("\n");

    
    // Step 2: Extract keypoint descriptors
    printf("Step 2: Extract keypoint descriptors\n");

    //-- Create keypoint descriptor extractor
    Ptr<DescriptorExtractor> extractor = DescriptorExtractor::create(FEATURE_TYPE);

    //-- Extract keypoint descriptors
    Mat descriptors_object, descriptors_scene;
    extractor->compute(objectImage, keypoints_object, descriptors_object);
    printf("-- object keypoint descriptors matrix size: %d X %d\n", descriptors_object.cols, descriptors_object.rows);
    extractor->compute(sceneImage, keypoints_scene, descriptors_scene);
    printf("-- scene keypoint descriptors matrix size: %d X %d\n", descriptors_scene.cols, descriptors_scene.rows);

    printf("\n");


    // Step 3: Match 

    system("pause");
    return EXIT_SUCCESS;
}


void AppFeatureDetector::readme()
{
    std::cout << " Usage: AppFeatureDetector <objectImage> <sceneImage>" << std::endl;
}


