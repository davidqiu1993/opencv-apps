/**
* Automatic text background application.
*
* Authors:
*    - David Qiu (david.qiu@dji.com)
*
* Copyright (C) 2016, David Qiu. All rights Reserved.
*/

#include "AppAutoTextBackground.h"
#include <conio.h>

#define APP_AUTO_TEXT_BACKGROUND_FILE_TYPE 1


/**
 * @brief Application entry.
 */
int AppAutoTextBackground::run(int argc, char* argv[])
{
    // Check parameters
    if (argc < 2)
    {
        readme();
    }

    
    // Configurations
    printf("Configurations\n");

    const string CONFIG_TEXT_CONTENT = "This is a text sample";
    printf("-- text content: %s\n", CONFIG_TEXT_CONTENT.c_str());

    const int CONFIG_TEXT_FONT = CV_FONT_HERSHEY_SIMPLEX;
    printf("-- text font: %d\n", CONFIG_TEXT_FONT);

    const double CONFIG_TEXT_SCALE = 0.8;
    printf("-- text scale: %lf\n", CONFIG_TEXT_SCALE);

    const Scalar CONFIG_TEXT_COLOR = Scalar(255, 255, 255);
    printf("-- text color: RGB(%.0lf, %.0lf, %.0lf)\n", CONFIG_TEXT_COLOR.val[0], CONFIG_TEXT_COLOR.val[1], CONFIG_TEXT_COLOR.val[2]);

    const int CONFIG_BACKGROUND_MARGIN = 16;
    printf("-- background margin: %d\n", CONFIG_BACKGROUND_MARGIN);

    const int CONFIG_BACKGROUND_FEATHER_MARGIN = -16;
    printf("-- background feather margin: %d\n", CONFIG_BACKGROUND_FEATHER_MARGIN);

    const uchar CONFIG_BACKGROUND_PROCESS_LIGHTNESS_THRESHOLD = 72;
    printf("-- background process lightness threshold: %d\n", CONFIG_BACKGROUND_PROCESS_LIGHTNESS_THRESHOLD);


#if (APP_AUTO_TEXT_BACKGROUND_FILE_TYPE == 0)
    
    // Step 1: Load scene image
    printf("Step 1: Load scene image\n");
    step_clock_start();

    Mat sceneImage = imread(argv[1]);

    step_clock_stop();

    printf("\n");

#elif (APP_AUTO_TEXT_BACKGROUND_FILE_TYPE == 1)

    VideoCapture capture(argv[1]);
    if (!capture.isOpened())
    {
        return -1;
    }

    Mat sceneFrame;
    while (capture.read(sceneFrame))
    {
        // Step 1: Load scene frame
        printf("Step 1: Load scene frame\n");
        step_clock_start();

        Mat sceneImage = sceneFrame;

        step_clock_stop();

        printf("\n");

#endif


    // Step 2: Put text onto the reference image
    printf("Step 2: Put text onto the reference image\n");
    step_clock_start();

    Mat sceneReferenceImage_text;
    sceneImage.copyTo(sceneReferenceImage_text);

    Rect textROI = putTextByCenter(sceneReferenceImage_text, CONFIG_TEXT_CONTENT, cvPoint(sceneImage.cols / 2, sceneImage.rows / 2), CONFIG_TEXT_FONT, CONFIG_TEXT_SCALE, CONFIG_TEXT_COLOR);

    rectangle(sceneReferenceImage_text, textROI, cvScalar(0, 255, 0), 1);
    printf("-- text ROI: Rect(%d, %d, %d, %d)\n", textROI.x, textROI.y, textROI.width, textROI.height);

    step_clock_stop();

    if (DO_SHOW_REFERENCE_IMAGE)
    {
        imshow("sceneReferenceImage_text", sceneReferenceImage_text);
    }

    printf("\n");


    // Step 3: Retrieve scene image ROI of the text background
    printf("Step 3: Retrieve scene image ROI of the text background\n");
    step_clock_start();

    Mat sceneImage_text;
    sceneImage.copyTo(sceneImage_text);

    Rect textBackgroundROI = cvRect(
        textROI.x - CONFIG_BACKGROUND_MARGIN,
        textROI.y - CONFIG_BACKGROUND_MARGIN,
        textROI.width + CONFIG_BACKGROUND_MARGIN * 2,
        textROI.height + CONFIG_BACKGROUND_MARGIN * 2);
    printf("-- text background ROI: Rect(%d, %d, %d, %d)\n", textBackgroundROI.x, textBackgroundROI.y, textBackgroundROI.width, textBackgroundROI.height);

    Mat sceneImageROI_text = sceneImage_text(textBackgroundROI);

    step_clock_stop();

    if (DO_SHOW_INTERNAL_STEP_IMAGES)
    {
        imshow("sceneImageROI_text", sceneImageROI_text);
    }

    printf("\n");


    // Step 4: Reverse color of the background image
    printf("Step 4: Reverse color of the background image\n");
    step_clock_start();

    Mat backgroundImage_reversed;
    sceneImageROI_text.copyTo(backgroundImage_reversed);

    reverseColor(backgroundImage_reversed);

    step_clock_stop();

    if (DO_SHOW_INTERNAL_STEP_IMAGES)
    {
        imshow("backgroundImage_reversed", backgroundImage_reversed);
    }

    printf("\n");


    // Step 5: Calculate the feather template
    printf("Step 5: Calculate the feather template\n");
    step_clock_start();

    assert(CONFIG_BACKGROUND_FEATHER_MARGIN < CONFIG_BACKGROUND_MARGIN);

    Mat featherTemplate = Mat::ones(sceneImageROI_text.rows, sceneImageROI_text.cols, CV_32F);
    
    int featherCenterSize = (CONFIG_BACKGROUND_MARGIN - CONFIG_BACKGROUND_FEATHER_MARGIN) / 2;
    int featherTemplateTopBoundary = featherCenterSize;
    int featherTemplateBottomBoundary = featherTemplate.rows - featherCenterSize;
    int featherTemplateLeftBoundary = featherCenterSize;
    int featherTemplateRightBoundary = featherTemplate.cols - featherCenterSize;
    for (unsigned int i = 0; i < featherTemplate.rows; ++i)
    {
        for (unsigned int j = 0; j < featherTemplate.cols; ++j)
        {
            if (i <= featherTemplateTopBoundary || i >= featherTemplateBottomBoundary ||
                j <= featherTemplateLeftBoundary || j >= featherTemplateRightBoundary) {
                featherTemplate.at<float>(i, j) = 0;
            }
        }
    }

    Mat featherTemplate_blur;
    blur(featherTemplate, featherTemplate_blur, cvSize(31, 31));

    step_clock_stop();
    
    if (DO_SHOW_INTERNAL_STEP_IMAGES)
    {
        imshow("featherTemplate", featherTemplate);
        imshow("featherTemplate_blur", featherTemplate_blur);
    }

    printf("\n");


    // Step 6: Calculate the highlighted area template
    printf("Step 6: Calculate the highlighted area template\n");
    step_clock_start();

    Mat highlightTemplate = Mat::zeros(sceneImageROI_text.rows, sceneImageROI_text.cols, CV_32F);

    Mat backgroundImage_gray;
    cvtColor(sceneImageROI_text, backgroundImage_gray, CV_RGB2GRAY);

    for (unsigned int i = 0; i < highlightTemplate.rows; ++i)
    {
        for (unsigned int j = 0; j < highlightTemplate.cols; ++j)
        {
            uchar pixelLightness = backgroundImage_gray.at<uchar>(i, j);
            if (pixelLightness > CONFIG_BACKGROUND_PROCESS_LIGHTNESS_THRESHOLD)
            {
                //highlightTemplate.at<float>(i, j) = 1.0;
                highlightTemplate.at<float>(i, j) = (float)pixelLightness / 255.0;
            }
        }
    }

    Mat highlightTemplate_mediumBlur;

    int highlightTemplateMediumBlurSize = 5;
    printf("-- medium blur size: %d\n", highlightTemplateMediumBlurSize);

    medianBlur(highlightTemplate, highlightTemplate_mediumBlur, highlightTemplateMediumBlurSize);

    step_clock_stop();

    if (DO_SHOW_INTERNAL_STEP_IMAGES)
    {
        imshow("highlightTemplate", highlightTemplate);
        imshow("highlightTemplate_mediumBlur", highlightTemplate_mediumBlur);
    }

    printf("\n");


    // Step 7: Merge the templates
    printf("Step 7: Merge the templates\n");
    step_clock_start();

    Mat mergedTemplate = Mat(sceneImageROI_text.rows, sceneImageROI_text.cols, CV_32F);

    for (unsigned int i = 0; i < mergedTemplate.rows; ++i)
    {
        for (unsigned int j = 0; j < mergedTemplate.cols; ++j)
        {
            mergedTemplate.at<float>(i, j) = highlightTemplate_mediumBlur.at<float>(i, j) * featherTemplate_blur.at<float>(i, j);
        }
    }

    step_clock_stop();

    if (DO_SHOW_INTERNAL_STEP_IMAGES)
    {
        imshow("mergedTemplate", mergedTemplate);
    }

    printf("\n");
    

    // Step 8: Process the background with the merged template
    printf("Step 8: Process the background with the merged template\n");
    step_clock_start();

    Mat alphaTemplate = mergedTemplate;

    Mat betaTemplate = Mat(mergedTemplate.rows, mergedTemplate.cols, CV_32F);
    for (unsigned int i = 0; i < betaTemplate.rows; ++i)
    {
        for (unsigned int j = 0; j < betaTemplate.cols; ++j)
        {
            betaTemplate.at<float>(i, j) = 1.0 - mergedTemplate.at<float>(i, j);
        }
    }
    
    Mat gammaTemplate = Mat::zeros(mergedTemplate.rows, mergedTemplate.cols, CV_32F);

    Mat backgroundImage_processed;
    addWeightedWithTemplates(backgroundImage_reversed, alphaTemplate, sceneImageROI_text, betaTemplate, gammaTemplate, backgroundImage_processed);

    backgroundImage_processed.copyTo(sceneImageROI_text);

    step_clock_stop();

    if (DO_SHOW_INTERNAL_STEP_IMAGES)
    {
        imshow("backgroundImage_processed", backgroundImage_processed);
    }

    printf("\n");
    

    // Step 9: Add text to the scene image
    printf("Step 9: Add text to the scene image\n");
    step_clock_start();

    putTextByCenter(sceneImage_text, CONFIG_TEXT_CONTENT, cvPoint(sceneImage.cols / 2, sceneImage.rows / 2), CONFIG_TEXT_FONT, CONFIG_TEXT_SCALE, CONFIG_TEXT_COLOR);

    step_clock_stop();

    printf("\n");

    
    imshow("sceneImage_text", sceneImage_text);
    

#if (APP_AUTO_TEXT_BACKGROUND_FILE_TYPE == 1)

    cvWaitKey(1);

    }

#endif


    cvWaitKey();
    return EXIT_SUCCESS;
}


void AppAutoTextBackground::step_clock_start()
{
    clock_step_begin = clock();
}


void AppAutoTextBackground::step_clock_stop()
{
    clock_step_end = clock();
    printf("-- time: %dms\n", clock_step_end - clock_step_begin);
}


void AppAutoTextBackground::readme()
{
    printf("Usage: AppAutoTextBackground <sceneImage>\n");
}


Rect AppAutoTextBackground::putTextByCenter(
    cv::Mat &img, const string &text, Point center,
    int fontFace, double fontScale, Scalar color, int thickness)
{
    // Get the text size
    int baseLine = 0;
    Size textSize = getTextSize(text, fontFace, fontScale, thickness, &baseLine);
    baseLine += thickness;

    // Put the text onto the image
    Point origin = cvPoint(center.x - textSize.width / 2, center.y - textSize.height / 2);
    putText(img, text, origin, fontFace, fontScale, color, thickness);

    // Calculate the ROI from the image
    Rect rectROI = cvRect(origin.x, origin.y - textSize.height, textSize.width, textSize.height);

    // Return the text ROI rect
    return rectROI;
}


void AppAutoTextBackground::reverseColor(cv::Mat &img)
{
    for (unsigned int i = 0; i<img.rows; ++i)
    {
        for (unsigned int j = 0; j<img.cols; ++j)
        {
            img.at<Vec3b>(i, j)[0] = 255 - img.at<Vec3b>(i, j)[0];
            img.at<Vec3b>(i, j)[1] = 255 - img.at<Vec3b>(i, j)[1];
            img.at<Vec3b>(i, j)[2] = 255 - img.at<Vec3b>(i, j)[2];
        }
    }
}


void AppAutoTextBackground::addWeightedWithTemplates(
    const cv::Mat& img1, const cv::Mat& alphaTemplate,
    const cv::Mat& img2, const cv::Mat& betaTemplate,
    const cv::Mat& gammaTemplate,
    cv::Mat& dst)
{
    assert(img1.rows == img2.rows && img1.rows == alphaTemplate.rows && img1.rows == betaTemplate.rows && img1.rows == gammaTemplate.rows);
    assert(img1.cols == img2.cols && img1.cols == alphaTemplate.cols && img1.cols == betaTemplate.cols && img1.cols == gammaTemplate.cols);
    assert(img1.channels() == 3 && img2.channels() == 3 && alphaTemplate.channels() == 1 && betaTemplate.channels() == 1 && gammaTemplate.channels() == 1);

    Mat _dst = Mat(img1.rows, img1.cols, CV_8UC3);
    for (unsigned int i = 0; i < _dst.rows; ++i)
    {
        for (unsigned int j = 0; j < _dst.cols; ++j)
        {
            for (unsigned k = 0; k < _dst.channels(); ++k)
            {
                _dst.at<Vec3b>(i, j)[k] =
                    (float)(img1.at<Vec3b>(i, j)[k]) * alphaTemplate.at<float>(i, j) +
                    (float)(img2.at<Vec3b>(i, j)[k]) * betaTemplate.at<float>(i, j) +
                    gammaTemplate.at<float>(i, j);
            }
        }
    }

    dst = _dst;
}


