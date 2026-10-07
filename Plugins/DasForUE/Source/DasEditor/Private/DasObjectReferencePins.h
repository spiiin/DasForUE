#pragma once
#include "DasCallFrame.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UObjectGlobals.h"
// Plain native snapshots are not GC-traced. Pin referenced objects for the scope
// and rewrite all snapshot references through forward/rollback replacements.
struct FDasObjectReferencePins {
    TArray<TStrongObjectPtr<UObject>> Pins;
    void Keep(const FDasScalarValue& V){if(V.Kind==8 && IsValid(V.ObjectValue.Get()) && !Pins.ContainsByPredicate([&](const auto& P){return P.Get()==V.ObjectValue.Get();}))Pins.Emplace(V.ObjectValue.Get());}
    void Keep(const TMap<FString,FDasScalarValue>& Values){for(const auto& P:Values)Keep(P.Value);}
    static void Remap(FDasScalarValue& V,const FCoreUObjectDelegates::FReplacementObjectMap& Map){if(V.Kind==8)if(auto* N=Map.Find(V.ObjectValue.Get()))V.ObjectValue=*N;}
    static void Remap(TMap<FString,FDasScalarValue>& Values,const FCoreUObjectDelegates::FReplacementObjectMap& Map){for(auto& P:Values)Remap(P.Value,Map);}
    void RemapPins(const FCoreUObjectDelegates::FReplacementObjectMap& Map){for(auto& P:Pins)if(auto* N=Map.Find(P.Get()))P.Reset(*N);}
};
