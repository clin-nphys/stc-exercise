// Main programme for STC analysis exercise
// Name : C. Lin
// The programme reads in a piece of 16 byte data representing a detection event.
// The data is then analysed and a possible track formula is determined.
// The distributions of drift velocity and beam angle are fitted with Gaussian, and 
// the fitting parameters are displayed.

#include <iostream>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <stdint.h>  // for uint16_t
#include <chrono>

//user
#include "anaTrack.h"

//root
#include "TH1F.h"
#include "TList.h"
#include "TF1.h"
using namespace std;

// some useful tables
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

const double shift[8] = {0.0, 0.5, 0.0, 0.5, 0.0, 0.5, 0.0, 0.5};

int main()
{
    cout << endl;
    cout << "  :: By choosing a different ASmallNum in anaTrack.cpp, you can trade computing" << endl;
    cout << "     time with accuracy." << endl << endl;
auto start = chrono::high_resolution_clock::now();  //Start timer

    //ifstream fin("onetrack.raw", std::ifstream::binary);
    ifstream fin("manytracks.raw", std::ifstream::binary);

    int trackNum = 0;               // Number of tracks taken from data set
    uint16_t dataChunck;            // 16 byte data chunck storing one track
    double TDC_tmp, x_tmp, y_tmp;   // tmp values 
    int i;                          // index
    track trackData;                // One track contains 8 hit events (struct in anaTrack.h)
    anaTrack newTrackData;
    double m, c;                    // Track slope and y-interception
    double goodHitNum = 0.;         // Number of hits with reasonable drift velocity
    // variables for storing fitting parameters
    double avgDriftV, sigmaDriftV, avgAng, sigmaAng;
    double avgDriftV_err, sigmaDriftV_err, avgAng_err, sigmaAng_err;
    double newAvgDriftV;            // For updating avg velocity
    double newBeamAngle;            // For updating avg angle

    // Histogrammes
    TH1F *histVelo = new TH1F("hVelo", "Drift velocity distribution", 100, 52.6 - 2.0, 52.6 + 2.0);
    TH1F *histAng = new TH1F("hAng", "Beam angle distribution", 100, 6.2 - 20., 6.2 + 20.);

    // while input and process the first of the eight hits, loop for the seven rest
    while( fin.read(reinterpret_cast<char*>(&dataChunck), sizeof (dataChunck)) ){
        TDC_tmp = 0.;
        x_tmp = 0.;
        y_tmp = 0.;
        for (i = 0; i <= 2; i++) x_tmp += ((dataChunck >> i) & 1) * twoPower[i];
        for (i = 3; i <= 5; i++) y_tmp += ((dataChunck >> i) & 1) * twoPower[i-3];
        for (i = 6; i < 16; i++) TDC_tmp += ((dataChunck >> i) & 1) * twoPower[i-6];

        trackData.hitData[0].TDC = 0.5 * TDC_tmp + 0.25;  // convert to midpoint in ns (0 corresponds to 0.0 - 0.5ns)
        trackData.hitData[0].x = x_tmp;
        trackData.hitData[0].y = y_tmp; // y offset not applied for x = 0 layer

        // loop for the seven remaining hits
        for (int j = 1; j < 8; j++) { 
            fin.read(reinterpret_cast<char*>(&dataChunck), sizeof (dataChunck));
            TDC_tmp = 0.;
            x_tmp = 0.;
            y_tmp = 0.;
            for (i = 0; i <= 2; i++) x_tmp += ((dataChunck >> i) & 1) * twoPower[i];
            for (i = 3; i <= 5; i++) y_tmp += ((dataChunck >> i) & 1) * twoPower[i-3];
            for (i = 6; i < 16; i++) TDC_tmp += ((dataChunck >> i) & 1) * twoPower[i-6];

            trackData.hitData[j].TDC = 0.5 * TDC_tmp + 0.25;  // convert to midpoint in ns (0 corresponds to 0.0 - 0.5ns)
            trackData.hitData[j].x = x_tmp;
            trackData.hitData[j].y = y_tmp + shift[j]; // account for the y offset in cm
        }
        // for (i = 0; i < 8; i++) cout << " (" << fixed << setprecision(2) << trackData.hitData[i].x << ", " << fixed << setprecision(2) << trackData.hitData[i].y << ")   " << setw(5) << trackData.hitData[i].TDC << endl;

        // Start analysis        
        newTrackData.calcTrack(trackData);

        // m = newTrackData.returnSlope();
        // c = newTrackData.returnInterception();
        
        // If the track is good enough, fill the histogrammes
        if ( newTrackData.trackRejected() == 0){
            histVelo->Fill( newTrackData.returnAvgDriftVelocity() );
            histAng->Fill( newTrackData.returnBeamAngle() );
            goodHitNum += 8.0;
        }

        trackNum ++;
        //cout << "Track " << trackNum << ": y = " << fixed << setprecision(5) << m << "x + " << c << endl;
        
        // newTrackData.calcTrack_test();
        // newTrackData.outputData();

    }

    // ROOT fitting
    histVelo->Fit("gausn", "q");
    histAng->Fit("gausn", "q");

    TF1 *fitVelo = (TF1*)histVelo->GetListOfFunctions()->FindObject("gausn");
    TF1 *fitAng = (TF1*)histAng->GetListOfFunctions()->FindObject("gausn");

    avgDriftV = fitVelo->GetParameter(1);
    avgDriftV_err = fitVelo->GetParError(1);
    sigmaDriftV = fitVelo->GetParameter(2);
    sigmaDriftV_err = fitVelo->GetParError(2);

    avgAng = fitAng->GetParameter(1);
    avgAng_err = fitAng->GetParError(1);
    sigmaAng = fitAng->GetParameter(2);
    sigmaAng_err = fitAng->GetParError(2);
    
    // Print summary
    cout << endl;
    cout << "#---------------------------------------------------------#" << endl;
    cout << setw(15) << "avg v" << " = " << setw(10) << avgDriftV << " +- " << avgDriftV_err << " um/ns" << endl;
    cout << setw(15) << "sigma v" << " = " << setw(10) << sigmaDriftV << " +- " << sigmaDriftV_err << " um/ns" << endl;
    cout << "#---------------------------------------------------------#" << endl;
    cout << setw(15) << "avg ang" << " = " << setw(10) << avgAng << " +- " << avgAng_err << " deg" << endl;
    cout << setw(15) << "sigma ang" << " = " << setw(10) << sigmaAng << " +- " << sigmaAng_err << " deg" << endl;
    cout << "#---------------------------------------------------------#" << endl;
    cout << setw(15) << "Good tracks" << " = (" << int(goodHitNum / 8.0) << "/" << trackNum << ") = " << goodHitNum / 8.0 / double(trackNum) * 100.0 << "%" << endl;
    cout << "#---------------------------------------------------------#" << endl;

auto finish = chrono::high_resolution_clock::now(); // stop timer
chrono::duration<double> elapsed = finish - start;
cout << setw(15) << "Elapsed time" << " = " << elapsed.count() << " s" << endl;
cout << "#---------------------------------------------------------#" << endl;
cout << endl;

    fin.close();
    return 0;
}
