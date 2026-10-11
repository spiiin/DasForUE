options gen2
require ue_delegate_examples
[export]
def main(input:int):int {
    let query=FScoreQuery()
    return query.Execute(input)
}
