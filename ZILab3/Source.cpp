#include <iostream>
#include <fstream>
#include <windows.h>

using namespace std;

BITMAPFILEHEADER bmfh;
BITMAPINFOHEADER bmih;
string FILE1 = "1.bmp";
string FILE2 = "2.bmp";
string FILETXT = "3.txt";

bool getInfo() {
    ifstream file(FILE1, ios::binary);

    file.read((char*)&bmfh, sizeof(BITMAPFILEHEADER));
    file.read((char*)&bmih, sizeof(BITMAPINFOHEADER));

    file.close();
}

void encrypt(string str) {
    int countPixel = 0;
    ifstream img(FILE1, ios::binary);
    ofstream img2(FILE2, ios::binary);

    img.seekg(0, ios_base::end);
    RGBQUAD* pixel = new RGBQUAD[(unsigned long int) img.tellg() - bmfh.bfOffBits];
    img.seekg(0, ios_base::beg);
    img.seekg(bmfh.bfOffBits);

    while (img.read((char*)&pixel[countPixel], sizeof(RGBQUAD))) {
        if (countPixel < str.size())
            hide_byte_into_pixel(&pixel[countPixel], str[countPixel]);
        if (countPixel == str.size())
            hide_byte_into_pixel(&pixel[countPixel], 0xFF);
        countPixel++;
    }
    img.close();

    img2.write((char*)&bmfh, sizeof(BITMAPFILEHEADER));
    img2.write((char*)&bmih, sizeof(BITMAPINFOHEADER));
    for (int i = 0; i < countPixel; i++) {
        img2.write((char*)&pixel[i], sizeof(RGBQUAD));
    }
    img2.close();
}

void hide_byte_into_pixel(RGBQUAD* pixel, uint8_t hide_byte)
{
    pixel->rgbBlue &= (0xFC);
    pixel->rgbBlue |= (hide_byte >> 6) & 0x3;
    pixel->rgbGreen &= (0xFC);
    pixel->rgbGreen |= (hide_byte >> 4) & 0x3;
    pixel->rgbRed &= (0xFC);
    pixel->rgbRed |= (hide_byte >> 2) & 0x3;
    pixel->rgbReserved &= (0xFC);
    pixel->rgbReserved |= (hide_byte) & 0x3;
}

void decrypt() {
    int countPixel = 0;


    ifstream img(FILE2, ios::binary);

    img.seekg(0, ios_base::end);
    int count = (unsigned long int) img.tellg() - bmfh.bfOffBits;
    RGBQUAD* pixel = new RGBQUAD[count];
    char* ch = new char[count];
    img.seekg(0, ios_base::beg);
    img.seekg(bmfh.bfOffBits);

    while (img.read((char*)&pixel[countPixel], sizeof(RGBQUAD))) {
        ch[countPixel] = read_byte_into_pixel(&pixel[countPixel]);
        countPixel++;
        if (0xFF == ch[countPixel - 1])
            break;
    }

    img.close();

    ofstream file(FILETXT, ios::binary);
    for (int i = 0; i < countPixel; i++)
        file << ch[i];
}

char read_byte_into_pixel(RGBQUAD* pixel) {
    char ch = 0;
    ch = pixel->rgbBlue & 3;
    ch <<= 2;
    ch |= pixel->rgbGreen & 3;
    ch <<= 2;
    ch |= pixel->rgbRed & 3;
    ch <<= 2;
    ch |= pixel->rgbReserved & 3;
    ch <<= 2;
    return ch;
}

int main() {
    setlocale(LC_ALL, "rus");
    string str = "abcd";

    getInfo();
    encrypt(str);
    decrypt();

}