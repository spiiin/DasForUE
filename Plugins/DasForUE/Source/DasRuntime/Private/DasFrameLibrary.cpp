#include "DasFrameLibrary.h"
#include "DasContainerTypes.h"
#include "UObject/Class.h"
FDasScalarValue UDasFrameLibrary::DefaultValue(int32 Kind,const FString& ClassPath,int32 Direction){FDasScalarValue V;V.Kind=Kind;V.Direction=Direction;V.ObjectClass=ClassPath;if(Kind==12){auto* S=FindObject<UScriptStruct>(nullptr,*ClassPath);if(S && DasContainerProperty(ClassPath))V.StructValue.InitializeAs(S);}return V;}
FDasScalarValue UDasFrameLibrary::OutputAt(const TArray<FDasScalarValue>& Values,int32 Index){if(Values.IsValidIndex(Index))return Values[Index];FDasScalarValue Invalid;Invalid.Kind=-1;return Invalid;}
