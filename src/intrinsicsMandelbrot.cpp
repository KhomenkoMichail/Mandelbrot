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
#include <immintrin.h>

int main(void) {

    BENCHMARK(const int   NUM_OF_TESTS = 500;)

    const int sizeX = 800, sizeY = 600;

    const float dx  = 1.0f / sizeX;
    const float dy  = 1.0f / sizeY;

    float xC = 0, yC = 0, scale = 3.0;

    const int   nMax = 256;

    __m256 r2Max            =   _mm256_set1_ps(4.0f);
    __m256 arr01234567      =   _mm256_set_ps(7, 6, 5, 4, 3, 2, 1, 0);
    __m256 const2           =   _mm256_set1_ps(2.0f);

    SIMPLE_MODE(__m256i byteMask        =   _mm256_set1_epi32(0xFF);)

    #ifndef BENCHMARK_MODE
        float fps = 0;
        DWORD lastTime = GetTickCount();

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

        float xOneStep = dx*scale;
        __m256 xOneStepArr     =  _mm256_set1_ps(xOneStep);
        xOneStepArr            =  _mm256_mul_ps(arr01234567, xOneStepArr);

        __m256 xEightStepsArr  =  _mm256_set1_ps(8.0f * xOneStep);

        for (int y = 0; y < sizeY; y++) {

            float y0 = (((float)y - 300.f) * dy + yC) *scale;
            __m256 y0Arr       =_mm256_set1_ps(y0);

            float x0Base       =  ((-400.0f) * dx + xC) * scale;
            __m256 x0Arr       =  _mm256_set1_ps(x0Base);
            x0Arr              =  _mm256_add_ps(x0Arr, xOneStepArr);

            SIMPLE_MODE(RGBQUAD* сurRow    =  graphBuf + (sizeY - 1 - y) * sizeX;)

            for (int x = 0; x < sizeX; x += 8) {

                __m256 xArr         =   x0Arr;
                __m256 yArr         =   y0Arr;

                __m256i N = _mm256_setzero_si256();
                for (int n = 0; n < nMax; n++) {
                    __m256 x2       =   _mm256_mul_ps(xArr, xArr);
                    __m256 y2       =   _mm256_mul_ps(yArr, yArr);
                    __m256 xy       =   _mm256_mul_ps(xArr, yArr);

                    __m256 r2       =   _mm256_add_ps(x2, y2);

                    __m256 cmp_f    =   _mm256_cmp_ps(r2, r2Max, _CMP_LE_OQ);

                    int mask = _mm256_movemask_ps(cmp_f);
                    if(!mask) break;

                    xArr             =  _mm256_sub_ps(x2, y2);
                    xArr             =  _mm256_add_ps(xArr, x0Arr);

                    yArr             =  _mm256_fmadd_ps(const2, xy, y0Arr);

                    __m256i cmp_i   =   _mm256_castps_si256 (cmp_f);
                    N                =  _mm256_sub_epi32(N, cmp_i);

                    BENCHMARK(__asm__ volatile("" : : "x"(N) : "memory");)
                }
                #ifndef BENCHMARK_MODE
                    __m256i b            =  _mm256_slli_epi32(N, 4);
                    __m256i g            =  _mm256_slli_epi32(N, 1);
                    __m256i r            =  _mm256_slli_epi32(N, 2);

                    b                    =  _mm256_and_si256(b, byteMask);
                    g                    =  _mm256_and_si256(g, byteMask);
                    r                    =  _mm256_and_si256(r, byteMask);

                    g                    =  _mm256_slli_epi32(g, 8);
                    r                    =  _mm256_slli_epi32(r, 16);

                    __m256i color        =  _mm256_or_si256(_mm256_or_si256(b, g), r);

                    _mm256_storeu_si256((__m256i*)(сurRow + x), color);
                #endif

                x0Arr = _mm256_add_ps(x0Arr, xEightStepsArr);
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
