import sys
from math import sqrt

from robot import Robot
from workbench import Workbench
from order import Order
from instruct import Instruct
from utils import dist, pi, Position, profit, rotate, direction, time_pretiction

readline = sys.stdin.readline

class ControlCenter:
    def __init__(self) -> None:
        self.robots = []    # robots list
        self.workbenches   = []    # workbenches list
        self.instruct = Instruct([])    # send the instructions

    def readMaps(self):
        """ read the maps from stdin. """
        # rand the map data to an 2d array `string_maps`
        string_maps = []
        for i in range(100):
            line = sys.stdin.readline().strip()
            string_maps.append(line)
        line = sys.stdin.readline().strip() # last line should be `OK`
        assert line == "OK"

        # record each robot and each workbench
        for i in range(100):
            for j in range(100):
                if string_maps[i][j] == '.':
                    continue
                x = j * 0.5 + 0.25
                y = (99 - i) * 0.5 + 0.25
                if string_maps[i][j] == 'A':
                    robot = Robot(len(self.robots), x, y, 0)
                    self.robots.append(robot)
                else:
                    workbench_type = int(string_maps[i][j])
                    workbench = Workbench(len(self.workbenches), workbench_type, x, y)
                    self.workbenches.append(workbench)
        self.instruct.robots = self.robots
        self.num_workbenches = len(self.workbenches)

        # self.dist stored the distance between any two workbenches
        self.dist = [[100.] * self.num_workbenches for _ in range(self.num_workbenches)]
        for i in range(self.num_workbenches):
            for j in range(self.num_workbenches):
                distance = dist(self.workbenches[i].pos, self.workbenches[j].pos)
                self.dist[i][j] = distance

        # make a 7th workbench list
        self.workbench7s = [workbench for workbench in self.workbenches if workbench.workbench_type == 7]

        # tell the game that we are ready
        sys.stdout.write("OK")
        sys.stdout.flush()

    def readFrameID_money(self) -> bool:
        """ read the first line data: frame ID and money we have now. """
        line = sys.stdin.readline()
        if not line:
            return False
        parts = line.split(' ')
        self.frame_id = int(parts[0])
        self.money = int(parts[1])
        return True

    def readWorkbenchesStatus(self):
        """ read status of each workbench """
        num_workbenches = int(readline().strip())
        assert num_workbenches == self.num_workbenches
        for i in range(num_workbenches):
            line = readline().strip()
            self.workbenches[i].updateStatus(line)

    def readRobotStatus(self):
        """ read status of each robot """
        for i in range(4):
            status = readline().strip()
            self.robots[i].updateStatus(status)
        line = readline().strip()
        assert line == 'OK'

    def formOrders(self):
        """ generate a list of orders for the robot """
        #@TODO
        self.type_rate = {workbench_type: 1 for workbench_type in range(1, 10)}
        for workbench7 in self.workbench7s:
            for goods in range(4, 7):
                self.type_rate[goods] = max(workbench7.needRate(goods), self.type_rate[goods])
        # sys.stderr.write(str(self.type_rate))

    def getOrders(self):
        """ get an order for each robot """
        for robot in self.robots:
            if not robot.hasOrder():
                self.getOrder(robot)
    
    def getOrder(self, robot: Robot) -> None:
        """ get an order for the specific robot """
        """ it shows an stupid way to get order, NEED BETTER WAYS """
        choices = []
        for buy_workbench in self.workbenches:
          dist1 = dist(robot.pos, buy_workbench.pos)
          if buy_workbench.readyToBuy(time_pretiction(robot.toward, robot.speed, robot.pos, buy_workbench.pos) * 50):
            for sell_workbench in self.workbenches:
              if sell_workbench.readyToSell(buy_workbench.sells()):
                  time1 = time_pretiction(robot.toward, robot.speed, robot.pos, buy_workbench.pos)
                  time2 = time_pretiction(direction(robot.pos, buy_workbench.pos), 6, buy_workbench.pos, sell_workbench.pos)
                  goods = buy_workbench.sells()
                  weight = profit(goods, time1, time2, self.frame_id) * \
                           sell_workbench.bonusRate(goods) * \
                           self.type_rate[goods] \
                           / (time1 + time2)
                  if weight > 0:
                    choices.append((buy_workbench.workbench_id, sell_workbench.workbench_id, weight))
        choices.sort(key=lambda x: x[2], reverse=True)
        if len(choices) == 0:
            robot.order = None
            return
        choice = choices[0]
        buy_workbench_id, sell_workbench_id, weight = choice
        self.workbenches[sell_workbench_id].preBook(self.workbenches[buy_workbench_id].sells())
        self.workbenches[buy_workbench_id].preBook(0)
        robot.order = Order(buy_workbench_id, sell_workbench_id)

    def navigate(self):
        """  """
        for robot in self.robots:
            # get the destination: go to buy workbench or go to sell workbench
            order = robot.order
            if order == None:
                robot.destination = Position(25, 25)
                continue
            if robot.goods > 0: # robot carry the right goods, just sell it
                assert robot.goods == self.workbenches[order.buy_workbench_id].workbench_type
                order.bought = True
                robot.destination = self.workbenches[order.sell_workbench_id].pos
                robot.todo = "sell"
            elif order.bought == True:  # finish the order
                order.sold = True
                robot.todo = None
            else:   # go to buy the goods
                robot.destination = self.workbenches[order.buy_workbench_id].pos
                robot.todo = "buy"# if self.frame_id < 8700 else None
                # if self.frame_id > 8700:
                #     robot.destination = Position(0,0)
            # set the speed and roration and buy or sell parameters for the robot
            robot.heading(self.workbenches)
            # boom detetion
        self.detectBoom()
    
    def detectBoom(self):
        controlled = []
        for robot in self.robots:
          if robot.robot_id not in controlled:
            for detected_robot in self.robots:
              if detected_robot.robot_id != robot.robot_id:
                safe_dist = robot.radius() + detected_robot.radius() - 0.01
                will_BOOM = any(dist(next_pos, obstacle) < safe_dist 
                                    for obstacle in detected_robot.followings 
                                        for next_pos in robot.followings)
                if will_BOOM:
                  he_turn_to_me = rotate(detected_robot.toward, direction(detected_robot.pos, robot.pos))
                  i_turn_to_him = rotate(robot.toward, direction(robot.pos, detected_robot.pos))
                  near_destination = robot.nearDestination() and detected_robot.nearDestination()
                  if he_turn_to_me * i_turn_to_him < 0 and dist(robot.pos, detected_robot.pos) > safe_dist + 0.4:
                    controlled.append(detected_robot.robot_id)
                    # he stop
                    if abs(he_turn_to_me) < abs(i_turn_to_him):
                        detected_robot.speed_ = -2 if near_destination else 0
                    else:   # I stop
                        robot.speed_ = -2 if near_destination else 0
                  elif not near_destination:
                    if rotate(robot.toward, direction(robot.pos, detected_robot.pos)) > 0:
                        robot.rotate_ = -pi
                    else:
                        robot.rotate_ = pi
                    break

    def sendInstruct(self):
        self.instruct.send(self.frame_id)   # send instructions to the game
    