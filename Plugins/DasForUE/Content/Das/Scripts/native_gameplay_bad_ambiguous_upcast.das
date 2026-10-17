options gen2
require ue_native_gameplay
def choose(left:NativePawnRef; right:ActorRef):int { return 1 }
def choose(left:ActorRef; right:NativePawnRef):int { return 2 }
[export]
def main():bool {
    let character=ue_character_self()
    return choose(character,character)==1
}
