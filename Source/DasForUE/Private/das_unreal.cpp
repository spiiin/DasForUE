#include "das_unreal.h"

#include "CoreMinimal.h"

namespace das
{
    template <> struct cast_arg<FString> {
        static __forceinline const FString& to(Context& ctx, SimNode* node) {
            vec4f res = node->eval(ctx);
            return *cast<FString*>::to(res);
        }
    };

    template <> struct cast_arg<FText> {
        static __forceinline const FText& to(Context& ctx, SimNode* node) {
            vec4f res = node->eval(ctx);
            return *cast<FText*>::to(res);
        }
    };

    template <> struct cast_arg<FName> {
        static __forceinline const FName& to(Context& ctx, SimNode* node) {
            vec4f res = node->eval(ctx);
            return *cast<FName*>::to(res);
        }
    };

    template <> struct cast_arg<FStaticConstructObjectParameters> {
        static __forceinline const FStaticConstructObjectParameters& to(Context& ctx, SimNode* node) {
            vec4f res = node->eval(ctx);
            return *cast<FStaticConstructObjectParameters*>::to(res);
        }
    };
};

struct UObjectAnnotation final : ManagedStructureAnnotation<UObject, false, false> {
    UObjectAnnotation(ModuleLibrary& ml) : ManagedStructureAnnotation("UObject", ml){
    }
    void init() {
    }
    bool hasNonTrivialCtor() const override { return true; }
    bool canClone() const override { return false; }
    bool canNew() const override { return false; }
};
MAKE_EXTERNAL_TYPE_FACTORY(UObject, UObject);


struct UClassAnnotation final : ManagedStructureAnnotation<UClass, false, false> {
    UClassAnnotation(ModuleLibrary& ml) : ManagedStructureAnnotation("UClass", ml) {
    }
    void init() {
    }
    bool hasNonTrivialCtor() const override { return true; }
    bool canClone() const override { return false; }
    bool canNew() const override { return false; }
};
MAKE_EXTERNAL_TYPE_FACTORY(UClass, UClass);


struct FStaticConstructObjectParametersAnnotation final : ManagedStructureAnnotation<FStaticConstructObjectParameters> {
    FStaticConstructObjectParametersAnnotation(ModuleLibrary& ml) : ManagedStructureAnnotation("FStaticConstructObjectParameters", ml) {
    }
    void init() {
    }
};
MAKE_EXTERNAL_TYPE_FACTORY(FStaticConstructObjectParameters, FStaticConstructObjectParameters);


struct FStringAnnotation final : ManagedStructureAnnotation<FString> {
    FStringAnnotation(ModuleLibrary& ml) : ManagedStructureAnnotation("FString", ml) {
    }
    void init() {
    }
    bool isLocal() const override { return true; }
    bool canMove() const override { return true; }
};
MAKE_EXTERNAL_TYPE_FACTORY(FString, FString);


struct FTextAnnotation final : ManagedStructureAnnotation<FText> {
    FTextAnnotation(ModuleLibrary& ml) : ManagedStructureAnnotation("FText", ml) {
    }
    void init() {
    }
    bool hasNonTrivialCtor() const override { return true; }
};
MAKE_EXTERNAL_TYPE_FACTORY(FText, FText);

struct FNameAnnotation final : ManagedStructureAnnotation<FName> {
    FNameAnnotation(ModuleLibrary& ml) : ManagedStructureAnnotation("FName", ml) {
    }
    void init() {
    }
    bool hasNonTrivialCtor() const override { return true; }
};
MAKE_EXTERNAL_TYPE_FACTORY(FName, FName);

namespace das {
    Module_Unreal::Module_Unreal()
        :Module("unreal")
    {
    }

    bool Module_Unreal::initDependencies() {
        ModuleLibrary lib(this);
        lib.addBuiltInModule();

        addAnnotation(make_smart<FStringAnnotation>(lib));
        addAnnotation(make_smart<FTextAnnotation>(lib));
        addAnnotation(make_smart<FNameAnnotation>(lib));

        addAnnotation(make_smart<UObjectAnnotation>(lib));
        addAnnotation(make_smart<UClassAnnotation>(lib));
        addAnnotation(make_smart<FStaticConstructObjectParametersAnnotation>(lib));

        addExtern<DAS_BIND_FUN(UFunctionCaller::callUFunctionOn)>(*this, lib, "call_ufunction_on", SideEffects::worstDefault);
        addExtern<DAS_BIND_FUN(getClassByName)>(*this, lib, "get_class_by_name", SideEffects::worstDefault);
        addExtern<UObject* (*)(UObject*, UClass*, FName), newObjectFromClass>(*this, lib, "new_object_from_class", SideEffects::worstDefault);
        addExtern<UObject* (*)(FStaticConstructObjectParameters), newObjectFromClass>(*this, lib, "new_object_from_class", SideEffects::worstDefault);

        addCtorAndUsing<FString>(*this, lib, "FString", "FString");
        addCtorAndUsing<FString, const char*>(*this, lib, "FString", "FString")
            ->args({ "Str" });

        addCtorAndUsing<FName>(*this, lib, "FName", "FName");
        addCtorAndUsing<FName, const char*>(*this, lib, "FName", "FName")
            ->args({ "Str" });

        addCtorAndUsing<FStaticConstructObjectParameters, UClass*>(*this, lib, "FStaticConstructObjectParameters", "FStaticConstructObjectParameters")
            ->args({ "Class" });

        return true;
    }

}; //namespace das
IMPLEMENT_EXTERNAL_TYPE_FACTORY(UObject, UObject);
IMPLEMENT_EXTERNAL_TYPE_FACTORY(UClass, UClass);
IMPLEMENT_EXTERNAL_TYPE_FACTORY(FStaticConstructObjectParameters, FStaticConstructObjectParameters);
IMPLEMENT_EXTERNAL_TYPE_FACTORY(FString, FString);
IMPLEMENT_EXTERNAL_TYPE_FACTORY(FText, FText);
IMPLEMENT_EXTERNAL_TYPE_FACTORY(FName, FName);

REGISTER_MODULE_IN_NAMESPACE(Module_Unreal, das);