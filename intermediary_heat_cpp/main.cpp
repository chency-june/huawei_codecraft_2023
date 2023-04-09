#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_set>
#include <vector>

#include "graph.h"
#include "robot.h"
#include "utils.h"
#include "workbench.h"

using namespace std;

vector<vector<Edge>> g_matrix;
vector<vector<double>> g_stright_thin;
vector<vector<double>> g_stright_fat;
vector<vector<double>> g_shortest_thin;
vector<vector<double>> g_shortest_fat;
vector<Vertax> g_vertaxes;
vector<vector<char>> g_thin_maps(99, vector<char>(99, '#'));
vector<vector<char>> g_fat_maps(98, vector<char>(98, '#'));
unordered_set<int> g_thin_vertaxes_indexes;
unordered_set<int> g_fat_vertaxes_indexes;

int num_vertaxes;

/* ------------start of dijkstra----------------------*/
#define i64 double
#define eb emplace_back
const int N = 1500;  // 点数
const i64 inf = 0x3f3f3f3f3f3f3f3fLL;
i64 chk[N][N];
struct node {
    int id;
    i64 w;
    node() {}
    node(int a, i64 b) : id(a), w(b) {}  // hdu6805 美好的回忆:>
    friend bool operator<(node a, node b) { return a.w > b.w; }
};
vector<node> G[N];
bool vis[N];
i64 dis[N];

void dij(int s, int n) {
    priority_queue<node> q;
    while (!q.empty()) q.pop();
    node cur;
    for (int i = 0; i <= n; ++i) {  // 另外,memset比for快哦
        dis[i] = inf;
        vis[i] = 0;
    }
    dis[s] = 0;
    q.push(node(s, dis[s]));
    while (!q.empty()) {
        cur = q.top();
        q.pop();  // 另外,能return要的值就return哦(ccpcfinal 2016 G 枚举m条边跑dij那题),会快很多
        if (vis[cur.id]) continue;
        vis[cur.id] = 1;
        for (node to : G[cur.id]) {
            if (!vis[to.id] && dis[to.id] > dis[cur.id] + to.w) {  // dis[to.id] > to.w 就变成了堆优化prim
                dis[to.id] = dis[cur.id] + to.w;                   // 堆优化prim下面也要改成dis[to.id] = to.w;
                q.push(node(to.id, dis[to.id]));
            }
        }
    }
}
void init(int n) {
    for (int i = 0; i <= n; ++i) G[i].clear();
}
/* -----------end of dijkstra--------------*/
struct ControlCenter {
    char string_maps[105][105];
    vector<Robot> robots;
    vector<Workbench> workbenches;
    // vector<vector<Edge>> matrix;
    int num_workbenches;
    vector<double> type_rate;

    int frame_id, money;
    ControlCenter() {
        robots = {};
        workbenches = {};
    }
    bool allNotWall(int row, int col, vector<vector<int>> &shifts) {
        int num_not_wall = 0;
        for (auto &shift : shifts) {
            if (string_maps[row + shift[0]][col + shift[1]] != '#') {
                num_not_wall++;
            }
        }
        return num_not_wall == shifts.size();
    }
    inline bool positionIsWall(double x, double y) {
        if (x < 0 || x > 50 || y < 0 || y > 50) {
            return true;
        }
        int i = int((50 - y) * 2);
        int j = int(x * 2);
        return (this->string_maps[i][j] == '#');
    }
    bool strightNoWall(const Position &from_pos, const Position &to_pos, int width_level) {
        static const double a = sqrt(2.) / 2;
        static vector<vector<double>> around_thin = {{0, 0},
                                                     {0, thin_radius},
                                                     {thin_radius, 0},
                                                     {0, -thin_radius},
                                                     {-thin_radius, 0},
                                                     {thin_radius * a, thin_radius * a},
                                                     {thin_radius * a, -thin_radius * a},
                                                     {-thin_radius * a, -thin_radius * a},
                                                     {-thin_radius * a, thin_radius * a}};
        static vector<vector<double>> around_fat = {{0, 0},
                                                    {0, fat_radius},
                                                    {fat_radius, 0},
                                                    {0, -fat_radius},
                                                    {-fat_radius, 0},
                                                    {fat_radius * a, fat_radius * a},
                                                    {fat_radius * a, -fat_radius * a},
                                                    {-fat_radius * a, -fat_radius * a},
                                                    {-fat_radius * a, fat_radius * a}};

        double toward = direction(from_pos, to_pos);
        double distance = calc_dist(from_pos, to_pos);
        double step = pi / 16;
        vector<vector<double>> &around = width_level > 0 ? around_fat : around_thin;
        for (int k = 1; k < distance / step; k++) {
            double x = from_pos.x + k * step * cos(toward);
            double y = from_pos.y + k * step * sin(toward);
            for (vector<double> &shift : around) {
                if (width_level == 2) {
                    if (positionIsWall(x + shift[0] * 2, y + shift[1] * 2)) {
                        return false;
                    }
                } else {
                    if (positionIsWall(x + shift[0], y + shift[1])) {
                        return false;
                    }
                }
            }
        }
        return true;
    }
    bool strightAllEmpty(Position &from_pos, Position &to_pos, vector<vector<char>> maps) {
        double radius_detect = 0.035;
        if (MAP == 1) {
            radius_detect = fat_radius;
        }
        static vector<vector<double>> around = {{0, 0}, {1, 0}, {1, 0}, {0, 1}, {0, 1}};
        int m = maps.size();
        bool fat = (m == 98);
        double toward = direction(from_pos, to_pos);
        double distance = calc_dist(from_pos, to_pos);
        double step = pi / 32;
        double delta_x = step * cos(toward);
        double delta_y = step * sin(toward);
        for (vector<double> &shift : around) {
            for (int k = 1; k < distance / step; k++) {
                double x = from_pos.x + k * delta_x + shift[0] * radius_detect;
                double y = from_pos.y + k * delta_y + shift[1] * radius_detect;
                Position check_pos = {x, y};
                vector<int> check_indexes = fat ? get_fat_map_indexes(check_pos) : get_thin_map_indexes(check_pos);
                int i = check_indexes[0];
                int j = check_indexes[1];
                if (i >= 0 && i < m && j >= 0 && j < m && maps[i][j] == '#') {
                    return false;
                }
            }
        }
        return true;
    }
    int addVertax(const Vertax &vertax) {
        auto it = find(g_vertaxes.begin(), g_vertaxes.end(), vertax);
        if (it == g_vertaxes.end()) {
            g_vertaxes.push_back(vertax);
            return g_vertaxes.size() - 1;
        } else {
            (*it).only_for_thin |= vertax.only_for_thin;
            (*it).ok_for_2 &= vertax.ok_for_2;
        }
        return distance(g_vertaxes.begin(), it);
    }
    void printMap(auto &map) {
        for (auto &line : map) {
            for (char &c : line) {
                cerr << c;
            }
            cerr << endl;
        }
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
                if (string_maps[i][j] == '.' || string_maps[i][j] == '#') continue;
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
        // create thin robot map
        for (int i = 0; i < 99; i++) {
            for (int j = 0; j < 99; j++) {
                if (string_maps[i][j] != '#' && string_maps[i + 1][j] != '#' && string_maps[i][j + 1] != '#' &&
                    string_maps[i + 1][j + 1] != '#') {
                    g_thin_maps[i][j] = '.';
                } else {
                    g_thin_maps[i][j] = '#';
                }
            }
        }
        for (int i = 0; i < 98; i++) {
            for (int j = 0; j < 98; j++) {
                if (string_maps[i + 1][j + 1] != '#' && string_maps[i][j + 1] != '#' && string_maps[i + 1][j] != '#' &&
                    string_maps[i + 1][j + 2] != '#' && string_maps[i + 2][j + 1] != '#' &&
                    ((string_maps[i][j] != '#') + (string_maps[i + 2][j] != '#') + (string_maps[i][j + 2] != '#') +
                         (string_maps[i + 2][j + 2] != '#') >=
                     3)) {
                    g_fat_maps[i][j] = '.';
                } else {
                    g_fat_maps[i][j] = '#';
                }
            }
        }
        // printMap(g_thin_maps);
        printMap(g_fat_maps);
        // workbench as a vertax
        for (Workbench &workbench : this->workbenches) {
            g_vertaxes.push_back(Vertax(workbench.pos, false, false));
        }
        cerr << "finish 512";
        // robot as a vertax
        for (Robot &robot : this->robots) {
            robot.vertax_id = g_vertaxes.size();
            g_vertaxes.push_back(Vertax(robot.pos, true, false));
        }
        cerr << "finish 518";
        // read corner

        vector<vector<int>> borders2 = {{2, 2}, {2, -2}, {-2, -2}, {-2, 2}};
        vector<vector<int>> borders3 = {{3, 3}, {3, -3}, {-3, -3}, {-3, 3}};
        vector<vector<int>> borders5 = {{5, 5}, {5, -5}, {-5, -5}, {-5, 5}};
        vector<vector<vector<int>>> detect_blocks_shift2 = {
            {{0, 1}, {1, 1}, {1, 0}, {0, 2}, {1, 2}, {2, 0}, {2, 1}, {2, 2}},
            {{1, 0}, {1, -1}, {0, -1}, {2, 0}, {2, -1}, {0, -2}, {1, -2}, {2, -2}},
            {{0, -1}, {-1, -1}, {-1, 0}, {0, -2}, {-1, -2}, {-2, 0}, {-2, -1}, {-2, -2}},
            {{-1, 0}, {-1, 1}, {0, 1}, {-2, 0}, {-2, 1}, {-2, 2}, {-1, 2}, {0, 2}}};
        vector<vector<vector<int>>> detect_blocks_shift3 = {{{0, 3}, {1, 3}, {3, 0}, {3, 1}},
                                                            {{3, 0}, {3, -1}, {0, -3}, {1, -3}},
                                                            {{0, -3}, {-1, -3}, {-3, 0}, {-3, -1}},
                                                            {{-3, 0}, {-3, 1}, {0, 3}, {-1, 3}}};
        vector<vector<vector<int>>> detect_blocks_shift5 = {
            {{0, 4},
             {0, 5},
             {1, 4},
             {1, 5},
             {2, 3},
             {2, 4},
             {2, 5},
             {3, 2},
             {3, 3},
             {3, 4},
             {4, 0},
             {4, 1},
             {4, 2},
             {4, 3},
             {5, 0},
             {5, 1},
             {5, 2}},
            {{0, -4},
             {0, -5},
             {1, -4},
             {1, -5},
             {2, -3},
             {2, -4},
             {2, -5},
             {3, -2},
             {3, -3},
             {3, -4},
             {4, -0},
             {4, -1},
             {4, -2},
             {4, -3},
             {5, 0},
             {5, -1},
             {5, -2}},
            {{0, -4},
             {0, -5},
             {-1, -4},
             {-1, -5},
             {-2, -3},
             {-2, -4},
             {-2, -5},
             {-3, -2},
             {-3, -3},
             {-3, -4},
             {-4, -0},
             {-4, -1},
             {-4, -2},
             {-4, -3},
             {-5, 0},
             {-5, -1},
             {-5, -2}},
            {{0, 4},
             {0, 5},
             {-1, 4},
             {-1, 5},
             {-2, 3},
             {-2, 4},
             {-2, 5},
             {-3, 2},
             {-3, 3},
             {-3, 4},
             {-4, 0},
             {-4, 1},
             {-4, 2},
             {-4, 3},
             {-5, 0},
             {-5, 1},
             {-5, 2}},
        };
        for (int i = 0; i < 100; i++) {
            for (int j = 0; j < 100; j++) {
                if (string_maps[i][j] == '#') {
                    double x = j * 0.5 + 0.25;
                    double y = (99 - i) * 0.5 + 0.25;
                    for (int ii = 0; ii < 4; ii++) {
                        bool width3 = false, width2 = false, width5 = false;
                        if (isLeagal(i + borders2[ii][0], j + borders2[ii][1]) &&
                            allNotWall(i, j, detect_blocks_shift2[ii])) {
                            width2 = true;
                            if (isLeagal(i + borders3[ii][0], j + borders3[ii][1]) &&
                                allNotWall(i, j, detect_blocks_shift3[ii])) {
                                width3 = true;
                                if (isLeagal(i + borders5[ii][0], j + borders5[ii][1]) &&
                                    allNotWall(i, j, detect_blocks_shift5[ii])) {
                                    width5 = true;
                                }
                            }
                        }
                        if (width5) {
                            Position pos1 = {x + borders2[ii][1] * 0.75, y - borders2[ii][0] * 0.125};
                            Position pos2 = {x + borders2[ii][1] * 0.125, y - borders2[ii][0] * 0.75};
                            addVertax(Vertax(pos1, false, true));
                            addVertax(Vertax(pos2, false, true));
                        } else if (width3) {
                            Position pos1 = {x + borders2[ii][1] * 0.5, y - borders2[ii][0] * 0.125};
                            Position pos2 = {x + borders2[ii][1] * 0.125, y - borders2[ii][0] * 0.5};
                            Position pos3 = {x + borders2[ii][1] * 0.3334, y - borders2[ii][0] * 0.3334};

                            int vertax_id1 = addVertax(Vertax(pos1, false, false));
                            int vertax_id2 = addVertax(Vertax(pos2, false, false));
                            addVertax(Vertax(pos3, false, false, vertax_id1, vertax_id2));
                        } else if (width2) {
                            Position pos1 = {x + borders2[ii][1] * 0.375, y - borders2[ii][0] * 0.375};
                            addVertax(Vertax(pos1, true, false));
                        }
                    }
                }
            }
        }
        cerr << "vertaxes ready, num_vertaxes = " << g_vertaxes.size() << endl;
        // make a set of all vertaxes
        for (Vertax &vertax : g_vertaxes) {
            // thin
            vector<int> pos_indexes = get_thin_map_indexes(vertax.pos);
            int encoding = indexes_encoding(pos_indexes[0], pos_indexes[1]);
            g_thin_vertaxes_indexes.insert(encoding);
            // fat
            if (!vertax.only_for_thin) {
                vector<int> pos_indexes = get_fat_map_indexes(vertax.pos);
                int encoding = indexes_encoding(pos_indexes[0], pos_indexes[1]);
                g_fat_vertaxes_indexes.insert(encoding);
            }
        }
        // get shortest path
        num_vertaxes = g_vertaxes.size();
        g_matrix = vector<vector<Edge>>(num_vertaxes, vector<Edge>(num_vertaxes, Edge(-1, -1, INF, INF, false, false)));
        g_stright_thin = vector<vector<double>>(num_vertaxes, vector<double>(num_vertaxes, INF));
        g_stright_fat = vector<vector<double>>(num_vertaxes, vector<double>(num_vertaxes, INF));
        g_shortest_thin = vector<vector<double>>(num_vertaxes, vector<double>(num_vertaxes, INF));
        g_shortest_fat = vector<vector<double>>(num_vertaxes, vector<double>(num_vertaxes, INF));
        for (int i = 0; i < num_vertaxes; i++) {
            for (int j = 0; j < num_vertaxes; j++) {
                // go from vertax i to vertax j
                double fat_distance = INF;
                double thin_distance = INF;
                bool onlythin = g_vertaxes[i].only_for_thin || g_vertaxes[j].only_for_thin;
                bool ok_for_2 = g_vertaxes[i].ok_for_2 && g_vertaxes[j].ok_for_2;
                if (i != j && ((g_vertaxes[i].only_link1 != -1 &&
                                (j != g_vertaxes[i].only_link1 && j != g_vertaxes[i].only_link2)) ||
                               (g_vertaxes[j].only_link1 != -1 &&
                                (i != g_vertaxes[j].only_link1 && i != g_vertaxes[j].only_link2)))) {
                    // cerr << "i = " << i << ", j = " << j << endl;;
                } else {
                    Position from_pos = g_vertaxes[i].pos;
                    Position to_pos = g_vertaxes[j].pos;
                    if (strightNoWall(from_pos, to_pos, 0)) {
                        thin_distance = calc_dist(from_pos, to_pos);
                        if (strightNoWall(from_pos, to_pos, 1)) {
                            fat_distance = calc_dist(from_pos, to_pos);
                            if (ok_for_2) {
                                ok_for_2 = strightNoWall(from_pos, to_pos, 2);
                            }
                        }
                    }
                }
                g_matrix[i][j] = Edge(i, j, fat_distance, thin_distance, onlythin, ok_for_2);

                g_shortest_thin[i][j] = g_stright_thin[i][j] = thin_distance;
                g_shortest_fat[i][j] = g_stright_fat[i][j] = fat_distance;
            }
        }
        cerr << "finish init length" << endl;
        // dijkstra algorithm
        int temp_num = num_vertaxes - 1;       // 对几个点求最短路
        init(temp_num);                        // 清空点[0,temp_num]的边,先搞g_shortest_thin
        for (int i = 0; i <= temp_num; i++) {  // 先插入g_shortest_thin的所有边
            for (int j = 0; j <= temp_num; j++) {
                if (g_shortest_thin[i][j] != INF) G[i].emplace_back(node(j, g_shortest_thin[i][j]));
            }
        }
        for (int i = 0; i <= 53; i++) {  // 对54个点轮流跑迪杰斯特拉
            dij(i, temp_num);
            // g_shortest_thin[i][j]是i到j的最短路
            for (int j = 0; j <= temp_num; j++) g_shortest_thin[i][j] = (abs(dis[j] - inf) < 1e-3) ? INF : dis[j];
        }
        init(temp_num);                        // 清空点[0,temp_num]的边，搞g_shortest_fat
        for (int i = 0; i <= temp_num; i++) {  // 插入g_shortest_fat的所有边
            for (int j = 0; j <= temp_num; j++) {
                if (g_shortest_fat[i][j] != INF) G[i].emplace_back(node(j, g_shortest_fat[i][j]));
            }
        }
        for (int i = 0; i <= 53; i++) {  // 对54个点轮流跑迪杰斯特拉
            dij(i, temp_num);
            // g_shortest_fat[i][j]是i到j的最短路
            for (int j = 0; j <= temp_num; j++) g_shortest_fat[i][j] = (abs(dis[j] - inf) < 1e-3) ? INF : dis[j];
        }
        // floyd algorithm
        /*for (int k = 0; k < num_vertaxes; k++) {
            for (int i = 0; i < num_vertaxes; i++) {
                for (int j = 0; j < num_vertaxes; j++) {
                    g_shortest_thin[i][j] = min(g_shortest_thin[i][j], g_shortest_thin[i][k] + g_shortest_thin[k][j]);
                    g_shortest_fat[i][j] = min(g_shortest_fat[i][j], g_shortest_fat[i][k] + g_shortest_fat[k][j]);
                }
            }
        }*/
        cerr << "finish floyd" << endl;
        // findNext();
        cerr << "finish get nexts" << endl;
        num_workbenches = SZ(workbenches);
        // tell the game that we are ready
        printf("OK\n");
        fflush(stdout);
    }
    void readFrameID_money() {
        scanf("%d%d", &frame_id, &money);
        SKIP_FRAMES = frame_id - FRAME_ID;
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
        for (Workbench &_buy_workbench : this->workbenches) {
            if (g_shortest_thin[robot.vertax_id][_buy_workbench.workbench_id] >= INF) {
                continue;
            }
            int frames_cost1 = int(g_shortest_thin[robot.vertax_id][_buy_workbench.workbench_id] / 6 * 50 * 1.2);
            if (!_buy_workbench.readyToBuy(frames_cost1)) {
                continue;
            }
            if (__builtin_popcount(_buy_workbench.book_mask & ~1) > 0 &&
                robot.workbench_id != _buy_workbench.workbench_id) {
                continue;
            }
            for (Workbench &_sell_workbench : this->workbenches) {
                if (g_shortest_fat[_buy_workbench.workbench_id][_sell_workbench.workbench_id] >= INF) {
                    continue;
                }
                int frames_cost2 =
                    int(g_shortest_fat[_buy_workbench.workbench_id][_sell_workbench.workbench_id] / 6 * 50);
                if (!_sell_workbench.readyToSell(_buy_workbench.sells(), frames_cost2)) {
                    continue;
                }
                double time1 = frames_cost1 / 50 * 2;
                double time2 = frames_cost2 / 50 * 2;
                int goods = _buy_workbench.sells();
                double weight = profit(goods, time1, time2, frame_id) * _sell_workbench.bonusRate(goods) *
                                type_rate[_sell_workbench.sells()] / (time1 + time2);
                // 好像vector<vector<int>>>不能用emplace_back，会报错，所以用push_back
                if (weight > 0) {
                    choices.emplace_back(node(_buy_workbench.workbench_id, _sell_workbench.workbench_id, weight));
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
        double time1 = g_shortest_thin[robot.vertax_id][buy_workbench_id] / 6 * 1.1;
        double time2 = g_shortest_fat[buy_workbench_id][sell_workbench_id] / 6 * 1.1;
        // book goods
        workbenches[sell_workbench_id].preBook(workbenches[buy_workbench_id].sells(), (time1 + time2) * 50);
        workbenches[buy_workbench_id].preBook(0, time1 * 50);
        robot.order = new Order(robot.vertax_id, buy_workbench_id, sell_workbench_id);
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
    Position bfsPos(Position des_pos, Position stand_pos, vector<vector<char>>& maps) {
        int m = maps.size();
        bool fat = (m == 98);
        vector<vector<char>> maps_stright = maps;
        vector<int> des_indexes = fat ? get_fat_map_indexes(des_pos) : get_thin_map_indexes(des_pos);
        vector<int> stand_indexes = fat ? get_fat_map_indexes(stand_pos) : get_thin_map_indexes(stand_pos);
        unordered_set<int> &vertaxes_set = fat ? g_fat_vertaxes_indexes : g_thin_vertaxes_indexes;
        int stand_i = stand_indexes[0], stand_j = stand_indexes[1];
        queue<vector<int>> q;
        q.push(des_indexes);
        int heading_i = des_indexes[0], heading_j = des_indexes[1];
        bool found = false;
        while (!q.empty()) {
            vector<int> block_indexes = q.front();
            int i = block_indexes[0], j = block_indexes[1];
            Position block_pos = fat ? get_fat_position_from_indexes(i, j) : get_thin_position_from_indexes(i, j);
            if (vertaxes_set.find(indexes_encoding(i, j)) != vertaxes_set.end() &&
                strightAllEmpty(block_pos, stand_pos, maps_stright)) {
                cerr << "(" << heading_i << ", " << heading_j << ")";
                return block_pos;
            }
            // stright
            if (i - 1 >= 0 && maps[i - 1][j] == '.') {
                if (i - 1 == stand_i && j == stand_j) {
                    found = true;
                }
                q.push({i - 1, j});
                maps[i - 1][j] = '#';
            }
            if (j + 1 < m && maps[i][j + 1] == '.') {
                if (i == stand_i && j + 1 == stand_j) {
                    found = true;
                }
                q.push({i, j + 1});
                maps[i][j + 1] = '#';
            }
            if (i + 1 < m && maps[i + 1][j] == '.') {
                if (i + 1 == stand_i && j == stand_j) {
                    found = true;
                }
                q.push({i + 1, j});
                maps[i + 1][j] = '#';
            }
            if (j - 1 >= 0 && maps[i][j - 1] == '.') {
                if (i == stand_i && j - 1 == stand_j) {
                    found = true;
                }
                q.push({i, j - 1});
                maps[i][j - 1] = '#';
            }
            if (found) {
                cerr << "found";
                heading_i = i;
                heading_j = j;
                break;
            }
            q.pop();
        }
        if (found) {
            // printMap(maps_stright);
            cerr << "heading:(" << heading_i << ", " << heading_j << "), stand: (" << stand_i << ", " << stand_j
                 << "), des: (" << des_indexes[0] << ", " << des_indexes[1] << ")" << endl;
            if (fat) {
                return get_fat_position_from_indexes(heading_i, heading_j);
            } else {
                return get_thin_position_from_indexes(heading_i, heading_j);
            }
        } else {
            if (MAP == 2) return {100,50};
            return {25, -78};
        }
    }
    void navigate_map() {
        for (int i = 0; i < 4; i++) {
            auto &robot = robots[i];
            Order &order = *robot.order;
            if (!robot.order) {
                Position des = {25, -100};
                robot.heading(des);
                continue;
            }
            if (robot.goods > 0) {  // robot carry the right goods, just sell it
                assert(robot.goods == workbenches[order.buy_workbench_id].workbench_type);
                order.bought = true;
                robot.todo = "sell";
                robot.vertax_id = order.buy_workbench_id;
                // sell suborder
                vector<vector<char>> fat_maps = g_fat_maps;
                // sign the robots
                for (int j = 0; j < 4; j++) {
                    if (j == i) {
                        continue;
                    }
                    vector<int> indexes = get_fat_map_indexes(robots[j].pos);
                    for (int row = indexes[0] - 1; row <= indexes[0] + 1; row++) {
                        for (int col = indexes[1] - 1; col <= indexes[1] + 1; col++) {
                            if (row >= 0 && row < 98 && col >= 0 && col < 98) {
                                fat_maps[row][col] = '#';
                            }
                        }
                    }
                }
                // printMap(fat_maps);
                Position des = bfsPos(workbenches[order.sell_workbench_id].pos, robot.pos, fat_maps);
                robot.heading(des);
            } else if (order.bought == true) {
                order.sold = true;
                robot.vertax_id = order.sell_workbench_id;
                robot.todo = "";
            } else {  // go to buy the goods
                robot.todo = "buy";
                // buy suborder
                vector<vector<char>> thin_maps = g_thin_maps;
                for (int j = 0; j < 4; j++) {
                    if (j == i) {
                        continue;
                    }
                    vector<int> indexes = get_thin_map_indexes(robots[j].pos);
                    for (int row = indexes[0] - 1; row <= indexes[0] + 1; row++) {
                        for (int col = indexes[1] - 1; col <= indexes[1] + 1; col++) {
                            if (row >= 0 && row < 99 && col >= 0 && col < 99) {
                                thin_maps[row][col] = '#';
                            }
                        }
                    }
                }
                // printMap(thin_maps);
                Position des = bfsPos(workbenches[order.buy_workbench_id].pos, robot.pos, thin_maps);
                // cerr << des.x << " " << des.y << " ;   ";
                robot.heading(des);
            }
            robot.buyAndSell(workbenches);
        }
    }
    void sendInstruct() {
        printf("%d\n", frame_id);
        for (auto &robot : robots) {
            if (!robot.hasOrder()) {
                // robot.speed_ = 0;
                // robot.rotate_ = 0;
            }
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
    for (int i = 0; i < END_FRAME; i++) {
        control_center.readFrameID_money();  // read the first line data of each frame
        control_center.readWorkbenchesStatus();
        control_center.readRobotStatus();
        control_center.formOrders();    // make a list of orders to do
        control_center.getOrders();     // a an order for each robot if they have none
        control_center.navigate_map();  // go to the destination, set the speed, rotation parameters
        control_center.sendInstruct();  // send the instructions to the game program
    }
    return 0;
}
