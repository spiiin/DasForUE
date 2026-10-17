// Curated adapter payloads are compiled with the adapter, independently of the
// optional generated Engine catalog. They retain canonical source/layout checks.
const FDasNativeGameplayStruct* DasAdapterValueStruct(UScriptStruct* Type){
 if(Type!=FDasMontageEvent::StaticStruct() && Type!=FDasMoveResult::StaticStruct())return nullptr;
 struct FRow {
  FString Signature;
  FDasNativeGameplayStruct Value;
  FRow(UScriptStruct* Type,const TCHAR* Path,const TCHAR* Source):Value{Path,Source,nullptr}{
   for(TFieldIterator<FProperty> I(Type);I;++I)AppendCallShape(*I,Signature);
   Value.Signature=*Signature;
  }
 };
 static const FRow Montage(FDasMontageEvent::StaticStruct(),TEXT("/Script/DasRuntime.DasMontageEvent"),TEXT("ue_animation::MontageEvent")); static const FRow Move(FDasMoveResult::StaticStruct(),TEXT("/Script/DasRuntime.DasMoveResult"),TEXT("ue_ai::MoveResult"));return Type==FDasMoveResult::StaticStruct()?&Move.Value:&Montage.Value;
}
