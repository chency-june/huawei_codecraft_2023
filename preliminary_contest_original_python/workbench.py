from typing import List
import sys
from utils import Position

# what goods the workbenches need (not used yet)
buys = {4: [1,2],
        5: [1,3],
        6: [2,3],
        7: [4,5,6],
        8: [7],
        9: [1,2,3,4,5,6,7]}

class Workbench:
    """ workbench: Should I rename `workbench` to `workbench` ? """
    def __init__(self, workbench_id: int, workbench_type: int, x: float, y: float):
        self.workbench_id     = workbench_id        # unique id [0, 49]
        self.workbench_type   = workbench_type      # type [1, 9]
        self.pos        = Position(x, y)# the position 
        self.book_mask = 0

    def updateStatus(self, status: str) -> None:
        """ update status from stdin """
        digits = status.split()
        assert int(digits[0])   == self.workbench_type
        assert float(digits[1]) == self.pos.x
        assert float(digits[2]) == self.pos.y
        self.remaining   = int(digits[3])
        self.buy_mask    = int(digits[4])
        self.product     = bool(int(digits[5]))  # if there is a product to sell
        # if not self.product:
        #     self.book_mask &= ~1
        # self.book_mask &= ~self.buy_mask

    def needRate(self, goods: int) -> float:
        if self.workbench_type != 7:
            return 1
        if goods not in self.buys():
            return 1
        have_got_nums = sum(1 for x in bin((self.buy_mask | self.book_mask) & (~1)) if x == '1')
        # sys.stderr.write(str(have_got_nums))
        need_bonus_rate = {1: 1.1,
                           2: 1.5}
        if ((self.buy_mask | self.book_mask) & (1 << goods)) == 0 and have_got_nums != 0:
            # sys.stderr.write("go into it")
            return need_bonus_rate.get(have_got_nums, 1)
        else:
            return 1

    def bonusRate(self, goods: int) -> float:
        if self.workbench_type == 9:
            return 1
        if self.workbench_type not in [4,5,6,7]:
            return 1
        if goods not in self.buys():
            return 0
        have_got_nums = sum(x == '1' for x in bin((self.buy_mask | self.book_mask) & (~1)))
        # sys.stderr.write(str(have_got_nums) + " ")
        # sys.stderr.flush()
        if self.workbench_type == 7:
            bonus_rate_dict = {1: 1.6,
                               2: 2.3}
        else:
            bonus_rate_dict = {1: 1.6}
        return bonus_rate_dict.get(have_got_nums, 1)

    def buys(self) -> List[int]:
        return buys.get(self.workbench_type, [])
    def sells(self) -> int:
        return self.workbench_type if self.workbench_type <= 7 else 0
    
    def readyToBuy(self, frames_later: float) -> bool:
        # frames_later = 0
        if self.workbench_type in [1, 2, 3]:
            return True
        if self.book_mask & 1 != 0:
            return False
        if self.product:
            return True
        else:
            return self.remaining > 0 and frames_later > self.remaining
    def readyToSell(self, goods: int, frames_later: float=0) -> bool:
        if goods not in self.buys():
            return False
        if self.workbench_type in [8, 9]:
            return True
        if ((self.buy_mask | self.book_mask) & (1 << goods)) != 0:
            return False
        else:
            return True
    def preBook(self, goods: int) -> None:
        self.book_mask |= (1 << goods)
    def unbook(self, goods: int) -> None:
        self.book_mask &= ~(1 << goods)
    @staticmethod
    def whoSells(self, goods_id):
        assert 1 <= goods_id <= 7
        return goods_id
    
    @staticmethod
    def whoBuys(self, goods_id):
        return [workbench for workbench, workbench_buys in buys.items() if goods_id in workbench_buys]