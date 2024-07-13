//
//  Structs.h
//  JPEGEncoding
//
//  Created by Norbert Szawloga on 13/07/2024.
//

#pragma once
#include <iostream>
#include <bitset>
#include <vector>
#include <fstream>
#include <cmath>
#include <assert.h>

using namespace std;


struct codeDC{
    int val;
    bitset<16> codeInBits;
    int branchDepth;
};

struct codeAC{
    bitset<8> huffmanSym;
    bitset<16> codeInBits;
    int branchDepth;
};

struct nodeAC{
    int freq;
    const bitset<8> huffmanSymbol;
    unique_ptr<nodeAC>  left = nullptr;
    unique_ptr<nodeAC> right = nullptr;
    
    nodeAC(int f, const bitset<8>& symbol)
            : freq(f), huffmanSymbol(symbol) {}
};

struct nodeDC{
    int freq;
    const int val;
    unique_ptr<nodeDC>  left = nullptr;
    unique_ptr<nodeDC> right = nullptr;
    
    nodeDC(int f, int v)
            : freq(f), val(v) {}
};

struct Block {
    int8_t data[64];
};


