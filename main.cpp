#include <iostream>
#include <vector>
#include "bmp.hpp"

class BmpProcessor {
public:
    BmpProcessor(const std::vector<std::vector<float>>& kernel) : kernel(kernel) {}

    void applyKernel(Bmp& bmp) {
        Bmp outputBmp;
        outputBmp.create(bmp.getWidth(), bmp.getHeight());
        for (int i = 0; i < bmp.getHeight(); i++) {
            for (int j = 0; j < bmp.getWidth(); j++) {
                float red = 0;
                float grn = 0;
                float blu = 0;
                for (int k = 0; k < kernel.size(); k++) {
                    for (int l = 0; l < kernel[k].size(); l++) {
                        int x = j + l - kernel.size() / 2;
                        int y = i + k - kernel.size() / 2;
                        if (x < 0 || x >= bmp.getWidth() || y < 0 || y >= bmp.getHeight()) continue;
                        red += bmp.getPixel(y, x).red * kernel[k][l];
                        grn += bmp.getPixel(y, x).grn * kernel[k][l];
                        blu += bmp.getPixel(y, x).blu * kernel[k][l];
                    }
                }
                outputBmp.setPixel(i, j, Pixel(red, grn, blu));
            }
        }
        bmp = outputBmp;
    }

private:
    std::vector<std::vector<float>> kernel;
};

int main() {
    Bmp bmp;
    bmp.read("Cutecat.bmp");

    BmpProcessor gaussianBlurProcessor({
        {1/16, 2/16, 1/16},
        {2/16, 4/16, 2/16},
        {1/16, 2/16, 1/16}
    });
    gaussianBlurProcessor.applyKernel(bmp);
    bmp.write("Cutecat_gaussianBlur.bmp");

    BmpProcessor sharpenProcessor({
        {0, -1, 0},
        {-1, 5,-1},
        {0, -1, 0}
    });
    sharpenProcessor.applyKernel(bmp);
    bmp.write("Cutecat_sharpen.bmp");

    BmpProcessor embossProcessor({
        {-2,-1,0},
        {-1, 1,1},
        { 0, 1,2}
    });
    embossProcessor.applyKernel(bmp);
    bmp.write("Cutecat_emboss.bmp");

    return 0;
}