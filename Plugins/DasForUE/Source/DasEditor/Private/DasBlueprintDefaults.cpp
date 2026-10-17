#include "DasBlueprintDefaults.h"
#include "DasContainerTypes.h"
#include "DasDefaultRoots.h"
#include "UObject/UnrealType.h"
#include "UObject/NoExportTypes.h"
namespace {
    FString PreciseDasStructDefault(const UScriptStruct* Type,const void* Memory){FString Text=TEXT("(");bool First=true;for(TFieldIterator<FProperty> It(Type);It;++It){auto* P=*It;const void* Value=P->ContainerPtrToValuePtr<void>(Memory);FString Field;if(auto* S=CastField<FStructProperty>(P))Field=PreciseDasStructDefault(S->Struct,Value);else if(auto* N=CastField<FNumericProperty>(P);N && N->IsFloatingPoint())Field=FString::Printf(TEXT("%.17g"),N->GetFloatingPointPropertyValue(Value));else P->ExportTextItem_Direct(Field,Value,nullptr,nullptr,PPF_Delimited);if(!First)Text+=TEXT(",");First=false;Text+=P->GetName()+TEXT("=")+Field;}return Text+TEXT(")");}
    FString BlueprintDasStructDefault(const UScriptStruct* Type,const void* Memory){
        if(Type!=TBaseStructure<FTransform>::Get())return PreciseDasStructDefault(Type,Memory);
        // Blueprint variable defaults use the Transform pin grammar, unlike struct ImportText.
        const auto& Value=*static_cast<const FTransform*>(Memory);const auto T=Value.GetTranslation(),S=Value.GetScale3D();const auto R=Value.Rotator();
        return FString::Printf(TEXT("%.17g,%.17g,%.17g|%.17g,%.17g,%.17g|%.17g,%.17g,%.17g"),T.X,T.Y,T.Z,R.Pitch,R.Yaw,R.Roll,S.X,S.Y,S.Z);
    }
}
bool DasBlueprintFieldDefault(const FDasFieldSchema& Field,FString& Out,FDasDefaultRoots& Roots,FString& Error){
    if(Field.Kind==EDasScalarKind::Struct){
        FInstancedStruct Value;
        if(!ParseDasStructDefault(Field.StructPath,Field.StructDefault,Value,Error))return false;
        auto* Container=DasContainerProperty(Field.StructPath);
        FString Text=Container?DasPropertyDefaultText(Container,Container->ContainerPtrToValuePtr<void>(Value.GetMemory())):BlueprintDasStructDefault(Value.GetScriptStruct(),Value.GetMemory());
        Roots.Values.Add(MoveTemp(Value));Out=MoveTemp(Text);return true;
    }
    if(Field.Kind==EDasScalarKind::Delegate || Field.Kind==EDasScalarKind::Event){Error=TEXT("Delegate defaults require the signature service");return false;}
    Out=Field.Kind==EDasScalarKind::Text?Field.StringDefault:Field.Kind==EDasScalarKind::Byte?FString::FromInt(Field.IntDefault):Field.Kind==EDasScalarKind::Enum?Field.EnumDefault:(Field.Kind==EDasScalarKind::Object || Field.Kind==EDasScalarKind::Class)?(Field.StringDefault.IsEmpty()?TEXT("None"):Field.StringDefault):Field.Kind==EDasScalarKind::Int32?FString::FromInt(Field.IntDefault):Field.Kind==EDasScalarKind::Float32?FString::Printf(TEXT("%.9g"),Field.FloatDefault):Field.Kind==EDasScalarKind::Bool?(Field.BoolDefault?TEXT("true"):TEXT("false")):Field.Kind==EDasScalarKind::Double?FString::Printf(TEXT("%.17g"),Field.DoubleDefault):Field.Kind==EDasScalarKind::String?Field.StringDefault:Field.NameDefault.ToString();return true;
}
