#include "order.h"

Suborder::Suborder(bool tobuy, int start_vertax_id, int target_vertax_id) {
    this->fat = !tobuy;
    this->at_vertax_id = start_vertax_id;
    this->last_vertax_id = start_vertax_id;
    this->des_vertax_id = target_vertax_id;
}
void Suborder::updatePos(int new_vertax_id) { this->at_vertax_id = new_vertax_id; }

Order::Order(int _stand_vertax_id, int _buy_workbench_id, int _sell_workbench_id) {
    this->buy_workbench_id = _buy_workbench_id;
    this->sell_workbench_id = _sell_workbench_id;
    suborders.clear();
    suborders.push_back(Suborder(true, _stand_vertax_id, _buy_workbench_id));
    suborders.push_back(Suborder(false, _buy_workbench_id, _sell_workbench_id));

    bought = false;
    sold = false;
}

bool Order::complete() { return bought && sold; }
