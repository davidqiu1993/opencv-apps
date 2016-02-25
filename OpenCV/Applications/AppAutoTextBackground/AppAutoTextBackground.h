/**
* Automatic text background application.
*
* Authors:
*    - David Qiu (david.qiu@dji.com)
*
* Copyright (C) 2016, David Qiu. All rights Reserved.
*/

#ifndef APP_AUTO_TEXT_BACKGROUND_H
#define APP_AUTO_TEXT_BACKGROUND_H

#include "../App.h"
#include <ctime>


class AppAutoTextBackground: public App
{
public:
    AppAutoTextBackground() :
        DO_SHOW_INTERNAL_STEP_IMAGES(true),
        DO_SHOW_REFERENCE_IMAGE(true) { ; }

    int run(int argc, char* argv[]);

public:
    const bool DO_SHOW_INTERNAL_STEP_IMAGES;
    const bool DO_SHOW_REFERENCE_IMAGE;

private:
    void step_clock_start();
    void step_clock_stop();
    void readme();
    Rect putTextByCenter(
        cv::Mat &img, const string &text, Point center,
        int fontFace, double fontScale, Scalar color, int thickness = 1);
    void reverseColor(cv::Mat &img);
    void addWeightedWithTemplates(
        const cv::Mat& src1, const cv::Mat& alphaTemplate,
        const cv::Mat& src2, const cv::Mat& betaTemplate,
        const cv::Mat& gammaTemplate,
        cv::Mat& dst);

private:
    clock_t clock_step_begin;
    clock_t clock_step_end;
};


#endif // APP_AUTO_TEXT_BACKGROUND_H
