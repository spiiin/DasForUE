options gen2
require ue_dsl
require ue_soft_references

[ue_class(Blueprintable,primary_asset_type="Item")]
class ItemDefinition : PrimaryDataAsset {
    properties=ue_properties(EditAnywhere,BlueprintReadWrite) {
        // Item score configured in each asset instance.
        var Score:int=10
        var Label:string="Potion"
        // Loaded with the UI bundle; an empty reference adds no dependency.
        var @AssetBundles="UI" Icon:SoftObject
    }
    [ue_function(BlueprintPure)]
    def ComputeValue(amount:int):int {return self.Score+amount}
}

// Ordinary data assets use the same field DSL; methods are optional.
[ue_class(Blueprintable)]
class TuningDefinition : DataAsset {
    properties=ue_properties(EditAnywhere,BlueprintReadWrite) {
        var Strength:double=1.5lf
    }
}
