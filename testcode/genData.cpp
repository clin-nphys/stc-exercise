#include <iostream>
#include <fstream>
#include <cmath>
#include <time.h>
using namespace std;

double m, c;

double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

double distance(double x, double y)
{
    double xx = c + m * x - y;
    double d = sqrt( xx * xx / (m * m + 1) );
    return d;
}

int main()
{
    srand (time(NULL));
    double x, y, y1, y2;
    ofstream fout("data.txt");
    for (int track = 0; track < 100000; track++) {
        double y1 = fRand(0.0, 10.0);
        double y2 = fRand(0.0, 10.0);

        c = y1;
        m = (y2 - y1) / 10.0;

        double y;
        for (int i = 0; i < 8; i ++) {
            x = fRand(0.0, 10.0);
            y = fRand(0.0, 10.0);
            fout << distance(x, y) << " " << x << " " << y << " ";
        }

        fout << m << " " << c << endl;
    }
    return 0;
}
