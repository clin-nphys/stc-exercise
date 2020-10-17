#include <iostream>
#include <string>
#include "anaTrack.h"
using namespace std;

#define ASmallNum      0.05

//constructor
anaTrack::anaTrack(int n)
{
    hitsPerTrack = n;
}


bool approxEqual(double a, double b)
{
    if (abs(a - b) / b < ASmallNum) return 1;
    else return 0;
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
// this is the track we are looking for. But if the track only passes through small number
// of division points, we should try to look for tracks formed by (outer) division points ie
// points on line AB but not in line segment AB with the correct proportion.

void anaTrack::calcPath()
{
    // divPoints[i][j] = division point on line segment ij. (not hit events)
    // TDC is the distance from point i to the calculated track divided by drift velcoty.
    hit divPoints[hitsPerTrack][hitsPerTrack];
    double x1, y1, t1, x2, y2, t2;

    int i, j, k, l, ii, jj;

    // Calculate division points
    for (i = 0; i < hitsPerTrack - 1; i++) {
        for (j = i+1; j < hitsPerTrack; j++) {
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

    // Picking pairs of divPoints to for test tracks
    double m_test, c_test, y_test;
    int score, score_test = 0;

    for (i = 0; i < hitsPerTrack - 1; i++){
        for (j = i+1; j < hitsPerTrack; j++) {
            x1 = divPoints[i][j].x;
            y1 = divPoints[i][j].y;

            for (k = i; k < hitsPerTrack - 1; k++){
                for (l = (i==k?j:k+1); l < hitsPerTrack; l++){
                    if (i == k && j == l) continue;
                    x2 = divPoints[k][l].x;
                    y2 = divPoints[k][l].y;

                    m_test = (y1 - y2) / (x1 - x2);
                    c_test = y1 - m_test * x1;

                    // count score
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

                    if (score >= hitsPerTrack -1) break;  // This means there is at least one point on the other side of the track
                }
                if (score >= hitsPerTrack -1) break;
            }
            if (score >= hitsPerTrack -1) break;
        }
        if (score >= hitsPerTrack -1) break;
    }

    //cout << "score before outer divPoints = " << score << endl;
    // After scanning through all trakcs formed by (inner) division points and the best score
    // is still too small, it is likely that all hit points are all at one side of the track.
    // In this case, we will consider outer division points.
    if (score <= 3) {
        x1 = (trackData.hitData[0].x * trackData.hitData[7].TDC - trackData.hitData[7].x * trackData.hitData[0].TDC) / (trackData.hitData[7].TDC - trackData.hitData[0].TDC);
        y1 = (trackData.hitData[0].y * trackData.hitData[7].TDC - trackData.hitData[7].y * trackData.hitData[0].TDC) / (trackData.hitData[7].TDC - trackData.hitData[0].TDC);

        x2 = (trackData.hitData[0].x * trackData.hitData[3].TDC - trackData.hitData[3].x * trackData.hitData[0].TDC) / (trackData.hitData[3].TDC - trackData.hitData[0].TDC);
        y2 = (trackData.hitData[0].y * trackData.hitData[3].TDC - trackData.hitData[3].y * trackData.hitData[0].TDC) / (trackData.hitData[3].TDC - trackData.hitData[0].TDC);

        m = (y1 - y2) / (x1 - x2);
        c = y1 - m_test * x1;
    }
}

double anaTrack::returnSlope()
{
    return m;
}

double anaTrack::returnInterception()
{
    return c;
}