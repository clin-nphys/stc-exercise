// This is the implementation file for anaTrack.h
// Name : C. Lin

#include <iostream>
#include <string>
#include <cmath>
#include <fstream>
#include <algorithm>
#include "anaTrack.h"
using namespace std;


// Various precisions to choose from. Numbers no smaller than 0.001 are recommended
//#define ASmallNum      0.1
//#define ASmallNum      0.01
#define ASmallNum      0.001
//define ASmallNum      0.0001
//#define ASmallNum      0.00001

ofstream fout("fitData.dat", std::ios_base::app); // .dat file storing output data

//constructor
anaTrack::anaTrack(int n)
{
    hitsPerTrack = n;
    vetoTrack = 0;
}
    
double d[8], ang, v[8];  // Distances to track and drift velocities

bool approxEqual(double a, double b)
{
    if (abs(a - b) / b < ASmallNum) return 1;
    else return 0;
}

// Rreturn distance from point (x, y) to line y = mx + c
double dist(double x, double y, double m, double c)
{
    double xx = c + m * x - y;
    return sqrt( xx * xx / (m * m + 1) ); 
}

// Apply some cuts
void anaTrack::checkTrack()
{
    ang = atan(m) * 180.0 / M_PI;;
    if ((ang >= 0.462 && ang <= 0.464) || (ang <= -0.462 && ang >= -0.464)) { // cut out the weird noise
        vetoTrack = 1;
        return;
    }

    for (int i = 0; i < hitsPerTrack; i++){
        d[i] = dist(trackData.hitData[i].x, trackData.hitData[i].y, m, c) * 10000.0; //[d] = um
        v[i] = d[i] / trackData.hitData[i].TDC;
        if (v[i] > 60.5 || v[i] < 44.5) { // cut out weird velocity. Limits are strick because the peak is sharp.
            vetoTrack = 1;
            return;
        }
    }
}

void anaTrack::receiveTrackData(track data)
{
    for (int i = 0; i < hitsPerTrack; i++){
        trackData.hitData[i].x = data.hitData[i].x;
        trackData.hitData[i].y = data.hitData[i].y;
        trackData.hitData[i].TDC = data.hitData[i].TDC;
        //cout << " (" << trackData.hitData[i].x << ", " << trackData.hitData[i].y << ")  " << trackData.hitData[i].TDC << endl;
    }
}

// If a track passes through a point P on the line connecting A and B, where A and B 
// are the hit wires, point P divides line segment AB with proportion equal to the distances
// from the hit wires to the track.

// If this track also passes through other (#hit - 1) division points, it is likely that
// this is the track we are looking for. For example, point A is on the left side of the 
// track while the 7 rest are at the right side, then the track should pass through (8 - 1)
// division points.

// But if the track only passes through small number of division points, we should try to 
// look for tracks formed by (outer) division points ie points on line AB but not in line 
// segment AB with the correct proportion.

void anaTrack::calcTrack()
{
    // divPoints[i][j] = division point on line segment ij. (not hit events)
    // TDC is the distance from point i to the calculated track divided by drift velcoty.
    hit divPoints[hitsPerTrack][hitsPerTrack];
    double x1, y1, t1, x2, y2, t2;

    int i, j, k, l, ii, jj;

    // Calculate division points
    for (i = 0; i < hitsPerTrack - 1; i++) {
        for (j = i+1; j < hitsPerTrack; j++) {
            // Perhaps using trackData.hitData will be faster?
            x1 = trackData.hitData[i].x;
            y1 = trackData.hitData[i].y;
            t1 = trackData.hitData[i].TDC;
            x2 = trackData.hitData[j].x;
            y2 = trackData.hitData[j].y;
            t2 = trackData.hitData[j].TDC;

            divPoints[i][j].x = ( t1*x2 + t2*x1 ) / ( t1 + t2 );
            divPoints[i][j].y = ( t1*y2 + t2*y1 ) / ( t1 + t2 );
        }
    }

    // Picking pairs of divPoints to form test tracks
    double m_test, c_test, y_test;
    int score, score_test = 0;

    // Picking point 1
    for (i = 0; i < hitsPerTrack - 1; i++){
        for (j = i+1; j < hitsPerTrack; j++) {
            x1 = divPoints[i][j].x;
            y1 = divPoints[i][j].y;

            // Picking point 2
            for (k = i; k < hitsPerTrack - 1; k++){
                for (l = (i==k?j:k+1); l < hitsPerTrack; l++){
                    if (i == k && j == l) continue;
                    x2 = divPoints[k][l].x;
                    y2 = divPoints[k][l].y;

                    // Calculate test track
                    m_test = (y1 - y2) / (x1 - x2);
                    c_test = y1 - m_test * x1;

                    // Count score. score_test++ if track comes near enough to a division point
                    score_test = 0;
                    for (ii = 0; ii < hitsPerTrack - 1; ii++){
                        for (jj = ii+1; jj < hitsPerTrack; jj++){
                            y_test = m_test * divPoints[ii][jj].x + c_test;
                            if (approxEqual(y_test, divPoints[ii][jj].y)) score_test ++;
                        }
                    }
                    // update best score
                    if (score_test > score) {
                        score = score_test;
                        m = m_test;
                        c = c_test;
                    }
                    if (score >= hitsPerTrack -1) break;  // If found a good enough track then terminate early
                }
                if (score >= hitsPerTrack -1) break;  // If found a good enough track then terminate early
            }
            if (score >= hitsPerTrack -1) break;  // If found a good enough track then terminate early
        }
        if (score >= hitsPerTrack -1) break;  // If found a good enough track then terminate early
    }

// C. Lin later found the "all points on one side" case can just be ignored
    //cout << "score before outer divPoints = " << score << endl;
    // After scanning through all trakcs formed by (inner) division points and the best score
    // is still too small, it is likely that all hit points are all at one side of the track.
    // In this case, we will consider outer division points.
    
    /*
    if (score <= 3) {
        x1 = (trackData.hitData[0].x * trackData.hitData[7].TDC - trackData.hitData[7].x * trackData.hitData[0].TDC) / (trackData.hitData[7].TDC - trackData.hitData[0].TDC);
        y1 = (trackData.hitData[0].y * trackData.hitData[7].TDC - trackData.hitData[7].y * trackData.hitData[0].TDC) / (trackData.hitData[7].TDC - trackData.hitData[0].TDC);

        x2 = (trackData.hitData[0].x * trackData.hitData[3].TDC - trackData.hitData[3].x * trackData.hitData[0].TDC) / (trackData.hitData[3].TDC - trackData.hitData[0].TDC);
        y2 = (trackData.hitData[0].y * trackData.hitData[3].TDC - trackData.hitData[3].y * trackData.hitData[0].TDC) / (trackData.hitData[3].TDC - trackData.hitData[0].TDC);

        m = (y1 - y2) / (x1 - x2);
        c = y1 - m_test * x1;
    }
*/
    checkTrack(); // Check various cuts


    // Calculate drift velocity averaged over the 8 hits
    double sum_v = 0;
    if (vetoTrack == 0){    
        for (i = 0; i < hitsPerTrack; i++)
            sum_v += dist(trackData.hitData[i].x, trackData.hitData[i].y, m, c) * 10000.0 / trackData.hitData[i].TDC;
    }
    avgDriftVelocity = sum_v / double(hitsPerTrack);
}

void anaTrack::outputData()
{
    if ( vetoTrack ) return;
    for (int i = 0; i < hitsPerTrack; i++) fout << ang << " " << d[i] << " " << v[i] << endl; // [drift velo] = um/ns
}

double anaTrack::returnSlope() {return m;}

double anaTrack::returnInterception() {return c;}

double anaTrack::returnAvgDriftVelocity() {return avgDriftVelocity;}

bool anaTrack::trackRejected() {return vetoTrack;}


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