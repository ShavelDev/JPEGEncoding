//
//  Channel.hpp
//  JPEGEncoding
//
//  Created by Norbert Szawloga on 13/07/2024.
//

#pragma once
#include <iostream>
#include "Structs.hpp"
using namespace std;

class Channel{
    

    
    uint8_t quantTableY[64] =  {16,12,14,14,18,24,49,72,
        11,12,13,17,22,35,64,92,
        10,14,16,22,37,55,78,95,
        16,19,24,29,56,64,87,98,
        24,26,40,51,68,81,103,112,
        40,58,57,87,109,104,121,100,
        51,60,69,80,103,113,120,103,
        61,55,56,62,77,92,101,99};
    
    int zigzagMap[64] = {0, 1, 8, 16, 9, 2, 3, 10,
        17, 24, 32, 25, 18, 11, 4, 5,
        12, 19, 26, 33, 40, 48, 41, 34,
        27, 20, 13, 6, 7, 14, 21, 28,
        35, 42, 49, 56, 57, 50, 43, 36,
        29, 22, 15, 23, 30, 37, 44, 51,
        58, 59, 52, 45, 38, 31, 39, 46,
        53, 60, 61, 54, 47, 55, 62, 63
    };
    
    
    
public:
    int imageWidth;
    int imageHeight;
    uint8_t *data;
    vector<Block> blocks;
    vector<codeAC> codesAC;
    vector<codeDC> codesDC;
    
    vector<bool> writeToDataArray();
    
    int getACCodesSize();
    int getDCCodesSize();
    
    vector<bool> getBlockData();
    
    Channel(int imageWidth, int imageHeight, int8_t* data);
  
    void performDCT();
    
    
    
    vector<bool> getCodeBySymbol(bitset<8> symbol);
    //channel
    vector<bool> getCodeByVal(bitset<8> val);
    //channel
    
    //returns the number as bits ex: 10 => 1010, -10 => 0101
    vector<bool> getBitsOfVal(int val);
    //channel
    
    vector<bool> blockToBits(int8_t prevDC, Block b);
    //channel
    
    void readBlock(int8_t block[64]);
    
    //returns the number as bits ex: 10 => 4, -10 => 4
    int getNumOfBits(int num);
    //channel
    string intToBitstring(int num);
    //channel
    bitset<8> getHuffmanSymbol(int zerosCount, int numOfBits);
    //channel
    void createCodesAC();
    //channel
    void createComponentTreeAC(vector<unique_ptr<nodeAC>>& huffmanSymbols);
    //channel
    void createComponentTreeDC(vector<unique_ptr<nodeDC>>& huffmanSymbols);
    //channel
    
    void joinTheLowestAC(vector<unique_ptr<nodeAC>>& nodes);
    //channel
    void joinTheLowestDC(vector<unique_ptr<nodeDC>>& nodes);
    //channel
    
    void calculateCodesAC(unique_ptr<nodeAC>& node, int branchNum);
    //channel
    static bool compareByBranchDepthAC(const codeAC& a, const codeAC& b);
    // channel
    
    static bool compareByBranchDepthDC(const codeDC& a, const codeDC& b);
    // channel
    bitset<16> getCodeBySymbol();
    //channel
    void createCodesDC();
    //channel
    void calculateCodesDC(unique_ptr<nodeDC>& node, int branchNum);
    //channel
    
    //void arraysToBlock(int width, int height,int8_t* array);
    // channel constructor
    
    void performDCT(Block& b, uint8_t quantTable[64]);
    // channel
    
};

