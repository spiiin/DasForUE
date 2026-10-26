#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DasProbeActor.h"
#include "DasProbeInterface.h"
#include "DasObjectFixture.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasFloatTestSignal, float, Value);

UENUM(BlueprintType)
enum class EDasCallMode : uint8
{
    Idle = 0,
    Active = 7,
    Paused = 42
};
UENUM()
enum EDasLegacyMode
{
    DasLegacyIdle = 0,
    DasLegacyActive = 9
};
USTRUCT(BlueprintType)
struct FDasNamedMode
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    EDasCallMode Mode = EDasCallMode::Idle;
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    FName Key;
};

USTRUCT(BlueprintType)
struct FDasEnumBundle
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    TEnumAsByte<EDasLegacyMode> Legacy = DasLegacyIdle;
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    FDasNamedMode Nested;
};

USTRUCT(BlueprintType)
struct FDasCallMetrics
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    float Ratio = 0;
};

USTRUCT(BlueprintType)
struct FDasCallPayload
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    FDasCallMetrics Metrics;
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    int32 Count = 0;
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    FString Label;
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    bool bEnabled = false;
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    double Weight = 0;
};

USTRUCT(BlueprintType)
struct FDasObjectSnapshot
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    TObjectPtr<UObject> Object;
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    TObjectPtr<AActor> Actor;
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    FName Key;
};
USTRUCT(BlueprintType)
struct FDasNestedObjectSnapshot
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    FDasObjectSnapshot Details;
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    TArray<FDasObjectSnapshot> Items;
};

USTRUCT(BlueprintType)
struct FDasHashSnapshot
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    TMap<int32, FString> Values;
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    TSet<FString> Tags;
};
USTRUCT(BlueprintType)
struct FDasHashContainerSnapshot
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    TArray<FDasHashSnapshot> Items;
};

UCLASS(Transient)
class UDasReferenceFixture : public UObject, public IDasEventInterface
{
    GENERATED_BODY()
public:
    virtual void Notify_Implementation(int32) override {}
    virtual void ResetCounter_Implementation() override {}
};

// Small reflected fixture shared by the editor and packaged interop tests.
UCLASS(Transient, NotBlueprintable)
class UDasObjectFixture : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    TScriptInterface<IDasEventInterface> Events;
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    FString DSLText;
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    double DSLPrecise = 1.23456789012345;
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    FName DSLKey;
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    bool bScalar = false;
    UPROPERTY(BlueprintAssignable, Category = "Probe")
    FDasIntSignal OnDasSignal;
    UPROPERTY(BlueprintAssignable, Category = "Probe")
    FDasFloatTestSignal OnFloatSignal;
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    TObjectPtr<UObject> Target;
    UFUNCTION(BlueprintCallable, Category = "Probe")
    FDasHashContainerSnapshot EchoNestedHashes(const FDasHashContainerSnapshot& Data)
    {
        ++MarshalledCalls;
        FDasHashContainerSnapshot Result = Data;
        for (auto& Item : Result.Items)
        {
            Item.Values.Remove(-100);
            Item.Tags.Remove(TEXT("drop"));
        }
        return Result;
    }
    UFUNCTION()
    void DelegatePair(UObject* Object, float Value) { health += Object ? int32(Value) : 0; }
    UFUNCTION()
    void DelegateIncrement(int32 Value) { health += Value; }
    UFUNCTION()
    void DelegateDouble(int32 Value) { health += Value * 2; }
    UFUNCTION(BlueprintCallable, Category = "Probe")
    int32 AddHealth(int32 Value)
    {
        health += Value;
        return health;
    }
    UFUNCTION(BlueprintCallable, Category = "Probe")
    float ScaleSpeed(float Value) { return speed * Value; }
    UFUNCTION(BlueprintCallable, Category = "Probe")
    void SetHealthValue(int32 Value) { health = Value; }
    UFUNCTION(BlueprintCallable, Category = "Probe")
    UObject* GetTargetObject() { return Target; }
    UFUNCTION(BlueprintCallable, Category = "Probe")
    void SetFixtureTarget(UDasObjectFixture* Value) { Target = Value; }
    UFUNCTION(BlueprintCallable, Category = "Probe")
    void OutValue(int32& Value) { Value = health; }
    UFUNCTION(BlueprintCallable, Category = "Probe")
    UObject* ExchangeObject(UDasObjectFixture* Input, UPARAM(ref) UObject*& InOut, UObject*& Output)
    {
        ++MarshalledCalls;
        Output = InOut;
        InOut = Input;
        return Input;
    }
    UFUNCTION(BlueprintCallable, Category = "Probe")
    int32 MutateValues(UPARAM(ref) int32& Count, UPARAM(ref) FString& Text, bool& Accepted, FString& Description)
    {
        ++MarshalledCalls;
        Count += 3;
        Text += TEXT(" 世界");
        Accepted = true;
        Description = Text;
        return Count * 2;
    }
    UFUNCTION(BlueprintCallable, Category = "Probe")
    void MutatePayload(UPARAM(ref) FDasCallPayload& Payload, FDasCallPayload& Copy)
    {
        ++MarshalledCalls;
        Payload.Count += 4;
        Payload.Label += TEXT("!");
        Copy = Payload;
    }
    UFUNCTION(BlueprintCallable, Category = "Probe")
    UDasObjectFixture* NewFixture()
    {
        ++MarshalledCalls;
        return NewObject<UDasObjectFixture>();
    }
    UFUNCTION(BlueprintCallable, Category = "Probe")
    void ForceTestGC() { CollectGarbage(RF_NoFlags); }
    UFUNCTION(BlueprintCallable, Category = "Probe")
    EDasCallMode ExchangeMode(EDasCallMode Input, UPARAM(ref) EDasCallMode& Current, EDasCallMode& Previous)
    {
        ++MarshalledCalls;
        Previous = Current;
        Current = Input;
        return Input;
    }
    UFUNCTION(BlueprintCallable, Category = "Probe")
    TEnumAsByte<EDasLegacyMode> EchoLegacy(TEnumAsByte<EDasLegacyMode> Input)
    {
        ++MarshalledCalls;
        return Input;
    }
    UFUNCTION(BlueprintCallable, Category = "Probe")
    EDasCallMode InvalidMode()
    {
        ++MarshalledCalls;
        return static_cast<EDasCallMode>(123);
    }
    UFUNCTION(BlueprintCallable, Category = "Probe")
    FName ExchangeName(FName Input, UPARAM(ref) FName& Current, FName& Previous)
    {
        ++MarshalledCalls;
        Previous = Current;
        Current = Input;
        return Input;
    }
    UFUNCTION(BlueprintCallable, Category = "Probe")
    bool EqualNames(FName A, FName B)
    {
        ++MarshalledCalls;
        return A == B;
    }
    UFUNCTION(BlueprintCallable, Category = "Probe")
    void ExchangeNamedMode(UPARAM(ref) FDasNamedMode& Value, FDasNamedMode& Copy)
    {
        ++MarshalledCalls;
        Value.Mode = EDasCallMode::Active;
        Copy = Value;
    }
    UFUNCTION(BlueprintCallable, Category = "Policy", meta = (DeprecatedFunction, DeprecationMessage = "Use AddHealth"))
    int32 PolicyDeprecated(int32 Value) { return Value; }
    UFUNCTION(BlueprintCallable, Category = "Policy", meta = (BlueprintInternalUseOnly = "true"))
    int32 PolicyInternal(int32 Value) { return Value; }
    UFUNCTION(BlueprintPure, Category = "Policy", meta = (DisplayName = "Policy default", ToolTip = "Metadata fixture"))
    int32 PolicyDefault(int32 Value = 7) const { return Value; }
    // Keep int64 unsupported here so exporter refusal tests do not depend on engine API coverage.
    UFUNCTION(BlueprintCallable, Category = "Unsupported")
    int64 UnsupportedInt64(int64 Value) { return Value; }
    UFUNCTION(BlueprintCallable, Category = "Defaults")
    FString DefaultScalars(int32 Count = 7, bool bEnabled = true, float Ratio = 1.5f, double Weight = 1.25,
                           FString Label = TEXT("Hello {world}"), FName Key = NAME_None,
                                                EDasCallMode Mode = EDasCallMode::Active, UObject* Object = nullptr)
    {
        ++MarshalledCalls;
        return Count == 7 && bEnabled && Ratio == 1.5f && Weight == 1.25 &&
               Label == TEXT("Hello {world}") && Key.IsNone() && Mode == EDasCallMode::Active && Object == nullptr
                   ? TEXT("defaults")
                   : Label;
    }
    // UHT's CPP_Default_Value rounds this source literal; scripts use reflected defaults.
    UFUNCTION(BlueprintCallable, Category = "Defaults")
    double DefaultWeight(double Value = 1.23456789012345) { return Value; }
    UFUNCTION(BlueprintCallable, Category = "Arrays")
    TArray<int32> TransformInts(const TArray<int32>& Values)
    {
        ++MarshalledCalls;
        TArray<int32> Result = Values;
        for (auto& Value : Result)
            Value += 10;
        return Result;
    }
    UFUNCTION(BlueprintCallable, Category = "Arrays")
    int32 MutateStrings(UPARAM(ref) TArray<FString>& Values, TArray<FString>& Copy)
    {
        ++MarshalledCalls;
        Values.Add(TEXT("世界"));
        Copy = Values;
        return Values.Num();
    }
    UFUNCTION(BlueprintCallable, Category = "Arrays")
    TArray<FName> EchoNames(const TArray<FName>& Values)
    {
        ++MarshalledCalls;
        return Values;
    }
    UFUNCTION(BlueprintCallable, Category = "Arrays")
    TArray<float> EchoFloats(const TArray<float>& Values)
    {
        ++MarshalledCalls;
        return Values;
    }
    UFUNCTION(BlueprintCallable, Category = "Arrays")
    TArray<double> EchoDoubles(const TArray<double>& Values)
    {
        ++MarshalledCalls;
        return Values;
    }
    UFUNCTION(BlueprintCallable, Category = "Arrays")
    TArray<bool> EchoBools(const TArray<bool>& Values)
    {
        ++MarshalledCalls;
        return Values;
    }
    UFUNCTION(BlueprintCallable, Category = "Arrays")
    TArray<int32> OversizedArray()
    {
        ++MarshalledCalls;
        TArray<int32> Result;
        Result.SetNumZeroed(65537);
        return Result;
    }
    UFUNCTION(BlueprintCallable, Category = "Arrays")
    TArray<FDasCallPayload> TransformPayloads(const TArray<FDasCallPayload>& Values)
    {
        ++MarshalledCalls;
        TArray<FDasCallPayload> Result = Values;
        for (auto& Value : Result)
        {
            Value.Count += 4;
            Value.Label += TEXT(" 世界");
            Value.Metrics.Ratio *= 2;
        }
        return Result;
    }
    UFUNCTION(BlueprintCallable, Category = "Arrays")
    int32 MutateNamedModes(UPARAM(ref) TArray<FDasNamedMode>& Values, TArray<FDasNamedMode>& Copy)
    {
        ++MarshalledCalls;
        for (auto& Value : Values)
            Value.Mode = EDasCallMode::Active;
        FDasNamedMode Added;
        Added.Mode = EDasCallMode::Paused;
        Added.Key = TEXT("Added_23");
        Values.Add(Added);
        Copy = Values;
        return Values.Num();
    }
    UFUNCTION(BlueprintCallable, Category = "Arrays")
    TArray<EDasCallMode> EchoModes(const TArray<EDasCallMode>& Values)
    {
        ++MarshalledCalls;
        return Values;
    }
    UFUNCTION(BlueprintCallable, Category = "Arrays")
    TArray<UDasObjectFixture*> EchoFixtureObjects(const TArray<UDasObjectFixture*>& Values)
    {
        ++MarshalledCalls;
        CollectGarbage(RF_NoFlags);
        return Values;
    }
    UFUNCTION(BlueprintCallable, Category = "Arrays")
    int32 SwapFixtureObjects(UPARAM(ref) TArray<UDasObjectFixture*>& Values, TArray<UDasObjectFixture*>& Copy)
    {
        ++MarshalledCalls;
        Copy = Values;
        Values.Empty();
        Values.Add(this);
        Values.Add(NewObject<UDasObjectFixture>());
        return Values.Num();
    }
    UFUNCTION(BlueprintCallable, Category = "Arrays")
    void MakeFixtureObjects(TArray<UDasObjectFixture*>& Values)
    {
        ++MarshalledCalls;
        LastArrayCreated = NewObject<UDasObjectFixture>();
        Values.Add(LastArrayCreated.Get());
        Values.Add(nullptr);
        Values.Add(NewObject<UDasObjectFixture>());
    }
    UFUNCTION(BlueprintCallable, Category = "Arrays")
    void ReplaceFixtureObjects(UPARAM(ref) TArray<UDasObjectFixture*>& Values)
    {
        ++MarshalledCalls;
        Values.Empty();
        Values.Add(NewObject<UDasObjectFixture>());
    }
    UFUNCTION(BlueprintCallable, Category = "Arrays")
    TArray<AActor*> EchoActors(const TArray<AActor*>& Values)
    {
        ++MarshalledCalls;
        return Values;
    }
    TWeakObjectPtr<UDasObjectFixture> LastArrayCreated;
    UFUNCTION(BlueprintCallable, Category = "HashContainers")
    TMap<int32, FString> TransformMap(const TMap<int32, FString>& Values)
    {
        ++MarshalledCalls;
        TMap<int32, FString> Result;
        for (const auto& Pair : Values)
            Result.Add(Pair.Key, Pair.Value + TEXT(" 世界"));
        return Result;
    }
    UFUNCTION(BlueprintCallable, Category = "HashContainers")
    int32 MutateMap(UPARAM(ref) TMap<int32, FString>& Values, TMap<int32, FString>& Copy)
    {
        ++MarshalledCalls;
        Values.Remove(1);
        Values.Add(99, TEXT("added"));
        Copy = Values;
        return Values.Num();
    }
    UFUNCTION(BlueprintCallable, Category = "HashContainers")
    TMap<FName, EDasCallMode> EchoNameModeMap(const TMap<FName, EDasCallMode>& Values)
    {
        ++MarshalledCalls;
        return Values;
    }
    UFUNCTION(BlueprintCallable, Category = "HashContainers")
    TMap<FString, double> EchoDoubleMap(const TMap<FString, double>& Values)
    {
        ++MarshalledCalls;
        return Values;
    }
    UFUNCTION(BlueprintCallable, Category = "HashContainers")
    TMap<int32, float> EchoFloatMap(const TMap<int32, float>& Values)
    {
        ++MarshalledCalls;
        return Values;
    }
    UFUNCTION(BlueprintCallable, Category = "HashContainers")
    TMap<int32, bool> EchoBoolMap(const TMap<int32, bool>& Values)
    {
        ++MarshalledCalls;
        return Values;
    }
    UFUNCTION(BlueprintCallable, Category = "HashContainers")
    TSet<int32> TransformSet(const TSet<int32>& Values)
    {
        ++MarshalledCalls;
        TSet<int32> Result = Values;
        Result.Remove(1);
        Result.Add(99);
        return Result;
    }
    UFUNCTION(BlueprintCallable, Category = "HashContainers")
    int32 MutateSet(UPARAM(ref) TSet<FString>& Values, TSet<FString>& Copy)
    {
        ++MarshalledCalls;
        Values.Remove(TEXT("remove"));
        Values.Add(TEXT("世界"));
        Copy = Values;
        return Values.Num();
    }
    UFUNCTION(BlueprintCallable, Category = "HashContainers")
    TSet<FName> EchoNameSet(const TSet<FName>& Values)
    {
        ++MarshalledCalls;
        return Values;
    }
    UFUNCTION(BlueprintCallable, Category = "HashContainers")
    TSet<EDasCallMode> EchoModeSet(const TSet<EDasCallMode>& Values)
    {
        ++MarshalledCalls;
        return Values;
    }
    UFUNCTION(BlueprintCallable, Category = "HashContainers")
    void MakeMap(TMap<int32, FString>& Values)
    {
        ++MarshalledCalls;
        Values.Add(7, TEXT("out"));
    }
    UFUNCTION(BlueprintCallable, Category = "HashContainers")
    void MakeSet(TSet<int32>& Values)
    {
        ++MarshalledCalls;
        Values.Add(7);
    }
    UFUNCTION(BlueprintCallable, Category = "HashContainers")
    TSet<int32> OversizedSet()
    {
        ++MarshalledCalls;
        TSet<int32> Result;
        for (int32 Index = 0; Index < 65537; ++Index)
            Result.Add(Index);
        return Result;
    }
    UFUNCTION(BlueprintCallable, Category = "HashContainers")
    TMap<int32, FString> OversizedMap()
    {
        ++MarshalledCalls;
        TMap<int32, FString> Result;
        for (int32 Index = 0; Index < 65537; ++Index)
            Result.Add(Index, TEXT(""));
        return Result;
    }
    UFUNCTION(BlueprintCallable, Category = "HashContainers")
    TSet<float> EchoFloatSet(const TSet<float>& Values)
    {
        ++MarshalledCalls;
        return Values;
    }
    UFUNCTION(BlueprintCallable, Category = "HashContainers")
    TSet<double> EchoDoubleSet(const TSet<double>& Values)
    {
        ++MarshalledCalls;
        return Values;
    }
    UFUNCTION(BlueprintCallable, Category = "HashContainers")
    TMap<float, bool> EchoFloatKeyMap(const TMap<float, bool>& Values)
    {
        ++MarshalledCalls;
        return Values;
    }
    int32 MarshalledCalls = 0;
    UFUNCTION(BlueprintCallable, Category = "Probe")
    FDasCallPayload TransformPayload(const FDasCallPayload& Payload, int32 Delta, bool bUpper, const FString& Suffix)
    {
        ++MarshalledCalls;
        FDasCallPayload Result = Payload;
        Result.Count += Delta;
        Result.Label = (bUpper ? Payload.Label.ToUpper() : Payload.Label) + Suffix;
        Result.bEnabled = !Payload.bEnabled;
        Result.Weight *= 2;
        Result.Metrics.Ratio *= 3;
        return Result;
    }
    UFUNCTION(BlueprintCallable, Category = "Probe")
    bool AcceptLabel(const FString& Label, bool bAccept)
    {
        ++MarshalledCalls;
        return bAccept && !Label.IsEmpty();
    }
    int32 ForeignReceived = 0;
    UFUNCTION()
    void RecordForeignSignal(int32 Value) { ForeignReceived += Value; }
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    float speed = 37.f;
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    int32 health = 100;
    UPROPERTY(BlueprintReadOnly, Category = "Probe")
    int32 readOnly = 7;
    // Self containers changed in place (DasSelfContainerTests.inl).
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    TArray<int32> SelfInts;
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    TArray<FString> SelfStrings;
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    TMap<FName, double> SelfScores;
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    TMap<int32, int32> InteropCounts;
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    TMap<FString, double> InteropStrings;
    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    TMap<uint8, int32> InteropBytes;
    UPROPERTY(BlueprintReadOnly, Category = "Probe")
    TMap<int32, int32> InteropLocked;

    UPROPERTY(BlueprintReadWrite, Category = "Probe")
    TSet<int32> SelfIds;
    UPROPERTY(BlueprintReadOnly, Category = "Probe")
    TArray<int32> SelfLockedInts;
};
