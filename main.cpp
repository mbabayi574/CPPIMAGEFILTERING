#include <iostream>

#include "bmp.hpp"

std::vector<std::vector<float>> gaussianBlurKernel = {
    {1/16, 2/16, 1/16},
    {2/16, 4/16, 2/16},
    {1/16, 2/16, 1/16}
};

std::vector<std::vector<float>> sharpenKernel = {
    {0, -1, 0},
    {-1, 5,-1},
    {0, -1, 0}
};

std::vector<std::vector<float>> embossKernel = {
    {-2,-1,0},
    {-1, 1,1},
    { 0, 1,2}
};

void applyKernel(const Bmp& inputBmp, Bmp& outputBmp, const std::vector<std::vector<float>>& kernel) {
    create(outputBmp, inputBmp.infoHdr.width, inputBmp.infoHdr.height);
    for (int i = 0; i < inputBmp.infoHdr.height; i++) {
        for (int j = 0; j < inputBmp.infoHdr.width; j++) {
            float red = 0;
            float grn = 0;
            float blu = 0;
            for (int k = 0; k < kernel.size(); k++) {
                for (int l = 0; l < kernel[k].size(); l++) {
                    int x = j + l - kernel.size() / 2;
                    int y = i + k - kernel.size() / 2;
                    if (x < 0 || x >= inputBmp.infoHdr.width || y < 0 || y >= inputBmp.infoHdr.height) continue;
                    red += inputBmp.data[y][x].red * kernel[k][l];
                    grn += inputBmp.data[y][x].grn * kernel[k][l];
                    blu += inputBmp.data[y][x].blu * kernel[k][l];
                }
            }
            outputBmp.data[i][j] = Pixel(red, grn, blu);
        }
    }
}

int main() {
    Bmp inputBmp;
    read(inputBmp, "Cutecat.bmp");
    Bmp outputBmp;
    create(outputBmp, inputBmp.infoHdr.width, inputBmp.infoHdr.height);
    applyKernel(inputBmp, outputBmp, gaussianBlurKernel);
    write(outputBmp, "Cutecat_gaussianBlur.bmp");
    applyKernel(inputBmp, outputBmp, sharpenKernel);
    write(outputBmp, "Cutecat_sharpen.bmp");
    applyKernel(inputBmp, outputBmp, embossKernel);
    write(outputBmp, "Cutecat_emboss.bmp");
    delete inputBmp.fileData;
    delete outputBmp.fileData;
    return 0;
}
