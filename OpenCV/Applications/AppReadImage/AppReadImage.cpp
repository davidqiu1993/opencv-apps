/**
* Read image application.
*
* Authors:
*    - David Qiu (david.qiu@dji.com)
*
* Copyright (C) 2015, David Qiu. All rights Reserved.
*/

#include "AppReadImage.h"


/**
 * @brief Application entry.
 */
int AppReadImage::run(int argc, char* argv[])
{
    // Check arguments
    if (!(argc >= 2))
    {
        cout << "[ ERROR ] Image file path is required." << endl;
        return EXIT_FAILURE;
    }

    // Load the image
    IplImage* image = cvLoadImage(argv[1]);

    // Show the image
    cvShowImage("AppReadImage", image);

    // Keep running
    waitKey();

    return EXIT_SUCCESS;
}


