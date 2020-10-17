#include <iostream>
#include <fstream>
#include "anaTrack.h"
using namespace std;

// some constants/parameters
const double cellWidth    = 1.0;  //cm
const int hitsPerTrack    = 8;
const double twoPower[10] = {1.0,
                             2.0,
                             4.0,
                             8.0,
                             16.0,
                             32.0,
                             64.0,
                             128.0,
                             256.0,
                             512.0};



int main()
{
    ifstream fin("onetrack.raw", std::ifstream::binary);


    int trackNum = 0;
    uint16_t dataChunck; //16 byte data chunck storing one track
    double TDC_tmp, x_tmp, y_tmp;
    int i;
    track trackData;

    // while input and process the first of the eight hits, loop for the seven rest
    while( fin.read(reinterpret_cast<char*>(&dataChunck), sizeof (dataChunck)) ){
        TDC_tmp = 0.;
        x_tmp = 0.;
        y_tmp = 0.;
        for (i = 0; i <= 2; i++) x_tmp += double((dataChunck >> i) & 1) * twoPower[i];
        for (i = 3; i <= 5; i++) y_tmp += double((dataChunck >> i) & 1) * twoPower[i-3];
        for (i = 6; i < 16; i++) TDC_tmp += double((dataChunck >> i) & 1) * twoPower[i-6];
        y_tmp += double ( int (x_tmp) % 2 ) * 0.5; // account for the y offset of true coordinate

        trackData.hitData[0].TDC = TDC_tmp;
        trackData.hitData[0].x = x_tmp;
        trackData.hitData[0].y = y_tmp;

        // loop for the seven remaining hits
        for (int j = 1; j < hitsPerTrack; j++) { 
            fin.read(reinterpret_cast<char*>(&dataChunck), sizeof (dataChunck));
            TDC_tmp = 0.;
            x_tmp = 0.;
            y_tmp = 0.;
            for (i = 0; i <= 2; i++) x_tmp += double((dataChunck >> i) & 1) * twoPower[i];
            for (i = 3; i <= 5; i++) y_tmp += double((dataChunck >> i) & 1) * twoPower[i-3];
            for (i = 6; i < 16; i++) TDC_tmp += double((dataChunck >> i) & 1) * twoPower[i-6];

            trackData.hitData[j].TDC = 0.5 * TDC_tmp + 0.25;  // convert to midpoint in ns (0 corresponds to 0.0 - 0.5ns)
            trackData.hitData[j].x = x_tmp;
            trackData.hitData[j].y = y_tmp += double ( int (x_tmp) % 2 ) * 0.5; // account for the y offset in cm
        }
        trackNum ++;

        for (int i = 0; i < 8; i++){
            cout << trackData.hitData[i].x << " " << trackData.hitData[i].y << " " << trackData.hitData[i].TDC << endl;
        }
    }
    return 0;
}