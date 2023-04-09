#include "utils.h"

int MAP = 0;
int FRAME_ID = 0;
int SKIP_FRAMES = 1;

bool equal(double a, double b) { return abs(a - b) < 1e-4; }
double discount(double x, int maxX, double minRate) {
    if (x < maxX)
        return (1. - sqrt(1 - (1 - x / maxX) * (1 - x / maxX))) * (1. - minRate) + minRate;
    else
        return minRate;
}
bool isLeagal(int row, int col) { return (row >= 0 && row < 100 && col >= 0 && col < 100); }
double calc_dist(Position a, Position b) {
    double delta_x = a.x - b.x;
    double delta_y = a.y - b.y;
    return sqrt(delta_x * delta_x + delta_y * delta_y);
}
double direction(Position _stand, Position _toward) { return atan2(_toward.y - _stand.y, _toward.x - _stand.x); }
double rotate(double _from, double _to) {
    // make sure that the direction difference is in half a circle
    if (abs(_to - _from) > pi) {
        if (_to + 2 * pi - _from <= pi)
            _to += 2 * pi;
        else
            _to -= 2 * pi;
    }
    assert(abs(_to - _from) <= pi);
    return _to - _from;
}
double time_pretiction(double toward, double speed, Position _stand, Position _des) {
    double rotate0 = rotate(toward, direction(_stand, _des));
    rotate0 = max(0.0, abs(rotate0) - pi / 2);
    if (rotate0 > 0) speed = 0;
    double t0 = rotate0 > 0 ? rotate0 / pi + 0.04 : 0;
    double distance = calc_dist(_stand, _des);
    double t1 = distance / 6 + 0.1;
    t1 -= speed / 16 - speed * speed / (2 * 16) / 6;
    return t0 + t1;
}
double profit(int goods, double time1, double time2, int frame_id) {
    vector<double> buy_price = {-1, 3000.0, 4400.0, 5800.0, 15400.0, 17200.0, 19200.0, 76000.0};
    vector<double> sell_price = {-1, 6000.0, 7600.0, 9200.0, 22500.0, 25000.0, 27500.0, 105000.0};
    double total_time = time1 + time2;
    if (frame_id + total_time * 50 + 50 > END_FRAME) return -1;
    return discount(time2 * 50, 9000, 0.8) * sell_price[goods] - buy_price[goods];
}
vector<int> get_thin_map_indexes(const Position &pos) {
    int i = int((49.75 - pos.y) / 0.5);
    int j = int((pos.x - 0.25) / 0.5);
    return {i, j};
}
vector<int> get_fat_map_indexes(const Position &pos) {
    int i = int((49.5 - pos.y) / 0.5);
    int j = int((pos.x - 0.5) / 0.5);
    return {i, j};
}
Position get_fat_position_from_indexes(int i, int j) { return {j * 0.5 + 0.75, (97 - i) * 0.5 + 0.75}; }
Position get_thin_position_from_indexes(int i, int j) { return {j * 0.5 + 0.5, (98 - i) * 0.5 + 0.5}; }
int indexes_encoding(int i, int j) {
    return (i << 16) + j;
}