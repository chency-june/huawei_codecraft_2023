#include "graph.h"

extern vector<vector<Edge>> g_matrix;

Vertax::Vertax(Position pos, bool only_for_thin, bool ok_for_2, int only_link1, int only_link2) {
    this->pos = pos;
    this->only_for_thin = only_for_thin;
    this->ok_for_2 = ok_for_2;
    this->only_link1 = only_link1;
    this->only_link2 = only_link2;
}
bool Vertax::operator==(const Vertax& r)  // 把等于符号给重载了
{
    return (this->pos.x == r.pos.x && this->pos.y == r.pos.y);
}
Edge::Edge(int from_id, int to_id, double fat_length, double thin_length, bool only_thin, bool ok_for_2) {
    this->from_id = from_id;
    this->to_id = to_id;
    this->length = thin_length;
    this->only_thin = only_thin || thin_length > fat_length - 1e-5;
    this->ok_for_2 = ok_for_2 && !this->only_thin;
    this->num_booked = 0;
}
void Edge::book() {
    if (this->ok_for_2) {
        return;
    }
    this->num_booked++;
}
bool Edge::isReady() {
    if (this->ok_for_2) {
        return true;
    } else {
        Edge& check_edge = g_matrix[this->to_id][this->from_id];
        if (check_edge.num_booked > 0) {
            return false;
        } else {
            return true;
        }
    }
}
void Edge::unbook() {
    if (this->ok_for_2) {
        return;
    }
    this->num_booked--;
}
double Edge::get_dis(bool fat) {
    if (this->only_thin) {
        if (!fat) {
            return this->length;
        } else {
            return INF;
        }
    } else {
        return this->length;
    }
}