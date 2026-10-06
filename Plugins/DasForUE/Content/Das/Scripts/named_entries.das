options gen2
var calls=0
[export]
def alpha(input:int):int { calls += 1; return calls + input }
[export]
def beta(input:int):int { calls += 1; return calls + input + 10 }
[export]
def wrong(input:float):float { return input }
