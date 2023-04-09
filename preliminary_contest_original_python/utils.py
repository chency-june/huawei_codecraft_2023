import math
import random

pi = math.pi    # 3.1415926

class Position:
    """ define a position class """
    def __init__(self, x: float, y: float):
        self.x  = x
        self.y  = y

    def update(self, x: float, y: float):
        self.x  = x
        self.y  = y

def dist(a: Position, b: Position) -> float:
    """ return the distance between a and b. """
    delta_x = a.x - b.x
    delta_y = a.y - b.y
    return math.sqrt(delta_x * delta_x + delta_y * delta_y)

def discont(x: float, maxX: int, minRate: float) -> float:
    if x < maxX:
        return (1 - math.sqrt(1 - (1 - x / maxX) ** 2)) * (1 - minRate) + minRate
    else:
        return minRate

def direction(_stand: Position, _toward: Position) -> float:
    return math.atan2(_toward.y - _stand.y, _toward.x - _stand.x)

def rotate(_from: float, _to: float) -> float:
    # make sure that the direction difference is in half a circle
    if abs(_to - _from) > pi:
        if (_to + 2 * pi - _from) <= pi:
            _to += 2 * pi
        else:
            _to -= 2 * pi
    assert abs(_to - _from) <= pi
    return _to - _from

def time_pretiction(toward: float, speed: float, _stand: Position, _des: Position) -> float:
    rotate0 = rotate(toward, direction(_stand, _des))
    rotate0 = max(0, abs(rotate0) - pi / 2)
    if rotate0 > 0:
        speed = 0
    t0 = rotate0 / pi + 0.04 if rotate0 > 0 else 0
    distance = dist(_stand, _des)
    t1 = distance / 6 + 0.1
    t1 -= speed / 16 - speed ** 2 / (2 * 16) / 6
    return t0 + t1

def profit(goods: int, time1: float, time2:float, frame_id: int) -> float:
    """ the profit we can get to sell the goods. """
    buy_price = {1: 3000,
                 2: 4400,
                 3: 5800,
                 4: 15400,
                 5: 17200,
                 6: 19200,
                 7: 76000}
    sell_price = {1: 6000,
                  2: 7600,
                  3: 9200,
                  4: 22500,
                  5: 25000,
                  6: 27500,
                  7: 105000}
    total_time = time1 + time2
    if frame_id + total_time * 50 + 50 > 9000:
        return -1
    def time_lambda(time: float) -> float:
        return discont(time, 9000, 0.8)
    return time_lambda(time2) * sell_price.get(goods, 0) - buy_price.get(goods, 0)