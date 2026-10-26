#include "DasStaticLibraryProbe.h"
#include "DasRuntime.h"
#include "DasSchema.h"
#include "Engine/World.h"
#include "UObject/UnrealType.h"
#include "UObject/TextProperty.h"
#include "UObject/StrongObjectPtr.h"
#include "Modules/ModuleManager.h"
bool RunDasTextByteContainerProbe(ADasStaticLibraryProbe* Carrier, FString& Report)
{
    bool Good = true;
    auto Check = [&](bool V, const TCHAR* N)
    {
        Good &= V;
        Report += FString(V ? TEXT("PASS "): TEXT("FAIL ")) + N + TEXT("\n");
    };
    UClass* Class = Carrier ? Carrier->TextByteContainerClass.Get() : nullptr;
    if (!Class)
    {
        Check(false, TEXT("text_byte_container_cooked_class"));
        return false;
    }
    auto* Bytes = FindFProperty<FArrayProperty>(Class, TEXT("Weights"));
    auto* Texts = FindFProperty<FArrayProperty>(Class, TEXT("Messages"));
    auto* Labels = FindFProperty<FMapProperty>(Class, TEXT("ByteTexts"));
    auto* Tags = FindFProperty<FSetProperty>(Class, TEXT("ByteTags"));
    auto RawByte = [](FProperty* P)
    {
        auto* B = CastField<FByteProperty>(P);
        return B && !B->Enum;
    };
    Check(Bytes && RawByte(Bytes->Inner) && Texts && Texts->Inner->IsA<FTextProperty>() && Labels &&
              RawByte(Labels->KeyProp) && Labels->ValueProp->IsA<FTextProperty>() && Tags && RawByte(Tags->ElementProp),
          TEXT("text_byte_container_reflected_identity"));
    if (!Bytes || !Texts || !Labels || !Tags)
        return false;
    auto* CDO = Class->GetDefaultObject();
    FScriptArrayHelper B(Bytes, Bytes->ContainerPtrToValuePtr<void>(CDO)),
        T(Texts, Texts->ContainerPtrToValuePtr<void>(CDO));
    bool Defaults = B.Num() == 2 && T.Num() == 2;
    if (Defaults)
    {
        const auto First = CastFieldChecked<FTextProperty>(Texts->Inner)->GetPropertyValue(T.GetRawPtr(0));
        const auto Second = CastFieldChecked<FTextProperty>(Texts->Inner)->GetPropertyValue(T.GetRawPtr(1));
        const auto Key = FTextInspector::GetKey(First);
        Defaults = CastFieldChecked<FByteProperty>(Bytes->Inner)->GetPropertyValue(B.GetRawPtr(0)) == 0 &&
                   CastFieldChecked<FByteProperty>(Bytes->Inner)->GetPropertyValue(B.GetRawPtr(1)) == 255 &&
                   Key.IsSet() && Key.GetValue() == TEXT("First") && Second.IsCultureInvariant();
    }
    Check(Defaults, TEXT("text_byte_container_saved_literal_defaults"));
    FActorSpawnParameters Spawn;
    Spawn.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    auto* A = Carrier->GetWorld()->SpawnActor<AActor>(Class, FTransform::Identity, Spawn);
    if (!A)
    {
        Check(false, TEXT("text_byte_container_spawn"));
        return false;
    }
    TStrongObjectPtr<AActor> Keep(A);
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    for (int32 I = 0; I < 2; ++I)
    {
        const auto V = R.RunNamedFramePersistent(R.ScriptPath(TEXT("dsl_text_byte_containers_calls.das")),
                                                              TEXT("main"), {}, EDasMethodSignature::Int, A);
        Check(V.bSucceeded && V.Value == 0, TEXT("text_byte_container_fields_structs_methods_ref_out_all_keys"));
        if (!V.bSucceeded)
            Report += V.Error + TEXT("\n");
        else if (V.Value)
            Report += FString::Printf(TEXT("case=%d\n"), V.Value);
        R.ReleaseObject(A);
    }
    A->Destroy();
    R.ReleaseObject(A);
    return RunDasTextByteEventsProbe(Carrier, Report) && Good;
}
