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

#include "../include/funcMandelbrot.h"

void _mm256_set1_ps (float arr[8], float a) { for (int i = 0; i < 8; i++) arr[i] = a; }

void _mm256_set_ps (float arr[8], float e7, float e6, float e5, float e4,
                                  float e3, float e2, float e1, float e0) {

    arr[0] = e0; arr[1] = e1; arr[2] = e2; arr[3] = e3;
    arr[4] = e4; arr[5] = e5; arr[6] = e6; arr[7] = e7;
}

void _mm256_add_ps (float dst[8], float a[8], float b[8]) { for (int i = 0; i < 8; i++) dst[i] = a[i] + b [i]; }
void _mm256_sub_ps (float dst[8], float a[8], float b[8]) { for (int i = 0; i < 8; i++) dst[i] = a[i] - b [i]; }
void _mm256_mul_ps (float dst[8], float a[8], float b[8]) { for (int i = 0; i < 8; i++) dst[i] = a[i] * b [i]; }
void _mm256_equ_ps (float dst[8], float src[8]) { for(int i = 0; i < 8; i++) dst[i] = src[i]; }

void _mm256_cmp_le_ps (int cmp[8], float a[8], float b[8]) { for (int i = 0; i < 8; i++) if (a[i] <= b[i]) cmp[i] = 1; }

void _mm256_add_epi32 (int dst[8], int a[8], int b[8]) { for (int i = 0; i < 8; i++) dst[i] = a[i] + b[i]; }

void _mm256_slli_epi32 (BYTE dst[8], int src[8], int shift) { for(int i = 0; i < 8; i++) dst[i] = (BYTE)(src[i] << shift); }


int _mm256_movemask_epi8 (int cmp[8]) {
    int mask = 0;
    for (int i = 0; i < 8; i++) mask |= cmp[i] << i;
    return mask;
}


int main(void) {

    BENCHMARK(const int   NUM_OF_TESTS = 500;)

    const int sizeX = 800, sizeY = 600;

    const float dx = 1.0f / sizeX;
    const float dy = 1.0f / sizeY;

    float xC = 0, yC = 0, scale = 3.0;

    float r2Max[8] = {}; _mm256_set1_ps (r2Max, 100.0f);
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

                float arr01234567[8] = {}; _mm256_set_ps(arr01234567, 7, 6, 5, 4, 3, 2, 1, 0);
                float xStepArr[8]    = {}; _mm256_set1_ps(xStepArr, xStep);
                _mm256_mul_ps(arr01234567, arr01234567, xStepArr);

                float x0Arr[8] = {}; _mm256_set1_ps(x0Arr, x0);
                _mm256_add_ps(x0Arr, x0Arr, arr01234567);


                float y0Arr[8] = {}; _mm256_set1_ps(y0Arr, y0);

                float xArr[8] = {};  _mm256_equ_ps(xArr, x0Arr);
                float yArr[8] = {};  _mm256_equ_ps(yArr, y0Arr);

                int N[8] = {};
                for (int n = 0; n < nMax; n++) {
                    float x2[8] = {};  _mm256_mul_ps(x2, xArr, xArr);
                    float y2[8] = {};  _mm256_mul_ps(y2, yArr, yArr);
                    float xy[8] = {};  _mm256_mul_ps(xy, xArr, yArr);

                    float r2[8] = {};  _mm256_add_ps(r2, x2, y2);

                    int cmp[8]  = {};  _mm256_cmp_le_ps (cmp, r2, r2Max);

                    int mask = _mm256_movemask_epi8(cmp);
                    if(!mask) break;

                    _mm256_sub_ps(xArr, x2, y2); _mm256_add_ps(xArr, xArr, x0Arr);
                    _mm256_add_ps(yArr, xy, xy); _mm256_add_ps(yArr, yArr, y0Arr);

                    _mm256_add_epi32(N, N, cmp);

                    BENCHMARK(__asm__ volatile("" : : "x"(N) : "memory");)
                }

                #ifndef BENCHMARK_MODE
                    BYTE b[8] = {}; _mm256_slli_epi32(b, N, 4);
                    BYTE g[8] = {}; _mm256_slli_epi32(g, N, 1);
                    BYTE r[8] = {}; _mm256_slli_epi32(r, N, 2);

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
