
//  CompressorJPG.hpp
//  JPEGSingleBlockEncoding
//
//  Created by Norbert Szawloga on 25/06/2024.
//

#pragma once
#include <iostream>
#include <bitset>
#include <vector>
#include <fstream>
#include <cmath>
#include <assert.h>
#include "Channel.hpp"


using namespace std;


class CompressorJPG{
    
public:
    
    int zero16Counter = 0;
    vector<int> zero16MCUs;
    
    
    void writeComponents(ofstream& file, vector<bool> data);
    
    void writeToFileY(vector<bool> data, Channel& chanY);
    
    void readBlock(int8_t block[64]);


    
    
    
    bool readFile(string imageName, vector<uint8_t>& pixelData);
    
    
    int imageWidth;
    int imageHeight;
    
    uint8_t *channelY;
    uint8_t *channelCr;
    uint8_t *channelCb;
    
    vector<Block> blocksY;
    vector<Block> blocksCb;
    vector<Block> blocksCr;
    
    vector<Block> blocksYDCT;
    vector<Block> blocksCbDCT;
    vector<Block> blocksCrDCT;
    
    
    uint8_t quantTableY[64] =  {16,12,14,14,18,24,49,72,
        11,12,13,17,22,35,64,92,
        10,14,16,22,37,55,78,95,
        16,19,24,29,56,64,87,98,
        24,26,40,51,68,81,103,112,
        40,58,57,87,109,104,121,100,
        51,60,69,80,103,113,120,103,
        61,55,56,62,77,92,101,99};
    
    /*{16,12,14,14,18,24,49,72,
        11,12,13,17,22,35,64,92,
        10,14,16,22,37,55,78,95,
        16,19,24,29,56,64,87,98,
        24,26,40,51,68,81,103,112,
        40,58,57,87,109,104,121,100,
        51,60,69,80,103,113,120,103,
        61,55,56,62,77,92,101,99};*/
    
    /*{
        8,  6,  5,  8, 12, 20, 26, 31,
        6,  6,  7, 10, 13, 29, 30, 28,
        7,  7,  8, 12, 20, 29, 35, 28,
        7,  9, 11, 15, 26, 44, 40, 31,
        9, 11, 19, 28, 34, 55, 52, 39,
        12, 18, 28, 32, 41, 52, 57, 46,
        25, 32, 39, 44, 52, 61, 60, 51,
        36, 46, 48, 49, 56, 50, 52, 50
    };*/
    
    /*
    uint8_t quantTableY[64] =
    {16,12,14,14,18,24,49,72,
        11,12,13,17,22,35,64,92,
        10,14,16,22,37,55,78,95,
        16,19,24,29,56,64,87,98,
        24,26,40,51,68,81,103,112,
        40,58,57,87,109,104,121,100,
        51,60,69,80,103,113,120,103,
        61,55,56,62,77,92,101,99};
    */
    
    /*
    {
        16, 11, 10, 16, 24, 40, 51, 61,
        12, 12, 14, 19, 26, 58, 60, 55,
        14, 13, 16, 24, 40, 57, 69, 56,
        14, 17, 22, 29, 51, 87, 80, 62,
        18, 22, 37, 56, 68, 109, 103, 77,
        24, 35, 55, 64, 81, 104, 113, 92,
        49, 64, 78, 87, 103, 121, 120, 101,
        72, 92, 95, 98, 112, 100, 103, 99
    };
    */

    
    int zigzagMap[64] = {0, 1, 8, 16, 9, 2, 3, 10,
        17, 24, 32, 25, 18, 11, 4, 5,
        12, 19, 26, 33, 40, 48, 41, 34,
        27, 20, 13, 6, 7, 14, 21, 28,
        35, 42, 49, 56, 57, 50, 43, 36,
        29, 22, 15, 23, 30, 37, 44, 51,
        58, 59, 52, 45, 38, 31, 39, 46,
        53, 60, 61, 54, 47, 55, 62, 63
    };
    
    
    
    
    

    CompressorJPG(string imageName);
    
    
    
};


#pragma pack(push,1)
struct BMPFileHeader {
    uint16_t fileType;       // File type, always 4D42h ("BM")
    uint32_t fileSize;       // Size of the file in bytes
    uint16_t reserved1;      // Always 0
    uint16_t reserved2;      // Always 0
    uint32_t offsetData;     // Start position of pixel data (bytes from the beginning of the file)
};

// Bitmap info header (40 bytes)
struct BMPInfoHeader {
    uint32_t size;           // Size of this header (40 bytes)
    int32_t width;           // Width of the bitmap in pixels
    int32_t height;          // Height of the bitmap in pixels
    uint16_t planes;         // Number of color planes, must be 1
    uint16_t bitCount;       // Number of bits per pixel
    uint32_t compression;    // Compression type (0 = uncompressed)
    uint32_t sizeImage;      // Size of the image data
    int32_t xPelsPerMeter;   // Horizontal resolution
    int32_t yPelsPerMeter;   // Vertical resolution
    uint32_t colorsUsed;     // Number of colors in the color palette
    uint32_t colorsImportant;// Number of important colors
};
#pragma pack(pop)
