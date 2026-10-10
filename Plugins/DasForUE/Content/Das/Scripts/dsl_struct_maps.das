options gen2
options gc
require ue_dsl
require ue_struct_map_types

// Import ue_struct_map_types.das into this folder before importing this class.
[ue_class(module_id="structmaps", type_folder="/Game/DasStructMaps", Blueprintable)]
class StructMapActor {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Entries:table<string;MapEntry>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Positions:table<int;UEVector>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Rotations:table<string;UERotator>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Transforms:table<int;UETransform>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Catalog:MapCatalog=MapCatalog()
    @ue_property=(EditAnywhere, BlueprintReadOnly)
    LockedEntries:table<string;MapEntry>

    [ue_function(BlueprintCallable)]
    def Put(name:string; entry:MapEntry):void {
        insert_clone(self.Entries,name,entry)
    }
    [ue_function(BlueprintCallable)]
    def Increase(name:string; amount:int):void {
        self.Entries[name].Score += amount
        push(self.Entries[name].Samples,double(amount))
        self.Entries[name].Position.x += double(amount)
    }
    [ue_function(BlueprintCallable)]
    def PutPosition(index:int; position:UEVector):void {
        insert_clone(self.Positions,index,position)
    }
    [ue_function(BlueprintCallable)]
    def CopyEntries(values:table<string;MapEntry>):table<string;MapEntry> {
        return <- clone(values)
    }
    [ue_function(BlueprintCallable)]
    def ReplaceEntries(var values:table<string;MapEntry>&;
                       @ue_out var previous:table<string;MapEntry>&):void {
        previous := self.Entries
        self.Entries := values
        clear(values)
    }
}
