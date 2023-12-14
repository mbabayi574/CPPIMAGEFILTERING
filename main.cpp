#include <iostream>

#include "bmp.hpp"

/*
To work with BMP files, first include the header and create an instance of the Bmp struct.
Now there are 3 functions we are interested in: create, read, write
These functions take the Bmp struct instance as the first argument.

To read a BMP file: read(bmp, "file.bmp");
To write the modified in-memory BMP to a file: write(bmp, "out.bmp");
To create a new BMP file: create(bmp, width, height);

We can view images as 2D vectors of Pixel structs.
The Pixel struct is just a red-green-blue tuple.

The structure we work with is the 'data' field of 'struct Bmp'
bmp.data[0][0].red // returns a value between 0 and 255
The (0, 0) pixel is the top-left corner of the image.
So we have data[row][column]
The bottom-right pixel is: bmp.data[bmp.infoHdr.height - 1][bmp.infoHdr.width - 1]
*/

int main() {
    Bmp bmp;
    create(bmp, 100, 200);
    bmp.data[0][0] = Pixel(255, 0, 0);
    write(bmp, "test.bmp");
    delete bmp.fileData;
    return 0;
}
