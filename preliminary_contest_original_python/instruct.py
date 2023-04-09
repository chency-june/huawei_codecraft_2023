import sys
from typing import List

from robot import Robot

class Instruct:
    def __init__(self, robots: List[Robot]):
        self.robots = robots    # all 4 robots
    
    def send(self, frame_id):
        """ send the instructions to the game program"""
        write = sys.stdout.write
        write(f"{frame_id}\n")
        for robot in self.robots:
            # forward speed m/s
            write(f"forward {robot.robot_id} {robot.speed_}\n")
            # rotate speed r/s
            write(f"rotate {robot.robot_id} {robot.rotate_}\n")
            if robot.sell_:
                write(f"sell {robot.robot_id}\n")
            if robot.destroy_:
                write(f"destroy {robot.robot_id}\n")
            if robot.buy_:
                write(f"buy {robot.robot_id}\n")
        write("OK\n")
        sys.stdout.flush()