#ifndef _UTILS_H_
#define _UTILS_H_

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <set>
#include <string>
#include <vector>

using namespace std;

#define SZ(x) int((x).size())
// #define double float

// defined in `utils.cpp`
extern int MAP;
extern int FRAME_ID;
extern int SKIP_FRAMES;

const double pi = std::acos(-1.0);
const double R = 6. / pi;
const double delta_t = 0.02;  // time of one frame
const int END_FRAME = 5 * 60 * 50;
const double INF = 1e5;
const double thin_radius = 0.47;
const double fat_radius = 0.54;

// defined in `main.cpp`
extern vector<vector<double>> g_shortest_thin;
extern vector<vector<double>> g_shortest_fat;

struct Position {
    double x;
    double y;
};

bool equal(double a, double b);
double discount(double x, int maxX, double minRate);
bool isLeagal(int row, int col);
double calc_dist(Position a, Position b);
double direction(Position _stand, Position _toward);
double rotate(double _from, double _to);
double time_pretiction(double toward, double speed, Position _stand, Position _des);
double profit(int goods, double time1, double time2, int frame_id);
vector<int> get_thin_map_indexes(const Position &pos);
vector<int> get_fat_map_indexes(const Position &pos);
Position get_fat_position_from_indexes(int i, int j);
Position get_thin_position_from_indexes(int i, int j);
int indexes_encoding(int i, int j);

#endif