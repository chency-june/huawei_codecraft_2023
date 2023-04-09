import typing
import math
import sys

from order import Order
from utils import Position, dist, pi, direction, rotate

class Robot:
    def __init__(self, robot_id: int, x: float, y: float, toward: float):
        self.robot_id   = robot_id          # unique id range[0,3]
        self.pos        = Position(x, y)    # the position of the robot
        self.toward     = toward            # the direction of the robot [-PI, PI]
        self.goods      = 0                 # the goods carried by the robot, 0 stands for no goods

        self.followings = [self.pos]    # following positions for boom detection

        # order
        self.order      = None  # the order to finish
        self.next_order = None  # the order to do when the first order finished (not used yet)

        # destination
        self.destination= Position(25, 42)  # the destination to go to
        self.todo       = None  # 'buy' or 'sell' or None

        # instruct
        self.speed_     = 6     # the speed we set for the robot
        self.rotate_    = 0     # the rotate we set for the robot
        self.buy_       = False # whether to buy the goods
        self.sell_      = False # whether to sell the goods
        self.destroy_   = False # whether to destroy the goods

    def updateStatus(self, status: str):
        """ status: the input string line """
        status_list = status.split()
        self.workbench_id      = int(status_list[0])
        self.goods       = int(status_list[1])
        # sys.stderr.write(f"robot[{self.robot_id}.goods = {self.goods}]")
        self.time_lambda = float(status_list[2])
        self.boom_lambda = float(status_list[3])
        self.omega       = float(status_list[4])
        self.speed_x     = float(status_list[5])
        self.speed_y     = float(status_list[6])
        if abs(self.omega) <= pi / 2 and self.speed_x < 0:
            self.speed_x = 0
        if self.omega * self.speed_y < 0:
            self.speed_y = 0
        self.speed = math.sqrt(self.speed_x ** 2 + self.speed_y ** 2)
        self.toward      = float(status_list[7])
        x   = float(status_list[8])
        y   = float(status_list[9])
        self.pos.update(x, y)
        self.followings = [self.nextPos(i) for i in range(1, 18)]

    def heading(self, workbenches) -> None:
        """ set the instructions. """
        
        toward_ = direction(self.pos, self.destination)  # the direction of the destination from the robot's stand
        rotate_ = rotate(self.toward, toward_)
        if abs(rotate_) < 0.1:    # stright to the detination, just go stright
            self.rotate_ = 0
        else: 
            # rotate in the right direction
            self.rotate_ = pi if rotate_ > 0 else -pi
        
        self.speed_ = dist(self.pos, self.destination) * pi / (2 * math.sin(abs(rotate_))) if self.rotate_ != 0 else 6

        if abs(rotate_) > pi * 0.62:
            self.speed_ = -2
        elif abs(rotate_) > pi * 0.5:
            self.speed_ = 0
        
        if self.todo == "buy" and self.workbench_id == self.order.buy_workbench_id:
            self.buy_ = True
            if workbenches[self.workbench_id].product:
                workbenches[self.workbench_id].unbook(0)
        else:
            self.buy_ = False
        if self.todo == "sell" and self.workbench_id == self.order.sell_workbench_id:
            self.sell_ = True
            if (workbenches[self.workbench_id].buy_mask & (1 << self.goods)) == 0:
                workbenches[self.workbench_id].unbook(self.goods)
        else:
            self.sell_ = False
    
    def hasOrder(self) -> bool:
        """ to check if the robot has an unfinished order """
        if self.order is None and self.next_order is None:
            return False
        if self.order is not None and self.order.complete():
            if self.next_order is not None:
                self.order = self.next_order
                self.next_order = None
                return True
            else:
                self.order = None
                return False
        else:
            return True

    def nextPos(self, frames: int) -> Position:
        x = self.pos.x + self.speed_x * frames / 50
        y = self.pos.y + self.speed_y * frames / 50
        return Position(x, y)
    
    def radius(self) -> float:
        return 0.53 if self.goods > 0 else 0.45
    
    def nearDestination(self, distance: float = 1):
        return dist(self.pos, self.destination) < distance