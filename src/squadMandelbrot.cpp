#define BENCHMARK_MODE

#ifndef BENCHMARK_MODE
    #define SIMPLE_MODE(code) code
    #define BENCHMARK(code)
#else
    #define SIMPLE_MODE(code)
    #define BENCHMARK(code) code
#endif

#include "TXLib.h"

#include <math.h>

int main(void) {

    BENCHMARK(const int NUM_OF_TESTS = 500;)

    const int sizeX = 800, sizeY = 600;

    const float dx = 1.0f / sizeX;
    const float dy = 1.0f / sizeY;

    float xC = 0, yC = 0, scale = 3.0;

    const float r2Max = 100;
    const int   nMax = 256;

    #ifndef BENCHMARK_MODE
        DWORD lastTime = GetTickCount();
        float fps = 0;

        txCreateWindow (sizeX, sizeY);

        RGBQUAD* graphBuf = txVideoMemory();
    #endif

    Win32::_fpreset();

    for (SIMPLE_MODE( ; ; ) BENCHMARK(int curTest = 0; curTest < NUM_OF_TESTS; curTest++)) {

        #ifndef BENCHMARK_MODE
            if (GetAsyncKeyState (VK_ESCAPE)) break;

            txBegin();

            if (txGetAsyncKeyState (VK_RIGHT))    xC    += dx * (txGetAsyncKeyState(VK_SHIFT) ? 100.0f : 1.0f);
            if (txGetAsyncKeyState (VK_LEFT))     xC    -= dx * (txGetAsyncKeyState(VK_SHIFT) ? 100.0f : 1.0f);
            if (txGetAsyncKeyState (VK_DOWN))     yC    += dy * (txGetAsyncKeyState(VK_SHIFT) ? 100.0f : 1.0f);
            if (txGetAsyncKeyState (VK_UP))       yC    -= dy * (txGetAsyncKeyState(VK_SHIFT) ? 100.0f : 1.0f);

            if (txGetAsyncKeyState ('W'))         scale -= dx * (txGetAsyncKeyState(VK_SHIFT) ? 100.0f : 1.0f);
            if (txGetAsyncKeyState ('S'))         scale += dx * (txGetAsyncKeyState(VK_SHIFT) ? 100.0f : 1.0f);
        #endif


        for (int y = 0; y < sizeY; y++) {

            float y0 = (((float)y - 300.f) * dy + yC) *scale;

            for (int x = 0; x < sizeX; x += 8) {

                float x0 = (((float)x - 400.f) * dx + xC) *scale;
                float xStep = dx*scale;

                float x0Arr[8] = { x0, x0 + xStep, x0 + 2*xStep, x0 + 3*xStep, x0 + 4*xStep, x0 + 5*xStep, x0 + 6*xStep, x0 + 7*xStep};
                float y0Arr[8] = { y0, y0,      y0,        y0,        y0,        y0,        y0,        y0       };

                float xArr[8] = {};  for (int i = 0; i < 8; i++) xArr[i] = x0Arr[i];
                float yArr[8] = {};  for (int i = 0; i < 8; i++) yArr[i] = y0Arr[i];

                int N[8] = {};
                for (int n = 0; n < nMax; n++) {
                    float x2[8] = {};  for (int i = 0; i < 8; i++) x2[i] = xArr[i] * xArr[i];
                    float y2[8] = {};  for (int i = 0; i < 8; i++) y2[i] = yArr[i] * yArr[i];
                    float xy[8] = {};  for (int i = 0; i < 8; i++) xy[i] = xArr[i] * yArr[i];

                    float r2[8] = {};  for (int i = 0; i < 8; i++) r2[i] = x2[i] + y2[i];

                    int cmp[8] = {};
                    for (int i = 0; i < 8; i++) if (r2[i] <= r2Max) cmp[i] = 1;

                    int mask = 0;
                    for (int i = 0; i < 8; i++) mask |= cmp[i] << i;
                    if(!mask) break;

                    for (int i = 0; i < 8; i++) xArr[i] = x2[i] - y2[i] + x0Arr[i];
                    for (int i = 0; i < 8; i++) yArr[i] = xy[i] + xy[i] + y0Arr[i];

                    for (int i = 0; i < 8; i++) N[i] += cmp[i];

                    BENCHMARK(__asm__ volatile("" : : "x"(N) : "memory");)
                }

                #ifndef BENCHMARK_MODE
                    BYTE b[8] = {}; for(int i = 0; i < 8; i++) b[i] = (BYTE)(N[i] << 4);
                    BYTE g[8] = {}; for(int i = 0; i < 8; i++) g[i] = (BYTE)(N[i] << 1);
                    BYTE r[8] = {}; for(int i = 0; i < 8; i++) r[i] = (BYTE)(N[i] << 2);

                    RGBQUAD color[8] = {}; for(int i = 0; i < 8; i++) color[i] = {b[i], g[i], r[i], 0};

                    int videoMemoryOffset = (sizeY - 1 - y) * sizeX + x;
                    for (int i = 0; i < 8; i++) graphBuf[videoMemoryOffset + i] = color[i];
                #endif
            }
        }

        #ifndef BENCHMARK_MODE
            DWORD currentTime = GetTickCount();
            float dt = ((float)currentTime - (float)lastTime) / 1000.0f;
            if (dt > 0) fps = 0.9f * fps + 0.1f * (1.0f / dt);
            lastTime = currentTime;

            char fpsBuf[20];
            sprintf(fpsBuf, "FPS: %.1f", fps);

            txSetColor(TX_WHITE);
            txSetFillColor(TX_BLACK);
            txSelectFont("Consolas", 24);
            txTextOut(10, 10, fpsBuf);
            txEnd();
        #endif
    }

    return 0;
}
