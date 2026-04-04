#include "TXLib.h"
#include <math.h>

int main(void) {

    const int sizeX = 800, sizeY = 600;

    DWORD lastTime = GetTickCount();
    float fps = 0;

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

            for (int x = 0; x < sizeX; x += 4) {

                float x0 = (((float)x - 400.f) * dx + xC) *scale;

                float x0Arr [4] = { x0, x0 + dx, x0 + 2*dx, x0 + 3*dx};
                float y0Arr [4] = { y0, y0,      y0,        y0       };

                float xArr [4] = {};  for (int i = 0; i < 4; i++) xArr[i] = x0Arr[i];
                float yArr [4] = {};  for (int i = 0; i < 4; i++) yArr[i] = y0Arr[i];

                int N[4] = {};
                for (int n = 0; n < nMax; n++) {
                    float x2 [4] = {};  for (int i = 0; i < 4; i++) x2[i] = xArr[i] * xArr[i];
                    float y2 [4] = {};  for (int i = 0; i < 4; i++) y2[i] = yArr[i] * yArr[i];
                    float xy [4] = {};  for (int i = 0; i < 4; i++) xy[i] = xArr[i] * yArr[i];

                    float r2 [4] = {};  for (int i = 0; i < 4; i++) r2[i] = x2[i] + y2[i];

                    int cmp[4] = {};
                    for (int i = 0; i < 4; i++) if (r2[i] <= r2Max) cmp[i] = 1;

                    int mask = 0;
                    for (int i = 0; i < 4; i++) mask |= cmp[i] << i;
                    if(!mask) break;

                    for (int i = 0; i < 4; i++) xArr[i] = x2[i] - y2[i] + x0Arr[i];
                    for (int i = 0; i < 4; i++) yArr[i] = xy[i] + xy[i] + y0Arr[i];

                    for (int i = 0; i < 4; i++) N[i] += cmp[i];
                }

                for (int i = 0; i < 4; i ++) {
                    RGBQUAD color = {(BYTE)(N[i] << 4), (BYTE)(N[i] << 1), (BYTE)(N[i] << 2), 0};

                    int videoMemoRyOffset = (sizeY - 1 - y) * sizeX + (x + i);
                    graphBuf[videoMemoRyOffset] = color;
                }

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
