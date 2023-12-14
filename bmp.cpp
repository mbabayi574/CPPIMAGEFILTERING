#include "bmp.hpp"

#include <algorithm>
#include <fstream>

Pixel::Pixel(uint8_t r, uint8_t g, uint8_t b) 
    : blu(b), grn(g), red(r) {}

bool Bmp::create(int width, int height) {
  validState = false;
  padding = calcBmpPadding(width);

  hdr.fileType = BMP_FILE_TYPE;
  hdr.fileSize = sizeof(Header) + sizeof(InfoHeader) + (width + padding) * height * sizeof(Pixel);
  hdr.reserved1 = 0;
  hdr.reserved2 = 0;
  hdr.offset = sizeof(Header) + sizeof(InfoHeader);

  delete[] fileData;
  fileData = new (std::nothrow) char[hdr.fileSize];
  if (fileData == nullptr) return false;
  std::fill(fileData, fileData + hdr.fileSize, 0x00);

  infoHdr.headerSize = sizeof(InfoHeader);
  infoHdr.width = width;
  infoHdr.height = height;
  infoHdr.planes = 1;
  infoHdr.bitCount = 24;
  infoHdr.compression = 0; 
  infoHdr.imageSize = (width + padding) * height * sizeof(Pixel);

  std::copy((char*)&hdr, (char*)&hdr + sizeof(Header), fileData);
  std::copy((char*)&infoHdr, (char*)&infoHdr + sizeof(InfoHeader), fileData + sizeof(Header));

  validState = true;
  data.assign(height, std::vector<Pixel>(width));
  return true;
}

bool Bmp::read(const std::string& filename) {
  std::ifstream file(filename, std::ios_base::binary);
  if(!file.is_open()) return false; 
  
  file.read((char*)&hdr, sizeof(Header));
  file.read((char*)&infoHdr, sizeof(InfoHeader));
  
  if (hdr.fileType != BMP_FILE_TYPE) return false;
  if (infoHdr.bitCount != 24 || infoHdr.compression != 0) return false;
  padding = calcBmpPadding(infoHdr.width);

  file.seekg(0, std::ios_base::end);
  std::streampos length = file.tellg();
  file.seekg(0, std::ios_base::beg);

  delete[] fileData;
  fileData = new (std::nothrow) char[length];
  if (fileData == nullptr) return false;
  file.read(fileData, length);
  
  validState = true;
  fileDataToVector();

  file.close();
  
  return true;
}

bool Bmp::write(const std::string& filename) {

  std::ofstream file(filename, std::ios_base::binary);
  if(!file.is_open()) return false;
  
  std::copy((char*)&hdr, (char*)&hdr + sizeof(Header), fileData);
  std::copy((char*)&infoHdr, (char*)&infoHdr + sizeof(InfoHeader), fileData + sizeof(Header));
  
  vectorToFileData();

  file.write(fileData, hdr.fileSize);

  file.close();

  return true;
}

int Bmp::getWidth() const {
  return infoHdr.width; 
}

int Bmp::getHeight() const {
  return infoHdr.height;
}
   
Pixel& Bmp::getPixel(int row, int col) {
  const int rowStart = (infoHdr.height - 1 - row) * (infoHdr.width * sizeof(Pixel) + padding);
  char* pixelPos = fileData + hdr.offset + rowStart + col * sizeof(Pixel);
  return *(Pixel*)pixelPos; 
}

void Bmp::setPixel(int row, int col, Pixel pixel) {
  const int rowStart = (infoHdr.height - 1 - row) * (infoHdr.width * sizeof(Pixel) + padding);
  char* pixelPos = fileData + hdr.offset + rowStart + col * sizeof(Pixel); 
  *(Pixel*)pixelPos = pixel;
}

int calcBmpPadding(int width) {
  const int bytesInRow = width * sizeof(Pixel);
  return bytesInRow % 4 ? 4 - bytesInRow % 4 : 0;  
}

void fileDataToVector() {
  data.assign(infoHdr.height, std::vector<Pixel>(infoHdr.width));
  for (int row = 0; row < infoHdr.height; ++row) {
      for (int col = 0; col < infoHdr.width; ++col) {
          data[row][col] = getFilePixel(*this, row, col); 
    }
  }
}

void vectorToFileData() {
  for (int row = 0; row < infoHdr.height; ++row) {
    for (int col = 0; col < infoHdr.width; ++col) {
        getFilePixel(*this, row, col) = data[row][col];
    }
  }
}