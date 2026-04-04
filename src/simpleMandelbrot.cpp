#include "TXLib.h"
#include <math.h>

int main(void) {

    DWORD lastTime = GetTickCount();
    float fps = 0;


    const int sizeX = 800, sizeY = 600;

    const float dx = 1.0f / sizeX;
    const float dy = 1.0f / sizeY;

    float xC = 0, yC = 0, scale = 3.0;


    const float r2Max = 100;
    const int   nMax = 256;

    txCreateWindow (sizeX, sizeY);
    Win32::_fpreset();

    RGBQUAD* graphBuf = txVideoMemory();

    for ( ; ; ) {
        if (GetAsyncKeyState (VK_ESCAPE)) break;

        txBegin();

        if (txGetAsyncKeyState (VK_RIGHT))    xC    += dx * (txGetAsyncKeyState(VK_SHIFT) ? 100.0f : 1.0f);
        if (txGetAsyncKeyState (VK_LEFT))     xC    -= dx * (txGetAsyncKeyState(VK_SHIFT) ? 100.0f : 1.0f);
        if (txGetAsyncKeyState (VK_DOWN))     yC    += dy * (txGetAsyncKeyState(VK_SHIFT) ? 100.0f : 1.0f);
        if (txGetAsyncKeyState (VK_UP))       yC    -= dy * (txGetAsyncKeyState(VK_SHIFT) ? 100.0f : 1.0f);

        if (txGetAsyncKeyState ('W'))         scale -= dx * (txGetAsyncKeyState(VK_SHIFT) ? 100.0f : 1.0f);
        if (txGetAsyncKeyState ('S'))         scale += dx * (txGetAsyncKeyState(VK_SHIFT) ? 100.0f : 1.0f);


        for (int y = 0; y < sizeY; y++) {
            if (GetAsyncKeyState (VK_ESCAPE)) break;

            float y0 = (((float)y - 300.f) * dy + yC) *scale;

            for (int x = 0; x < sizeX; x++) {

               float x0 = (((float)x - 400.f) * dx + xC) *scale;

                float curX = x0;
                float curY = y0;

                int n = 0;
                for ( ; n < nMax; n++) {
                    float x2   = curX * curX;
                    float y2   = curY * curY;
                    float xy   = curX * curY;

                    float r2 = x2 + y2;
                    if (r2 >= r2Max) break;


                    curX = x2 - y2 + x0;
                    curY = xy + xy + y0;
                }

                RGBQUAD color = {(BYTE)(n << 4), (BYTE)(n << 1), (BYTE)(n << 2), 0};

                int videoMemoRyOffset = (sizeY - 1 - y) * sizeX + x;
                graphBuf[videoMemoRyOffset] = color;
            }
        }

        DWORD currentTime = GetTickCount();
        float dt = ((float)currentTime - (float)lastTime) / 1000.0f;
        lastTime = currentTime;

        char fpsBuf[20];
        sprintf(fpsBuf, "FPS: %.1f", fps);

        txSetColor(TX_WHITE);
        txSetFillColor(TX_BLACK);
        txSelectFont("Consolas", 24);
        txTextOut(10, 10, fpsBuf);
        txEnd();
    }

    return 0;
}
