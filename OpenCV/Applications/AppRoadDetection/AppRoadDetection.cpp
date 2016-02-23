/**
* Road detection application.
*
* Authors:
*    - David Qiu (david.qiu@dji.com)
*
* Copyright (C) 2016, David Qiu. All rights Reserved.
*/

#include "AppRoadDetection.h"
#include <opencv2/features2d/features2d.hpp>


/**
 * @brief Application entry.
 */
int AppRoadDetection::run(int argc, char* argv[])
{
    // Check parameters
    if (argc < 2)
    {
        readme();
    }


    // Step 1: Load input images
    printf("Step 1: Load input images\n");
    step_clock_start();

    Mat sceneImage = imread(argv[1]);
    Mat sceneImage_gray;
    cvtColor(sceneImage, sceneImage_gray, CV_RGB2GRAY);

    step_clock_stop();

    if (DO_SHOW_INTERNAL_STEP_IMAGES)
    {
        imshow("sceneImage_gray", sceneImage_gray);
    }

    printf("\n");


    // Step 2: Detect edges from the scene image
    printf("Step 2: Detect edges from the scene image\n");
    step_clock_start();

    const double CONFIG_CANNY_LOWER_THRESHOLD = 50.0;
    printf("-- canny lower threshold: %lf\n", CONFIG_CANNY_LOWER_THRESHOLD);

    const double CONFIG_CANNY_UPPER_THRESHOLD = 150.0;
    printf("-- canny upper threshold: %lf\n", CONFIG_CANNY_UPPER_THRESHOLD);

    Mat sceneImage_edges;
    Canny(sceneImage_gray, sceneImage_edges, CONFIG_CANNY_LOWER_THRESHOLD, CONFIG_CANNY_UPPER_THRESHOLD);

    step_clock_stop();

    if (DO_SHOW_INTERNAL_STEP_IMAGES)
    {
        imshow("sceneImage_edges", sceneImage_edges);
    }

    printf("\n");


    // Step 3: Hough lines detection
    printf("Step 3: Hough lines detection\n");
    step_clock_start();

    const double CONFIG_HOUGH_LINES_DISTANCE_PRECISION = 1.0;
    printf("-- hough lines distance precision: %lf\n", CONFIG_HOUGH_LINES_DISTANCE_PRECISION);

    const double CONFIG_HOUGH_LINES_ANGULAR_PRECISION = 1.0; // in degrees
    const double CONFIG_HOUGH_LINES_ANGULAR_PRECISION_RAD = CONFIG_HOUGH_LINES_ANGULAR_PRECISION * CV_PI / 180;
    printf("-- hough lines angular precision: %lf\n", CONFIG_HOUGH_LINES_ANGULAR_PRECISION);
    
    const int CONFIG_HOUGH_LINES_ACCUMULATION_THRESHOLD = 80;
    printf("-- hough lines accumulation threshold: %d\n", CONFIG_HOUGH_LINES_ACCUMULATION_THRESHOLD);

    const double CONFIG_HOUGH_LINES_MIN_LINE_LENGTH = 30;
    printf("-- hough lines minimum line length: %lf\n", CONFIG_HOUGH_LINES_MIN_LINE_LENGTH);

    const double CONFIG_HOUGH_LINES_MAX_LINE_GAP = 10;
    printf("-- hough lines maximum line gap: %lf\n", CONFIG_HOUGH_LINES_MAX_LINE_GAP);

    vector<Vec4i> lines_scene;
    HoughLinesP(sceneImage_edges, lines_scene,
        CONFIG_HOUGH_LINES_DISTANCE_PRECISION,
        CONFIG_HOUGH_LINES_ANGULAR_PRECISION_RAD,
        CONFIG_HOUGH_LINES_ACCUMULATION_THRESHOLD,
        CONFIG_HOUGH_LINES_MIN_LINE_LENGTH,
        CONFIG_HOUGH_LINES_MAX_LINE_GAP);

    step_clock_stop();

    if (DO_SHOW_INTERNAL_STEP_IMAGES)
    {
        Mat sceneImage_lines;
        sceneImage.copyTo(sceneImage_lines);

        for (unsigned int i = 0; i < lines_scene.size(); ++i)
        {
            Vec4i currentLine = lines_scene[i];
            line(sceneImage_lines, Point(currentLine[0], currentLine[1]), Point(currentLine[2], currentLine[3]), Scalar(0, 255, 0), 1);
        }

        imshow("sceneImage_lines", sceneImage_lines);
    }

    printf("\n");



    cvWaitKey();
    system("pause");
    return EXIT_SUCCESS;
}


void AppRoadDetection::step_clock_start()
{
    clock_step_begin = clock();
}


void AppRoadDetection::step_clock_stop()
{
    clock_step_end = clock();
    printf("-- time: %dms\n", clock_step_end - clock_step_begin);
}


void AppRoadDetection::readme()
{
    printf("Usage: AppRoadDetection <sceneImage>\n");
}


