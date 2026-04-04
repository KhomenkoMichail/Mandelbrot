#include "TXLib.h"

int main(void) {
    const int sizeX = 800, sizeY = 600;

    float x_min = -2,     x_max = 2;
    float y_min = -2,   y_max = 2;

    const int x0 = sizeX / 2;
    const float y0 = 0.5;

    const float scaleX = (x_max - x_min) / sizeX;
    const float scaleY = (y_max - y_min) / sizeX;

    const float r2 = 1;

    //FreeConsole();
    txCreateWindow (sizeX, sizeY);
    RGBQUAD* graphBuf = txVideoMemory();

    for (int y = 0; y < sizeY; y++) {
        for (int x = 0; x < sizeX; x++) {

            float vmX = x_min + (float)x * scaleX;
            float vmY = y_min + (float)y * scaleY + y0;

            RGBQUAD color = {255, 0, 0};
            if(vmX*vmX + vmY*vmY <= r2)
                color = {0, 0, 255};

            int videoMemoRyOffset = (sizeY - 1 - y) * sizeX + x;
            graphBuf[videoMemoRyOffset] = color;
        }
    }

    return 0;
}
