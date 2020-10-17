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

//extern track trackData;

class anaTrack
{
private:
    int hitsPerTrack;
    track trackData;
    double m, c;     // track path : y = m*x + c  
public:
    anaTrack(int n); // n = hitsPerTrack
    void receiveTrackData(track data);
    void calcPath();

    double returnSlope();
    double returnInterception();
    // return slope and y-interception
};


#endif