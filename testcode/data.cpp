#include <iostream>
#include <fstream>
//#include <sstream>
//#include <bitset>
#include <iomanip>
//#include <chrono>
using namespace std;

double twoPower[10] = {1.0,
					   2.0,
					   4.0,
					   8.0,
					   16.0,
					   32.0,
					   64.0,
					   128.0,
					   256.0,
					   512.0};

struct hit
{
	double TDC;
	int wire_x;
	int wire_y;
};

struct track
{
	hit hits[8];
};

int main()
{
    uint16_t n;
	ifstream fin("onetrack.raw", std::ifstream::binary);

	int h = 0;
	double TDC_tmp, x_tmp, y_tmp;

	//hit hits[8];
    while( fin.read(reinterpret_cast<char*>(&n), sizeof (n)) ){
    	//bitset<16> x(n);
    	//cout << x << endl;

    	TDC_tmp = 0.;
		x_tmp = 0.;
		y_tmp = 0.;
    	int i;
    	for (i = 0; i <= 2; i++) {
    		cout << double((n >> i) & 1);
    		x_tmp += double((n >> i) & 1) * twoPower[i];
    		
    	} cout << " ";
    	for (i = 3; i <= 5; i++) {
    		cout << double((n >> i) & 1);
    		y_tmp += double((n >> i) & 1) * twoPower[i-3];
    	} cout << " ";
    	for (i = 6; i < 16; i++) {
    		cout << double((n >> i) & 1);
    		TDC_tmp += double((n >> i) & 1) * twoPower[i-6];
    	} cout << " ";
    	cout << endl;
    	cout << setw (2) << ++h << " : (" << x_tmp << "," << y_tmp << ")   " << setw(5) << TDC_tmp << endl << endl;
    }
	return 0;
}
