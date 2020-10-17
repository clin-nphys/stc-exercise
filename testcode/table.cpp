#include <iostream>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <fstream>
using namespace std;

#define ASmallNum      0.00001
#define LessSmallNum   0.001
#define dotNum         8
auto start = std::chrono::high_resolution_clock::now();
bool approxEqual(double a, double b)
{
    if (abs(a - b) / b < ASmallNum) return 1;
    else return 0;
}

bool roughEqual(double a, double b)
{
    if (abs(a - b) / b < LessSmallNum) return 1;
    else return 0;
}

struct coord
{
    double x;
    double y;
    double t;
};

coord divs[dotNum][dotNum];
coord dots[dotNum];


int main()
{
    ofstream fout("wronglines.txt");
    double m_ans, c_ans;
    ifstream fin("data.txt");

    int tnum = 0;
    int tcorrect = 0;


    while (fin >> dots[0].t >> dots[0].x >> dots[0].y) {
    tnum++;    
    for (int i = 1; i < dotNum; i++) {
        fin >> dots[i].t >> dots[i].x >> dots[i].y;
    } fin >> m_ans >> c_ans;

    double x1, y1, t1, x2, y2, t2;

    for (int i = 0; i < dotNum - 1; i++){
        for (int j = i+1; j < dotNum; j++){
            x1 = dots[i].x;
            y1 = dots[i].y;
            t1 = dots[i].t;
            x2 = dots[j].x;
            y2 = dots[j].y;
            t2 = dots[j].t;
            divs[i][j].x = ( t1*x2 + t2*x1 ) / ( t1 + t2 );
            //divs[j][i].x = divs[i][j].x;
            divs[i][j].y = ( t1*y2 + t2*y1 ) / ( t1 + t2 );
            //divs[j][i].y = divs[i][j].y;
        }
    }
    // Display div points
    /*
    for (int i = 0; i < 5; i ++) {
        for (int j = 0; j < 5; j++) {
            if (i == j) cout << " ---------------- ";
            else cout << " (" << fixed << setprecision(4) << divs[i][j].x << ", " << divs[i][j].y << ") ";
        }
        cout << endl;
    }
    */
    int pairs = 0;
    int score = 0;
    int score_best = 0;
    double m, m_best; //slope
    double c, c_best; //offset
    double y_calc;
    for (int i = 0; i < dotNum - 1; i++){
        for (int j = i+1; j < dotNum; j++){
            x1 = divs[i][j].x;
            y1 = divs[i][j].y;
            for (int k = i; k < dotNum - 1; k++){
                for (int l = (i==k?j:k+1); l < dotNum; l++){
                    if (i == k && j == l) continue;
                    x2 = divs[k][l].x;
                    y2 = divs[k][l].y;
                    pairs++;
//                    cout << setw(3) << pairs << " : (" << i << "," << j << ")  (" << k << "," << l << ")" << endl;
                    score = 0;
                    m = (y1 - y2) / (x1 - x2);
                    c = y1 - m*x1;
                    //cout << setw(3) << pairs << " : y = " << m << " x + " << c << endl; 

                    // count score
                    for (int ii = 0; ii < dotNum - 1; ii++){
                        for (int jj = ii+1; jj < dotNum; jj++){
                            if (ii == jj) continue;
                            y_calc = m*divs[ii][jj].x + c;
                            if (approxEqual(y_calc, divs[ii][jj].y)) score ++;

                        }
                    }

                    if (score > score_best) {
                        m_best = m;
                        c_best = c;
                        score_best = score;
                    }
                    if (score_best >= dotNum -1) break;
                }
                if (score_best >= dotNum -1) break;
            }
            if (score_best >= dotNum -1) break;
        }
        if (score_best >= dotNum -1) break;
    }

    //cout << "pairs = " << pairs << endl;

    // Possibly all points on one side of the track, pick two pairs and make a line
    if (score_best <= 3) {

        int i = 0;
        int j = 1;
        int k = 2;
        double xa = (dots[i].x * dots[j].t - dots[j].x * dots[i].t) / (dots[j].t - dots[i].t);
        double ya = (dots[i].y * dots[j].t - dots[j].y * dots[i].t) / (dots[j].t - dots[i].t);

        double xb = (dots[i].x * dots[k].t - dots[k].x * dots[i].t) / (dots[k].t - dots[i].t);
        double yb = (dots[i].y * dots[k].t - dots[k].y * dots[i].t) / (dots[k].t - dots[i].t);

        double new_m_best = (ya - yb) / (xa - xb);
        double new_c_best = ya - new_m_best * xa;

        //int score = 10000;
        /*
        for (int ii = 0; ii < dotNum - 1; ii++){
            for (int jj = ii+1; jj < dotNum; jj++){
                if (ii == jj) continue;
                y_calc = new_m_best*divs[ii][jj].x + new_c_best;
                if (approxEqual(y_calc, divs[ii][jj].y)) score ++;

            }
        } */
        //if (score > score_best) {
            //cout << " Change!!" << endl;
            m_best = new_m_best;
            c_best = new_c_best;
        /*} else {
            cout << " Don't change!!" << endl;
            cout << " take : y = " << m_best << "x + " << c_best << "  (" << score_best << ")" << endl;
            cout << "  not : y = " << new_m_best << "x + " << new_c_best << "  (" << score << ")" << endl;
        }*/
    }
    
    
    // Check distances
    /*int correct = 0;
    for (int i = 0; i < dotNum; i++){
        double xx = c_best + m_best * dots[i].x - dots[i].y;
        double d = sqrt( xx * xx / (m_best * m_best + 1) );
        //cout << "  d = " << d / sqrt(5) * 5 << endl;
        //y_calc = m_best*dots[i].x + c_best;
        //if (approxEqual(y_calc, dots[i].y)) correct ++;
    }
    */

    

    //if (roughEqual(m_best, m_ans) && roughEqual(c_best, c_ans)) {
    if (roughEqual(m_best, m_ans) ) {
        tcorrect ++;
    } /*else {
        for (int i = 0; i < dotNum; i++) {
            fout << dots[i].t << " " << dots[i].x << " " << dots[i].y << " ";
        } fout << m_ans << " " << c_ans << " " << m_best << " " << c_best << endl;
    }*/

    //cout << setw(3) << "(" << score_best << ") : y = " << m_best << " x + " << c_best << setw(15) << "(" << tcorrect << "/" << tnum << ")" << endl; 
    //cout << setw(3) << "ANS : y = " << m_ans << " x + " << c_ans << endl; 
    }
    cout << "correct (" << tcorrect << "/" << tnum << ")" << endl; 
auto finish = std::chrono::high_resolution_clock::now();
std::chrono::duration<double> elapsed = finish - start;
std::cout << "Elapsed time: " << elapsed.count() << " s\n";
//cout << elapsed.count() * 1000000 << endl;

    return 0;
}


// 0.00382089