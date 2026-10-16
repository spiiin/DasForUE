options gen2
require dsl_math_library
[export]
def main(input:int):int {
    if (GameMath`DefaultAdd()!=5 || GameMath`DefaultAdd(10)!=8 || GameMath`DefaultAdd(10,3)!=13) { return -101 }
    if (GameMath`DefaultDouble()!=1.23456789012345lf || GameMath`DefaultFloat()!=-0.125) { return -102 }
    if (GameMath`DefaultBool() || GameMath`DefaultText()!="Пример 世界" || GameMath`DefaultEmpty()!="") { return -103 }
    var counter=input
    var doubled=0
    GameMath`Stats(counter,doubled)
    return GameMath`Add(counter,doubled)
}
