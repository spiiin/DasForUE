options gen2
require ue_foundation

[export]
def baseline(count:int):int {
    let value=ue_get_self_struct("Snapshot")
    var sum=0
    for (i in range(0,count)) { sum+=i & 7 }
    return sum+ue_struct_map_get_int(value,"IntMap",0)
}
[export]
def read_queries(count:int):int {
    let value=ue_get_self_struct("Snapshot")
    var sum=0
    for (i in range(0,count)) {
        sum+=ue_struct_map_size(value,"IntMap")+ue_struct_map_get_int(value,"IntMap",0)
        if (ue_struct_set_contains_int(value,"IntSet",0)) { sum+=1 }
        sum+=int(ue_struct_get_byte(value,"Rows.0.Priority"))
    }
    return sum
}
[export]
def keys(count:int):int {
    let value=ue_get_self_struct("Snapshot")
    let size=ue_struct_map_size(value,"IntMap")
    var sum=0
    for (i in range(0,count)) { sum+=ue_struct_map_key(value,"IntMap",i % size) }
    return sum
}
[export]
def single_edit(count:int):int {
    let original=ue_get_self_struct("Snapshot")
    var sum=0
    for (i in range(0,count)) {
        let changed=ue_struct_map_with_int(original,"IntMap",0,i+1)
        sum+=ue_struct_map_get_int(changed,"IntMap",0)
    }
    verify(ue_struct_map_get_int(original,"IntMap",0)==0)
    return sum
}
[export]
def four_edits(count:int):int {
    let original=ue_get_self_struct("Snapshot")
    var sum=0
    for (i in range(0,count)) {
        var changed=ue_struct_map_with_int(original,"IntMap",0,i+1)
        changed=ue_struct_map_with_int(changed,"IntMap",1,i+2)
        changed=ue_struct_map_with_int(changed,"IntMap",2,i+3)
        changed=ue_struct_map_with_int(changed,"IntMap",3,i+4)
        sum+=ue_struct_map_get_int(changed,"IntMap",3)
    }
    verify(ue_struct_map_get_int(original,"IntMap",0)==0)
    return sum
}
[export]
def vm_to_ue(count:int):int {
    let target=ue_self_object()
    var sum=0
    for (i in range(0,count)) { sum+=ue_call_int(target,"AddHealth",0) }
    return sum
}
[export]
def scalar(value:int):int { return value+1 }

// Positive control for heap allocation counters, outside snapshot storage.
[export]
def vm_allocations(count:int):int {
    var values:array<int>
    resize(values,count)
    for (i in range(0,count)) { values[i]=i }
    return int(length(values))
}

// Same four writes as four_edits, on one private copy and with one publication.
[export]
def batch_four_edits(count:int):int {
    let original=ue_get_self_struct("Snapshot")
    var sum=0
    for (i in range(0,count)) {
        let changed=ue_struct_edit(original) $(var draft:UEStructValue) {
            draft=ue_struct_map_with_int(draft,"IntMap",0,i+1)
            draft=ue_struct_map_with_int(draft,"IntMap",1,i+2)
            draft=ue_struct_map_with_int(draft,"IntMap",2,i+3)
            draft=ue_struct_map_with_int(draft,"IntMap",3,i+4)
        }
        sum+=ue_struct_map_get_int(changed,"IntMap",3)
    }
    verify(ue_struct_map_get_int(original,"IntMap",0)==0)
    return sum
}
