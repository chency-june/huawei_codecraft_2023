#ifndef _ROBOT_H_
#define _ROBOT_H_

#include "order.h"
#include "utils.h"
#include "workbench.h"

struct Robot {
    int robot_id, workbench_id, goods;
    int vertax_id;
    double x, y, toward, time_lambda, boom_lambda, omega, speed_x, speed_y, speed;
    double radius = 0.45;  // 半径
    double a;              // 加速度
    double alpha;          // 角加速度
    Position pos;
    vector<Position> followings;
    Order *order, *next_order;  // order
    Position destination;
    string todo;
    double speed_, rotate_;      // 速度，角速度
    bool buy_, sell_, destroy_;  // 买，卖，销毁操作(销毁操作应该没用
    Robot(int robot_id_i = 0, double x_i = 0, double y_i = 0, double toward_i = 0);
    Position nextPos(int frames);
    void updateStatus(int workbench_id_i, int goods_i, double time_lambda_i, double boom_lambda_i, double omega_i,
                      double speed_x_i, double speed_y_i, double toward_i, double x_i, double y_i);
    bool hasOrder();
    void heading(Position &_des);
    void buyAndSell(vector<Workbench> &workbenches);
    bool nearDestination(double distance = 1);
};

#endif