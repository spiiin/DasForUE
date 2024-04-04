#include "dasUnreal.h"
#include "need_dasUnreal.h"
//#include "aot_dasUnreal.h"

#include "../ReflectionHelpers.h"
#include "../UFunctionCaller.h"

DAS_BASE_BIND_ENUM_GEN(ESearchDir::Type, ESearchDir_Type);
DAS_BASE_BIND_ENUM_GEN(ESearchCase::Type, ESearchCase_Type);
DAS_BASE_BIND_ENUM_GEN(EObjectFlags, EObjectFlags);

IMPLEMENT_EXTERNAL_TYPE_FACTORY(UObject, UObject);
IMPLEMENT_EXTERNAL_TYPE_FACTORY(UClass, UClass);
IMPLEMENT_EXTERNAL_TYPE_FACTORY(FStaticConstructObjectParameters, FStaticConstructObjectParameters);
IMPLEMENT_EXTERNAL_TYPE_FACTORY(FText, FText);
IMPLEMENT_EXTERNAL_TYPE_FACTORY(FName, FName);


namespace das {

    struct UObjectAnnotation final : ManagedStructureAnnotation<UObject, false, false> {
        UObjectAnnotation(ModuleLibrary& ml) : ManagedStructureAnnotation("UObject", ml) {
        }
        void init() {
        }
        bool hasNonTrivialCtor() const override { return true; }
        bool canClone() const override { return false; }
        bool canNew() const override { return false; }
    };

    struct UClassAnnotation final : ManagedStructureAnnotation<UClass, false, false> {
        UClassAnnotation(ModuleLibrary& ml) : ManagedStructureAnnotation("UClass", ml) {
        }
        void init() {
        }
        bool hasNonTrivialCtor() const override { return true; }
        bool canClone() const override { return false; }
        bool canNew() const override { return false; }
    };

    struct FStaticConstructObjectParametersAnnotation final : ManagedStructureAnnotation<FStaticConstructObjectParameters> {
        FStaticConstructObjectParametersAnnotation(ModuleLibrary& ml) : ManagedStructureAnnotation("FStaticConstructObjectParameters", ml) {
        }
        void init() {
        }
    };

    struct FTextAnnotation final : ManagedStructureAnnotation<FText> {
        FTextAnnotation(ModuleLibrary& ml) : ManagedStructureAnnotation("FText", ml) {
        }
        void init() {
        }
        bool hasNonTrivialCtor() const override { return true; }
    };


    struct FNameAnnotation final : ManagedStructureAnnotation<FName> {
        FNameAnnotation(ModuleLibrary& ml) : ManagedStructureAnnotation("FName", ml) {
        }
        void init() {
        }
        bool hasNonTrivialCtor() const override { return true; }
    };

    class ESearchDir_Annotation : public das::Enumeration {
    public:
        ESearchDir_Annotation() : das::Enumeration("ESearchDir_Type") {
            external = true;
            cppName = "ESearchDir::Type";
            baseType = (das::Type)das::ToBasicType<int>::type;
            addIEx("FromStart", "FromStart", int32_t(ESearchDir::Type::FromStart), das::LineInfo());
            addIEx("FromEnd", "FromStart", int32_t(ESearchDir::Type::FromEnd), das::LineInfo());
        }
    };

    class ESearchCase_Annotation : public das::Enumeration {
    public:
        ESearchCase_Annotation() : das::Enumeration("ESearchCase_Type") {
            external = true;
            cppName = "ESearchCase_Type";
            baseType = (das::Type)das::ToBasicType<int>::type;
            addIEx("CaseSensitive", "CaseSensitive", int32_t(ESearchCase::Type::CaseSensitive), das::LineInfo());
            addIEx("IgnoreCase", "IgnoreCase", int32_t(ESearchCase::Type::IgnoreCase), das::LineInfo());
        }
    };
    
    class EObjectFlags_Annotation : public das::Enumeration {
    public:
        EObjectFlags_Annotation() : das::Enumeration("EObjectFlags") {
            external = true;
            cppName = "EObjectFlags";
            baseType = (das::Type)das::ToBasicType<int>::type;
            addIEx("RF_NoFlags", "RF_NoFlags", int32_t(EObjectFlags::RF_NoFlags), das::LineInfo());
            addIEx("RF_Public", "RF_Public", int32_t(EObjectFlags::RF_Public), das::LineInfo());
            addIEx("RF_Standalone", "RF_Standalone", int32_t(EObjectFlags::RF_Standalone), das::LineInfo());
            addIEx("RF_MarkAsNative", "RF_MarkAsNative", int32_t(EObjectFlags::RF_MarkAsNative), das::LineInfo());
            addIEx("RF_Transactional", "RF_Transactional", int32_t(EObjectFlags::RF_Transactional), das::LineInfo());
            addIEx("RF_ClassDefaultObject", "RF_ClassDefaultObject", int32_t(EObjectFlags::RF_ClassDefaultObject), das::LineInfo());
            addIEx("RF_ArchetypeObject", "RF_ArchetypeObject", int32_t(EObjectFlags::RF_ArchetypeObject), das::LineInfo());
            addIEx("RF_Transient", "RF_Transient", int32_t(EObjectFlags::RF_Transient), das::LineInfo());
            addIEx("RF_MarkAsRootSet", "RF_MarkAsRootSet", int32_t(EObjectFlags::RF_MarkAsRootSet), das::LineInfo());
            addIEx("RF_TagGarbageTemp", "RF_TagGarbageTemp", int32_t(EObjectFlags::RF_TagGarbageTemp), das::LineInfo());
            addIEx("RF_NeedInitialization", "RF_NeedInitialization", int32_t(EObjectFlags::RF_NeedInitialization), das::LineInfo());
            addIEx("RF_NeedLoad", "RF_NeedLoad", int32_t(EObjectFlags::RF_NeedLoad), das::LineInfo());
            addIEx("RF_KeepForCooker", "RF_KeepForCooker", int32_t(EObjectFlags::RF_KeepForCooker), das::LineInfo());
            addIEx("RF_NeedPostLoad", "RF_NeedPostLoad", int32_t(EObjectFlags::RF_NeedPostLoad), das::LineInfo());
            addIEx("RF_NeedPostLoadSubobjects", "RF_NeedPostLoadSubobjects", int32_t(EObjectFlags::RF_NeedPostLoadSubobjects), das::LineInfo());
            addIEx("RF_NewerVersionExists", "RF_NewerVersionExists", int32_t(EObjectFlags::RF_NewerVersionExists), das::LineInfo());
            addIEx("RF_BeginDestroyed", "RF_BeginDestroyed", int32_t(EObjectFlags::RF_BeginDestroyed), das::LineInfo());
            addIEx("RF_FinishDestroyed", "RF_FinishDestroyed", int32_t(EObjectFlags::RF_FinishDestroyed), das::LineInfo());
            addIEx("RF_BeingRegenerated", "RF_BeingRegenerated", int32_t(EObjectFlags::RF_BeingRegenerated), das::LineInfo());
            addIEx("RF_DefaultSubObject", "RF_DefaultSubObject", int32_t(EObjectFlags::RF_DefaultSubObject), das::LineInfo());
            addIEx("RF_WasLoaded", "RF_WasLoaded", int32_t(EObjectFlags::RF_WasLoaded), das::LineInfo());
            addIEx("RF_TextExportTransient", "RF_TextExportTransient", int32_t(EObjectFlags::RF_TextExportTransient), das::LineInfo());
            addIEx("RF_LoadCompleted", "RF_LoadCompleted", int32_t(EObjectFlags::RF_LoadCompleted), das::LineInfo());
            addIEx("RF_InheritableComponentTemplate", "RF_InheritableComponentTemplate", int32_t(EObjectFlags::RF_InheritableComponentTemplate), das::LineInfo());
            addIEx("RF_DuplicateTransient", "RF_DuplicateTransient", int32_t(EObjectFlags::RF_DuplicateTransient), das::LineInfo());
            addIEx("RF_StrongRefOnFrame", "RF_StrongRefOnFrame", int32_t(EObjectFlags::RF_StrongRefOnFrame), das::LineInfo());
            addIEx("RF_NonPIEDuplicateTransient", "RF_NonPIEDuplicateTransient", int32_t(EObjectFlags::RF_NonPIEDuplicateTransient), das::LineInfo());
            addIEx("RF_WillBeLoaded", "RF_WillBeLoaded", int32_t(EObjectFlags::RF_WillBeLoaded), das::LineInfo());
            addIEx("RF_HasExternalPackage", "RF_HasExternalPackage", int32_t(EObjectFlags::RF_HasExternalPackage), das::LineInfo());
            addIEx("RF_AllocatedInSharedPage", "RF_AllocatedInSharedPage", int32_t(EObjectFlags::RF_AllocatedInSharedPage), das::LineInfo());
        }
    };

    //------------------------------------------------------------------------

    void Module_dasUnreal::initAdditional () {

        addAnnotation(make_smart<FTextAnnotation>(lib));
        addAnnotation(make_smart<FNameAnnotation>(lib));

        addAnnotation(make_smart<UObjectAnnotation>(lib));
        addAnnotation(make_smart<UClassAnnotation>(lib));
        addAnnotation(make_smart<FStaticConstructObjectParametersAnnotation>(lib));

        addEnumeration(make_smart<ESearchDir_Annotation>());
        addEnumeration(make_smart<ESearchCase_Annotation>());
        addEnumeration(make_smart<EObjectFlags_Annotation>());

        addExtern<DAS_BIND_FUN(UFunctionCaller::callUFunctionOn)>(*this, lib, "call_ufunction_on", SideEffects::worstDefault);
        addExtern<DAS_BIND_FUN(getClassByName)>(*this, lib, "get_class_by_name", SideEffects::worstDefault);
        addExtern<UObject* (*)(UObject*, UClass*, FName), newObjectFromClass>(*this, lib, "new_object_from_class", SideEffects::worstDefault);
        addExtern<UObject* (*)(FStaticConstructObjectParameters), newObjectFromClass>(*this, lib, "new_object_from_class", SideEffects::worstDefault);

        addCtorAndUsing<FName>(*this, lib, "FName", "FName");
        addCtorAndUsing<FName, const char*>(*this, lib, "FName", "FName")
            ->args({ "Str" });

        addCtorAndUsing<FStaticConstructObjectParameters, UClass*>(*this, lib, "FStaticConstructObjectParameters", "FStaticConstructObjectParameters")
            ->args({ "Class" });
    }

	void Module_dasUnreal::initMain () {
    }

	ModuleAotType Module_dasUnreal::aotRequire ( TextWriter & tw ) const {
        // add your stuff here
        return ModuleAotType::no_aot; //TODO: ModuleAotType::cpp
    }

}
