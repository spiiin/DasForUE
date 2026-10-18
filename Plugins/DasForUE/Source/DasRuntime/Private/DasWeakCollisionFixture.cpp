#include "DasWeakCollisionFixture.h"
#include "UObject/GarbageCollection.h"
FDasWeakCollisionSnapshot UDasWeakCollisionFixture::MakeWeakSnapshot(){FDasWeakCollisionSnapshot V;V.Target=NewObject<UDasWeakCollisionFixture>();return V;}
bool UDasWeakCollisionFixture::CollectWeakSnapshot(const FDasWeakCollisionSnapshot& Value){CollectGarbage(RF_NoFlags);return Value.Marker==17 && !Value.Target.IsValid();}
