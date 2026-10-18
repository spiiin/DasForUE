// Curated world/UI adapters share existing checked object handles and owner contexts.
UWorld* GameplayWorld(das::Context* C){
 RequireObjectContext(C);
 if(ConstructingObject || ActiveContext->bCollectingSubscriptions || ActiveContext->bEnding)C->throw_error("gameplay service is forbidden in construction/bind_events/EndPlay");
 auto* Object=ResolveWorldContextObject(C);auto* World=Object?Object->GetWorld():nullptr;
 if(!World || !World->IsGameWorld() || World->bIsTearingDown)C->throw_error("gameplay service requires a live game world");return World;
}
FDasTraceHit TraceWorld(const FVector& Start,const FVector& End,double Radius,int32 Channel,const das::TArray<FDasObjectHandle>& Ignored,bool Complex,bool IgnoreSelf,das::Context* C){
 auto* World=GameplayWorld(C);
 if(Start.ContainsNaN() || End.ContainsNaN() || !FMath::IsFinite((End-Start).SizeSquared()) || !FMath::IsFinite(Radius) || Radius<0 || Radius>1000000 || Channel<0 || Channel>=int32(ECC_OverlapAll_Deprecated) || Ignored.size>4096)C->throw_error("invalid trace vectors/radius/channel/ignore count");
 FCollisionQueryParams Params(SCENE_QUERY_STAT(DasTrace),Complex);Params.bReturnFaceIndex=true;Params.bReturnPhysicalMaterial=true;
 auto* Handles=reinterpret_cast<const FDasObjectHandle*>(Ignored.data);
 for(uint32 I=0;I<Ignored.size;++I){if(!Handles[I].Token)continue;auto* A=Cast<AActor>(ResolveObject(Handles[I],C));if(!A || A->GetWorld()!=World)C->throw_error("trace ignores require live same-world Actors");Params.AddIgnoredActor(A);}
 if(IgnoreSelf){auto* O=ResolveWorldContextObject(C);if(auto* A=Cast<AActor>(O))Params.AddIgnoredActor(A);else if(auto* Component=Cast<UActorComponent>(O))Params.AddIgnoredActor(Component->GetOwner());}
 FHitResult Hit;
 if(Radius==0)World->LineTraceSingleByChannel(Hit,Start,End,ECollisionChannel(Channel),Params);
 else World->SweepSingleByChannel(Hit,Start,End,FQuat::Identity,ECollisionChannel(Channel),FCollisionShape::MakeSphere(float(Radius)),Params);
 FDasTraceHit R{};R.Scope=ActiveContext->TraceScope;R.BlockingHit=Hit.bBlockingHit;R.StartPenetrating=Hit.bStartPenetrating;R.Distance=Hit.Distance;R.Time=Hit.Time;R.Location=Hit.Location;R.ImpactPoint=Hit.ImpactPoint;R.Normal=Hit.Normal;R.ImpactNormal=Hit.ImpactNormal;R.FaceIndex=Hit.FaceIndex;
 R.TraceStart=Hit.TraceStart;R.TraceEnd=Hit.TraceEnd;R.PenetrationDepth=Hit.PenetrationDepth;R.Item=Hit.Item;R.MyItem=Hit.MyItem;R.ElementIndex=Hit.ElementIndex;R.BoneName=Hit.BoneName;R.MyBoneName=Hit.MyBoneName;
 if(auto* Material=Hit.PhysMaterial.Get())R.PhysMaterial=MakeObjectHandle(Material,Material->GetClass(),C);
 if(auto* Owner=Hit.PhysicsObjectOwner.Get())R.PhysicsObjectOwner=MakeObjectHandle(Owner,UObject::StaticClass(),C);
 if(auto* A=Hit.HitObjectHandle.GetCachedActor())R.Actor=MakeObjectHandle(A,AActor::StaticClass(),C);
 if(auto* P=Hit.GetComponent())R.Component=MakeObjectHandle(P,UPrimitiveComponent::StaticClass(),C);
 return R;
}
FDasObjectHandle TraceActor(const FDasTraceHit& H,das::Context* C){RequireObjectContext(C);if(H.Scope!=ActiveContext->TraceScope)C->throw_error("trace result belongs to another context or is uninitialized");return TryObject(H.Actor,C)?H.Actor:FDasObjectHandle{};}
FDasObjectHandle TraceComponent(const FDasTraceHit& H,das::Context* C){RequireObjectContext(C);if(H.Scope!=ActiveContext->TraceScope)C->throw_error("trace result belongs to another context or is uninitialized");auto* P=Cast<UActorComponent>(TryObject(H.Component,C));return P && IsValid(P->GetOwner()) && !P->GetOwner()->IsActorBeingDestroyed()?H.Component:FDasObjectHandle{};}
void RequireCollisionScope(uint64 Scope,das::Context* C){RequireObjectContext(C);if(Scope!=ActiveContext->TraceScope)C->throw_error("collision result belongs to another context or is uninitialized");}
FDasObjectHandle TraceMaterial(const FDasTraceHit& H,das::Context* C){RequireCollisionScope(H.Scope,C);return TryObject(H.PhysMaterial,C)?H.PhysMaterial:FDasObjectHandle{};}
FDasObjectHandle TracePhysicsOwner(const FDasTraceHit& H,das::Context* C){RequireCollisionScope(H.Scope,C);return TryObject(H.PhysicsObjectOwner,C)?H.PhysicsObjectOwner:FDasObjectHandle{};}
const char* TraceBone(const FDasTraceHit& H,bool Mine,das::Context* C,das::LineInfoArg* At){RequireCollisionScope(H.Scope,C);const FTCHARToUTF8 Name(*(Mine?H.MyBoneName:H.BoneName).ToString());return C->allocateString(Name.Get(),Name.Length(),At);}
void OverlapWorld(const FVector& Position,double Radius,int32 Channel,const das::TArray<FDasObjectHandle>& Ignored,bool IgnoreSelf,das::TArray<FDasOverlapResult>& Output,das::Context* C,das::LineInfoArg* At){
 auto* World=GameplayWorld(C);
 if(Position.ContainsNaN() || !FMath::IsFinite(Radius) || Radius<=0 || Radius>1000000 || Channel<0 || Channel>=int32(ECC_OverlapAll_Deprecated) || Ignored.size>4096)C->throw_error("invalid overlap position/radius/channel/ignore count");
 FCollisionQueryParams Params(SCENE_QUERY_STAT(DasOverlap));auto* Handles=reinterpret_cast<const FDasObjectHandle*>(Ignored.data);
 for(uint32 I=0;I<Ignored.size;++I){if(!Handles[I].Token)continue;auto* A=Cast<AActor>(ResolveObject(Handles[I],C));if(!A || A->GetWorld()!=World)C->throw_error("overlap ignores require live same-world Actors");Params.AddIgnoredActor(A);}
 if(IgnoreSelf){auto* O=ResolveWorldContextObject(C);if(auto* A=Cast<AActor>(O))Params.AddIgnoredActor(A);else if(auto* Comp=Cast<UActorComponent>(O))Params.AddIgnoredActor(Comp->GetOwner());}
 TArray<FOverlapResult> Hits;World->OverlapMultiByChannel(Hits,Position,FQuat::Identity,ECollisionChannel(Channel),FCollisionShape::MakeSphere(float(Radius)),Params);
 if(Hits.Num()>4096)C->throw_error("overlap result limit exceeded");
 das::builtin_array_resize(Output,uint32(Hits.Num()),sizeof(FDasOverlapResult),C,At);
 auto* Values=reinterpret_cast<FDasOverlapResult*>(Output.data);
 for(int32 I=0;I<Hits.Num();++I){const auto& H=Hits[I];FDasOverlapResult R{};R.Scope=ActiveContext->TraceScope;R.BlockingHit=H.bBlockingHit;R.ItemIndex=H.GetItemIndex();R.MyItemIndex=H.MyItemIndex;
  // GetCachedActor avoids materializing dehydrated actor instances as a side effect of inspection.
  if(auto* A=H.OverlapObjectHandle.GetCachedActor())R.Actor=MakeObjectHandle(A,AActor::StaticClass(),C);
  if(auto* Component=H.GetComponent())R.Component=MakeObjectHandle(Component,UPrimitiveComponent::StaticClass(),C);
  if(auto* Owner=H.PhysicsObjectOwner.Get())R.PhysicsObjectOwner=MakeObjectHandle(Owner,UObject::StaticClass(),C);
  Values[I]=R;
 }
}
FDasObjectHandle OverlapReference(const FDasOverlapResult& H,int32 Kind,das::Context* C){RequireCollisionScope(H.Scope,C);if(Kind<0 || Kind>2)C->throw_error("invalid overlap reference kind");const auto V=Kind==0?H.Actor:Kind==1?H.Component:H.PhysicsObjectOwner;auto* Object=TryObject(V,C);if(auto* Component=Cast<UActorComponent>(Object);Component && (!IsValid(Component->GetOwner()) || Component->GetOwner()->IsActorBeingDestroyed()))return {};return Object?V:FDasObjectHandle{};}
float ApplyGameplayDamage(const FDasObjectHandle& Target,float Amount,const FDasObjectHandle& Source,const FDasObjectHandle& Instigator,das::Context* C){
 auto* World=GameplayWorld(C);auto* A=Cast<AActor>(ResolveObject(Target,C));auto* Cause=Source.Token?Cast<AActor>(ResolveObject(Source,C)):nullptr;auto* Controller=Instigator.Token?Cast<AController>(ResolveObject(Instigator,C)):nullptr;
 if(!A || A->GetWorld()!=World || (Source.Token && (!Cause || Cause->GetWorld()!=World)) || (Instigator.Token && (!Controller || Controller->GetWorld()!=World)) || !FMath::IsFinite(Amount) || Amount<0)C->throw_error("damage requires finite nonnegative amount and same-world Actors/controller");
 if(!A->HasAuthority())C->throw_error("damage requires authority");
 return UGameplayStatics::ApplyDamage(A,Amount,Controller,Cause,UDamageType::StaticClass());
}
UUserWidget* OwnedWidget(const FDasObjectHandle& H,das::Context* C){auto* World=GameplayWorld(C);auto* W=Cast<UUserWidget>(ResolveObject(H,C));if(!W || W->GetWorld()!=World || !ActiveContext->WidgetLifetime.Owns(W))C->throw_error("widget is not owned by this script context");return W;}
FDasObjectHandle CreateGameplayWidget(const FDasClassHandle& Class,const FDasObjectHandle& Player,das::Context* C){
 auto* World=GameplayWorld(C);auto* PC=Cast<APlayerController>(ResolveObject(Player,C));auto* Kind=Class.Token?Cast<UClass>(ResolveObject({Class.Token},C)):nullptr;
 if(!PC || PC->GetWorld()!=World || !PC->IsLocalController() || !PC->GetLocalPlayer() || !Kind || !Kind->IsChildOf(UUserWidget::StaticClass()) || Kind->HasAnyClassFlags(CLASS_Abstract|CLASS_Deprecated|CLASS_NewerVersionExists))C->throw_error("widget requires a local owning player and concrete UserWidget class");
 if(ActiveContext->WidgetLifetime.Widgets.Num()>=32)C->throw_error("owned widget limit exceeded");
 auto* W=CreateWidget<UUserWidget>(PC,Kind);if(!W)C->throw_error("CreateWidget failed");ActiveContext->WidgetLifetime.Widgets.Emplace(W);return MakeObjectHandle(W,UUserWidget::StaticClass(),C);
}
bool GameplayWidgetVisible(const FDasObjectHandle& H,das::Context* C){RequireObjectContext(C);auto* W=Cast<UUserWidget>(TryObject(H,C));return W && ActiveContext->WidgetLifetime.Owns(W) && W->IsInViewport();}
bool ShowGameplayWidget(const FDasObjectHandle& H,int32 Z,das::Context* C){auto* W=OwnedWidget(H,C);return W->IsInViewport() || W->AddToPlayerScreen(Z);}
void RemoveGameplayWidget(const FDasObjectHandle& H,das::Context* C){auto* W=OwnedWidget(H,C);ActiveContext->WidgetLifetime.Remove(W);}
void GameplayWidgetText(const FDasObjectHandle& H,const char* Name,FDasTextValue Text,das::Context* C){auto* W=OwnedWidget(H,C);if(!Name || !*Name || FCStringAnsi::Strlen(Name)>=NAME_SIZE)C->throw_error("invalid widget name");auto* T=Cast<UTextBlock>(W->GetWidgetFromName(FName(UTF8_TO_TCHAR(Name))));if(!T)C->throw_error("named widget is not a TextBlock");T->SetText(ResolveDSLText(Text,C));}
void GameplayWidgetClick(const FDasObjectHandle& H,const char* Name,const char* Handler,das::Context* C){
 auto* W=OwnedWidget(H,C);auto* Owner=ResolveSelf(C);
 if(!Name || !*Name || !Handler || !*Handler || FCStringAnsi::Strlen(Name)>=NAME_SIZE || FCStringAnsi::Strlen(Handler)>=NAME_SIZE)C->throw_error("invalid widget/handler name");
 auto* Button=Cast<UButton>(W->GetWidgetFromName(FName(UTF8_TO_TCHAR(Name))));auto* Function=Owner->FindFunction(FName(UTF8_TO_TCHAR(Handler)));
 if(!Button || !Function || !Function->HasAnyFunctionFlags(FUNC_BlueprintCallable) || Function->NumParms!=0 || Function->GetReturnProperty())C->throw_error("button handler must be a BlueprintCallable void function without arguments");
 auto& Life=ActiveContext->WidgetLifetime;
 for(auto& P:Life.Buttons)if(P->Button.Get()==Button)P->Deactivate();Life.Buttons.RemoveAll([&](const auto& P){return P->Button.Get()==Button;});
 if(Life.Buttons.Num()>=64)C->throw_error("widget callback limit exceeded");
 auto* Proxy=NewObject<UDasWidgetCallback>();Proxy->Owner=Owner;Proxy->Widget=W;Proxy->Button=Button;Proxy->Handler=Function->GetFName();Life.Buttons.Emplace(Proxy);Button->OnClicked.AddDynamic(Proxy,&UDasWidgetCallback::Receive);
}
void GameplayWidgetInput(const FDasObjectHandle& H,bool Enabled,das::Context* C){auto* W=OwnedWidget(H,C);auto* PC=W->GetOwningPlayer();if(!IsValid(PC) || !PC->IsLocalController())C->throw_error("widget has no local owning player");auto& Life=ActiveContext->WidgetLifetime;
 if(!Enabled){Life.ResetInput();return;}
 if(!Life.InputPlayer.IsValid()){Life.PreviousCursor=PC->bShowMouseCursor;Life.InputPlayer=PC;}
 FInputModeGameAndUI Mode;Mode.SetWidgetToFocus(W->TakeWidget());Mode.SetHideCursorDuringCapture(false);PC->SetInputMode(Mode);PC->bShowMouseCursor=true;
}
