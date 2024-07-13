//
//  CompressorJPG.cpp
//  JPEGEncoding
//
//  Created by Norbert Szawloga on 13/07/2024.
//

#include "CompressorJPG.hpp"
#include <vector>

//use only for num > abs
bool CompressorJPG::readFile(string imageName, vector<uint8_t>& pixelData){
    ifstream file(imageName, std::ios::binary);
    
    if (!file) {
        cerr << "File could not be opened" << endl;
        return 0;
    }
    
    BMPFileHeader fileHeader;
    file.read(reinterpret_cast<char*>(&fileHeader), 14);
    
    if (fileHeader.fileType == 0x244D) {
        cerr << "Error: Not a BMP file." << endl;
        return 0;
    }
   
    
    BMPInfoHeader infoHeader;
    file.read(reinterpret_cast<char*>(&infoHeader), 40);
    
    imageWidth = infoHeader.width;
    imageHeight = infoHeader.height;
    
    if (imageWidth % 8 != 0 || imageHeight % 8 != 0) {
        cerr << "Error: Wrong Image Size" << endl;
    }
    
    int bitCount = infoHeader.bitCount;
    
    cout << "Offset Data: "<< fileHeader.offsetData << endl;
    cout << "width: "<< (int)infoHeader.width << endl;
    cout << "height: "<< (int)infoHeader.height << endl;
    
    
    
    cout << "Size: " << infoHeader.sizeImage << endl;
    int rowPadded = (imageWidth * bitCount / 8 + 3) & (~3);
    pixelData.resize(infoHeader.sizeImage);
    file.seekg(fileHeader.offsetData, ios::beg);
    
    cout << "SIZE OF VECTOR" << pixelData.size() << endl;
    
    for (int i = 0; i < imageHeight; ++i) {
        file.read(reinterpret_cast<char*>(&pixelData[(imageHeight - 1 - i) * rowPadded]), rowPadded);
    }
    
    
//    file.read(reinterpret_cast<char*>(pixelData.data()), infoHeader.sizeImage);
//    reverse(pixelData.begin(), pixelData.end());
    
    file.close();
    
    
    
    
    return 1;
    
}



CompressorJPG::CompressorJPG(string imageName){
    vector<uint8_t> pixelData;
    
    readFile(imageName, pixelData);
    
    
    
    int imageSize = imageWidth * imageHeight;
    
    int8_t arrY[imageSize];
    int8_t arrCb[imageSize];
    int8_t arrCr[imageSize];
    
    cout << "ImageSize: "<< imageSize << endl;
    for (int i = 0; i < imageSize; i++) {
        uint8_t blue = pixelData[3*i];
        uint8_t green = pixelData[3*i+1];
        uint8_t red = pixelData[3*i+2];
        
        
        
        cout << "Red: " << (int)red << " Green: " << (int)green << " Blue: " << (int)blue<< endl;
        
        int8_t Y = (int8_t)(0.114 * (float)blue + 0.587 * (float)green + 0.299 * (float) red - (int)128);
        int8_t Cb = (int8_t)(0.500 * (int)blue + (-0.331) * (int)green + (-0.169) * (int) red);
        int8_t Cr = (int8_t)((-0.081) * (float)blue + (-0.419) * (float)green + (0.500) * (float) red);
        
        cout << "Y: " << (int)Y << " Cb: " << (int)Cb << " Cr: " << (int)Cr << endl;
        
        
        arrY[i] = Y;
        arrCb[i] = Cb;
        arrCr[i] = Cr;
        
        
        
    }
    
    
    
    //Block test = {{-76, -73, -67, -62, -58, -67, -64, -55, -65, -69, -73, -38, -19, -43, -59, -56, -66, -69, -60, -15, 16, -24, -62, -55, -65, -70, -57, -6, 26, -22, -58, -59, -61, -67, -60, -24, -2, -40, -60, -58, -49, -63, -68, -58, -51, -60, -70, -53, -43, -57, -64, -69, -73, -67, -63, -45, -41, -49, -59, -60, -63, -52, -50, -34}};
    
    /*
    Block test = {{
        -66, -73, -73, -74, -79, -80, -81, -73,
        -66, -71, -74, -76, -80, -81, -80, -75,
        -67, -68, -76, -79, -80, -81, -79, -74,
        -65, -67, -68, -68, -65, -63, -60, -63,
        -61, -61, -58, -54, -49, -43, -37, -36,
        -46, -33, -27, -22, -14, -13, -16, -11,
        -32, -17, -13, -9, 0, 0, 2, -1,
        -19, -7, -1, 5, 11, 13, 12, 5
    }};
    performDCT(test, quantTableY);
    */
    
    
    Channel ChanY(imageWidth,imageHeight, arrY);
    Channel ChanCb(imageWidth,imageHeight, arrCb);
    Channel ChanCr(imageWidth,imageHeight, arrCr);
    
    
    
    cout << "Performing DCT"<< endl;
    for (int i = 0; i < ChanY.blocks.size(); i++) {
        ChanY.performDCT(ChanY.blocks[i], quantTableY);
        ChanCb.performDCT(ChanCb.blocks[i], quantTableY);
        ChanCr.performDCT(ChanCr.blocks[i], quantTableY);
    }
    
    
    cout << "creating AC codes"<< endl;
    ChanY.createCodesAC();
    ChanCb.createCodesAC();
    ChanCr.createCodesAC();
    
    /*
    cout << "AC CODES" << endl<< endl;
    for (int i = 0; i < codesAC.size(); i++) {
        
        cout << "Huffman symbol: " << codesAC[i].huffmanSym.to_string()<< endl;
        cout << "Code: " << codesAC[i].codeInBits.to_string().substr(16 - codesAC[i].branchDepth)<< endl << endl;
    }*/
    
    cout << "creating DC codes"<< endl;
    ChanY.createCodesDC();
    ChanCr.createCodesDC();
    ChanCb.createCodesDC();
    
    
    /*
    cout << "DC Coeff:" << endl << endl;
    for (int i = 0; i < blocksY.size(); i++) {
        
        cout << "Val: " << (int) blocksY[i].data[0]<< endl;

    }
    cout << "DC CODES" << endl<< endl;
    for (int i = 0; i < codesDC.size(); i++) {
        
        cout << "Val: " << codesDC[i].val<< endl;
        cout << "Code: " << codesDC[i].codeInBits.to_string().substr(16 - codesDC[i].branchDepth)<< endl << endl;
    }
    */

    
    //

    
   /* cout<< "getCodeByVal: "  << endl;
    test = getCodeByVal(2);
    for (int i = 0; i < test.size(); i++) {
        cout << test[i];
    }
    cout << endl;*/
    
    
    
    //LEAVE THIS
    /*
    vector<bool> data;
    for (int i = 0; i < blocksY.size(); i++) {
        int prevDC = i == 0 ? 0 : blocksY[i-1].data[0];
        int current16Counter = zero16Counter;
        vector<bool> blockData = blockToBits(prevDC, blocksY[i]);
        if (zero16Counter != current16Counter) {
            zero16MCUs.push_back(i);
        }
        data.insert(data.end(), blockData.begin(), blockData.end());
    }
    
    //writeToFile(data);
    //writeToFile2(data);
    cout << "Writing To File" << endl;
    writeToFile3(data);
    */
    
    vector <bool> data = ChanY.writeToDataArray();
    writeToFileY(data, ChanY);
    
    
    
}


void printVectorr(vector<bool> vec){
    for (int i = 0; i < vec.size(); i++) {
        cout << vec[i];
    }
    cout << endl;
}


void write16(std::ofstream& file, uint16_t value) {
    file.put(static_cast<char>((value >> 8) & 0xFF));
    file.put(static_cast<char>(value & 0xFF));
}


// Function to write an 8-bit value
void write8(std::ofstream& file, uint8_t value) {
    file.put(static_cast<char>(value));
}



void CompressorJPG::writeComponents(ofstream &file, vector<bool> data){
    bitset<8> buffer(0);
    int i;
    printVectorr(data);
    for (i = 0; i < data.size(); i++) {
        buffer[7-(i%8)] = data[i];
        
        if (i != 0 && (i + 1) % 8 == 0) {
            cout << "Buffer: " << buffer << endl;
            file.put(static_cast<char>((uint8_t)buffer.to_ulong()));
            if (buffer.all()) {
                file.put(0);
            }
        }
        
    }
    
    if ((i+1) % 8 != 0) {
        int bitsLeft = 8 - (i%8);
        for (int i = 0; i < bitsLeft; i++) {
            buffer[i] = 1;
        }
        cout << "Buffer: " << buffer << endl;
        file.put(static_cast<char>((uint8_t)buffer.to_ulong()));
    }
}


void CompressorJPG::writeToFileY(vector<bool> data, Channel& chanY){
    std::ofstream out("output3.jpeg", std::ios::binary);
    
    write16(out, 0xFFD8);
    
    write16(out, 0xFFE0);
    write16(out, 16);
    out.write("JFIF", 5);
    write8(out, 1); // Version
    write8(out, 1); //unsure of this
    write8(out, 1); // Units
    write16(out, 1); // X density
    write16(out, 1); // Y density
    write8(out, 0); // X thumbnail
    write8(out, 0); // Y thumbnail
    
    // DQT (Define Quantization Table)
    write16(out, 0xFFDB);
    write16(out, 67); // Length
    write8(out, 0); // Precision and identifier
    
    out.write(reinterpret_cast<const char*>(quantTableY), 64);
    
    
    // SOF0 (Start of Frame)
    write16(out, 0xFFC0);
    write16(out, 11); // Length
    write8(out, 8); // Precision
    write16(out, 512);
    write16(out, 512);
    write8(out, 1); // Number of components
    write8(out, 1); // Component identifier
    write8(out, 0x11); // Sampling factors
    write8(out, 0); // Quantization table selector
    
    // DHT (Define Huffman Tables) for DC
    write16(out, 0xFFC4);
    write16(out, 19+ chanY.codesDC.size()); // Length
    write8(out, 0x00); // Table class and identifier
    
    
    uint8_t codesLengthMapDC[16] = {};
    for (int i = 0; i < chanY.codesDC.size(); i++) {
        codesLengthMapDC[chanY.codesDC[i].branchDepth-1]++;
    }
    
    
    out.write(reinterpret_cast<const char*>(codesLengthMapDC), 16);
    // Values for each code (example values)
    const int sizeDC = (const int)chanY.codesDC.size();
    uint8_t dcValues[sizeDC];
    for (int i = 0; i < sizeDC; i++) {
        dcValues[i] = (uint8_t) chanY.codesDC[i].val;
    }
    out.write(reinterpret_cast<const char*>(dcValues), sizeDC);
    
    
    
    // DHT (Define Huffman Tables) for AC
    write16(out, 0xFFC4);
    write16(out, 19 + chanY.codesAC.size()); // Length
    write8(out, 0x10); // Table class and identifier
    
    
    uint8_t codesLengthMapAC[16] = {};
    for (int i = 0; i < chanY.codesAC.size(); i++) {
        codesLengthMapAC[chanY.codesAC[i].branchDepth-1]++;
    }
    
    
   out.write(reinterpret_cast<const char*>(codesLengthMapAC), 16);
    // Values for each code example values)
    const int sizeAC = (const int)chanY.codesAC.size();
    uint8_t acValues[sizeAC];
    for (int i = 0; i < sizeAC; i++) {
        acValues[i] = (uint8_t)chanY.codesAC[i].huffmanSym.to_ulong();
    }
    out.write(reinterpret_cast<const char*>(acValues), sizeAC);
    
   
    
    // SOS (Start of Scan)
    write16(out, 0xFFDA);
    write16(out, 8); // Length
    write8(out, 1); // Number of components
    write8(out, 1); // Component identifier
    write8(out, 0); // Huffman table selectors
    write8(out, 0); // Start of spectral selection
    write8(out, 63); // End of spectral selection
    write8(out, 0); // Successive approximation
    
    
    
    writeComponents(out, data);
    
    // EOI (End of Image)
    write16(out, 0xFFD9);
    
    
    out.close();
    
    
    
}




