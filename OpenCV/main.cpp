/**
 * Program entry.
 *
 * Authors:
 *    - David Qiu (david.qiu@dji.com)
 *
 * Copyright (C) 2015, David Qiu. All rights Reserved.
 */

#include "stdafx.h"

#include <map>
#include <vector>
#include <sstream>
using namespace std;

#include "Applications/App.h"
#include "Applications/AppExample/AppExample.h"


/**
 * @brief Applications.
 */
typedef enum _applicationTag {
    APP_EXAMPLE
} applicationTag;


/**
 * @brief Program Entry.
 */
int main(int argc, char* argv[])
{
    // Define application argument structure general variables
    int appArgc = 0;
    char** appArgv = NULL;

    // Check program arguments
    if (argc >= 2)
    {
        // Construct the application argument structures
        appArgc = argc - 1;
        appArgv = new char*[appArgc];
        for (int i = 0; i < appArgc; ++i)
        {
            appArgv[i] = argv[i + 1];
        }
    }
    else
    {
        // Require application launch command
        cout << "> ";

        // Read the application launch command
        string cmd;
        getline(cin, cmd, '\n');

        // Read the command tokens
        stringstream cmdStream;
        string cmdToken;
        vector<string> cmdTokens;
        appArgc = 0;
        cmdStream << cmd;
        while (cmdStream >> cmdToken)
        {
            appArgc++;
            cmdTokens.push_back(cmdToken);
        }

        // Check the application argument count
        if (appArgc <= 0)
        {
            cout << "[ ERROR ] Application name is required." << endl;
            return EXIT_FAILURE;
        }

        // Construct the argument array
        appArgv = new char*[appArgc];
        for (int i = 0; i < appArgc; ++i)
        {
            appArgv[i] = new char[cmdTokens[i].length() + 1];
            strcpy_s(appArgv[i], cmdTokens[i].length() + 1, cmdTokens[i].c_str());
        }
    }

    // Map application names
    map<string, applicationTag> applications;
    applications["AppExample"] = APP_EXAMPLE;

    // Check if the selected application exists
    if (applications.find(appArgv[0]) == applications.end())
    {
        cout << "[ ERROR ] Application does not exist." << endl;
        return EXIT_FAILURE;
    }

    // Select application
    App* currentApp = NULL;
    switch (applications[appArgv[0]])
    {
    case APP_EXAMPLE: currentApp = new AppExample(); break;
    default:
        cout << "[ ERROR ] Unexpected application tag." << endl;
        return EXIT_FAILURE;
    }

    // Run the application
    return currentApp->run(appArgc, appArgv);
}


