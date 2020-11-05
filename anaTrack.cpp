// This is the implementation file for anaTrack.h
// Name : C. Lin
#include <iostream>
#include <string>
#include <cmath>
#include <fstream>
#include <algorithm>

// user
#include "anaTrack.h"
using namespace std;

// Various precisions to choose from. 
// By using a smaller number, you trade computing time for better accuracy.
// Numbers no larger than 0.001 are recommended.

//#define ASmallNum      0.0001
//#define ASmallNum      0.00005
#define ASmallNum      0.000015
//#define ASmallNum      0.00001

//ofstream fout("fitData.dat", std::ios_base::app); // .dat file storing output data
double v[8];  // drift velocities

//constructor
anaTrack::anaTrack() {}

bool approxEqual(double a, double b)
{
    if (abs(a - b) < ASmallNum * b) return 1;
    else return 0;
}

// Rreturn distance from point (x, y) to line y = mx + c
double dist(double x, double y, double m, double c)
{
    double xx = c + m * x - y;
    return sqrt( xx * xx / (m * m + 1) ); 
}

// Apply some cuts
void anaTrack::checkTrack(track &data)
{
    beamAngle = atan(m) * 180.0 / M_PI;
    sum_v = 0.;
    for (int i = 0; i < 8; i++){
        v[i] = dist(data.hitData[i].x, data.hitData[i].y, m, c) * 10000.0 / data.hitData[i].TDC; // um/ns
        if (v[i] > 60.5 || v[i] < 44.5) { // cut out weird velocity. Limits are strick because the peak is sharp.
            vetoTrack = 1;
            return;
        }
        sum_v += v[i];
    }
    avgDriftVelocity = sum_v / 8.0;
}

// If a track passes through a point P on the line connecting A and B, where A and B 
// are the hit wires, point P divides line segment AB with proportion equal to the distances
// from the hit wires to the track.

// If this track also passes through other (#hit - 1) division points, it is likely that
// this is the track we are looking for. For example, point A is on the left side of the 
// track while the 7 rest are at the right side, then the track should pass through (8 - 1)
// division points.
void anaTrack::calcTrack(track &data)
{
    vetoTrack = 0;
    double x1, y1, t1, t2;
    // Calculate division points
    int ccc = 0;
    for (int i = 0; i < 8 - 1; i++) {
        for (int j = i+1; j < 8; j++) {
            t1 = data.hitData[i].TDC;
            t2 = data.hitData[j].TDC;
            
            divPoints[ccc].x = ( t1*data.hitData[j].x + t2*data.hitData[i].x ) / ( t1 + t2 ); //divPoints[k].x = ( t1*x2 + t2*x1 ) / ( t1 + t2 );
            divPoints[ccc].y = ( t1*data.hitData[j].y + t2*data.hitData[i].y ) / ( t1 + t2 ); //divPoints[k].y = ( t1*y2 + t2*y1 ) / ( t1 + t2 );
            ccc++;
        }
    }

    double m_test, c_test, y_test;
    int score = -999, score_test = 0;
    // Picking pairs of divPoints to form test tracks
    for (int i = 0; i < 27; i ++ ){
        x1 = divPoints[i].x;
        y1 = divPoints[i].y;

        for (int j = i+1; j < 28; j ++){
            // x2 = divPoints[j].x;
            // y2 = divPoints[j].y;
            m_test = (y1 - divPoints[j].y) / (x1 - divPoints[j].x);
            c_test = y1 - m_test * x1;

            score_test = 0;
            for (int k = 0; k < 28; k++) {
                // y_test = m_test * divPoints[k].x + c_test;
                // if (approxEqual(y_test, divPoints[k].y)) score_test ++;
                if (approxEqual(m_test * divPoints[k].x + c_test, divPoints[k].y)) score_test ++;
            }
            if (score_test > score) {
                score = score_test;
                m = m_test;
                c = c_test;
            }
            if (score >= 7) break;
        }
        if (score >= 7) break;
    }
    checkTrack(data); // Check velocities
}

void anaTrack::outputData()
{
    if ( vetoTrack ) return;
    for (int i = 0; i < 8; i++) fout << beamAngle << " " << v[i] << endl; // [drift velo] = um/ns
}

double anaTrack::returnSlope() {return m;}
double anaTrack::returnInterception() {return c;}
double anaTrack::returnAvgDriftVelocity() {return avgDriftVelocity;}
double anaTrack::returnBeamAngle() {return beamAngle;}
bool anaTrack::trackRejected() {return vetoTrack;}



// Test : Connecting points (0, y1) and (7, y2) form a straight line. We know that signals 
// are collected by the nearest wires, so we only need to search for y1 y2 in the range 
// [Y+0.5, Y-0.5], where Y is the wire address. 
// This transforms the task into a search in a 2D plane (ie y1 and y2), where the goal is
// to find the best combination of (y1, y2) that fits the TDC data provided the best.
// Use Monte Carlo method??

// Nothing to see below
/*
double anaTrack::evaluateTrack(double slope, double interception)
{
    double assumedV;
    double min = 999., max = -1.;
    for (int x = 0; x < 8; x ++) {

        assumedV = dist(trackData.hitData[x].x, trackData.hitData[x].y, slope, interception) * 10000.0 / trackData.hitData[x].TDC;
        
        min = (min<assumedV?min:assumedV);
        max = (max>assumedV?max:assumedV);
    }
    return max - min;
}

// Experiment
void anaTrack::calcTrack_test()
{
    ofstream file("map.dat");
    double step = 0.1;
    double slope;
    double interception;
    for (double y1 = trackData.hitData[0].y - 0.5; y1 <= trackData.hitData[0].y + 0.5 ; y1 += step) {
        for (double y2 = trackData.hitData[7].y - 0.5; y2 <= trackData.hitData[7].y + 0.5; y2 += step) {
            slope = (y1 - y2) / (0.0 - 7.0);
            interception = y1;

            file << y1 << " " << y2 << " " << evaluateTrack(slope, interception) << endl;
        }
    }
}

*/
