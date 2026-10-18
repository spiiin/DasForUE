// A query consumes only the current native input. Owned snapshots alone grant no authority.
bool DispatchInputNavigationCall(FMarshalledCall& Call,das::Context* C){
 if(!DasInputNavigationFunctionAllowed(Call.Function.Get()))return false;
 auto& S=WidgetReplyScope(C);
 if(S.NavigationRevoked || !S.Widget.IsValid() || !S.Widget->GetCachedWidget().IsValid() || !S.Widget->GetWorld() || S.Widget->GetWorld()->bIsTearingDown || Call.WidgetReplyEpoch!=S.Epoch)C->throw_error("navigation callback is expired");
 if(!FSlateApplication::IsInitialized() || !S.NavigationUser.IsValid() || FSlateApplication::Get().GetUser(int32(S.NavigationUserIndex))!=S.NavigationUser.Pin())C->throw_error("navigation requires an initialized Slate user");
 const FName Name=Call.Function->GetFName();const bool Analog=Name==TEXT("Key_GetNavigationDirectionFromAnalog");
 const FString Path=Analog?TEXT("inanalogevent"):TEXT("inkeyevent");const auto* Token=Call.InputEventTokens.Find(Path);
 auto* P=FindFProperty<FStructProperty>(Call.Function.Get(),Analog?TEXT("InAnalogEvent"):TEXT("InKeyEvent"));
 if(!P || P->Struct!=S.NavigationInputType || !Token || !S.NavigationInputToken || *Token!=S.NavigationInputToken)C->throw_error("navigation requires the current callback event");
 auto* Output=CastField<FEnumProperty>(Call.Function->GetReturnProperty());
 if(!Output || Output->GetEnum()->GetPathName()!=(Name==TEXT("Key_GetNavigationActionFromKey")?TEXT("/Script/SlateCore.EUINavigationAction"):TEXT("/Script/SlateCore.EUINavigation")))C->throw_error("navigation result enum identity mismatch");
 int64 Value=0;
 if(const auto* Cached=S.NavigationResults.Find(Name))Value=*Cached;
 else{
  // Analog updates native repeat state. Always consume this input, including error fallback.
  if(Analog)S.NavigationConsumedAnalog=true;
  Call.Target->ProcessEvent(Call.Function.Get(),Call.Frame->GetStructMemory());
  Value=Output->GetUnderlyingProperty()->GetSignedIntPropertyValue(Output->ContainerPtrToValuePtr<void>(Call.Frame->GetStructMemory()));
  S.NavigationResults.Add(Name,Value);
 }
 Output->GetUnderlyingProperty()->SetIntPropertyValue(Output->ContainerPtrToValuePtr<void>(Call.Frame->GetStructMemory()),uint64(Value));return true;
}
