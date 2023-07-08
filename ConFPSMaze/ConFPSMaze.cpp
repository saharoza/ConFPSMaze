// ConFPSMaze.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <string>
#include <chrono>
using namespace std;

int nScreenWidth = 120;
int nScreenHeight = 40;

float fPlayerX = 8.0f;
float fPlayerY = 8.0f;
float fPlayerA = 0.0f;

int nMapHeight = 16;
int nMapWidth = 16;

//Float Field of View
float fFOV = 3.14159 / 0.4;
float fDepth = 16.0;

int main()
{
        //Creating Screen Buffer
    wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    //Creating Map
    wstring map;
    map += L"################";
    map += L"#              #";
    map += L"#   #          #";
    map += L"#   ###        #";
    map += L"#           #  #";
    map += L"#          #   #";
    map += L"#    ##  # #   #";
    map += L"#    #         #";
    map += L"#     #####    #";
    map += L"#         #    #";
    map += L"#  ###    #    #";
    map += L"#   #     ###  #";
    map += L"#    #         #";
    map += L"#    #####     #";
    map += L"#        #     #";
    map += L"################";

    //Creating System times
    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();

    //Game Loop
    while (1)
    {

        //Compare previous system time with the current one
        tp2 = chrono::system_clock::now();
        chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float fElapsedTime = elapsedTime.count();



        //Controls
        //Handle CCW Rotation
        if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
            fPlayerA -= ((1.5f) * fElapsedTime);

        if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
            fPlayerA += (1.5f) * fElapsedTime;

        if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
        {
            fPlayerX += sinf(fPlayerA) * 5.0f * fElapsedTime;
            fPlayerY += cosf(fPlayerA) * 5.0f * fElapsedTime;
            //Checking if we hit the wall and undoing the step
            if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
            {
                fPlayerX -= sinf(fPlayerA) * 5.0f * fElapsedTime;
                fPlayerY -= cosf(fPlayerA) * 5.0f * fElapsedTime;
            }
        }

        if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
        {
            fPlayerX -= sinf(fPlayerA) * 5.0f * fElapsedTime;
            fPlayerY -= cosf(fPlayerA) * 5.0f * fElapsedTime;
            //Checking if we hit the wall and undoing the step
            if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
            {
                fPlayerX += sinf(fPlayerA) * 5.0f * fElapsedTime;
                fPlayerY += cosf(fPlayerA) * 5.0f * fElapsedTime;
            }
        }

        for (int x = 0; x < nScreenWidth; x++)
        {
            //For each column calculate the projected ray angle into world space
            float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;

            float fDistanceToWall = 0;
            bool bHitWall = false;
            bool bBoundary = false;

            //Unit vector for ray in player space
            float fEyeX = sinf(fRayAngle);
            float fEyeY = cosf(fRayAngle);

            while (!bHitWall && fDistanceToWall < fDepth)
            {
                fDistanceToWall += 0.1f;

                int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
                int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

                // Test if ray is out of bounds
                if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
                {
                    bHitWall = true; 
                    fDistanceToWall = fDepth; //Set distance to maximum depth
                }
                else
                {
                    //Ray is inbounds so test to see if the ray cell is a wall block
                    if (map[nTestY * nMapWidth + nTestX] == '#')
                    {
                        bHitWall = true;


                    }
                }

            }


            //Calculate distance to ceiling and floor
            int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistanceToWall);
            int nFloor = nScreenHeight - nCeiling;

            short nShade = ' ';
            short nFloorShade = ' ';

            if (fDistanceToWall <= fDepth / 4.0f) nShade = 0x2588; //Very close to the wall
            else if (fDistanceToWall < fDepth / 3.0f) nShade = 0x2593;
            else if (fDistanceToWall < fDepth / 2.0f) nShade = 0x2592;
            else if (fDistanceToWall < fDepth) nShade = 0x2591;
            else nShade = ' '; //Very far from the wall

            for (int y = 0; y < nScreenHeight; y++)
            {
                if (y < nCeiling)
                    screen[y * nScreenWidth + x] = ' ';
                else if (y > nCeiling && y <= nFloor)
                    screen[y * nScreenWidth + x] = nShade;
                else 
                {
                    //Shade floor based on distance
                    float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
                    if (b < 0.25) nFloorShade = '#';
                    else if (b < 0.5) nFloorShade = 'x';
                    else if (b < 0.75) nFloorShade = '-';
                    else if (b < 0.9) nFloorShade = '.';
                    else nFloorShade = ' ';
                    screen[y * nScreenWidth + x] = nFloorShade;
                }
            }

        }

        //Writing on the screen
        screen[nScreenWidth * nScreenHeight - 1] = '\0';
        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);


    }





    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
