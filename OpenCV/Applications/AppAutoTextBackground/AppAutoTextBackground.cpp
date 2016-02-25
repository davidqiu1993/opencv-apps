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


/**
 * @brief Application entry.
 */
int AppAutoTextBackground::run(int argc, char* argv[])
{
    cout << "This is the automatic text background application." << endl;
    cout << "argc: " << argc << endl;
    cout << "argv: " << endl;
    for (int i = 0; i < argc; ++i)
    {
        cout << "  [" << i << "] " << argv[i] << endl;
    }

    while (!_kbhit());

    return EXIT_SUCCESS;
}


