options gen2
require ue_dsl
require ue_gameplay_tags
[ue_object_ref(script="gameplaytag_items.das",type="TaggedItem")]
struct ItemRef {handle:UObjectHandle}
var saved_tag=GameplayTag()
var saved_tags=GameplayTagContainer()
[export]
def exercise(input:int):int {
    let sword=ue_tag("Item.Weapon.Sword")
    let potion=ue_tag("Item.Consumable.Potion")
    let weapon=ue_tag("Item.Weapon")
    let root=ue_tag("Item")
    if (!ue_tag_valid(sword) || ue_tag_name(sword)!="Item.Weapon.Sword") {return -1}
    if (!ue_tag_matches(sword,weapon) || ue_tag_matches(sword,weapon,true) || ue_tag_matches(weapon,sword) || !ue_tag_matches(sword,sword,true)) {return -2}
    if (ue_tag_valid(ue_find_tag("DasForUE.Unregistered.Smoke")) || ue_tag_valid(ue_tag())) {return -3}
    var rejected=false
    try {ue_tag("DasForUE.Unregistered.Smoke")} recover {rejected=true}
    if (!rejected) {return -4}
    var tags=ue_tags(sword,potion)
    if (ue_tags_count(tags)!=2 || !ue_tags_has(tags,weapon) || ue_tags_has(tags,weapon,true) || !ue_tags_has(tags,sword,true)) {return -5}
    if (!ue_tags_has_all(tags,ue_tags(weapon,root)) || ue_tags_has_all(tags,ue_tags(weapon),true) || !ue_tags_has_any(tags,ue_tags(weapon),false)) {return -6}
    if (!ue_tags_has_all(tags,ue_tags()) || ue_tags_has_any(tags,ue_tags())) {return -7}
    var original=tags
    ue_tags_add(tags,sword)
    ue_tags_remove(tags,potion)
    if (ue_tags_count(tags)!=1 || ue_tags_count(original)!=2 || ue_tags_has(tags,potion)) {return -8}
    var listed:array<GameplayTag>
    push(listed,potion)
    push(listed,sword)
    push(listed,sword)
    if (ue_tags_count(ue_tags(listed))!=2 || !ue_tags_has_all(ue_tags(listed),original,true)) {return -16}
    let names=ue_tags_names(original)
    if (length(names)!=2 || names[0]!="Item.Consumable.Potion" || names[1]!="Item.Weapon.Sword") {return -9}
    rejected=false
    try {__ue_tag_check(sword.value,true)} recover {rejected=true}
    if (!rejected) {return -10}
    rejected=false
    try {ue_struct_with_name(sword.value,"TagName",ue_name_value("DasForUE.Unregistered.Smoke"))} recover {rejected=true}
    if (!rejected) {return -11}
    rejected=false
    try {ue_struct_array_resize(original.value,"ParentTags",0)} recover {rejected=true}
    if (!rejected || !ue_tags_has(original,weapon)) {return -12}
    let item=ue_ref(type<ItemRef>,ue_self_object())
    item.ConfigureSword()
    if (!ue_tag_matches(item.Kind,sword,true)) {return -13}
    if (ue_tags_count(item.ItemTags)!=1) {return -17}
    if (!item.Matches(weapon) || item.Matches(potion)) {return -18}
    if (!ue_tag_matches(item.EchoTag(potion),potion,true) || !ue_tags_has_all(item.EchoTags(original),original,true)) {return -14}
    var exchange=potion
    item.Exchange(exchange)
    if (!ue_tag_matches(exchange,sword,true) || !ue_tag_matches(item.Kind,potion,true)) {return -15}
    item.ItemTags=original
    saved_tag=exchange
    saved_tags=original
    return 1
}
[export]
def after_gc(input:int):int {
    return (ue_tag_name(saved_tag)=="Item.Weapon.Sword" && ue_tags_count(saved_tags)==2 && ue_tags_has(saved_tags,ue_tag("Item.Weapon"))) ? 1 : -1
}
