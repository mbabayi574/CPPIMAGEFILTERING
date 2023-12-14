#include "bmp.hpp"

#include <algorithm>
#include <fstream>

Pixel::Pixel(uint8_t r, uint8_t g, uint8_t b)
    : blu(b), grn(g), red(r) {}

bool create(Bmp &bmp, int width, int height)
{
    bmp.validState = false;
    bmp.padding = calcBmpPadding(width);

    bmp.hdr.fileType = BMP_FILE_TYPE;
    bmp.hdr.fileSize = sizeof(Header) + sizeof(InfoHeader) + (width + bmp.padding) * height * sizeof(Pixel);
    bmp.hdr.reserved1 = 0;
    bmp.hdr.reserved2 = 0;
    bmp.hdr.offset = sizeof(Header) + sizeof(InfoHeader);

    delete[] bmp.fileData;
    bmp.fileData = new (std::nothrow) char[bmp.hdr.fileSize];
    if (bmp.fileData == nullptr)
        return false;
    std::fill(bmp.fileData, bmp.fileData + bmp.hdr.fileSize, 0x00);

    bmp.infoHdr = {0};
    bmp.infoHdr.headerSize = sizeof(InfoHeader);
    bmp.infoHdr.width = width;
    bmp.infoHdr.height = height;
    bmp.infoHdr.planes = 1;
    bmp.infoHdr.bitCount = 24;
    bmp.infoHdr.imageSize = (width + bmp.padding) * height * sizeof(Pixel);

    std::copy((char *)&bmp.hdr, (char *)&bmp.hdr + sizeof(Header), bmp.fileData);
    std::copy((char *)&bmp.infoHdr, (char *)&bmp.infoHdr + sizeof(InfoHeader), bmp.fileData + sizeof(Header));

    bmp.validState = true;
    bmp.data.assign(height, std::vector<Pixel>(width));
    return true;
}

ReadResult read(Bmp &bmp, const std::string &filename)
{
    std::ifstream file(filename, std::ios_base::binary);
    if (!file.is_open())
        return ReadResult::open_err;
    bmp.validState = false;

    file.read((char *)&bmp.hdr, sizeof(Header));
    file.read((char *)&bmp.infoHdr, sizeof(InfoHeader));

    if (bmp.hdr.fileType != BMP_FILE_TYPE)
        return ReadResult::invalid_file;
    if (bmp.infoHdr.bitCount != 24 || bmp.infoHdr.compression != 0)
        return ReadResult::unsupported_bmp;
    bmp.padding = calcBmpPadding(bmp.infoHdr.width);

    file.seekg(0, std::ios_base::end);
    std::streampos length = file.tellg();
    file.seekg(0, std::ios_base::beg);

    delete[] bmp.fileData;
    bmp.fileData = new (std::nothrow) char[length];
    if (bmp.fileData == nullptr)
        return ReadResult::alloc_err;
    file.read(bmp.fileData, length);

    bmp.validState = true;
    fileDataToVector(bmp);
    return ReadResult::success;
}

bool write(Bmp &bmp, const std::string &filename)
{
    if (!bmp.validState)
        return false;
    vectorToFileData(bmp);

    std::ofstream file(filename, std::ios_base::binary);
    if (!file.is_open())
        return false;

    file.write(bmp.fileData, bmp.hdr.fileSize);
    return true;
}

int calcBmpPadding(int width)
{
    const int bytesInRow = width * sizeof(Pixel);
    return bytesInRow % 4 ? 4 - bytesInRow % 4 : 0;
}

Pixel &getFilePixel(const Bmp &bmp, int row, int col)
{
    const int rowStart = (bmp.infoHdr.height - 1 - row) * (bmp.infoHdr.width * sizeof(Pixel) + bmp.padding);
    char *const pixelPos = bmp.fileData + bmp.hdr.offset + rowStart + col * sizeof(Pixel);
    return *(Pixel *)pixelPos;
}

void fileDataToVector(Bmp &bmp)
{
    bmp.data.assign(bmp.infoHdr.height, std::vector<Pixel>(bmp.infoHdr.width));
    for (int row = 0; row < bmp.infoHdr.height; ++row)
    {
        for (int col = 0; col < bmp.infoHdr.width; ++col)
        {
            bmp.data[row][col] = getFilePixel(bmp, row, col);
        }
    }
}

void vectorToFileData(Bmp &bmp)
{
    for (int row = 0; row < bmp.infoHdr.height; ++row)
    {
        for (int col = 0; col < bmp.infoHdr.width; ++col)
        {
            getFilePixel(bmp, row, col) = bmp.data[row][col];
        }
    }
}

bool Bmp::create(int width, int height)
{
    // Initialize the file header
    fileHdr.type = 0x4D42; // 'BM'
    fileHdr.size = sizeof(BmpFileHeader) + sizeof(BmpInfoHeader) + width * height * sizeof(Pixel);
    fileHdr.reserved1 = 0;
    fileHdr.reserved2 = 0;
    fileHdr.offBits = sizeof(BmpFileHeader) + sizeof(BmpInfoHeader);

    // Initialize the info header
    infoHdr.size = sizeof(BmpInfoHeader);
    infoHdr.width = width;
    infoHdr.height = height;
    infoHdr.planes = 1;
    infoHdr.bitCount = 24;   // 24-bit color
    infoHdr.compression = 0; // No compression
    infoHdr.sizeImage = width * height * sizeof(Pixel);

    return true;
}

class InfoHeader : public BmpInfoHeader
{
public:
    InfoHeader() = default;
    InfoHeader(const BmpInfoHeader &infoHdr)
        : BmpInfoHeader(infoHdr) {}
    int getWidth() const;

private:
    int width;

    friend class Bmp;

    void setWidth(int width)
    {
        this->width = width;
    }

    void setHeight(int height)
    {
        this->height = height;
    }

    void setBitCount(int bitCount)
    {
        this->bitCount = bitCount;
    }

    void setCompression(int compression)
    {
        this->compression = compression;
    }

    void setImageSize(int imageSize)
    {
        this->imageSize = imageSize;
    }

    void setXPixelsPerMeter(int xPixelsPerMeter)
    {
        this->xPixelsPerMeter = xPixelsPerMeter;
    }

    void setYPixelsPerMeter(int yPixelsPerMeter)
    {
        this->yPixelsPerMeter = yPixelsPerMeter;
    }

    void setColorsUsed(int colorsUsed)
    {
        this->colorsUsed = colorsUsed;
    }
};

class BmpInfoHeader : public InfoHeader
{
public:
    BmpInfoHeader() = default;
    BmpInfoHeader(const InfoHeader &infoHdr)
        : InfoHeader(infoHdr) {}
};

int Bmp::getWidth() const
{
    return infoHdr.width;
}
