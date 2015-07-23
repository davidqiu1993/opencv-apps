/**
* Application framework.
*
* Authors:
*    - David Qiu (david.qiu@dji.com)
*
* Copyright (C) 2015, David Qiu. All rights Reserved.
*/

#ifndef APP_H
#define APP_H

#include "../stdafx.h"


class App
{
public:
    virtual int run(int argc, char* argv[]) = 0;
};


#endif // APP_H
