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

public:
    anaTrack();
    void hello();

    // void receiveTrackData(track data);

    // return slope and y-interception
};


#endif