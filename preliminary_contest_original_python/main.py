#!/bin/bash
import sys

from control_center import ControlCenter
from order import Order
from robot import Robot
from instruct import Instruct
from workbench import Workbench

if __name__ == '__main__':
    control_center = ControlCenter()
    control_center.readMaps()   
    for _ in range(9000):
        control_center.readFrameID_money() # read the first line data of each frame
        control_center.readWorkbenchesStatus()
        control_center.readRobotStatus()
        control_center.formOrders() # make a list of orders to do 
        control_center.getOrders()  # a an order for each robot if they have none
        control_center.navigate()   # go to the destination, set the speed, rotation parameters
        control_center.sendInstruct()   # send the instructions to the game program
