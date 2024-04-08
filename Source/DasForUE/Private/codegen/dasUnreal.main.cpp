#include "dasUnreal.h"
#include "need_dasUnreal.h"
//#include "aot_dasUnreal.h"

#include "../ReflectionHelpers.h"

DAS_BASE_BIND_ENUM_GEN(ESearchDir::Type, ESearchDir_Type);
DAS_BASE_BIND_ENUM_GEN(ESearchCase::Type, ESearchCase_Type);

IMPLEMENT_EXTERNAL_TYPE_FACTORY(FText, FText);
IMPLEMENT_EXTERNAL_TYPE_FACTORY(FName, FName);

namespace das {

    struct FTextAnnotation final : ManagedStructureAnnotation<FText> {
        FTextAnnotation(ModuleLibrary& ml) : ManagedStructureAnnotation("FText", ml) {
        }
        void init() {
        }
        virtual bool canCopy() const override { return std::is_copy_constructible<FString>::value; }
        virtual bool canClone() const override { return std::is_copy_constructible<FString>::value; }
        virtual bool canMove() const override { return std::is_copy_constructible<FString>::value; }
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
    struct DasUeCallback {
        Lambda      lambda;
        Context* context = nullptr;
    };

    thread_local das_map<const UFunction*, DasUeCallback> g_Callbacks;

    static void dasForUeNativeFunc(UObject* unrealContext, FFrame& theStack, void*const param) {
        auto key = theStack.CurrentNativeFunction;
        auto it = g_Callbacks.find(key);
        if (it == g_Callbacks.end()) {
            das_to_stderr("Error, native function doesn't exist for UFunction [%p]", key); //TODO: add function name
            return;
        }
        auto& [dasFunction, dasContext] = it->second;
        if (dasContext) {
            das_invoke_lambda<void>::invoke<UObject*, FFrame&, void* const>(dasContext, nullptr, dasFunction,
                unrealContext, theStack, param);
        }
    }


    void setNativeFunc(UFunction* uFunction, das::TLambda<void, UObject*, FFrame&, void*const> dasFunction, Context* dasContext) {
        g_Callbacks[uFunction] = { dasFunction, dasContext };
        uFunction->SetNativeFunc(&dasForUeNativeFunc);
    }

    //------------------------------------------------------------------------

    void Module_dasUnreal::initAdditionalAnnotations () {
        addAnnotation(make_smart<FTextAnnotation>(lib));
        addAnnotation(make_smart<FNameAnnotation>(lib));

        addEnumeration(make_smart<ESearchDir_Annotation>());
        addEnumeration(make_smart<ESearchCase_Annotation>());
    }

    void Module_dasUnreal::initAdditionalFunctions() {
        addExtern<UPackage* (*)(), AnyPackage>(*this, lib, "AnyPackage", SideEffects::worstDefault);
        addExtern<DAS_BIND_FUN(FStringToCStr)>(*this, lib, "string", SideEffects::worstDefault);

        addCtorAndUsing<FName>(*this, lib, "FName", "FName");
        addCtorAndUsing<FName, const char*>(*this, lib, "FName", "FName")
            ->args({ "Str" });

        //FFieldVariant has only template move ctor(T&&), so it needs to be instantiated for UObject explicitly
        addCtorAndUsing<FFieldVariant, const UObject*>(*this, lib, "FFieldVariant", "FFieldVariant")
            ->args({ "Object" });

        addExtern<DAS_BIND_FUN(setNativeFunc)>(*this, lib, "SetNativeFunc", SideEffects::worstDefault, "setNativeFunc");

    }

	void Module_dasUnreal::initMain () {
    }

	ModuleAotType Module_dasUnreal::aotRequire ( TextWriter & tw ) const {
        // add your stuff here
        return ModuleAotType::no_aot; //TODO: ModuleAotType::cpp
    }

}
