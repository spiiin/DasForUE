#include "DasMathAdapters.h"
#include "InputActionValue.h"
#include "UObject/Class.h"

namespace
{
template <class T> bool MathValid(const T& Value)
{
    if constexpr (std::is_same_v<T, FTransform>)
        return !Value.ContainsNaN() && Value.IsRotationNormalized();
    else if constexpr (std::is_same_v<T, FQuat>)
        return !Value.ContainsNaN();
    else if constexpr (std::is_same_v<T, FLinearColor>)
        return FMath::IsFinite(Value.R) && FMath::IsFinite(Value.G) && FMath::IsFinite(Value.B) &&
               FMath::IsFinite(Value.A);
    else if constexpr (std::is_same_v<T, FColor>)
        return true;
    else
        return !Value.ContainsNaN();
}
// Leaf value structs with text literal defaults whose values must be finite.
template <class T> class TDasMathAdapter final : public IDasTypeAdapter
{
public:
    TDasMathAdapter(const TCHAR* InDefaultKind, TOptional<EDasMethodSignature> InSignature = {})
        : DefaultKind(InDefaultKind), Signature(InSignature)
    {
    }
    virtual const TCHAR* GetDefaultKind() const override { return DefaultKind; }
    virtual TOptional<EDasMethodSignature> GetSignature() const override { return Signature; }
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

private:
    const TCHAR* DefaultKind;
    TOptional<EDasMethodSignature> Signature;
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
    return MakeShared<TDasMathAdapter<FVector>>(TEXT("vector"), EDasMethodSignature::Vector);
}
TSharedRef<const IDasTypeAdapter> MakeDasVector2DAdapter()
{
    return MakeShared<TDasMathAdapter<FVector2D>>(TEXT("vector2d"));
}
TSharedRef<const IDasTypeAdapter> MakeDasRotatorAdapter()
{
    return MakeShared<TDasMathAdapter<FRotator>>(TEXT("rotator"), EDasMethodSignature::Rotator);
}
TSharedRef<const IDasTypeAdapter> MakeDasTransformAdapter()
{
    return MakeShared<TDasMathAdapter<FTransform>>(TEXT("transform"), EDasMethodSignature::Transform);
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

TSharedRef<const IDasTypeAdapter> MakeDasQuatAdapter()
{
    return MakeShared<TDasMathAdapter<FQuat>>(TEXT("quat"));
}

TSharedRef<const IDasTypeAdapter> MakeDasLinearColorAdapter()
{
    return MakeShared<TDasMathAdapter<FLinearColor>>(TEXT("linear_color"));
}

TSharedRef<const IDasTypeAdapter> MakeDasColorAdapter()
{
    return MakeShared<TDasMathAdapter<FColor>>(TEXT("color"));
}
