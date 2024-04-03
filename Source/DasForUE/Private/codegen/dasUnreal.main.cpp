#include "dasUnreal.h"
#include "need_dasUnreal.h"
//#include "aot_dasUnreal.h"

#include "../ReflectionHelpers.h"
#include "../UFunctionCaller.h"

DAS_BASE_BIND_ENUM_GEN(ESearchDir::Type, ESearchDir_Type);
DAS_BASE_BIND_ENUM_GEN(ESearchCase::Type, ESearchCase_Type);

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

    //------------------------------------------------------------------------

    void Module_dasUnreal::initAdditional () {

        addAnnotation(make_smart<FTextAnnotation>(lib));
        addAnnotation(make_smart<FNameAnnotation>(lib));

        addAnnotation(make_smart<UObjectAnnotation>(lib));
        addAnnotation(make_smart<UClassAnnotation>(lib));
        addAnnotation(make_smart<FStaticConstructObjectParametersAnnotation>(lib));

        addEnumeration(make_smart<ESearchDir_Annotation>());
        addEnumeration(make_smart<ESearchCase_Annotation>());

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
