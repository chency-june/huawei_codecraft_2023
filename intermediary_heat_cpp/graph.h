#ifndef _GRAPH_H_
#define _GRAPH_H_

#include "utils.h"

struct Vertax {
    bool only_for_thin = false;
    bool ok_for_2 = false;
    int only_link1 = -1, only_link2 = -1;
    Position pos = {0, 0};
    Vertax(Position pos, bool only_for_thin, bool ok_for_2, int only_link1 = -1, int only_link2 = -1);
    bool operator==(const Vertax &r);
};
struct Edge {
    int from_id = -1;
    int to_id = -1;
    bool only_thin = false;
    bool ok_for_2 = false;
    double length = INF;
    int num_booked = 0;
    Edge(int from_id, int to_id, double fat_length, double thin_length, bool only_thin, bool ok_for_2);
    void book();
    void unbook();
    bool isReady();
    double get_dis(bool fat);
};

#endif