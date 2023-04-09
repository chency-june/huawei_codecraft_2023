#include "robot.h"

extern vector<vector<Edge>> g_matrix;

Robot::Robot(int robot_id_i, double x_i, double y_i, double toward_i) {
    robot_id = robot_id_i;  // unique id range[0,3]
    pos = {x_i, y_i};       // the position of the robot
    toward = toward_i;      // the direction of the robot [-PI, PI]
    goods = 0;

    followings = {pos};
    // order
    // 这里给结构体变量赋值为空不会写，感觉不赋值为空也没事？因为我构造函数里默认值设为-1，判是否为-1可以替代判None
    order = nullptr;       // the order to finish
    next_order = nullptr;  // the order to do when the first order finished
    // (not used yet)
    todo = "";
    // instruct
    speed_ = 6.0;
    rotate_ = 0.0;
    buy_ = false;
    sell_ = false;
    destroy_ = false;
}
Position Robot::nextPos(int frames) {
    double x = pos.x + speed_x * frames / 50.0;
    double y = pos.y + speed_y * frames / 50.0;
    return {x, y};
}
void Robot::updateStatus(int workbench_id_i, int goods_i, double time_lambda_i, double boom_lambda_i, double omega_i,
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
    pos = {x_i, y_i};
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
    int detect_frames = 18;
    for (int i = 1; i <= detect_frames; i++) followings.emplace_back(nextPos(i));
}
bool Robot::hasOrder() {
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
void Robot::heading(Position &_des) {
    // set the instructions
    // rotate是要转的角速度，rotate_l是暂时变量
    double toward_l = direction(pos, _des);
    double rotate_l = rotate(toward, toward_l);  // 区间变量，仅函数内暂时使用
    if (abs(rotate_l) < 0.1)
        rotate_ = 0;  // stright to the detination, just go stright
    else
        rotate_ = rotate_l > 0 ? sqrt(2 * alpha * rotate_l) : -sqrt(-2 * alpha * rotate_l);
    this->rotate_ /= SKIP_FRAMES;
    double alpha = max(1e-3, abs(rotate_l));
    double d = calc_dist(this->pos, _des);
    this->speed_ = d * pi / (2.0 * sin(abs(rotate_l)));
    this->speed_ = min(this->speed_, sqrt(2 * this->a * (alpha * d / sin(alpha))));
    if (abs(rotate_l) > pi * 0.5) {
        double t = (abs(rotate_l) - pi * 0.5) / pi;
        if (MAP == 2) {
            this->speed_ = 0;
        } else {
            this->speed_ = -this->a * t;
        }
    } else if (rotate_) {
        this->speed_ = min(6.1, this->speed_);
        this->speed_ *= 0.6;
    }
}
void Robot::buyAndSell(vector<Workbench> &workbenches) {
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
bool Robot::nearDestination(double distance) { return calc_dist(pos, destination) < distance; }