/**
* Write video application.
*
* Authors:
*    - David Qiu (david.qiu@dji.com)
*
* Copyright (C) 2015, David Qiu. All rights Reserved.
*/

#include "AppWriteVideo.h"
#include <conio.h>
#include <ctime>


/**
 * @param argc The number of arguments.
 * @param argv The argument vector, where
 *               - argv[0]: The application name in calling;
 *               - argv[1]: The video source path;
 *               - argv[2]: The selected channel to output, within { 'R', 'G', 'B' };
 *               - argv[3]: The indicator of selecting video encoder manually, within { 'Y', 'N' }.
 * @brief Application entry.
 */
int AppWriteVideo::run(int argc, char* argv[])
{
    if (argc != 4)
    {
        cout << "Not enough parameters" << endl;
        return -1;
    }

    const string source = argv[1];            // the source file name
    const bool askOutputType = argv[3][0] == 'Y';  // If false it will use the inputs codec type

    VideoCapture inputVideo(source);        // Open input
    if (!inputVideo.isOpened())
    {
        cout << "Could not open the input video." << source << endl;
        return -1;
    }

    string::size_type pAt = source.find_last_of('.');   // Find extension point
    const string NAME = source.substr(0, pAt) + argv[2][0] + ".avi";   // Form the new name with container
    int ex = static_cast<int>(inputVideo.get(CV_CAP_PROP_FOURCC));     // Get Codec Type- Int form

    // Transform from int to char via Bitwise operators
    char EXT[] = { ex & 0XFF, (ex & 0XFF00) >> 8, (ex & 0XFF0000) >> 16, (ex & 0XFF000000) >> 24, 0 };

    Size S = Size((int)inputVideo.get(CV_CAP_PROP_FRAME_WIDTH),    //Acquire input size
        (int)inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));

    VideoWriter outputVideo;                                        // Open the output
    if (askOutputType)
        outputVideo.open(NAME, ex = -1, inputVideo.get(CV_CAP_PROP_FPS), S, true);
    else
        outputVideo.open(NAME, ex, inputVideo.get(CV_CAP_PROP_FPS), S, true);

    if (!outputVideo.isOpened())
    {
        cout << "Could not open the output video for write: " << source << endl;
        return -1;
    }

    union { int v; char c[5]; } uEx;
    uEx.v = ex;                              // From Int to char via union
    uEx.c[4] = '\0';

    cout << "Input frame resolution: Width=" << S.width << "  Height=" << S.height
        << " of nr#: " << inputVideo.get(CV_CAP_PROP_FRAME_COUNT) << endl;
    cout << "Input codec type: " << EXT << endl;

    int channel = 2;    // Select the channel to save
    switch (argv[2][0])
    {
    case 'R': {channel = 2; break; }
    case 'G': {channel = 1; break; }
    case 'B': {channel = 0; break; }
    }
    Mat src, res;
    vector<Mat> spl;

    while (true) //Show the image captured in the window and repeat
    {
        inputVideo >> src;              // read
        if (src.empty()) break;         // check if at end

        split(src, spl);                 // process - extract only the correct channel
        for (int i = 0; i < 3; ++i)
        if (i != channel)
            spl[i] = Mat::zeros(S, spl[0].type());
        merge(spl, res);

        imshow("output", res);
        cout << "[" << time(NULL) << "] " << "Output Video Frame." << endl;

        //outputVideo.write(res); //save or
        outputVideo << res;
    }

    cout << "Finished writing" << endl;
    return EXIT_SUCCESS;
}


