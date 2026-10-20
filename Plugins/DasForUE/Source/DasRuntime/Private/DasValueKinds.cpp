#include "DasValueKinds.h"

namespace
{
using F = EDasScalarKind;
using S = EDasMethodSignature;
const FDasValueKindRow Rows[] = {
    {EDasTypeKind::Void, TEXT("void"), {}, S::Void},
     {EDasTypeKind::Byte, TEXT("byte"), F::Byte, S::Byte},
      {EDasTypeKind::Text, TEXT("text"), F::Text, S::Text},
       {EDasTypeKind::Int, TEXT("int"), F::Int32, S::Int},
        {EDasTypeKind::Float, TEXT("float"), F::Float32, S::Float},
         {EDasTypeKind::Double, TEXT("double"), F::Double, S::Double},
          {EDasTypeKind::Bool, TEXT("bool"), F::Bool, S::Bool},
           {EDasTypeKind::String, TEXT("string"), F::String, S::String},
            {EDasTypeKind::Name, TEXT("name"), F::Name, S::Name},
             {EDasTypeKind::Object, TEXT("object"), F::Object, S::Object},
              {EDasTypeKind::Class, TEXT("class"), F::Class, S::Class},
               {EDasTypeKind::Struct, TEXT("struct"), F::Struct, S::Struct},
                {EDasTypeKind::Enum, TEXT("enum"), F::Enum, S::Enum},
                 {EDasTypeKind::Array, TEXT("array"), F::Struct, S::Struct},
                  {EDasTypeKind::Set, TEXT("set"), F::Struct, S::Struct},
                   {EDasTypeKind::Map, TEXT("map"), F::Struct, S::Struct},
                    {EDasTypeKind::Delegate, TEXT("delegate"), F::Delegate, S::Delegate},
                     {EDasTypeKind::Event, TEXT("event"), F::Event, {}},
};
const FDasValueKindRow* Row(EDasTypeKind Kind)
{
    for (const auto& R : Rows)
        if (R.Kind == Kind)
            return &R;
    return nullptr;
}
}

TConstArrayView<FDasValueKindRow> DasValueKinds()
{
    return Rows;
}
EDasTypeKind DasTypeKind(const FString& Name)
{
    for (const auto& R : Rows)
        if (Name == R.Name)
            return R.Kind;
    return EDasTypeKind::Invalid;
}
FString DasTypeKindName(EDasTypeKind Kind)
{
    const auto* R = Row(Kind);
    return R ? R->Name : FString();
}
TOptional<EDasScalarKind> DasFieldKind(EDasTypeKind Kind)
{
    const auto* R = Row(Kind);
    return R ? R->Field : TOptional<EDasScalarKind>();
}
TOptional<EDasMethodSignature> DasSignatureKind(EDasTypeKind Kind)
{
    const auto* R = Row(Kind);
    return R ? R->Signature : TOptional<EDasMethodSignature>();
}
EDasTypeKind DasTypeKindOf(EDasScalarKind Kind)
{
    for (const auto& R : Rows)
        if (R.Field == Kind)
            return R.Kind;
    return EDasTypeKind::Invalid;
}
EDasTypeKind DasTypeKindOf(EDasMethodSignature Kind)
{
    if (Kind == S::Vector || Kind == S::Rotator || Kind == S::Transform)
        return EDasTypeKind::Struct;
    for (const auto& R : Rows)
        if (R.Signature == Kind)
            return R.Kind;
    return EDasTypeKind::Invalid;
}
