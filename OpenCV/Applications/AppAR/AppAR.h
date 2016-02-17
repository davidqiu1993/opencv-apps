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
        DO_SHOW_INTERNAL_STEP_IMAGES(true),
        INPUT_FILE_TYPE(1),
        MAX_PROCESSING_IMAGE_WIDTH(480) { ; }

    int run(int argc, char* argv[]);

public:
    const bool DO_SHOW_INTERNAL_STEP_IMAGES;
    /*
    Input File Type:
    - 0 = Image
    - 1 = Video
    */
    const unsigned int INPUT_FILE_TYPE;
    const unsigned int MAX_PROCESSING_IMAGE_WIDTH;

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
