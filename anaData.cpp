// Main programme for STC analysis exercise
// Name : C. Lin
// The programme reads in 16 bytes data at once representing a detection event.
// The data is then analysed and a possible track formula is determined.
// The average drift velocity, and mean and var of beam angle are then calculated
// and displayed at the end.

#include <iostream>
#include <cmath>
#include <fstream>
#include <iomanip>
#include "anaTrack.h"
#include <chrono>
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
    bool outputData;
    cout << "Output data? (Yes:1/No:0) ";
    cin >> outputData;
auto start = chrono::high_resolution_clock::now();  //Start timer

    //ifstream fin("onetrack.raw", std::ifstream::binary);
    ifstream fin("manytracks.raw", std::ifstream::binary);


    int trackNum = 0;               // Number of tracks taken from data set
    uint16_t dataChunck;            // 16 byte data chunck storing one track
    double TDC_tmp, x_tmp, y_tmp;   // tmp values 
    int i;                          // index
    track trackData;                // One track contains 8 hit events (struct in anaTrack.h)
    double m, c;                    // Track slope and y-interception
    double goodHitNum = 0.;         // Number of hits with reasonable drift velocity
    double avgDriftV = 0.;          // Average drift velocity (um/ns)
    double avgAng = 0.;             // Average beam angle     (deg)
    double varAng = 0.;             // Beam angle variance
    double newAvgDriftV;            // For updating avg velocity
    double tmpA;                    // For updating var(ang)
    double ang;                     // For updating avg angle

    int xxx = 0;

    // while input and process the first of the eight hits, loop for the seven rest
    while( fin.read(reinterpret_cast<char*>(&dataChunck), sizeof (dataChunck)) ){
        xxx ++;

        TDC_tmp = 0.;
        x_tmp = 0.;
        y_tmp = 0.;
        for (i = 0; i <= 2; i++) x_tmp += double((dataChunck >> i) & 1) * twoPower[i];
        for (i = 3; i <= 5; i++) y_tmp += double((dataChunck >> i) & 1) * twoPower[i-3];
        for (i = 6; i < 16; i++) TDC_tmp += double((dataChunck >> i) & 1) * twoPower[i-6];

        trackData.hitData[0].TDC = TDC_tmp;
        trackData.hitData[0].x = x_tmp;
        trackData.hitData[0].y = y_tmp;

        trackData.hitData[0].TDC = 0.5 * TDC_tmp + 0.25;  // convert to midpoint in ns (0 corresponds to 0.0 - 0.5ns)
        trackData.hitData[0].x = x_tmp;
        // trackData.hitData[0].y = y_tmp += double ( int (x_tmp) % 2 ) * 0.5; // y offset not applied for x = 0 layer

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
        
        //for (int i = 0; i < 8; i++) cout << " (" << fixed << setprecision(2) << trackData.hitData[i].x << ", " << fixed << setprecision(2) << trackData.hitData[i].y << ")   " << setw(5) << trackData.hitData[i].TDC << endl;
    

        // Start analysis
        anaTrack newTrackData(hitsPerTrack);
        newTrackData.receiveTrackData(trackData);
        newTrackData.calcTrack();

        m = newTrackData.returnSlope();
        c = newTrackData.returnInterception();
        
        // If the track is good enough
        if ( newTrackData.trackRejected() == 0){
            // Update average average velocity
            newAvgDriftV = newTrackData.returnAvgDriftVelocity();
            ang = atan(m) * 180.0 / M_PI;
            avgDriftV = ( (avgDriftV * goodHitNum) + ( newAvgDriftV * double (hitsPerTrack)) ) / (goodHitNum + double (hitsPerTrack));

            // Update beam angle average
            tmpA = avgAng;
            avgAng = ( (avgAng * goodHitNum / 8.0) + ( ang * 1.0 ) ) / ( goodHitNum / 8.0 + 1);
            
            // Update beam angle variance
            if (goodHitNum /8.0 == 1.) varAng = 0.5 * (tmpA - ang) * (tmpA - ang);
            else varAng = ( (goodHitNum / 8.0 - 1.) * varAng + ( ang - avgAng ) * ( ang - tmpA )  ) / (goodHitNum / 8.0);

            goodHitNum += double (hitsPerTrack);
        }

        trackNum ++;
        //cout << "Track " << trackNum << ": y = " << fixed << setprecision(5) << m << "x + " << c << endl;
        
        //newTrackData.calcTrack_test();
        if (outputData) newTrackData.outputData();

    }
    // Print summary
    cout << "#---------------------------------------------#" << endl;
    cout << "         avg v = " << avgDriftV << " um/ns" << endl;
    cout << "#---------------------------------------------#" << endl;
    cout << "       avg ang = " << avgAng << " deg" << endl;
    cout << "     sigma ang = " << sqrt(varAng) << " deg" << endl;
    cout << "#---------------------------------------------#" << endl;
    cout << "   Good tracks = (" << int(goodHitNum / 8.0) << "/" << trackNum << ") = " << goodHitNum / 8.0 / double(trackNum) * 100.0 << "%" << endl;
    cout << "#---------------------------------------------#" << endl;
auto finish = chrono::high_resolution_clock::now();
chrono::duration<double> elapsed = finish - start;
cout << "  Elapsed time = " << elapsed.count() << " s" << endl;
cout << "#---------------------------------------------#" << endl;
    return 0;
}