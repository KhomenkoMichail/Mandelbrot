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

                    BENCHMARK(__asm__ volatile("" : : "x"(n) : "memory");)
                }

                #ifndef BENCHMARK_MODE
                RGBQUAD color = {(BYTE)(n << 4), (BYTE)(n << 1), (BYTE)(n << 2), 0};

                int videoMemoryOffset = (sizeY - 1 - y) * sizeX + x;
                graphBuf[videoMemoryOffset] = color;
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
