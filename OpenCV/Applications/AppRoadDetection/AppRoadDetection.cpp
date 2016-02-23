/**
* Road detection application.
*
* Authors:
*    - David Qiu (david.qiu@dji.com)
*
* Copyright (C) 2016, David Qiu. All rights Reserved.
*/

#include "AppRoadDetection.h"
#include <conio.h>


/**
 * @brief Application entry.
 */
int AppRoadDetection::run(int argc, char* argv[])
{
    cout << "This is the road detection application." << endl;
    cout << "argc: " << argc << endl;
    cout << "argv: " << endl;
    for (int i = 0; i < argc; ++i)
    {
        cout << "  [" << i << "] " << argv[i] << endl;
    }

    while (!_kbhit());

    return EXIT_SUCCESS;
}


