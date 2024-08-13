//
//  main.cpp
//  JPEGEncoding
//
//  Created by Norbert Szawloga on 13/07/2024.
//

#include <iostream>
#include "CompressorJPG.hpp"

int main(int argc, const char * argv[]) {
    // insert code here...
    
    
    if (argc == 1) {
        cout << "Provide the name of the BMP file" << endl;
        return 1;
        
    }else{
        CompressorJPG c;
        if (c.compress(argv[1])) {
            cout << "Image created successfuly" << endl;
        }
        
    }
    
    

    
    return 0;
}
