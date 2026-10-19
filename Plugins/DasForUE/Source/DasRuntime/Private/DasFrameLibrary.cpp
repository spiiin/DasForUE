#include "DasFrameLibrary.h"
#include "DasContainerTypes.h"
#include "UObject/Class.h"
#include "DasSchema.h"
FDasScalarValue UDasFrameLibrary::DefaultValue(int32 Kind, const FString& ClassPath, int32 Direction)
{
    FDasScalarValue V;
    V.Kind = Kind;
    V.Direction = Direction;
    V.ObjectClass = ClassPath;
    if (Kind == 12)
    {
        if (auto* P = DasContainerProperty(ClassPath))
            V.StructValue.InitializeAs(CastChecked<UScriptStruct>(P->GetOwnerStruct()));
        else
        {
            FString Error;
            ParseDasStructDefault(ClassPath, TEXT(""), V.StructValue, Error);
        }
    }
    if (Kind == 13)
    {
        FString Error;
        if (auto* Enum = ResolveDasEnum(ClassPath, Error))
            for (int32 I = 0; I < Enum->NumEnums(); ++I)
                if (ValidateDasEnumNumber(ClassPath, Enum->GetValueByIndex(I), Error))
                {
                    V.IntValue = int32(Enum->GetValueByIndex(I));
                    break;
                }
    }
    if (Kind == 11)
        V.TransformValue = FTransform::Identity;
    return V;
}
FDasScalarValue UDasFrameLibrary::OutputAt(const TArray<FDasScalarValue>& Values, int32 Index)
{
    if (Values.IsValidIndex(Index))
        return Values[Index];
    FDasScalarValue Invalid;
    Invalid.Kind = -1;
    return Invalid;
}
