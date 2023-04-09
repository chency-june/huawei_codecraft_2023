#include <bits/stdc++.h>
#define SZ(x) int(x.size())
#define pdd pair<double, double>
#define x first
#define y second

using namespace std;
const double pi = acos(-1.0);
const double R = 6. / pi;
const double delta_t = 0.02;  // time of one frame

int MAP = 0;
int FRAME_ID = 0;

/* utils parameter */
bool equal(double a, double b) { return abs(a - b) < 1e-4; }
double discount(double x, int maxX, double minRate) {
    if (x < maxX)
        return (1. - sqrt(1 - (1 - x / maxX) * (1 - x / maxX))) * (1. - minRate) + minRate;
    else
        return minRate;
}
double calc_dist(pdd a, pdd b) {
    double delta_x = a.x - b.x;
    double delta_y = a.y - b.y;
    return sqrt(delta_x * delta_x + delta_y * delta_y);
}
double direction(pdd _stand, pdd _toward) { return atan2(_toward.y - _stand.y, _toward.x - _stand.x); }
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
double time_pretiction(double toward, double speed, pdd _stand, pdd _des) {
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
    if (frame_id + total_time * 50 + 50 > 9000) return -1;
    return discount(time2 * 50, 9000, 0.8) * sell_price[goods] - buy_price[goods];
}
/* workbench parameter */
struct Workbench {
    int workbench_id, workbench_type, book_mask, remaining, buy_mask;
    pair<double, double> pos;
    bool product;
    vector<int> load_frames = vector<int>();
    vector<int> buys[30] = {{0}, {0}, {0}, {0}, {1, 2}, {1, 3}, {2, 3}, {4, 5, 6}, {7}, {1, 2, 3, 4, 5, 6, 7}};
    Workbench(int workbench_id_i = 0, int workbench_type_i = 0, double x_i = 0, double y_i = 0) {
        workbench_id = workbench_id_i;      // unique id [0, 49]
        workbench_type = workbench_type_i;  // type [1, 9]
        pos = make_pair(x_i, y_i);          // the position
        book_mask = 0;
    }
    bool notInBuys(int goods) {
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
    void updateStatus(int workbench_type_i, double x_i, double y_i, int remaining_i, int buy_mask_i, int product_i) {
        assert(workbench_type == workbench_type_i);
        assert(x_i == pos.x);
        assert(y_i == pos.y);
        remaining = remaining_i;
        buy_mask = buy_mask_i;
        product = (product_i == 1 ? true : false);
    }
    bool readyToBuy(double frames_later) {
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
    bool readyToSell(int goods, double frames_later = 0.0) {
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
    int sells() { return (workbench_type <= 7 ? workbench_type : 0); }
    double bonusRate(int goods) {
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
        // 不在其中，返回默认值1
        if (have_got_nums > SZ(bonus_rate_dict) || bonus_rate_dict[have_got_nums] == -1) return 1.0;
        return bonus_rate_dict[have_got_nums];
    }

    double needRate(int goods) {  // only for this->workbench_type == 7, for the need of the goods
        if (MAP == 2) {
            if (goods == 6) {
                return 1.2;
            } else {
                return 1.;
            }
        }
        if (this->workbench_type == 7) {
            if (MAP == 4 && goods == 4) {
                return 3.;
            }
            if (notInBuys(goods)) return 1;
            //__builtin_popcount(x) ： 统计无符号数x里用二进制表示，1的个数
            int have_got_nums = __builtin_popcount((buy_mask | book_mask) & (~1));
            vector<double> need_bouns_rate = {-1, 1.3, 1.8};
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
    void preBook(int goods, double frames_later) {  // 预定书，不让别的机器人抢？
        int load_frame = FRAME_ID + int(frames_later);
        this->load_frames.push_back(load_frame);
        book_mask |= (1 << goods);
    }
    void unbook(int goods) {  // 取消预定
        book_mask &= ~(1 << goods);
    }
};
/* Order parameter */
struct Suborder {
    pdd pos_from;
    pdd pos_to;
    vector<pdd> vias;
    Suborder(pdd &_pos_from, pdd &_pos_to, bool lane = false) {
        this->pos_from = _pos_from;
        this->pos_to = _pos_to;

        if (lane && calc_dist(pos_from, pos_to) > 2 * R) {
            double _direction = direction(pos_from, pos_to);
            double angle_para = 0.25;
            if (MAP == 1) {
                angle_para = 0.15;
            }
            double route1_dir = _direction - pi * angle_para;
            double route2_dir = _direction - pi * (1. - angle_para);
            this->vias = {
                make_pair(pos_from.x + cos(route1_dir) * R * sqrt(2), pos_from.y + sin(route1_dir) * R * sqrt(2)),
                make_pair(pos_to.x + cos(route2_dir) * R * sqrt(2), pos_to.y + sin(route2_dir) * R * sqrt(2))};
        } else {
            this->vias.clear();
        }
    }
};
struct Order {
    bool bought, sold;
    int buy_workbench_id, sell_workbench_id;
    vector<Suborder> suborders;
    Order(pdd _stand_pos, Workbench &_buy_workbench, Workbench &_sell_workbench) {
        this->buy_workbench_id = _buy_workbench.workbench_id;
        this->sell_workbench_id = _sell_workbench.workbench_id;
        bool lane = (MAP == 4 || MAP == 1);
        suborders.clear();
        suborders.push_back(Suborder(_stand_pos, _buy_workbench.pos, lane));
        suborders.push_back(Suborder(_buy_workbench.pos, _sell_workbench.pos, lane));

        bought = false;
        sold = false;
    }

    bool complete() { return bought && sold; }
};
/* Robot parameter */
struct Robot {
    int robot_id, workbench_id, goods;
    double x, y, toward, time_lambda, boom_lambda, omega, speed_x, speed_y, speed;
    double radius = 0.45;  // 半径
    double a;              // 加速度
    double alpha;          // 角加速度
    pair<double, double> pos;
    vector<pair<double, double>> followings;
    Order *order, *next_order;  // order
    pdd destination;
    string todo;
    double speed_, rotate_;      // 速度，角速度
    bool buy_, sell_, destroy_;  // 买，卖，销毁操作(销毁操作应该没用
    Robot(int robot_id_i = 0, double x_i = 0, double y_i = 0, double toward_i = 0) {
        robot_id = robot_id_i;      // unique id range[0,3]
        pos = make_pair(x_i, y_i);  // the position of the robot
        toward = toward_i;          // the direction of the robot [-PI, PI]
        goods = 0;

        followings = {pos};
        // order
        // 这里给结构体变量赋值为空不会写，感觉不赋值为空也没事？因为我构造函数里默认值设为-1，判是否为-1可以替代判None
        order = nullptr;       // the order to finish
        next_order = nullptr;  // the order to do when the first order finished
        // (not used yet)
        // destination
        destination = make_pair(25, 42);
        todo = "";
        // instruct
        speed_ = 6.0;
        rotate_ = 0.0;
        buy_ = false;
        sell_ = false;
        destroy_ = false;
    }
    pdd nextPos(int frames) {
        double x = pos.first + speed_x * frames / 50.0;
        double y = pos.second + speed_y * frames / 50.0;
        return make_pair(x, y);
    }
    void updateStatus(int workbench_id_i, int goods_i, double time_lambda_i, double boom_lambda_i, double omega_i,
                      double speed_x_i, double speed_y_i, double toward_i, double x_i, double y_i) {
        workbench_id = workbench_id_i;
        goods = goods_i;
        time_lambda = time_lambda_i;
        boom_lambda = boom_lambda_i;
        omega = omega_i;
        speed_x = speed_x_i;
        speed_y = speed_y_i;
        if (abs(rotate(toward_i, atan2(speed_y, speed_x))) > pi / 3) {
            this->speed_x = 0;
            this->speed_y = 0;
        }
        speed = sqrt(speed_x * speed_x + speed_y * speed_y);
        toward = toward_i;
        pos = make_pair(x_i, y_i);
        followings.clear();
        if (goods) {
            radius = 0.53;
            a = 19.;
            alpha = 38.;
        } else {
            radius = 0.45;
            a = 14.;
            alpha = 20.;
        }
        // int detect_frames = int(speed / a * 50) + 1;
        int detect_frames = 15;
        switch (MAP) {
            case 1:
                detect_frames = 17;
                break;
            case 2:
                detect_frames = 24;
                break;
            case 3:
                detect_frames = 19;
                break;
            case 4:
                detect_frames = 17;
                break;
        }
        for (int i = 1; i <= detect_frames; i++) followings.emplace_back(nextPos(i));
    }
    bool hasOrder() {
        // to check if the robot has an unfinished order
        //  cerr << order.bought << ' ' << order.sold << endl;
        if (!order && !next_order) {
            return false;
        }
        if (!order && next_order) {
            order = next_order;
        }
        if (order && this->order->complete()) {
            delete order;
            if (next_order) {
                order = next_order;
                next_order = nullptr;
                return true;
            } else {
                order = nullptr;
                return false;
            }
        } else if (order) {
            return true;
        } else {
            return false;
        }
    }
    void finishSuborder(Suborder &suborder) {
        if (SZ(suborder.vias) == 0) {
            this->heading(suborder.pos_to);
        } else {
            if (calc_dist(this->pos, suborder.pos_from) < R) {
                this->heading(suborder.vias[0]);
            } else if (calc_dist(this->pos, suborder.pos_to) < R * 2) {
                this->heading(suborder.pos_to);
            } else {
                this->heading(suborder.vias[1]);
            }
        }
    }
    void heading(pdd &_des) {
        // set the instructions
        // rotate是要转的角速度，rotate_l是暂时变量
        double toward_l = direction(pos, _des);
        double rotate_l = rotate(toward, toward_l);  // 区间变量，仅函数内暂时使用
        if (abs(rotate_l) < 0.1)
            rotate_ = 0;  // stright to the detination, just go stright
        else
            rotate_ = rotate_l > 0 ? pi : -pi;  // rotate in the right direction
        double alpha = max(1e-3, abs(rotate_l));
        double d = calc_dist(this->pos, _des);
        this->speed_ = d * pi / (2.0 * sin(abs(rotate_l)));
        if (MAP != 4) {
            this->speed_ = min(this->speed_, sqrt(2 * this->a * (alpha * d / sin(alpha))));
        }
        if (abs(rotate_l) > pi * 0.5) {
            double t = (abs(rotate_l) - pi * 0.5) / pi;
            if (MAP == 2) {
                this->speed_ = 0;
            } else {
                this->speed_ = -this->a * t;
            }
        }
    }
    void buyAndSell(vector<Workbench> &workbenches) {
        if (todo == "buy" && workbench_id == this->order->buy_workbench_id) {
            buy_ = true;
            if (workbenches[workbench_id].product) workbenches[workbench_id].unbook(0);
        } else {
            buy_ = false;
        }
        if (todo == "sell" && workbench_id == this->order->sell_workbench_id) {
            sell_ = true;
            if ((workbenches[workbench_id].buy_mask & (1 << goods)) == 0) {
                workbenches[workbench_id].unbook(goods);
            }
        } else {
            sell_ = false;
        }
    }
    bool nearDestination(double distance = 1) { return calc_dist(pos, destination) < distance; }
};

/* control center parameter */
struct ControlCenter {
    char string_maps[105][105];
    vector<Robot> robots;
    vector<Workbench> workbenches;
    int num_workbenches;
    vector<vector<double>> dist;
    vector<double> type_rate;

    int frame_id, money;
    ControlCenter() {
        robots = {};
        workbenches = {};
    }
    void readMaps() {
        // rand the map data to an 2d array `string_maps`
        for (int i = 0; i < 100; i++) scanf("%s", string_maps[i]);
        char tmp_string[5];
        scanf("%s", tmp_string);
        assert(tmp_string[0] == 'O' && tmp_string[1] == 'K');
        // record each robot and each workbench
        for (int i = 0; i < 100; i++) {
            for (int j = 0; j < 100; j++) {
                if (string_maps[i][j] == '.') continue;
                double x = j * 0.5 + 0.25;
                double y = (99 - i) * 0.5 + 0.25;
                if (string_maps[i][j] == 'A') {
                    robots.emplace_back(Robot(SZ(robots), x, y, 0));
                } else {
                    int workbench_type = int(string_maps[i][j] - '0');
                    int tmp = SZ(workbenches);
                    workbenches.emplace_back(Workbench(tmp, workbench_type, x, y));
                }
            }
        }
        num_workbenches = SZ(workbenches);
        // self.dist stored the distance between any two workbenches
        for (int i = 0; i < 100; i++) {
            vector<double> tmp_vector = {};
            for (int j = 0; j < 100; j++) tmp_vector.emplace_back(100.0);
            dist.emplace_back(tmp_vector);
        }
        for (int i = 0; i < num_workbenches; i++) {
            for (int j = 0; j < num_workbenches; j++) {
                // 这个函数如果叫dist就和dist数组重名了，就改成calc_dist了
                double distance = calc_dist(workbenches[i].pos, workbenches[j].pos);
                dist[i][j] = distance;
            }
        }
        if (workbenches[0].workbench_type == 1 && workbenches[1].workbench_type == 5) {
            MAP = 1;
        } else if (workbenches[0].workbench_type == 6) {
            MAP = 2;
        } else if (workbenches[0].workbench_type == 3 && workbenches[1].workbench_type == 5) {
            MAP = 3;
        } else if (workbenches[0].workbench_type == 7 && workbenches[1].workbench_type == 1) {
            MAP = 4;
        }
        // tell the game that we are ready
        printf("OK\n");
        fflush(stdout);
    }
    void readFrameID_money() {
        scanf("%d%d", &frame_id, &money);
        FRAME_ID = this->frame_id;
    }
    void readWorkbenchesStatus() {
        int num_workbenches_t;
        scanf("%d", &num_workbenches_t);
        assert(num_workbenches == num_workbenches_t);
        int workbench_type_i, remaining_i, buy_mask_i, product_i;
        double x_i, y_i;
        for (int i = 0; i < num_workbenches; i++) {
            scanf("%d%lf%lf%d%d%d", &workbench_type_i, &x_i, &y_i, &remaining_i, &buy_mask_i, &product_i);
            workbenches[i].updateStatus(workbench_type_i, x_i, y_i, remaining_i, buy_mask_i, product_i);
        }
    }
    void readRobotStatus() {
        int workbench_id_i, goods_i;
        double time_lambda_i, boom_lambda_i, omega_i, speed_x_i, speed_y_i, toward_i, x_i, y_i;
        for (int i = 0; i < 4; i++) {
            scanf("%d%d%lf%lf%lf%lf%lf%lf%lf%lf", &workbench_id_i, &goods_i, &time_lambda_i, &boom_lambda_i, &omega_i,
                  &speed_x_i, &speed_y_i, &toward_i, &x_i, &y_i);
            robots[i].updateStatus(workbench_id_i, goods_i, time_lambda_i, boom_lambda_i, omega_i, speed_x_i, speed_y_i,
                                   toward_i, x_i, y_i);
        }
        char tmp_string[5];
        scanf("%s", tmp_string);
        assert(tmp_string[0] == 'O' && tmp_string[1] == 'K');
    }
    void getOrder(Robot &robot) {
        // get an order for the specific robot
        // it shows an stupid way to get order, NEED BETTER WAYS
        struct node {
            int buy_workbench_id, sell_work_bench_id;
            double weight;
            node(int _a = -1, int _b = -1, double _c = -1) {
                buy_workbench_id = _a;
                sell_work_bench_id = _b;
                weight = _c;
            }
        };
        vector<node> choices;
        choices.clear();
        if (MAP == 2 || MAP == 1) {
            vector<vector<int>> orders_write;
            if (MAP == 1) {
                orders_write = {{-1, 9},  {-1, 13}, {-1, 20}, {-1, 24}, {-1, 29},
                                {-1, 32}, {-1, 17}, {-1, 15}, {17, -1}, {15, -1}};
            } else if (MAP == 2) {
                orders_write = {{-1, 0}, {-1, 3}, {-1, 1}, {-1, 12}, {-1, 23}, {-1, 21}, {-1, 24}, {3, -1}, {21, -1}};
            }
            vector<vector<int>> orders;
            for (vector<int> &_order : orders_write) {
                if (_order[0] >= 0 && _order[1] >= 0) {
                    orders.push_back(_order);
                } else if (_order[0] == -1) {
                    for (int buy_wb_id = 0; buy_wb_id < SZ(workbenches); buy_wb_id++) {
                        orders.push_back({buy_wb_id, _order[1]});
                    }
                } else {
                    for (int sell_wb_id = 0; sell_wb_id < SZ(workbenches); sell_wb_id++) {
                        orders.push_back({_order[0], sell_wb_id});
                    }
                }
            }
            for (vector<int> &_order : orders) {
                int buy_workbench_id = _order[0], sell_workbench_id = _order[1];
                Workbench &buy_workbench = workbenches[buy_workbench_id];
                Workbench &sell_workbench = workbenches[sell_workbench_id];
                if (__builtin_popcount(buy_workbench.book_mask & ~1) > 0 && robot.workbench_id != buy_workbench_id) {
                    continue;
                }
                double time1 = time_pretiction(robot.toward, robot.speed, robot.pos, buy_workbench.pos);
                double time2 =
                    time_pretiction(direction(robot.pos, buy_workbench.pos), 6, buy_workbench.pos, sell_workbench.pos);
                if (buy_workbench.readyToBuy(time1 * 50) &&
                    sell_workbench.readyToSell(buy_workbench.sells(), (time1 + time2) * 50)) {
                    int goods = buy_workbench.sells();
                    double weight = profit(goods, time1, time2, frame_id) * sell_workbench.bonusRate(goods) *
                                    type_rate[sell_workbench.sells()] / (time1 + time2);
                    if (weight > 0) {
                        choices.emplace_back(node(buy_workbench.workbench_id, sell_workbench.workbench_id, weight));
                    }
                }
            }
        } else
            for (auto &buy_workbench : workbenches) {
                double dist1 = calc_dist(robot.pos, buy_workbench.pos);
                if (__builtin_popcount(buy_workbench.book_mask & ~1) > 0 &&
                    robot.workbench_id != buy_workbench.workbench_id) {
                    continue;
                }
                if (buy_workbench.readyToBuy(time_pretiction(robot.toward, robot.speed, robot.pos, buy_workbench.pos) *
                                             50)) {
                    for (auto &sell_workbench : workbenches) {
                        if (sell_workbench.readyToSell(buy_workbench.sells())) {
                            double time1 = time_pretiction(robot.toward, robot.speed, robot.pos, buy_workbench.pos);
                            double time2 = time_pretiction(direction(robot.pos, buy_workbench.pos), 6,
                                                           buy_workbench.pos, sell_workbench.pos);
                            int goods = buy_workbench.sells();
                            double weight = profit(goods, time1, time2, frame_id) * sell_workbench.bonusRate(goods) *
                                            type_rate[sell_workbench.sells()] / (time1 + time2);
                            // 好像vector<vector<int>>>不能用emplace_back，会报错，所以用push_back
                            if (weight > 0) {
                                choices.emplace_back(
                                    node(buy_workbench.workbench_id, sell_workbench.workbench_id, weight));
                            }
                        }
                    }
                }
            }
        sort(choices.begin(), choices.end(), [&](node a, node b) {  // 用引用会快一点点
            return a.weight > b.weight;                             // 按权值从大大小排序
        });
        if (SZ(choices) == 0) {
            robot.order = nullptr;
            return;
        }
        node choice = choices[0];
        int buy_workbench_id = choice.buy_workbench_id, sell_workbench_id = choice.sell_work_bench_id;
        double time1 = time_pretiction(robot.toward, robot.speed, robot.pos, this->workbenches[buy_workbench_id].pos);
        double time2 =
            time_pretiction(direction(robot.pos, this->workbenches[buy_workbench_id].pos), 6,
                            this->workbenches[buy_workbench_id].pos, this->workbenches[sell_workbench_id].pos);
        workbenches[sell_workbench_id].preBook(workbenches[buy_workbench_id].sells(), (time1 + time2) * 50);
        workbenches[buy_workbench_id].preBook(0, time1 * 50);
        pdd stand = robot.pos;
        if (robot.workbench_id >= 0) {
            stand = workbenches[robot.workbench_id].pos;
        }
        robot.order = new Order(stand, workbenches[buy_workbench_id], workbenches[sell_workbench_id]);
    }
    void getOrders() {
        // get an order for each robot
        for (auto &robot : robots) {
            if (!robot.hasOrder()) {
                getOrder(robot);
            }
        }
    }
    void formOrders() {
        // generate a list of orders for the robot
        type_rate.clear();
        for (int i = 0; i < 10; i++) type_rate.emplace_back(1);
        for (auto &workbench : workbenches) {
            if (workbench.workbench_type == 7) {
                for (int goods = 4; goods < 7; goods++) {
                    type_rate[goods] = max(workbench.needRate(goods), type_rate[goods]);
                }
            }
        }
    }
    void navigate() {
        for (auto &robot : robots) {
            // get the destination: go to buy workbench or go to sell workbench
            Order &order = *robot.order;
            if (!robot.order) {
                continue;
            }
            if (robot.goods > 0) {  // robot carry the right goods, just sell it
                assert(robot.goods == workbenches[order.buy_workbench_id].workbench_type);
                order.bought = true;
                robot.todo = "sell";
                // sell suborder
                Suborder &suborder = robot.order->suborders[1];
                robot.finishSuborder(suborder);
            } else if (order.bought == true) {
                order.sold = true;
                robot.todo = "";  // 用空字符串来代替None
            } else {              // go to buy the goods
                robot.todo = "buy";
                // buy suborder
                Suborder &suborder = robot.order->suborders[0];
                robot.finishSuborder(suborder);
            }
            robot.buyAndSell(workbenches);
        }
        detectWall();
        detectBoom();
    }
    void detectWall() {
        for (auto &robot : this->robots) {
            bool boom = false;
            for (pdd &pos : robot.followings) {
                if (pos.x <= robot.radius || pos.x >= 50. - robot.radius || pos.y <= robot.radius ||
                    pos.y >= 50. - robot.radius) {
                    robot.speed_ = 0;
                }
            }
        }
    }
    void detectBoom() {
        set<int> controlled;
        for (auto &robot : robots) {
            if (controlled.find(robot.robot_id) == controlled.end()) {  // 在controlled里没找到robot.robot_id
                for (auto &detected_robot : robots) {
                    if (detected_robot.robot_id != robot.robot_id) {  // 这里写法感觉可以写成如果==,就continue;
                        double safe_dist = robot.radius + detected_robot.radius - 0.01;
                        bool will_BOOM = false;  //|= 替代any
                        for (auto &obstacle : detected_robot.followings)
                            for (auto &next_pos : robot.followings)
                                will_BOOM |= (calc_dist(next_pos, obstacle) < safe_dist);
                        if (will_BOOM) {
                            double he_turn_to_me =
                                rotate(detected_robot.toward, direction(detected_robot.pos, robot.pos));
                            double i_turn_to_him = rotate(robot.toward, direction(robot.pos, detected_robot.pos));
                            bool near_destination = (robot.nearDestination() && detected_robot.nearDestination());
                            if (he_turn_to_me * i_turn_to_him < 0 &&
                                calc_dist(robot.pos, detected_robot.pos) > safe_dist + 0.4) {
                                controlled.insert(
                                    detected_robot.robot_id);  // 往controlled里插入detected_robot.robot_id
                                if (abs(he_turn_to_me) < abs(i_turn_to_him))
                                    detected_robot.speed_ = (near_destination ? -2 : 0);  // 他停
                                else
                                    robot.speed_ = (near_destination ? -2 : 0);  // 我停
                            } else if (!near_destination) {
                                if (rotate(robot.toward, direction(robot.pos, detected_robot.pos)) > 0)
                                    robot.rotate_ = -pi;
                                else
                                    robot.rotate_ = pi;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    void sendInstruct() {
        printf("%d\n", frame_id);
        for (auto &robot : robots) {
            printf("forward %d %lf\n", robot.robot_id, robot.speed_);
            printf("rotate %d %lf\n", robot.robot_id, robot.rotate_);
            if (robot.buy_) printf("buy %d\n", robot.robot_id);
            if (robot.sell_) printf("sell %d\n", robot.robot_id);
            if (robot.destroy_) printf("destroy %d\n", robot.robot_id);
        }
        printf("OK\n");
        fflush(stdout);
    }
};

int main() {
    ControlCenter control_center;
    control_center.readMaps();
    for (int i = 0; i < 9000; i++) {
        control_center.readFrameID_money();  // read the first line data of each frame
        control_center.readWorkbenchesStatus();
        control_center.readRobotStatus();
        control_center.formOrders();  // make a list of orders to do
        control_center.getOrders();   // a an order for each robot if they have none
        control_center.navigate();    // go to the destination, set the speed,
        // rotation parameters
        control_center.sendInstruct();  // send the instructions to the game program
    }
    return 0;
}
