#include "workbench.h"

#include "utils.h"

Workbench::Workbench(int workbench_id_i, int workbench_type_i, double x_i, double y_i) {
    workbench_id = workbench_id_i;      // unique id [0, 49]
    workbench_type = workbench_type_i;  // type [1, 9]
    pos = {x_i, y_i};          // the position
    book_mask = 0;
}
bool Workbench::notInBuys(int goods) {
    vector<int> tmp = buys[workbench_type];
    bool notIn = true;
    for (int x : tmp) {
        if (x == goods) {
            notIn = false;
            break;
        }
    }
    return notIn;
}
void Workbench::updateStatus(int workbench_type_i, double x_i, double y_i, int remaining_i, int buy_mask_i,
                             int product_i) {
    assert(workbench_type == workbench_type_i);
    assert(x_i == pos.x);
    assert(y_i == pos.y);
    remaining = remaining_i;
    buy_mask = buy_mask_i;
    product = (product_i == 1 ? true : false);
}
bool Workbench::readyToBuy(double frames_later) {
    if (MAP != 2 && frames_later > 50) {
        for (int load_frame : this->load_frames) {
            if (abs(FRAME_ID + frames_later - load_frame) < 80) {
                return false;
            }
        }
    }
    if (workbench_type == 1 || workbench_type == 2 || workbench_type == 3) return true;
    if (book_mask & 1 != 0) return false;
    if (product) return true;
    return remaining > 0 && frames_later >= remaining;
}
bool Workbench::readyToSell(int goods, double frames_later) {
    if (MAP != 2 && frames_later > 50) {
        for (int load_frame : this->load_frames) {
            if (abs(FRAME_ID + frames_later - load_frame) < 80) {
                return false;
            }
        }
    }
    if (notInBuys(goods)) return false;
    if (workbench_type == 8 || workbench_type == 9) return true;
    if (((buy_mask | book_mask) & (1 << goods)) != 0) return false;
    return true;
}
int Workbench::sells() { return (workbench_type <= 7 ? workbench_type : 0); }
double Workbench::bonusRate(int goods) {
    if (MAP == 1) {
        ;
    }
    if (MAP == 4 && this->workbench_type == 7 && goods == 4) {
        return 3.;
    } else if (MAP == 2) {
        if (goods == 6) {
            return 1.6;
        } else {
            return 1.;
        }
    }
    if (workbench_type == 9) {
        if (goods == 7) {
            return 1.;
        } else {
            if (MAP == 1) {
                return 0.2;
            } else if (MAP == 3) {
                return 1.;
            }
        }
    }
    if (workbench_type < 4) return 1.0;
    if (notInBuys(goods)) return 0.0;
    int have_got_nums = __builtin_popcount((buy_mask | book_mask) & (~1));
    vector<double> bonus_rate_dict;
    if (workbench_type == 7) {
        bonus_rate_dict = {-1, 1.6, 2.2};
    } else
        bonus_rate_dict = {-1, 1.6};
    if (have_got_nums > SZ(bonus_rate_dict) || bonus_rate_dict[have_got_nums] == -1) return 1.0;
    return bonus_rate_dict[have_got_nums];
}

double Workbench::needRate(int goods) {  // only for this->workbench_type == 7, for the need of the goods
    if (MAP == 1) {
        if (FRAME_ID > 4500) {
            if (goods == 5) return 1.0;
            if (goods == 6) return 1.5;
            if (goods == 4) return 2.;
        }
        else return 1.;
    }
    if (this->workbench_type == 7) {
        if (notInBuys(goods)) return 1;
        //__builtin_popcount(x) ： 统计无符号数x里用二进制表示，1的个数
        int have_got_nums = __builtin_popcount((buy_mask | book_mask) & (~1));
        vector<double> need_bouns_rate = {1.0, 1.3, 1.8};
        if (((buy_mask | book_mask) & (1 << goods)) == 0 && have_got_nums != 0) {
            if (have_got_nums == 1 || have_got_nums == 2)
                return need_bouns_rate[have_got_nums];  // 有数取
            else
                return 1.0;  // 没数取，返回默认值
        } else
            return 1.0;
    } else if (this->workbench_type > 3) {
        return 1.;
    } else {
        return 1.;
    }
};
void Workbench::preBook(int goods, double frames_later) {
    int load_frame = FRAME_ID + int(frames_later);
    this->load_frames.push_back(load_frame);
    book_mask |= (1 << goods);
}
void Workbench::unbook(int goods) {  // 取消预定
    book_mask &= ~(1 << goods);
}