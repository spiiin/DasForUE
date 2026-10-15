#include "DasFunctionLibraryFixture.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
int32 UDasFunctionLibraryFixture::Add(int32 A,int32 B){return A+B;}
bool UDasFunctionLibraryFixture::IsContextActor(const UObject* Context){return Context && Context->IsA<AActor>() && Context->GetWorld() && Context->GetWorld()->IsGameWorld();}

int32 UDasFunctionLibraryFixture::ContextWorldId(const UObject* Context){return Context && Context->GetWorld()?int32(Context->GetWorld()->GetUniqueID()):-1;}

TSubclassOf<AActor> UDasFunctionLibraryFixture::EchoActorClass(TSubclassOf<AActor> Value){return Value;}
UClass* UDasFunctionLibraryFixture::EchoAnyClass(UClass* Value){return Value;}
void UDasFunctionLibraryFixture::ClassOutputs(TSubclassOf<AActor> Value,TSubclassOf<AActor>& Output){Output=Value;}
TArray<TSubclassOf<AActor>> UDasFunctionLibraryFixture::EchoClassArray(const TArray<TSubclassOf<AActor>>& Values){return Values;}
int64 UDasFunctionLibraryFixture::UnsupportedInt64(int64 Value){return Value;}
