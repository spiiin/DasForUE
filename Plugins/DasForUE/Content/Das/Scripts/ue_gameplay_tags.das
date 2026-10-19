options gen2
module ue_gameplay_tags public
require ue_foundation public
[ue_gameplay_value]
struct GameplayTag {value:UEStructValue}
[ue_gameplay_value]
struct GameplayTagContainer {value:UEStructValue}

def ue_tag(name:string):GameplayTag {return GameplayTag(value=__ue_tag_request(name,true))}
def ue_tag():GameplayTag {return GameplayTag(value=__ue_tag_request("",false))}
def ue_find_tag(name:string):GameplayTag {return GameplayTag(value=__ue_tag_request(name,false))}
def ue_tag_valid(tag:GameplayTag):bool {return __ue_tag_valid(tag.value)}
def ue_tag_name(tag:GameplayTag):string {return __ue_tag_name(tag.value)}
def ue_tag_matches(tag:GameplayTag;parent:GameplayTag;exact:bool=false):bool {return __ue_tag_matches(tag.value,parent.value,exact)}
def ue_tags():GameplayTagContainer {return GameplayTagContainer(value=__ue_tag_check(ue_struct_value("/Script/GameplayTags.GameplayTagContainer"),true))}
def ue_tags_add(var tags:GameplayTagContainer;tag:GameplayTag):void {tags.value=__ue_tags_with(tags.value,tag.value,true)}
def ue_tags_remove(var tags:GameplayTagContainer;tag:GameplayTag):void {tags.value=__ue_tags_with(tags.value,tag.value,false)}
def ue_tags(first:GameplayTag):GameplayTagContainer {var result=ue_tags();ue_tags_add(result,first);return result}
def ue_tags(first:GameplayTag;second:GameplayTag):GameplayTagContainer {var result=ue_tags(first);ue_tags_add(result,second);return result}
def ue_tags(items:array<GameplayTag>):GameplayTagContainer {
 var result=ue_tags()
 for (tag in items) {ue_tags_add(result,tag)}
 return result
}
def ue_tags_count(tags:GameplayTagContainer):int {return __ue_tags_count(tags.value)}
def ue_tags_has(tags:GameplayTagContainer;tag:GameplayTag;exact:bool=false):bool {return __ue_tags_has(tags.value,tag.value,exact)}
def ue_tags_has_any(tags:GameplayTagContainer;query:GameplayTagContainer;exact:bool=false):bool {return __ue_tags_has_set(tags.value,query.value,false,exact)}
def ue_tags_has_all(tags:GameplayTagContainer;query:GameplayTagContainer;exact:bool=false):bool {return __ue_tags_has_set(tags.value,query.value,true,exact)}
def ue_tags_names(tags:GameplayTagContainer):array<string> {
 let snapshot=__ue_tags_names(tags.value)
 var result:array<string>
 for (index in range(ue_struct_array_size(snapshot,"Values"))) {push(result,ue_struct_get_string(snapshot,"Values."+string(index)))}
 return <- result
}
