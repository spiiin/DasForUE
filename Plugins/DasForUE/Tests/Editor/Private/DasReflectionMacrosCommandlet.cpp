#include "DasReflectionMacrosCommandlet.h"
#include "DasAuthoring.h"
#include "DasSourceCatalog.h"
#include "DasInterfaceService.h"
#include "Engine/Blueprint.h"
#include "DasRuntime.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "HAL/FileManager.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/Script.h"
UDasReflectionMacrosCommandlet::UDasReflectionMacrosCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasReflectionMacrosCommandlet::Main(const FString& Params)
{
    int32 Checks = 0, Failures = 0;
    auto Check = [&](bool OK, const TCHAR* Name, const FString& Error = FString())
    {
        ++Checks;
        Failures += !OK;
        UE_LOG(LogTemp, Display, TEXT("DAS_REFLECTION_MACRO_%s %s %s"), OK ? TEXT("PASS") : TEXT("FAIL"), Name, *Error);
    };
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Repo = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("../.."));
    const FString Path =
        R.ScriptPath(TEXT("reflection_macro_") + FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT(".das"));
    ON_SCOPE_EXIT
    {
        IFileManager::Get().Delete(*Path, false, true);
    };
    FString Source;
    if (!FFileHelper::LoadFileToString(Source, *(Repo / TEXT("Tests/ReflectionMacros/probe.das"))))
        return 2;
    auto Write = [&](const FString& Text)
    {
        return FFileHelper::SaveStringToFile(Text, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    };
    const FString CatalogPath = R.ScriptPath(TEXT("ue_type_catalog.json"));
    TArray<uint8> OriginalCatalog;
    const bool HadCatalog = FFileHelper::LoadFileToArray(OriginalCatalog, *CatalogPath);
    ON_SCOPE_EXIT
    {
        if (HadCatalog) FFileHelper::SaveArrayToFile(OriginalCatalog, *CatalogPath);
        else IFileManager::Get().Delete(*CatalogPath, false, true);
    };
    FDasSourceCatalog Catalog;
    Catalog.Entries.Add({TEXT("inheritance.BaseCounter"),TEXT("BaseCounter"),TEXT("ue_inheritance_base.das"),TEXT("FirstCounterRef")});
    Catalog.Entries.Add({TEXT("other.BaseCounter"),TEXT("BaseCounter"),TEXT("unused_counter.das"),TEXT("OtherCounterRef")});
    auto SaveCatalog = [&](const FDasSourceCatalog& Value)
    {
        FString Json, Issue;
        return Value.ToJson(Json, Issue) && FFileHelper::SaveStringToFile(Json, *CatalogPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    };
    if (!SaveCatalog(Catalog)) return 2;
    const FString ExplicitReference = TEXT("UEObjectRef(\"ue_inheritance_base.das\", \"BaseCounter\")");
    const FString CatalogReference = TEXT("UEObjectRef(\"inheritance.BaseCounter\")");
    const FString CatalogSource = Source.Replace(*ExplicitReference, *CatalogReference);
    Write(Source);
    FDasAuthoringModule Model;
    FString Error;
    Check(R.ValidateIntMethod(Path, Error, TEXT("main")), TEXT("compile_three_macros"), Error);
    auto Reject = [&](const FString& Text, const TCHAR* Expected, const TCHAR* Name)
    {
        Write(Text);
        Error.Reset();
        Check(!R.ValidateIntMethod(Path, Error, TEXT("main")) && Error.Contains(Expected), Name, Error);
    };
    Reject(Source.Replace(TEXT("GameplayStatics.GetTimeSeconds"), TEXT("GameplayStatics.NoSuchFunction")),
                          TEXT("Unknown callable static"), TEXT("unknown_static_rejected"));
    Reject(Source.Replace(TEXT("KismetMathLibrary.Abs"), TEXT("DasFunctionLibraryFixture.UnsupportedInt64")),
                          TEXT("No generated binding"), TEXT("unsupported_transport_rejected"));
    Reject(Source.Replace(TEXT("ue_call(type<DasProbeInterface>, Query,"),
                               TEXT("ue_call(type<DasProbeInterface>, Missing,")),
                          TEXT("Missing"), TEXT("unknown_method_rejected"));
    Reject(Source.Replace(TEXT("ue_struct_get(payload, Count)"), TEXT("ue_struct_get(payload, Missing)")),
                          TEXT("Unknown Unreal struct field"), TEXT("unknown_field_rejected"));
    Reject(Source.Replace(TEXT("ue_struct_get(payload, Count)"), TEXT("ue_struct_get(payload, \"Count\")")),
                          TEXT("Expected ue_struct_get"), TEXT("field_string_rejected"));
    Reject(Source.Replace(TEXT("ue_static(KismetMathLibrary.Abs, -2.5lf)"),
                               TEXT("ue_static(KismetMathLibrary.Abs, \"wrong\")")),
                          TEXT("ue_lib_Abs(string"), TEXT("wrong_argument_type_rejected"));
    Reject(Source.Replace(TEXT("ue_static(KismetMathLibrary.Abs, -2.5lf)"), TEXT("ue_static(KismetMathLibrary.Abs)")),
                          TEXT("ue_lib_Abs()"), TEXT("wrong_argument_count_rejected"));
    Reject(Source.Replace(TEXT("ue_struct_get(payload, Count)"),
                               TEXT("ue_struct_get(ue_struct_value(\"/Script/DasRuntime.DasCallPayload\"), Count)")),
                          TEXT("typed Unreal value struct"), TEXT("untyped_snapshot_rejected"));
    Reject(Source.Replace(TEXT("ue_static(GameplayStatics.GetTimeSeconds)"),
                               TEXT("ue_static(GameplayStatics.GetTimeSeconds)()")),
                          TEXT("syntax error"), TEXT("curried_form_is_not_daScript_grammar"));
    Reject(Source.Replace(TEXT("ue_call(type<DasProbeInterface>, Query, target,5)"),
                               TEXT("ue_call(type<DasProbeInterface>, Query, ue_self_object(),5)")),
                          TEXT("no matching functions"), TEXT("raw_receiver_rejected"));
    Reject(Source.Replace(TEXT("local.Query(5)"), TEXT("local.NoSuchMethod(5)")),
                          TEXT("NoSuchMethod"), TEXT("unbound_interface_unknown_method_rejected"));
    Reject(Source.Replace(TEXT("local.Query(5)"), TEXT("ue_call(type<LocalProbeRef>, Query, local, \"wrong\")")),
                          TEXT("no matching"), TEXT("unbound_interface_wrong_argument_rejected"));
    Reject(Source.Replace(TEXT("ue_arg(call, next_field_name(nameReads), hit)"), TEXT("ue_arg(call, 33, hit)")),
                          TEXT("field name must be a string"), TEXT("generic_arg_field_name_type_rejected"));
    Reject(Source.Replace(TEXT("ue_arg(call, next_field_name(nameReads), hit)"), TEXT("ue_arg(33, \"Value\", hit)")),
                          TEXT("UCallHandle"), TEXT("generic_arg_invalid_frame_type_rejected"));
    Reject(Source.Replace(TEXT("ue_arg(call, next_field_name(nameReads), hit)"),
                               TEXT("ue_arg(call, \"Value\", PlainArgRecord(Value=1))")),
                          TEXT("supported by-value types"), TEXT("generic_arg_plain_struct_rejected"));
    Reject(Source.Replace(TEXT("UEInterfaceRef(\"/Script/DasRuntime.DasProbeInterface\")"),
                          TEXT("UEInterfaceRef(33)")),
           TEXT("string literals"), TEXT("reference_macro_nonliteral_rejected"));
    Reject(Source.Replace(TEXT("UEInterfaceRef(\"/Script/DasRuntime.DasProbeInterface\")"),
                          TEXT("UEInterfaceRef(\"/Script/Engine.Actor\")")),
           TEXT("loaded interface"), TEXT("reference_macro_object_as_interface_rejected"));
    Reject(Source.Replace(TEXT("UEInterfaceRef(\"/Script/DasRuntime.DasProbeInterface\")"),
                          TEXT("UEInterfaceRef(\"/Script/DasRuntime.DasProbeInterface\", \"wrong.Id\")")),
           TEXT("identity mismatch"), TEXT("reference_macro_authored_interface_identity_rejected"));
    Reject(Source.Replace(TEXT("UEInterfaceRef(\"/Script/DasRuntime.DasProbeInterface\")"),
                          TEXT("UEInterfaceRef()")),
           TEXT("Expected UEObjectRef"), TEXT("reference_macro_arity_rejected"));
    Reject(Source.Replace(TEXT("UEObjectRef(\"ue_inheritance_base.das\", \"BaseCounter\")"),
                          TEXT("UEObjectRef(\"../ue_inheritance_base.das\", \"BaseCounter\")")),
           TEXT("local authored source filename"), TEXT("reference_macro_nonlocal_source_rejected"));
    Reject(Source.Replace(TEXT("UEObjectRef(\"ue_inheritance_base.das\", \"BaseCounter\")"),
                          TEXT("UEObjectRef(\"ue_inheritance_base.das\", \"MissingCounter\")")),
           TEXT("Unknown authored reference class"), TEXT("reference_macro_unknown_authored_class_rejected"));
    Reject(Source.Replace(TEXT("UEObjectRef(\"ue_inheritance_base.das\", \"BaseCounter\")"),
                          TEXT("UEObjectRef(\"ue_inheritance_base.das\", \"\")")),
           TEXT("requires type"), TEXT("reference_macro_empty_authored_class_rejected"));
    Reject(Source.Replace(TEXT("UEObjectRef(\"ue_inheritance_base.das\", \"BaseCounter\")"),
                          TEXT("UEObjectRef(\"ue_inheritance_base.das\", 33)")),
           TEXT("string literals"), TEXT("reference_macro_class_argument_type_rejected"));
    const FString NativeReference = TEXT("UENativeObjectRef(\"/Script/Engine.SkeletalMesh\")");
    IFileManager::Get().Delete(*CatalogPath, false, true);
    Write(Source);
    Check(R.ValidateIntMethod(Path, Error, TEXT("main")), TEXT("native_object_macro_without_catalog_compiles"), Error);
    SaveCatalog(Catalog);
    Reject(Source.Replace(*NativeReference, TEXT("UENativeObjectRef(33)")),
           TEXT("string literals"), TEXT("native_object_macro_nonliteral_rejected"));
    Reject(Source.Replace(*NativeReference, TEXT("UENativeObjectRef()")),
           TEXT("Expected UENativeObjectRef"), TEXT("native_object_macro_empty_arity_rejected"));
    Reject(Source.Replace(*NativeReference, TEXT("UENativeObjectRef(\"/Script/Engine.Actor\",\"extra\")")),
           TEXT("Expected UENativeObjectRef"), TEXT("native_object_macro_extra_argument_rejected"));
    Reject(Source.Replace(*NativeReference, TEXT("UENativeObjectRef(\"/Script/Engine.NoSuchNativeClass\")")),
           TEXT("loaded native UObject class"), TEXT("native_object_macro_unknown_class_rejected"));
    Reject(Source.Replace(*NativeReference, TEXT("UENativeObjectRef(\"/Script/CoreUObject.Vector\")")),
           TEXT("loaded native UObject class"), TEXT("native_object_macro_value_struct_rejected"));
    Reject(Source.Replace(*NativeReference, TEXT("UENativeObjectRef(\"/Script/DasRuntime.DasProbeInterface\")")),
           TEXT("loaded native UObject class"), TEXT("native_object_macro_interface_rejected"));
    Reject(Source.Replace(*NativeReference, TEXT("UENativeObjectRef(\"/Game/Types/SomeObject.SomeObject_C\")")),
           TEXT("loaded native UObject class"), TEXT("native_object_macro_authored_path_rejected"));
    Reject(Source.Replace(*NativeReference, TEXT("UENativeObjectRef(\"\")")),
           TEXT("loaded native UObject class"), TEXT("native_object_macro_empty_path_rejected"));
    Write(CatalogSource);
    Check(R.ValidateIntMethod(Path, Error, TEXT("main")), TEXT("catalog_reference_full_id_compiles"), Error);
    Reject(CatalogSource.Replace(TEXT("inheritance.BaseCounter\")"),TEXT("missing.Class\")")),
           TEXT("Unknown catalog class"), TEXT("catalog_reference_unknown_id_rejected"));
    Reject(CatalogSource.Replace(TEXT("inheritance.BaseCounter\")"),TEXT("BaseCounter\")")),
           TEXT("Ambiguous"), TEXT("catalog_reference_ambiguous_short_name_rejected"));
    Reject(CatalogSource.Replace(*CatalogReference,TEXT("UEObjectRef(33)")),
           TEXT("string literals"), TEXT("catalog_reference_nonliteral_rejected"));
    IFileManager::Get().Delete(*CatalogPath, false, true);
    Reject(CatalogSource, TEXT("Missing/oversized source catalog"), TEXT("catalog_reference_missing_catalog_rejected"));
    FDasSourceCatalog Stale = Catalog;
    Stale.Entries[0].Id = TEXT("wrong.BaseCounter");
    SaveCatalog(Stale);
    Reject(CatalogSource.Replace(TEXT("inheritance.BaseCounter\")"),TEXT("wrong.BaseCounter\")")),
           TEXT("stable ID differs"), TEXT("catalog_reference_stale_id_rejected"));
    FDasSourceCatalog Unique = Catalog;
    Unique.Entries.SetNum(1);
    SaveCatalog(Unique);
    Write(CatalogSource.Replace(TEXT("inheritance.BaseCounter\")"),TEXT("BaseCounter\")")));
    Check(R.ValidateIntMethod(Path, Error, TEXT("main")), TEXT("catalog_reference_unique_short_name_compiles"), Error);
    SaveCatalog(Catalog);
    TMap<FString,FString> InventorySources;
    InventorySources.Add(TEXT("action.das"),TEXT("// [ue_class] class Fake {}\nlet text=\"[ue_class] class Fake {}\"\n[ue_class(module_id=\"rogue\")] class RogueAction {}\n"));
    InventorySources.Add(TEXT("fixed.das"),TEXT("[ue_class(module_id=\"other\",id=\"stable.Fixed\")] class Fixed {}\n"));
    FDasSourceCatalog Inventory;
    Check(FDasSourceCatalog::DeclarationInventory(InventorySources, Inventory, Error) && Inventory.Entries.Num()==2 &&
          Inventory.Entries[0].Id==TEXT("rogue.RogueAction") && Inventory.Entries[1].Id==TEXT("stable.Fixed"),
          TEXT("automatic_inventory_ignores_comments_strings_and_respects_explicit_id"), Error);
    InventorySources.Add(TEXT("renamed_action.das"),InventorySources.FindChecked(TEXT("action.das")));
    InventorySources.Remove(TEXT("action.das"));
    Check(FDasSourceCatalog::DeclarationInventory(InventorySources, Inventory, Error) && Inventory.Entries[0].Script==TEXT("renamed_action.das") &&
          Inventory.Entries[0].Id==TEXT("rogue.RogueAction"),TEXT("automatic_inventory_rename_keeps_id"),Error);
    InventorySources.Add(TEXT("duplicate.das"),InventorySources.FindChecked(TEXT("renamed_action.das")));
    Check(!FDasSourceCatalog::DeclarationInventory(InventorySources, Inventory, Error),TEXT("automatic_inventory_duplicate_id_rejected"),Error);
    InventorySources.Remove(TEXT("duplicate.das"));
    InventorySources.Add(TEXT("bad.das"),TEXT("[ue_class(module_id=dynamic)] class Bad {}"));
    Check(!FDasSourceCatalog::DeclarationInventory(InventorySources, Inventory, Error) && Error.Contains(TEXT("string literals")),
          TEXT("automatic_inventory_nonliteral_identity_rejected"),Error);
    Check(FDasSourceCatalog::LocalFile(TEXT("rogue/character.das"),TEXT("das")) &&
          !FDasSourceCatalog::LocalFile(TEXT("../character.das"),TEXT("das")) &&
          !FDasSourceCatalog::LocalFile(TEXT("rogue//character.das"),TEXT("das")),
          TEXT("nested_catalog_paths_validate_without_traversal"));
    TMap<FString,FString> NestedSources;
    NestedSources.Add(TEXT("rogue/actor.das"),TEXT("module actor public\n[ue_class(module_id=\"nested\")] class NestedActor {}"));
    FDasSourceCatalog NestedCatalog;
    Check(FDasSourceCatalog::DeclarationInventory(NestedSources,NestedCatalog,Error) &&
          NestedCatalog.Entries.Num()==1 && NestedCatalog.Entries[0].Script==TEXT("rogue/actor.das") &&
          NestedCatalog.Entries[0].Id==TEXT("nested.NestedActor"),TEXT("nested_inventory_keeps_path_and_stable_id"),Error);
    const FString NestedName=TEXT("nested_probe_")+FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString NestedRelative=TEXT("catalogfolders/")+NestedName+TEXT(".das");
    const FString NestedRoot=FPaths::GetPath(Path)/TEXT("catalogfolders");
    const FString NestedPath=NestedRoot/(NestedName+TEXT(".das"));
    IFileManager::Get().MakeDirectory(*NestedRoot,true);
    FFileHelper::SaveStringToFile(FString::Printf(TEXT("options gen2\nmodule %s public\ndef nested_value():int{return 42}\n"),*NestedName),*NestedPath,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    // Use the project root: plugin imports retain their existing flat search semantics.
    const FString ProjectNested=FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir()/TEXT("Das/Scripts")/NestedRelative);
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(ProjectNested),true);
    IFileManager::Get().Copy(*ProjectNested,*NestedPath);
    ON_SCOPE_EXIT {
        IFileManager::Get().Delete(*NestedPath,false,true);
        IFileManager::Get().Delete(*ProjectNested,false,true);
        R.NotifyScriptsChanged({ProjectNested},true);
    };
    R.NotifyScriptsChanged({ProjectNested},true);
    Check(R.ScriptPath(NestedName+TEXT(".das"))==ProjectNested && R.ScriptPath(NestedRelative)==ProjectNested,
          TEXT("nested_project_script_resolves_by_module_and_explicit_path"));
    Write(FString::Printf(TEXT("options gen2\nrequire %s\n[export]\ndef main(input:int):int{return nested_value()}\n"),*NestedName));
    Check(R.ValidateIntMethod(Path,Error,TEXT("main")),TEXT("nested_project_module_import_compiles"),Error);
    const FString NestedClassSource=FString::Printf(TEXT("options gen2\nmodule %s public\nrequire ue_native_gameplay public\n[ue_class(module_id=\"nested\")] class NestedThing { [ue_function(BlueprintCallable)] def Shift(value:UEVector):UEVector{return value} }\n"),*NestedName);
    FFileHelper::SaveStringToFile(NestedClassSource,*ProjectNested,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    R.NotifyScriptsChanged({ProjectNested},false);
    FDasSourceCatalog NestedReferenceCatalog=Catalog;
    NestedReferenceCatalog.Entries.Add({TEXT("nested.NestedThing"),TEXT("NestedThing"),NestedRelative,TEXT("NestedThingRef")});
    SaveCatalog(NestedReferenceCatalog);
    Write(TEXT("options gen2\nrequire ue_native_gameplay\ntypedef NestedRef=UEObjectRef(\"nested.NestedThing\")\ndef shiftNested(receiver:NestedRef;value:UEVector):UEVector{return receiver.Shift(value)}\n[export]\ndef main(input:int):int{return 42}\n"));
    Check(R.ValidateIntMethod(Path,Error,TEXT("main")),TEXT("nested_catalog_reference_typed_method_compiles"),Error);
    SaveCatalog(Catalog);
    TMap<FString,FString> TypeSources;
    TypeSources.Add(TEXT("types.das"),TEXT("module types public\n[ue_enum(module_id=\"game\")] enum Mode { A }\n[ue_struct(module_id=\"game\")] struct Payload {value:int}\n[ue_interface(module_id=\"game\")] class Receiver {}\n"));
    FDasSourceCatalog TypeIndex; FDasCatalogValueModule FoundModule; FDasCatalogValueType FoundType;
    Check(FDasSourceCatalog::DeclarationInventory(TypeSources,TypeIndex,Error) && TypeIndex.Entries.IsEmpty() &&
          TypeIndex.ValueModules.Num()==1 && TypeIndex.ValueModules[0].Types.Num()==3,
          TEXT("automatic_inventory_interface_struct_enum_without_classes"),Error);
    Check(TypeIndex.ResolveType(TEXT("struct"),TEXT("game.Payload"),FoundModule,FoundType,Error) &&
          FoundModule.Script==TEXT("types.das") && FoundType.Name==TEXT("Payload"),TEXT("catalog_struct_source_lookup"),Error);
    Check(TypeIndex.ResolveType(TEXT("enum"),TEXT("Mode"),FoundModule,FoundType,Error) && FoundType.Id==TEXT("game.Mode"),
          TEXT("catalog_enum_short_name_lookup"),Error);
    Check(TypeIndex.ResolveType(TEXT("interface"),TEXT("game.Receiver"),FoundModule,FoundType,Error),
          TEXT("catalog_interface_source_lookup"),Error);
    Check(!TypeIndex.ResolveType(TEXT("enum"),TEXT("Payload"),FoundModule,FoundType,Error),
          TEXT("catalog_lookup_rejects_wrong_kind"),Error);
    TypeSources.Add(TEXT("other_types.das"),TEXT("module other_types public\n[ue_enum(module_id=\"other\")] enum Mode { A }"));
    FDasSourceCatalog AmbiguousTypes;
    Check(FDasSourceCatalog::DeclarationInventory(TypeSources,AmbiguousTypes,Error) &&
          !AmbiguousTypes.ResolveType(TEXT("enum"),TEXT("Mode"),FoundModule,FoundType,Error) && Error.Contains(TEXT("Ambiguous")),
          TEXT("catalog_value_short_name_ambiguity_rejected"),Error);
    FDasSourceCatalog InterfaceAliases = TypeIndex;
    InterfaceAliases.ValueModules.Add({TEXT("refs"),TEXT("refs.das"),{{TEXT("interface_ref"),TEXT("game.Receiver"),TEXT("ReceiverRef")}}});
    Check(InterfaceAliases.ResolveType(TEXT("interface"),TEXT("game.Receiver"),FoundModule,FoundType,Error) &&
          FoundType.Kind==TEXT("interface") && FoundModule.Script==TEXT("types.das"),TEXT("catalog_interface_alias_deduplicates_by_id"),Error);
    TypeSources.Add(TEXT("bad_identity.das"),TEXT("[ue_class(id=\"\")] class EmptyId {}"));
    Check(!FDasSourceCatalog::DeclarationInventory(TypeSources,AmbiguousTypes,Error),TEXT("automatic_inventory_explicit_empty_id_rejected"),Error);
    const FString InterfaceFile = TEXT("catalog_interface_") + FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT(".das");
    const FString InterfaceModule = FPaths::GetBaseFilename(InterfaceFile);
    const FString InterfaceId = TEXT("catalog.Contract_") + FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString InterfacePath = R.ScriptPath(InterfaceFile);
    ON_SCOPE_EXIT { IFileManager::Get().Delete(*InterfacePath,false,true); };
    const FString InterfaceSource = FString::Printf(TEXT("options gen2\nmodule %s public\nrequire ue_dsl\n[ue_interface(module_id=\"catalog\",id=\"%s\",BlueprintType)]\nclass CatalogContract { [ue_function(BlueprintCallable)] def Ping():void{} }\n"),*InterfaceModule,*InterfaceId);
    FFileHelper::SaveStringToFile(InterfaceSource,*InterfacePath,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    TArray<UBlueprint*> CatalogInterfaces;
    Check(FDasInterfaceService::Apply(InterfaceFile,TEXT("/Game/CatalogMacro/")+InterfaceModule,CatalogInterfaces,Error) && CatalogInterfaces.Num()==1,
          TEXT("catalog_interface_fixture_apply"),Error);
    FDasSourceCatalog CatalogWithInterface = Catalog;
    CatalogWithInterface.ValueModules.Add({InterfaceModule,InterfaceFile,{{TEXT("interface"),InterfaceId,TEXT("CatalogContract")}}});
    SaveCatalog(CatalogWithInterface);
    const FString InterfaceRefDeclaration = FString::Printf(TEXT("\ntypedef CatalogContractRef = UEInterfaceRef(\"%s\")\n"),*InterfaceId);
    const FString InterfaceCases = TEXT("    if(mode==23){let none=CatalogContractRef();return ue_is_valid(none)?-1:42}\n    if(mode==24){var rejected=false;try{let value=ue_ref(type<CatalogContractRef>,ue_self_object());verify(ue_is_valid(value))}recover{rejected=true}\nreturn rejected?42:-1}\n");
    const FString CatalogInterfaceSource = Source.Replace(TEXT("def actor_view_valid"), *(InterfaceRefDeclaration + TEXT("def actor_view_valid"))).Replace(TEXT("    let payload = DasCallPayload("),*(InterfaceCases+TEXT("    let payload = DasCallPayload(")));
    Write(CatalogInterfaceSource);
    Check(R.ValidateIntMethod(Path,Error,TEXT("main")),TEXT("catalog_interface_reference_by_full_id_compiles"),Error);
    Write(CatalogInterfaceSource.Replace(*InterfaceId,TEXT("CatalogContract")));
    Check(R.ValidateIntMethod(Path,Error,TEXT("main")),TEXT("catalog_interface_reference_by_short_name_compiles"),Error);
    Reject(CatalogInterfaceSource.Replace(*InterfaceId,TEXT("catalog.Missing")),TEXT("Unknown catalog type"),TEXT("catalog_interface_unknown_id_rejected"));
    Reject(CatalogInterfaceSource.Replace(*InterfaceId,TEXT("inheritance.BaseCounter")),TEXT("Unknown catalog type"),TEXT("catalog_class_cannot_be_interface_reference"));
    SaveCatalog(Catalog);
    const FString Descriptor = TEXT("\n[ue_reflected_call(owner=\"/Script/DasRuntime.DasFunctionLibraryFixture\",method=\"Add\",signature=\"@SIGNATURE@\")]\ndef duplicate(a:int;b:int):int{return a+b}\n");
    Reject(Source + Descriptor.Replace(TEXT("@SIGNATURE@"), TEXT("stale")),
                                       TEXT("Reflected signature changed"), TEXT("stale_binding_rejected_at_compile"));
    Reject(Source +
           Descriptor.Replace(TEXT("@SIGNATURE@"),
                                   TEXT("v1;Add;A:IntProperty:128;B:IntProperty:128;ReturnValue:IntProperty:1408;")),
                              TEXT("Ambiguous static bindings"), TEXT("ambiguous_bindings_rejected"));
    const FString ClassSource = Source.Replace(TEXT("require ue_native_gameplay"), TEXT("require ue_dsl\nrequire ue_native_gameplay")) +
        TEXT("\n[ue_class(module_id=\"reflectionmacro\",Blueprintable)]\nclass MacroUser {\n[ue_function(BlueprintCallable)]\ndef Read():int { return ue_static(DasFunctionLibraryFixture.Add,20,22) }\n}\n");
    Write(ClassSource);
    Check(R.ReadAuthoringScript(Path, Model, Error) && Model.Classes.Num() == 1, TEXT("macro_inside_authored_method"),
                                                                                      Error);
    const FString InputSource = Source + TEXT(R"DAS(
require ue_enhanced_input
[ue_class(module_id="inputmacro", Blueprintable)]
class InputMacroUser {
    settings = ue_properties(EditAnywhere) {
        var JumpAction : InputActionRef
        var MoveAction : InputActionRef
    }

    def Bind() : void { ue_bind_inputs() }

    [ue_input(action="JumpAction", Started, Completed, Canceled)]
    def Jump(value:bool) : void {}

    [ue_input(action="MoveAction", Triggered)]
    def Move(value:InputActionValue) : void {}
}
)DAS");
    Write(InputSource);
    Error.Reset();
    Check(R.ReadAuthoringScript(Path, Model, Error) && Model.Classes.Num() == 1,
          TEXT("input_annotations_multiple_triggers_compile"), Error);
    Reject(InputSource.Replace(TEXT("action=\"JumpAction\""), TEXT("action=\"Missing\"")),
           TEXT("InputActionRef property"), TEXT("input_missing_action_rejected"));
    Reject(InputSource.Replace(TEXT("var JumpAction : InputActionRef"), TEXT("var JumpAction : int")),
           TEXT("InputActionRef property"), TEXT("input_wrong_action_type_rejected"));
    Reject(InputSource.Replace(TEXT("Started, Completed, Canceled"), TEXT("Strated")),
           TEXT("trigger"), TEXT("input_unknown_trigger_rejected"));
    Reject(InputSource.Replace(TEXT(", Started, Completed, Canceled"), TEXT("")),
           TEXT("at least one trigger"), TEXT("input_empty_trigger_set_rejected"));
    Reject(InputSource.Replace(TEXT("Started, Completed, Canceled"), TEXT("Started, Started")),
           TEXT("Repeated ue_input"), TEXT("input_repeated_trigger_rejected"));
    Reject(InputSource.Replace(TEXT("value:bool"), TEXT("value:int")),
           TEXT("ue_input requires"), TEXT("input_wrong_handler_value_type_rejected"));
    Reject(InputSource.Replace(TEXT("Jump(value:bool) : void"), TEXT("Jump(var value:bool&) : void")),
           TEXT("by-value input"), TEXT("input_reference_handler_rejected"));
    Reject(InputSource.Replace(TEXT("ue_bind_inputs()"), TEXT("ue_bind_inputs(1)")),
           TEXT("ue_bind_inputs()"), TEXT("input_setup_arguments_rejected"));
    Reject(InputSource.Replace(TEXT("action=\"MoveAction\", Triggered"), TEXT("action=\"JumpAction\", Started")),
           TEXT("Duplicate ue_input action/trigger"), TEXT("input_duplicate_action_trigger_rejected"));
    const FString EmptyInputSource = Source + TEXT(R"DAS(
require ue_enhanced_input
[ue_class(module_id="inputmacro", Blueprintable)]
class EmptyInputUser {
    def Bind() : void { ue_bind_inputs() }
}
)DAS");
    Reject(EmptyInputSource, TEXT("at least one ue_input handler"), TEXT("input_empty_setup_rejected"));
    const FString ReferenceClassSource = Source + TEXT(R"DAS(
require ue_dsl
[ue_class(module_id="refmacro", Blueprintable)]
class ReferenceMacroUser {
    settings=ue_properties(EditAnywhere) {
        var Counter : CounterRef
        var Interface : LocalProbeRef
        var Mesh : LocalMeshRef
    }

    [ue_function(BlueprintCallable)]
    def GetCounter():CounterRef { return self.Counter }

    [ue_function(BlueprintCallable)]
    def GetMesh():LocalMeshRef { return self.Mesh }
}
)DAS");
    Write(ReferenceClassSource);
    Error.Reset();
    Check(R.ReadAuthoringScript(Path, Model, Error) && Model.Classes.Num() == 1,
          TEXT("reference_macros_in_authored_signature_and_properties"), Error);
    Write(ReferenceClassSource.Replace(*ExplicitReference, *CatalogReference));
    Error.Reset();
    Check(R.ReadAuthoringScript(Path, Model, Error) && Model.Classes.Num()==1,
          TEXT("catalog_reference_in_authored_schema"),Error);
    Check(R.ValidateIntMethod(R.ScriptPath(TEXT("array_gc_exception_fixture.das")), Error, TEXT("main")),
                              TEXT("unchanged_array_GC_frame_fixture_compiles"), Error);
    for (const TCHAR* Package :
         {TEXT("ue_api_world"), TEXT("ue_api_math"), TEXT("ue_api_collision"), TEXT("ue_api_umg_input")})
    {
        Write(FString::Printf(TEXT("options gen2\nrequire %s\n[export]\ndef main(input:int):int{return input}\n"),
                                   Package));
        Check(R.ValidateIntMethod(Path, Error, TEXT("main")), Package, Error);
    }
    Write(Source);
    auto IV = UWorld::InitializationValues()
                  .CreatePhysicsScene(false)
                  .CreateNavigation(false)
                  .CreateAISystem(false)
                  .AllowAudioPlayback(false);
    TStrongObjectPtr<UWorld> World(
        UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &IV));
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    World->InitializeActorsForPlay(FURL());
    Check(R.PreparePrograms(World.Get(), {FPaths::GetCleanFilename(Path)}, Error),
          TEXT("prepare_isolated_world_program"), Error);
    auto* Actor = World->SpawnActor<ADasReflectionMacroActor>();
    if (!Actor)
        return 2;
    TStrongObjectPtr<ADasReflectionMacroActor> Keep(Actor);
    FEditorScriptExecutionGuard Guard;
    auto Run = [&](int32 Mode, int32 Expected, const TCHAR* Name)
    {
        const auto Result = R.RunNamedPersistent(Path, TEXT("main"), Mode, Actor);
        Check(Result.bSucceeded && Result.Value == Expected, Name, Result.Error);
    };
    Run(0, 42, TEXT("static_scalar_alias_independent"));
    Run(1, 1, TEXT("automatic_WorldContext"));
    Run(2, 42, TEXT("native_interface_dispatch"));
    Run(3, 27, TEXT("interface_ref_out"));
    Run(4, 17, TEXT("typed_struct_read"));
    Run(5, 1, TEXT("nested_struct_read"));
    Run(6, 3, TEXT("interface_container_result"));
    Run(7, 42, TEXT("interface_member_without_generated_wrapper"));
    Run(8, 27, TEXT("interface_ref_out_without_generated_wrapper"));
    Run(9, 3, TEXT("interface_container_without_generated_wrapper"));
    Run(10, 42, TEXT("equivalent_native_reference_view_argument"));
    Run(11, 42, TEXT("typed_class_to_native_class_argument"));
    Run(12, 42, TEXT("generic_arg_scalars_single_evaluation"));
    Run(15, 1, TEXT("generic_arg_runtime_property_type_rejected"));
    Check(Actor->EchoCalls == 0, TEXT("generic_arg_rejection_has_no_native_side_effect"));
    Run(13, 42, TEXT("generic_arg_struct_nested_math_name_byte_object"));
    Check(Actor->EchoCalls == 1, TEXT("generic_arg_struct_invokes_once"));
    Run(14, 3, TEXT("generic_arg_array_carrier"));
    Run(16, 42, TEXT("generic_arg_math_value"));
    Run(17, 42, TEXT("generic_arg_typed_class"));
    Run(18, 42, TEXT("reference_macro_repeated_alias_interface_dispatch"));
    Run(19, 42, TEXT("reference_macro_authored_nullable_constructor"));
    Run(20, 42, TEXT("reference_macro_authored_runtime_identity_rejected"));
    Run(25, 42, TEXT("native_object_macro_repeated_qualified_alias"));
    Run(26, 42, TEXT("native_object_macro_nullable_value"));
    Run(27, 42, TEXT("native_object_macro_wrong_runtime_class_rejected"));
    Run(28, 42, TEXT("native_object_macro_accepts_derived_class"));
    Write(CatalogSource);
    Check(R.PreparePrograms(World.Get(), {FPaths::GetCleanFilename(Path)}, Error),TEXT("prepare_catalog_reference_program"),Error);
    R.ReleaseObject(Actor);
    Run(19,42,TEXT("catalog_reference_nullable_runtime_value"));
    Run(20,42,TEXT("catalog_reference_runtime_identity_preserved"));
    SaveCatalog(CatalogWithInterface);
    Write(CatalogInterfaceSource);
    Check(R.PreparePrograms(World.Get(),{FPaths::GetCleanFilename(Path)},Error),TEXT("prepare_catalog_interface_program"),Error);
    R.ReleaseObject(Actor);
    Run(23,42,TEXT("catalog_interface_nullable_runtime_value"));
    Run(24,42,TEXT("catalog_interface_implementation_check_preserved"));
    R.ReleaseObject(Actor);
    const auto NoOwner = R.RunFile(Path, 1);
    Check(!NoOwner.bSucceeded && NoOwner.Error.Contains(TEXT("self is missing")),
                                                        TEXT("WorldContext_without_owner_rejected"), NoOwner.Error);
    const auto Frame = R.RunFile(R.ScriptPath(TEXT("actor_location.das")), 0, false, Actor);
    Check(Frame.bSucceeded, TEXT("unchanged_low_level_frame_fixture"), Frame.Error);
    R.ReleaseObject(Actor);
    UE_LOG(LogTemp, Display,
           TEXT("DAS_REFLECTION_MACRO_RESULT=%s checks=%d failures=%d"),
                Failures ? TEXT("FAIL") : TEXT("PASS"), Checks, Failures);
    return Failures ? 1 : 0;
}
