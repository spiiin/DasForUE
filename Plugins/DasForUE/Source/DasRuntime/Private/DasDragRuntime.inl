FWidgetDragScope& WidgetDragScope(das::Context* C){
 if(!ActiveWidgetDrag || ActiveWidgetDrag->Context!=C || ActiveContext!=C || !ActiveWidgetDrag->Widget.IsValid() || !IsInGameThread())C->throw_error("drag operation requires the active synchronous drag callback");return *ActiveWidgetDrag;
}
UDasDragDropOperation* DragOperation(const FDasObjectHandle& H,das::Context* C){
 auto& S=WidgetDragScope(C);auto* Op=Cast<UDasDragDropOperation>(ResolveObject(H,C));
 if(!Op || !Op->IsLive() || Op->GetWorld()!=S.Widget->GetWorld() || (Op!=S.Operation.Get() && Op!=S.Created.Get()))C->throw_error("drag operation is expired, foreign or outside its callback");return Op;
}
FDasObjectHandle CreateDragOperation(const FDasObjectHandle& Payload,const FDasObjectHandle& Visual,const char* Tag,int32 Pivot,const FVector2D& Offset,das::Context* C){
 auto& S=WidgetDragScope(C);if(S.Event!=TEXT("OnDragDetected") || S.Created)C->throw_error("create exactly one operation during OnDragDetected");
 const FString Label=Tag?UTF8_TO_TCHAR(Tag):TEXT("");if(Label.Len()>1024 || Pivot<0 || Pivot>9 || Offset.ContainsNaN() || FMath::Abs(Offset.X)>1 || FMath::Abs(Offset.Y)>1)C->throw_error("invalid drag tag, pivot or offset");
 UObject* Data=Payload.Token?ResolveObject(Payload,C):nullptr;UWidget* Decorator=Visual.Token?Cast<UWidget>(ResolveObject(Visual,C)):nullptr;
 if(Data && (Data->IsTemplate() || (Data->GetWorld() && Data->GetWorld()!=S.Widget->GetWorld()) || (Cast<AActor>(Data) && CastChecked<AActor>(Data)->IsActorBeingDestroyed())))C->throw_error("drag payload must be live and in the callback world");
 if(Visual.Token && (!Decorator || Decorator==S.Widget.Get() || Decorator->IsTemplate() || Decorator->GetWorld()!=S.Widget->GetWorld() || !Decorator->GetCachedWidget().IsValid()))C->throw_error("drag visual must be a separate constructed widget in the callback world");
 S.Created.Reset(NewObject<UDasDragDropOperation>(S.Widget->GetWorld()));S.Created->Initialize(S.Widget.Get(),Data,Decorator,Label,Pivot,Offset,S.UserIndex,S.PointerIndex);
 if(!ActiveContext->DragLifetime.Track(S.Created.Get()))C->throw_error("drag operation lifetime limit exceeded");
 return MakeObjectHandle(S.Created.Get(),UObject::StaticClass(),C);
}
FDasObjectHandle DragPayload(const FDasObjectHandle& H,das::Context* C){return MakeObjectHandle(DragOperation(H,C)->Payload,UObject::StaticClass(),C);}
FDasObjectHandle DragVisual(const FDasObjectHandle& H,das::Context* C){return MakeObjectHandle(DragOperation(H,C)->DefaultDragVisual,UObject::StaticClass(),C);}
char* DragTag(const FDasObjectHandle& H,das::Context* C){return CopyDSLString(DragOperation(H,C)->Tag,C);}
bool DragValid(const FDasObjectHandle& H,das::Context* C){if(!H.Token)return false;auto* Op=Cast<UDasDragDropOperation>(TryObject(H,C));return Op && Op->IsLive();}
void BindWidgetDrag(const FDasObjectHandle& H,const char* Script,const char* Event,const char* Handler,das::Context* C){
 auto* Widget=Cast<UDasScriptUserWidget>(OwnedWidget(H,C));const FString File=Script?UTF8_TO_TCHAR(Script):TEXT("");
 if(!Widget || File.IsEmpty() || File!=FPaths::GetCleanFilename(File) || FPaths::GetExtension(File)!=TEXT("das") || !Event || !Handler || FCStringAnsi::Strlen(Event)>=NAME_SIZE || FCStringAnsi::Strlen(Handler)>=NAME_SIZE || !UDasScriptUserWidget::IsSupportedDragEvent(FName(UTF8_TO_TCHAR(Event))))C->throw_error("invalid script widget drag binding");
 if(Widget->ScriptFile!=File){FModuleManager::GetModuleChecked<FDasRuntimeModule>("DasRuntime").ReleaseObject(Widget);Widget->InputHandlers.Empty();Widget->DragHandlers.Empty();Widget->ScriptFile=File;}
 Widget->BindDragHandler(FName(UTF8_TO_TCHAR(Event)),FName(UTF8_TO_TCHAR(Handler)));
}
