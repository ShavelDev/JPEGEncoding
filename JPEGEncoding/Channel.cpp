//
//  Channel.cpp
//  JPEGEncoding
//
//  Created by Norbert Szawloga on 13/07/2024.
//

#include "Channel.hpp"
#include <vector>


vector<bool> Channel::writeToDataArray(){
    
    vector<bool> data;
    for (int i = 0; i < blocks.size(); i++) {
        int prevDC = i == 0 ? 0 : blocks[i-1].data[0];
        //int current16Counter = zero16Counter;
        vector<bool> blockData = blockToBits(prevDC, blocks[i]);
        data.insert(data.end(), blockData.begin(), blockData.end());
    }
    
    return data;
    
}


int Channel::getNumOfBits(int num){
    
    num = abs(num);
    const size_t sizeBits = sizeof(num) * 8;
    bitset<sizeBits> bits(num);
    for (int i = sizeBits - 1; i >= 0 ; i--) {
        if (bits[i] == 0) {
            continue;
        }else{
            return (i + 1);
        }
    }
    
    //in case its zero
    return 0;
}

int Channel::getACCodesSize(){
    return (int)codesAC.size();
}

int Channel::getDCCodesSize(){
    return (int)codesDC.size();
}



bitset<8> Channel::getHuffmanSymbol(int zerosCount, int numOfBits){
    bitset<4> zeros(zerosCount);
    bitset<4> length(numOfBits);
    
    
   bitset<8> result;
    
    // Copy bits from the first bitset into the higher part of the result
    for (size_t i = 0; i < 4; ++i) {
        result[i + 4] = zeros[i];
    }
    
    // Copy bits from the second bitset into the lower part of the result
    for (size_t i = 0; i < 4; ++i) {
        result[i] = length[i];
    }
    
    return result;
}



void Channel::createCodesDC(){
    vector<unique_ptr<nodeDC>> nodes;

    cout <<  "create Component Tree" << endl;
    createComponentTreeDC(nodes);
    
    while (nodes.size() != 1) {
        joinTheLowestDC(nodes);
    }
    
    
    unique_ptr<nodeDC> head = std::move(nodes[0]);
    
    cout <<  "calculate Codes" << endl;
    calculateCodesDC(head, 0);
    sort(codesDC.begin(), codesDC.end(), compareByBranchDepthDC);
    
    int codeLengthMap[16] = {};
    for (int i= 0; i < codesDC.size(); i++) {
        codeLengthMap[codesDC[i].branchDepth-1]++;
    }
    
    int currCode = 0;
    short codeCandidate = 0;
    
    for (int i = 0; i < 16; i++) {
        
        
        
        for (int j = 0; j < codeLengthMap[i]; j++) {
            codesDC[currCode].codeInBits = bitset<16>(codeCandidate);
            
            cout << "Val: " << codesDC[currCode].val << " code: " << codesDC[currCode].codeInBits.to_string().substr(16-codesDC [currCode].branchDepth) << " code-length: "<< codesDC[currCode].branchDepth << endl;
            codeCandidate++;
            currCode++;
            
        }
        
        codeCandidate = codeCandidate << 1;
        
        //cout << "Length: " << i+1 << " Num of codes: " << codeLengthMap[i] << endl;
    }

    
    
    
}



void Channel::createCodesAC(){
    
    vector<unique_ptr<nodeAC>> nodes;
    createComponentTreeAC(nodes);
    
    while (nodes.size() != 1) {
        joinTheLowestAC(nodes);
    }
    
    unique_ptr<nodeAC> head = std::move(nodes[0]);
    
    calculateCodesAC(head, 0);
    sort(codesAC.begin(), codesAC.end(), compareByBranchDepthAC);
    
    int codeLengthMap[16] = {};
    for (int i= 0; i < codesAC.size(); i++) {
        codeLengthMap[codesAC[i].branchDepth-1]++;
    }
    
    int currCode = 0;
    short codeCandidate = 0;
    
    for (int i = 0; i < 16; i++) {
        
        
        
        for (int j = 0; j < codeLengthMap[i]; j++) {
            codesAC[currCode].codeInBits = bitset<16>(codeCandidate);
            
            cout << "Symbol: " << codesAC[currCode].huffmanSym << " code: " << codesAC[currCode].codeInBits.to_string().substr(16-codesAC[currCode].branchDepth) << " code-length: "<< codesAC[currCode].branchDepth << endl;
            codeCandidate++;
            currCode++;
            
        }
        
        codeCandidate = codeCandidate << 1;
        
        //cout << "Length: " << i+1 << " Num of codes: " << codeLengthMap[i] << endl;
    }

    
    
    
}
void Channel::createComponentTreeAC(vector<unique_ptr<nodeAC>>& nodes){
    
    //add EOB because there will always be at least one
    nodes.push_back(std::make_unique<nodeAC>(blocks.size(), bitset<8>(0)));
    
    
    for (int k = 0; k < blocks.size(); k++) {
        
        int zeroCounter = 0;
        for (int i = 1; i < 64; i++) {
            
            //USE ZIGZAG HERE zigzag[i]
            int currVal = blocks[k].data[zigzagMap[i]];

            if (currVal == 0) {
                zeroCounter++;
                
                
                
                continue;
                
            }
            else{
                cout << zeroCounter << "/" << getNumOfBits(currVal) << ";  val: " << currVal <<  endl;
                cout << "Huffman Symbol: " <<  getHuffmanSymbol( zeroCounter,getNumOfBits(currVal))<< " binary: " <<  intToBitstring(currVal)<< endl << endl;
                
                
                
                bitset<8> currSymbol = getHuffmanSymbol(zeroCounter, getNumOfBits(currVal));
                
                bool symInNodes = false;
                for (int j = 0; j < nodes.size(); j++) {
                    if (nodes[j]->huffmanSymbol == currSymbol) {
                        symInNodes = true;
                        nodes[j]->freq += 1;
                        break;
                        
                    }
                }
                if (!symInNodes) {
                    nodes.push_back(std::make_unique<nodeAC>(1, currSymbol));
                }
                
                
                
                zeroCounter = 0;
            }
            
        }
        
    }
    
}

void Channel::createComponentTreeDC(vector<unique_ptr<nodeDC>>& nodes){
    
    int currCoeff = blocks[0].data[0];
    nodes.push_back(std::make_unique<nodeDC>(1, getNumOfBits(currCoeff)));
    
    for (int i = 1; i < blocks.size(); i++) {
        currCoeff = blocks[i].data[0] - blocks[i-1].data[0];
        bool valInNodes = false;
        for (int j = 0; j < nodes.size(); j++) {
            if (nodes[j]->val == getNumOfBits(currCoeff)) {
                nodes[j]->freq += 1;
                valInNodes = true;
                break;
            }
        }
        if (!valInNodes) {
            nodes.push_back(std::make_unique<nodeDC>(1,getNumOfBits(currCoeff)));
        }
    }
    
}

string Channel::intToBitstring(int num){
    return "";
}


void Channel::joinTheLowestAC(vector<unique_ptr<nodeAC>>& nodes){
    int lowestInd = -1;
    int secondLow = -1;
    int minVal = INT_MAX;
    int secondMinVal = INT_MAX;
    
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i]->freq < minVal) {
            secondLow = lowestInd;
            secondMinVal = minVal;
            lowestInd = i;
            minVal = nodes[i]->freq;
        } else if (nodes[i]->freq < secondMinVal) {
            secondLow = i;
            secondMinVal = nodes[i]->freq;
        }
    }
    
    
    int firstIndex = lowestInd < secondLow ? lowestInd : secondLow;
    int secondIndex = lowestInd > secondLow ? lowestInd : secondLow;
    unique_ptr<nodeAC> newNode = make_unique<nodeAC>((nodes[lowestInd]->freq + nodes[secondLow]->freq), bitset<8>(0));
    newNode->left = std::move(nodes[secondLow]);
    newNode->right = std::move(nodes[lowestInd]);
    nodes.push_back(std::move(newNode));
    
    nodes.erase(nodes.begin() + secondIndex);
    nodes.erase(nodes.begin() + firstIndex);
}

void Channel::joinTheLowestDC(vector<unique_ptr<nodeDC>>& nodes){
    int lowestInd = -1;
    int secondLow = -1;
    int minVal = INT_MAX;
    int secondMinVal = INT_MAX;
    
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i]->freq < minVal) {
            secondLow = lowestInd;
            secondMinVal = minVal;
            lowestInd = i;
            minVal = nodes[i]->freq;
        } else if (nodes[i]->freq < secondMinVal) {
            secondLow = i;
            secondMinVal = nodes[i]->freq;
        }
    }
    
    
    int firstIndex = lowestInd < secondLow ? lowestInd : secondLow;
    int secondIndex = lowestInd > secondLow ? lowestInd : secondLow;
    unique_ptr<nodeDC> newNode = make_unique<nodeDC>((nodes[lowestInd]->freq + nodes[secondLow]->freq), 0);
    newNode->left = std::move(nodes[secondLow]);
    newNode->right = std::move(nodes[lowestInd]);
    nodes.push_back(std::move(newNode));
    
    nodes.erase(nodes.begin() + secondIndex);
    nodes.erase(nodes.begin() + firstIndex);
}

void Channel::calculateCodesDC( unique_ptr<nodeDC>& node, int branchNum){
    if (node->left != nullptr) {
        calculateCodesDC( node->left, branchNum + 1);
    }
    
    if (node->right != nullptr) {
        calculateCodesDC(node->right, branchNum + 1);
    }
    
    if (node->left == nullptr && node->right == nullptr) {
        codesDC.push_back({node->val, bitset<16>(0), branchNum});
        cout << "Val: " << node->val << " Occurances: " << node->freq << " Branch: "<< branchNum<< endl;
        
    }
    
}



void Channel::calculateCodesAC(unique_ptr<nodeAC>& node, int branchNum){
    if (node->left != nullptr) {
        calculateCodesAC(node->left, branchNum + 1);
    }
    
    if (node->right != nullptr) {
        calculateCodesAC(node->right, branchNum + 1);
    }
    
    if (node->left == nullptr && node->right == nullptr) {
        codesAC.push_back({node->huffmanSymbol, bitset<16>(0), branchNum});
        cout << "Symbol: " << node->huffmanSymbol  << " Occurances: " << node->freq << " Branch: "<< branchNum<< endl;
        
    }
    
}
bool Channel::compareByBranchDepthAC(const codeAC& a, const codeAC& b){
    return a.branchDepth < b.branchDepth;
}

bool Channel::compareByBranchDepthDC(const codeDC& a, const codeDC& b){
    return a.branchDepth < b.branchDepth;
}






/*
void Channel::arraysToBlock(int width, int height, int8_t* array, vector<Block>& blockStorage){
    
    int blocksInX = width/8;
    int blocksInY = height/8;
    cout << "NumOfBlocks: x: " << blocksInX << " y: " << blocksInY << endl;
    
    blockStorage.resize(blocksInX * blocksInY);
    
    
    for (int yB = 0; yB < blocksInY; yB++) {
                for (int xB = 0; xB < blocksInX; xB++) {
                    for (int y = 0; y < 8; y++) {
                        for (int x = 0; x < 8; x++) {
                            blockStorage[xB + yB * blocksInX].data[x + 8 * y] = array[(xB * 8 + x) + (yB * 8 + y) * width];
                        }
                    }
                }
            }
    
    
    //test for the first block
    
    for (int i = 0; i < 64; i++) {
        cout << "ARRAY: "<< i << " VAL: "<< (int)blockStorage[0].data[i] << endl;
    }

}*/

void Channel::performDCT(Block& b, uint8_t quantTable[64]){
    
    int DctCoeff[64];
    
    for (int i = 0; i < 64; ++i) {
            DctCoeff[i] = static_cast<int>(b.data[i]);
       }
    
    
    cout << "BEFORE: " << endl;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            cout <<  (int) b.data[x + 8*y] << " " ;
        }
        cout << endl;
    }
    
    
    for (int yC = 0; yC < 8; yC++) {
        for (int xC = 0; xC < 8; xC++) {
            
            float currCoeff  = 0;
            
            for (int y = 0; y < 8; y++) {
                for (int x = 0; x < 8; x++) {
                    float coeffX = (xC == 0) ? 0.707107 : 1;
                    float coeffY = (yC == 0) ? 0.707107: 1;
                    
                    currCoeff += ((float)DctCoeff[x + 8*y]) * coeffX*coeffY * cos((2*x+1)*xC*M_PI/16) * cos((2*y+1)*yC*M_PI/16)/4;
                    
                    
                }
            }
            
            b.data[xC+8*yC] = static_cast<int>(std::round(currCoeff/(float)quantTable[xC+8*yC])); //(int)(currCoeff/(float)quantTable[xC+8*yC]);
            
        }
        
    }
    
    cout << "After: " << endl;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            cout <<  (int) b.data[x + 8*y] << " " ;
        }
        cout << endl;
    }
    
}


vector<bool> Channel::getCodeBySymbol( bitset<8> symbol){
    
    for (int i = 0; i < codesAC.size(); i++) {
        if (codesAC[i].huffmanSym == symbol) {
            vector<bool> res;
            for (int j = codesAC[i].branchDepth-1; j >=0; j--) {
                res.push_back(codesAC[i].codeInBits[j]);
            }
            return res;
        }
    }
    
    assert(false);
    
}

vector<bool> Channel::getCodeByVal( bitset<8> val){
    
    for (int i = 0; i < codesDC.size(); i++) {
        if (codesDC[i].val == val) {
            vector<bool> res;
            for (int j = codesDC[i].branchDepth-1; j >=0; j--) {
                res.push_back(codesDC[i].codeInBits[j]);
            }
            return res;
        }
    }
    
    assert(false);
    
}

vector<bool> Channel::getBitsOfVal(int val){
    
    bitset<32> bits(abs(val));
    for (int i = 31; i >= 0 ; i--) {
        if (bits[i] != 0) {
            vector<bool> res;
            for (int j = i; j >= 0 ; j--) {
                bool bit = val > 0 ? bits[j] : !bits[j];
                res.push_back(bit);
            }
            return res;
        }
        
    }
    
    
    //return {0};
    cout << "VAL: " << val << endl;
    assert(false);
}

void printVector(vector<bool> vec){
    for (int i = 0; i < vec.size(); i++) {
        cout << vec[i];
    }
    cout << endl;
}

vector<bool> Channel::blockToBits(int8_t prevDC, Block b){
    vector<bool> bits;
    cout << (int)b.data[0]-(int)prevDC << endl;
    vector<bool> codeDC = getCodeByVal(getNumOfBits((int)b.data[0]-(int)prevDC));
    cout << "Code: " << endl;
    printVector(codeDC);
    
    vector<bool> coeffDC;
    if ((int)b.data[0]-(int)prevDC == 0) {
        //DELETE THIS
        coeffDC = {};
    }else{
        
        coeffDC = getBitsOfVal((int)b.data[0]-(int)prevDC);
        codeDC.insert(codeDC.end(), coeffDC.begin(),coeffDC.end());
    }
    
    
    //codeDC.insert(codeDC.end(), coeffDC.begin(),coeffDC.end());
    printVector(codeDC);
    bits.insert(bits.end(), codeDC.begin(), codeDC.end());
    
    
    int zeroCounter = 0;
    for (int i = 1; i < 64; i++) {
        
        //USE ZIGZAG HERE zigzag[i]
        int currVal = b.data[zigzagMap[i]];

        if (currVal == 0) {
            zeroCounter++;
            
            
            continue;
            
        }
        else{
            
            
            //cout << "FIRST VAL: " <<(int) b.data[zigzagMap[i]];
            bitset<8> currSymbol = getHuffmanSymbol(zeroCounter, getNumOfBits(currVal));
            
            vector<bool> bitsSymbol = getCodeBySymbol(currSymbol);
            vector<bool> bitsCoeff = getBitsOfVal((int) b.data[zigzagMap[i]] );
            bitsSymbol.insert(bitsSymbol.end(), bitsCoeff.begin(), bitsCoeff.end());
            
            
            
            bits.insert(bits.end(), bitsSymbol.begin(), bitsSymbol.end());
            
            
            zeroCounter = 0;
        }
        
    }
    
    
    
    
    //EOB
    vector<bool> bitsEOB = getCodeBySymbol(bitset<8>(0));
    
    bits.insert(bits.end(), bitsEOB.begin(), bitsEOB.end());
    
    
    printVector(bits);
    return bits;
}

Channel::Channel(int width, int height, int8_t* data): imageWidth(width), imageHeight(height){
    
    int blocksInX = width/8;
    int blocksInY = height/8;
    cout << "NumOfBlocks: x: " << blocksInX << " y: " << blocksInY << endl;
    
    blocks.resize(blocksInX * blocksInY);
    
    
    for (int yB = 0; yB < blocksInY; yB++) {
                for (int xB = 0; xB < blocksInX; xB++) {
                    for (int y = 0; y < 8; y++) {
                        for (int x = 0; x < 8; x++) {
                            blocks[xB + yB * blocksInX].data[x + 8 * y] = data[(xB * 8 + x) + (yB * 8 + y) * width];
                        }
                    }
                }
            }
    
    
    //test for the first block
    //DELETE
    for (int i = 0; i < 64; i++) {
        cout << "ARRAY: "<< i << " VAL: "<< (int)blocks[0].data[i] << endl;
    }
    
    
    
    
}


