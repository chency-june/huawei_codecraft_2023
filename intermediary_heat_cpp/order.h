#ifndef _ORDER_H_
#define _ORDER_H_

#include "utils.h"
#include "graph.h"

extern vector<Vertax> g_vertaxes;
extern int num_vertaxes;

struct Suborder {
    bool fat;
    int last_vertax_id;
    int at_vertax_id;
    int des_vertax_id;
    Suborder(bool tobuy, int start_vertax_id, int target_vertax_id);
    void updatePos(int new_vertax_id);
};
struct Order {
    bool bought, sold;
    int buy_workbench_id, sell_workbench_id;
    vector<Suborder> suborders;
    Order(int _stand_vertax_id, int _buy_workbench_id, int _sell_workbench_id);

    bool complete();
};
#endif