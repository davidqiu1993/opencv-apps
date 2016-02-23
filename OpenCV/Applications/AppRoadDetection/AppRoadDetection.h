/**
* Road detection application.
*
* Authors:
*    - David Qiu (david.qiu@dji.com)
*
* Copyright (C) 2016, David Qiu. All rights Reserved.
*/

#ifndef APP_ROAD_DETECTION_H
#define APP_ROAD_DETECTION_H

#include "../App.h"
#include <ctime>


class AppRoadDetection: public App
{
public:
    AppRoadDetection() :
        DO_SHOW_INTERNAL_STEP_IMAGES(true) { ; }

    int run(int argc, char* argv[]);

public:
    const bool DO_SHOW_INTERNAL_STEP_IMAGES;

private:
    void step_clock_start();
    void step_clock_stop();
    void readme();

private:
    clock_t clock_step_begin;
    clock_t clock_step_end;
};


#endif // APP_ROAD_DETECTION_H
