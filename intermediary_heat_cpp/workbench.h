#ifndef _WORKBENCH_H_
#define _WORKBENCH_H_

#include "utils.h"

struct Workbench {
    int workbench_id, workbench_type, book_mask, remaining, buy_mask;
    Position pos;
    bool product;
    vector<int> load_frames = vector<int>();
    vector<int> buys[30] = {{0}, {0}, {0}, {0}, {1, 2}, {1, 3}, {2, 3}, {4, 5, 6}, {7}, {1, 2, 3, 4, 5, 6, 7}};
    Workbench(int workbench_id_i = 0, int workbench_type_i = 0, double x_i = 0, double y_i = 0);
    bool notInBuys(int goods);
    void updateStatus(int workbench_type_i, double x_i, double y_i, int remaining_i, int buy_mask_i, int product_i);
    bool readyToBuy(double frames_later);
    bool readyToSell(int goods, double frames_later = 0.0);
    int sells();
    double bonusRate(int goods);
    double needRate(int goods);
    void preBook(int goods, double frames_later);
    void unbook(int goods);
};

#endif