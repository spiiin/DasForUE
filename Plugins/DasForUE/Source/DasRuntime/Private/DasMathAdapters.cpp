#include "DasMathAdapters.h"
#include "InputActionValue.h"
#include "UObject/Class.h"

namespace
{
template <class T> bool MathValid(const T& Value)
{
    if constexpr (std::is_same_v<T, FTransform>)
        return !Value.ContainsNaN() && Value.IsRotationNormalized();
    else
        return !Value.ContainsNaN();
}
// Leaf value structs with text literal defaults whose values must be finite.
template <class T> class TDasMathAdapter final : public IDasTypeAdapter
{
public:
    virtual EDasTypeKind GetKind() const override { return EDasTypeKind::Struct; }
    virtual const UScriptStruct* GetStruct() const override { return TBaseStructure<T>::Get(); }
    virtual bool IsLeafValue() const override { return true; }
    virtual bool IsMath() const override { return true; }
    virtual bool ValidateValue(const void* Data, FString& Error) const override
    {
        if (MathValid(*static_cast<const T*>(Data)))
            return true;
        Error = TEXT("Invalid nested math snapshot value");
        return false;
    }
};
// InputActionValue: identity and its handle view only; values travel through the input snapshot
// (FDasInputActionValue) and their own bridge.
class FDasInputActionValueAdapter final : public IDasTypeAdapter
{
public:
    virtual EDasTypeKind GetKind() const override { return EDasTypeKind::Struct; }
    virtual const UScriptStruct* GetStruct() const override { return FInputActionValue::StaticStruct(); }
};
}

TSharedRef<const IDasTypeAdapter> MakeDasVectorAdapter()
{
    return MakeShared<TDasMathAdapter<FVector>>();
}
TSharedRef<const IDasTypeAdapter> MakeDasVector2DAdapter()
{
    return MakeShared<TDasMathAdapter<FVector2D>>();
}
TSharedRef<const IDasTypeAdapter> MakeDasRotatorAdapter()
{
    return MakeShared<TDasMathAdapter<FRotator>>();
}
TSharedRef<const IDasTypeAdapter> MakeDasTransformAdapter()
{
    return MakeShared<TDasMathAdapter<FTransform>>();
}
TSharedRef<const IDasTypeAdapter> MakeDasInputActionValueAdapter()
{
    return MakeShared<FDasInputActionValueAdapter>();
}
bool DasMathValueValid(const UScriptStruct* Type, const void* Data)
{
    FString Error;
    const auto* Adapter = FDasTypeAdapters::Get().FindStruct(Type);
    return !Adapter || !Adapter->IsMath() || Adapter->ValidateValue(Data, Error);
}
