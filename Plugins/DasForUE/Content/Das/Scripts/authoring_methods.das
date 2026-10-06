options gen2
require ue_foundation
var calls = 0
[export]
def inspect(input:int):int { calls += 1; return ue_get_self_int("Locked") + input + calls }
[export]
def inspect_again(input:int):int { calls += 1; return input + calls + 1000 }
[export]
def readonly_write(input:int):int { ue_set_self_int("Locked", 99); return 1 }
