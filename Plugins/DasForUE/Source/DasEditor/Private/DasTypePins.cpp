#include "DasTypePins.h"
#include "DasTypeAdapters.h"
#include "DasValueKinds.h"
#include "DasPropertyTypes.h"
#include "DasContainerTypes.h"
#include "EdGraphSchema_K2.h"
#include "KismetCompilerMisc.h"
#include "KismetCompiler.h"
#include "UObject/Package.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"

namespace
{
bool ResolveType(const FDasDeclaredType& Input, FDasDeclaredType& Output, const FDasPinTypeResolver& Resolver,
                 FString& Error, int32 Depth, int32& Nodes)
{
    if (Depth > 16 || ++Nodes > 1024)
    {
        Error = TEXT("Pin descriptor exceeds depth/node limit");
        return false;
    }
    Output = Input;
    const auto Kind = Input.GetKind();
    if (Resolver && (Kind == EDasTypeKind::Struct || Kind == EDasTypeKind::Enum || Kind == EDasTypeKind::Object ||
                     Kind == EDasTypeKind::Class))
    {
        auto* Object = Resolver(Kind, Input.TypePath);
        if (!Object || (Kind == EDasTypeKind::Struct ? !Object->IsA<UScriptStruct>()
                        : Kind == EDasTypeKind::Enum ? !Object->IsA<UEnum>()
                                                     : !Object->IsA<UClass>()))
        {
            Error = TEXT("Unresolved or incompatible pin type: ")+ Input.TypePath;
            return false;
        }
        Output.TypePath = Object->GetPathName();
    }
    auto Child = [&](const TSharedPtr<const FDasDeclaredType>& In, TSharedPtr<const FDasDeclaredType>& Out)
    {
        if (!In)
            return true;
        auto Copy = MakeShared<FDasDeclaredType>();
        if (!ResolveType(*In, *Copy, Resolver, Error, Depth + 1, Nodes))
            return false;
        Out = Copy;
        return true;
    };
    return Child(Input.Element, Output.Element) && Child(Input.Key, Output.Key) && Child(Input.Value, Output.Value);
}
bool ValueCategory(FName C)
{
    return C == UEdGraphSchema_K2::PC_SoftObject || C == UEdGraphSchema_K2::PC_SoftClass ||
           C == UEdGraphSchema_K2::PC_Int || C == UEdGraphSchema_K2::PC_Byte || C == UEdGraphSchema_K2::PC_Text ||
           C == UEdGraphSchema_K2::PC_Real || C == UEdGraphSchema_K2::PC_Boolean || C == UEdGraphSchema_K2::PC_String ||
           C == UEdGraphSchema_K2::PC_Name || C == UEdGraphSchema_K2::PC_Object ||
           C == UEdGraphSchema_K2::PC_Interface || C == UEdGraphSchema_K2::PC_Class ||
           C == UEdGraphSchema_K2::PC_Struct;
}
bool ConcreteTerm(FName Category, FName Subcategory, UObject* Object)
{
    if (!ValueCategory(Category) || Subcategory == UEdGraphSchema_K2::PSC_Self)
        return false;
    if (Category == UEdGraphSchema_K2::PC_Real)
        return Subcategory == UEdGraphSchema_K2::PC_Float || Subcategory == UEdGraphSchema_K2::PC_Double;
    if (Category == UEdGraphSchema_K2::PC_Struct)
        return Cast<UScriptStruct>(Object) != nullptr;
    if (Category == UEdGraphSchema_K2::PC_SoftObject || Category == UEdGraphSchema_K2::PC_SoftClass ||
        Category == UEdGraphSchema_K2::PC_Object || Category == UEdGraphSchema_K2::PC_Interface ||
        Category == UEdGraphSchema_K2::PC_Class)
    {
        auto* Class = Cast<UClass>(Object);
        return Class && (Class->HasAnyClassFlags(CLASS_Interface) == (Category == UEdGraphSchema_K2::PC_Interface));
    }
    if (Category == UEdGraphSchema_K2::PC_Byte)
        return !Object || Cast<UEnum>(Object);
    return Object == nullptr;
}
}

bool DasTypeToPin(const FDasDeclaredType& Type, FEdGraphPinType& Out, FString& Error,
                  const FDasPinTypeResolver& Resolver)
{
    Error.Reset();
    if (!IsInGameThread())
    {
        Error = TEXT("Pin conversion requires the game thread");
        return false;
    }
    FDasDeclaredType Resolved;
    int32 Nodes = 0;
    if (!ResolveType(Type, Resolved, Resolver, Error, 0, Nodes))
        return false;
    // The property is deliberately unattached: conversion reads its shape, while
    // temporary ownership never modifies a Blueprint or a native declaration.
    TUniquePtr<FProperty> Property(
        CreateDasProperty(FFieldVariant(), TEXT("PinValue"), Resolved, EDasPropertyUsage::Value, Error));
    if (!Property)
        return false;
    FEdGraphPinType Pin;
    if (!GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(Property.Get(), Pin))
    {
        Error = TEXT("Kismet cannot represent property type: ")+ Resolved.Kind;
        return false;
    }
    Out = MoveTemp(Pin);
    return true;
}

bool DescribeDasPin(const FEdGraphPinType& Pin, FDasDeclaredType& Out, FString& Error)
{
    Error.Reset();
    if (!IsInGameThread())
    {
        Error = TEXT("Pin conversion requires the game thread");
        return false;
    }
    if (!ConcreteTerm(Pin.PinCategory, Pin.PinSubCategory, Pin.PinSubCategoryObject.Get()) ||
        (Pin.IsMap() && !ConcreteTerm(Pin.PinValueType.TerminalCategory, Pin.PinValueType.TerminalSubCategory,
                                      Pin.PinValueType.TerminalSubCategoryObject.Get())) ||
        Pin.bIsWeakPointer ||
        (Pin.ContainerType != EPinContainerType::None && !Pin.IsArray() && !Pin.IsSet() && !Pin.IsMap()))
    {
        Error = TEXT("Unsupported or unresolved pin value type");
        return false;
    }
    TStrongObjectPtr<UScriptStruct> Scope(NewObject<UScriptStruct>(GetTransientPackage()));
    FCompilerResultsLog Log;
    TUniquePtr<FProperty> Property(FKismetCompilerUtilities::CreatePropertyOnScope(
        Scope.Get(), TEXT("PinValue"), Pin, nullptr, CPF_None, GetDefault<UEdGraphSchema_K2>(), Log));
    FDasDeclaredType Type;
    if (Log.NumErrors || !Property)
    {
        Error = TEXT("Kismet cannot create the pin value property");
        return false;
    }
    if (!DescribeDasProperty(Property.Get(), Type, Error) ||
        !ValidateDasPropertyType(Type, EDasPropertyUsage::Value, Error))
        return false;
    Out = MoveTemp(Type);
    return true;
}

bool DasSchemaType(EDasMethodSignature Kind, const FString& Path, FDasDeclaredType& Out, FString& Error)
{
    Error.Reset();
    const EDasTypeKind TypeKind = DasTypeKindOf(Kind);
    if (TypeKind == EDasTypeKind::Invalid || TypeKind == EDasTypeKind::Void)
    {
        Error = TEXT("Schema kind has no pin value type");
        return false;
    }
    if (Kind == EDasMethodSignature::Struct && !Path.IsEmpty())
        if (auto* Property = DasContainerProperty(Path))
            return DescribeDasProperty(Property, Out, Error);
    FDasDeclaredType Type;
    Type.Kind = DasTypeKindName(TypeKind);
    if (const auto* Adapter = FDasTypeAdapters::Get().FindSignature(Kind))
        Type.TypePath = Adapter->GetStruct()->GetPathName();
    else if (Kind == EDasMethodSignature::Delegate)
        Type.DelegateWire = Path;
    else if (TypeKind == EDasTypeKind::Object || TypeKind == EDasTypeKind::Class || TypeKind == EDasTypeKind::Enum ||
             TypeKind == EDasTypeKind::Struct)
        Type.TypePath = Path;
    Out = MoveTemp(Type);
    return true;
}

FEdGraphPinType DasTypePinChecked(const FDasDeclaredType& Type, const FDasPinTypeResolver& Resolver)
{
    FEdGraphPinType Pin;
    FString Error;
    const bool Valid = DasTypeToPin(Type, Pin, Error, Resolver);
    checkf(Valid, TEXT("Invalid prevalidated pin descriptor: %s"), *Error);
    return Pin;
}
FEdGraphPinType DasSchemaPinChecked(EDasMethodSignature Kind, const FString& Path)
{
    FDasDeclaredType Type;
    FString Error;
    const bool Valid = DasSchemaType(Kind, Path, Type, Error);
    checkf(Valid, TEXT("Invalid prevalidated schema pin: %s"), *Error);
    return Valid ? DasTypePinChecked(Type) : FEdGraphPinType();
}
