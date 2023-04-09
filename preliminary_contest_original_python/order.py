

class Order:
    def __init__(self, buy_workbench_id: int, sell_workbench_id: int):
        self.buy_workbench_id = buy_workbench_id    # which workbench to buy the goods
        self.sell_workbench_id= sell_workbench_id   # which workbench to sell the goods
        self.bought     = False         # whether the goods was bought
        self.sold       = False         # whether the goods was sold
    
    def complete(self):
        return self.bought and self.sold

