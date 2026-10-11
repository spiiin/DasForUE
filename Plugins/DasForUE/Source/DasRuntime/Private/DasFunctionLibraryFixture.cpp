#include "DasFunctionLibraryFixture.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
int32 UDasFunctionLibraryFixture::Add(int32 A,int32 B){return A+B;}
bool UDasFunctionLibraryFixture::IsContextActor(const UObject* Context){return Context && Context->IsA<AActor>() && Context->GetWorld() && Context->GetWorld()->IsGameWorld();}

int32 UDasFunctionLibraryFixture::ContextWorldId(const UObject* Context){return Context && Context->GetWorld()?int32(Context->GetWorld()->GetUniqueID()):-1;}
