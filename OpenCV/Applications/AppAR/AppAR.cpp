/**
* AR application.
*
* Authors:
*    - David Qiu (david.qiu@dji.com)
*
* Copyright (C) 2016, David Qiu. All rights Reserved.
*/

#include "AppAR.h"
#include <conio.h>
#include <ctime>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#define DO_SHOW_INTERNAL_STEP_IMAGES 1
#define INPUT_FILE_TYPE              1 // 0 = Image, 1 = Video

#define MAX_PROCESSING_IMAGE_WIDTH 480

clock_t clock_begin;
clock_t clock_end;
clock_t clock_step_begin;
clock_t clock_step_end;

void process(const Mat& img_object, const Mat& img_scene);
void step_clock_start();
void step_clock_stop();
void readme();


/**
 * @brief Application entry.
 */
int AppAR::run(int argc, char* argv[])
{
    if (argc != 3)
    {
        readme(); return -1;
    }
    

#if (INPUT_FILE_TYPE == 0)

    clock_begin = clock();

    //-- Step 0: Load and crop the images
    printf("Step 0: Load and crop the input images\n");
    step_clock_start();

    Mat img_object_read = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
    Mat img_object;
    img_object_read.copyTo(img_object);

    Mat img_scene_read = imread(argv[2], CV_LOAD_IMAGE_GRAYSCALE);
    Mat img_scene;
    if (img_scene_read.cols > MAX_PROCESSING_IMAGE_WIDTH)
    {
        resize(img_scene_read, img_scene, cvSize(MAX_PROCESSING_IMAGE_WIDTH, MAX_PROCESSING_IMAGE_WIDTH * img_scene_read.rows / img_scene_read.cols));
    }
    else
    {
        img_scene_read.copyTo(img_scene);
    }

    if (!img_object.data || !img_scene.data)
    {
        std::cout << " --(!) Error reading images " << std::endl; return -1;
    }
    
    step_clock_stop();

    //-- Process
    process(img_object, img_scene);

    clock_end = clock();
    printf("-- total time: %dms\n", clock_end - clock_begin);

    waitKey(0);

#elif (INPUT_FILE_TYPE == 1)

    //-- Preprocess 0: Load the input video
    printf("Preprocess 0: Load the input video\n");

    VideoCapture capture(argv[2]);
    if (!capture.isOpened())
    {
        return -1;
    }

    double fps = capture.get(CV_CAP_PROP_FPS);
    printf("-- fps: %lf\n", fps);

    unsigned int frame_count = capture.get(CV_CAP_PROP_FRAME_COUNT);
    printf("-- frame count: %d\n", frame_count);

    unsigned int frame_width = capture.get(CV_CAP_PROP_FRAME_WIDTH);
    unsigned int frame_height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    printf("-- frame size: %d X %d\n", frame_width, frame_height);

    printf("\n");


    //-- Preprocess 1: Load and crop the object image
    printf("Preprocess 1: Load and crop the object image\n");
    
    Mat img_object_read = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
    Mat img_object;
    img_object_read.copyTo(img_object);
    
    printf("\n");


    //-- Loop read the frames
    Mat frame;
    unsigned int frame_index = -1;
    while (capture.read(frame))
    {
        ++frame_index;
        printf("Process\n");
        printf("-- frame index: %d\n", frame_index);


        clock_begin = clock();

        //-- Step 0: Crop the frame
        printf("Step 0: Crop the frame\n");
        step_clock_start();
        
        Mat img_scene_read;
        cvtColor(frame, img_scene_read, CV_RGB2GRAY);
        Mat img_scene;
        if (img_scene_read.cols > MAX_PROCESSING_IMAGE_WIDTH)
        {
            resize(img_scene_read, img_scene, cvSize(MAX_PROCESSING_IMAGE_WIDTH, MAX_PROCESSING_IMAGE_WIDTH * img_scene_read.rows / img_scene_read.cols));
        }
        else
        {
            img_scene_read.copyTo(img_scene);
        }

        if (!img_object.data || !img_scene.data)
        {
            std::cout << " --(!) Error reading images " << std::endl; return -1;
        }

        step_clock_stop();

        //-- Process
        //imshow("img_scene", img_scene);
        process(img_object, img_scene);

        clock_end = clock();
        printf("-- total time: %dms\n", clock_end - clock_begin);

        if (cvWaitKey(1) == 32) // [ Space ]
        {
            cvWaitKey(0);
        }
    }
    

    //-- Release
    capture.release();

#endif


    system("pause");
    return EXIT_SUCCESS;
}

void process(const Mat& img_object, const Mat& img_scene)
{
    //-- Step 1: Detect the keypoints using SURF Detector
    printf("Step 1: Detect the keypoints using SURF Detector\n");
    step_clock_start();

    int minHessian = 400;

    SurfFeatureDetector detector(minHessian);

    std::vector<KeyPoint> keypoints_object, keypoints_scene;

    detector.detect(img_object, keypoints_object);
    detector.detect(img_scene, keypoints_scene);

    step_clock_stop();


    //-- Step 2: Calculate descriptors (feature vectors)
    printf("Step 2: Calculate descriptors (feature vectors)\n");
    step_clock_start();

    SurfDescriptorExtractor extractor;

    Mat descriptors_object, descriptors_scene;

    extractor.compute(img_object, keypoints_object, descriptors_object);
    extractor.compute(img_scene, keypoints_scene, descriptors_scene);

    step_clock_stop();


    //-- Step 3: Matching descriptor vectors using FLANN matcher
    printf("Step 3: Matching descriptor vectors using FLANN matcher\n");
    step_clock_start();

    FlannBasedMatcher matcher;
    std::vector< DMatch > matches;
    matcher.match(descriptors_object, descriptors_scene, matches);

    step_clock_stop();


    //-- Step 4: Quick calculation of max and min distances between keypoints
    printf("Step 4: Quick calculation of max and min distances between keypoints\n");
    step_clock_start();

    double max_dist = 0; double min_dist = 100;
    for (int i = 0; i < descriptors_object.rows; i++)
    {
        double dist = matches[i].distance;
        if (dist < min_dist) min_dist = dist;
        if (dist > max_dist) max_dist = dist;
    }

    printf("-- Max dist : %f \n", max_dist);
    printf("-- Min dist : %f \n", min_dist);

    step_clock_stop();


    //-- Step 5: Find good matches (i.e. whose distance is less than 2*min_dist)
    printf("Step 5: Find good matches (i.e. whose distance is less than 2*min_dist)\n");
    step_clock_start();

    std::vector< DMatch > good_matches;

    for (int i = 0; i < descriptors_object.rows; i++)
    {
        if (matches[i].distance < 2 * min_dist)
        {
            good_matches.push_back(matches[i]);
        }
    }

#if DO_SHOW_INTERNAL_STEP_IMAGES
    Mat img_matches;
    drawMatches(img_object, keypoints_object, img_scene, keypoints_scene,
        good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
        vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
#endif

    step_clock_stop();


    //-- Step 6: Localize the object
    printf("Step 6: Localize the object\n");
    step_clock_start();

    std::vector<Point2f> obj;
    std::vector<Point2f> scene;

    for (int i = 0; i < good_matches.size(); i++)
    {
        //-- Get the keypoints from the good matches
        obj.push_back(keypoints_object[good_matches[i].queryIdx].pt);
        scene.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
    }

    Mat H = findHomography(obj, scene, CV_RANSAC);

    step_clock_stop();


    //-- Check distance threshold
    if (min_dist < 0.15)
    {
        //-- Step 7: Get the corners from the image_1 (the object to be detected)
        printf("Step 7: Get the corners from the image_1 (the object to be detected)\n");
        step_clock_start();

        std::vector<Point2f> obj_corners(4);
        obj_corners[0] = cvPoint(0, 0); obj_corners[1] = cvPoint(img_object.cols, 0);
        obj_corners[2] = cvPoint(img_object.cols, img_object.rows); obj_corners[3] = cvPoint(0, img_object.rows);
        std::vector<Point2f> scene_corners(4);

        perspectiveTransform(obj_corners, scene_corners, H);

        step_clock_stop();

#if DO_SHOW_INTERNAL_STEP_IMAGES
        //-- Draw lines between the corners (the mapped object in the scene - image_2 )
        line(img_matches, scene_corners[0] + Point2f(img_object.cols, 0), scene_corners[1] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
        line(img_matches, scene_corners[1] + Point2f(img_object.cols, 0), scene_corners[2] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
        line(img_matches, scene_corners[2] + Point2f(img_object.cols, 0), scene_corners[3] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
        line(img_matches, scene_corners[3] + Point2f(img_object.cols, 0), scene_corners[0] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
#endif
    }


#if DO_SHOW_INTERNAL_STEP_IMAGES
    //-- Summary
    printf("Summary\n");

    //-- Show detected matches
    imshow("Good Matches & Object detection", img_matches);
#endif
}

void step_clock_start()
{
    clock_step_begin = clock();
}

void step_clock_stop()
{
    clock_step_end = clock();
    printf("-- time: %dms\n\n", clock_step_end - clock_step_begin);
}

void readme()
{
    std::cout << " Usage: ./SURF_descriptor <img1> <img2>" << std::endl;
}


