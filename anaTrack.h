// This is a header file for anaTrack class. The class contains the analysis for 
// a single track containing 8 hits. These parameters are determined
//   - possible particle track (slope and y interception)
//   - average drift velocty of the 8 hits
// Name : C. Lin

#ifndef ANATRACK_H
#define ANATRACK_H

struct hit
{
    // Drift time
    double TDC;

    // Wire coordinates. (0, 0) corresponds to wire address (0, 0)
    double x;
    double y;
};

struct track
{
    // Each track contains 8 hits
    hit hitData[8];
};

class anaTrack
{
private:
    int hitsPerTrack;
    track trackData;
    double m, c;     // track path : y = m*x + c
    double avgDriftVelocity;
    bool vetoTrack;
public:
    anaTrack(int n); // n = hitsPerTrack
    void receiveTrackData(track data);
    void calcTrack();
    void outputData();
    void checkTrack();

    bool trackRejected();

    double returnSlope();
    double returnInterception();
    double returnAvgDriftVelocity();


    //void calcTrack_test();
    //double evaluateTrack(double slope, double interception);

    // return slope and y-interception
};


#endif