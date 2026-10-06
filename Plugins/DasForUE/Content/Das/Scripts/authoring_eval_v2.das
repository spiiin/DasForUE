options gen2
require ue_foundation
[export]
def main(input:int):int { return ue_get_self_int("Score") + input + ue_get_self_int("Bonus") + 100 }
