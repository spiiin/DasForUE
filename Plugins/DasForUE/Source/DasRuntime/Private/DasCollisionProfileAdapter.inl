// Only this known CustomThunk is adapted. Never call its C++ check(0) stub.
bool IsCollisionProfilePropertyAdapter(const UFunction* Function)
{
    return Function && Function->GetOuterUClass() == UKismetSystemLibrary::StaticClass() &&
           Function->GetFName() == TEXT("SetCollisionProfileNameProperty");
}
void ValidateCollisionProfileAdapterSignature(UFunction* Function, das::Context* C)
{
    if (!Function->HasAllFunctionFlags(FUNC_Static | FUNC_Native | FUNC_BlueprintCallable) ||
        Function->HasAnyFunctionFlags(FUNC_Net | FUNC_Delegate | FUNC_MulticastDelegate))
        C->throw_error("collision profile adapter function contract changed");
    auto* Object = FindFProperty<FObjectProperty>(Function, TEXT("Object"));
    auto* Name = FindFProperty<FNameProperty>(Function, TEXT("PropertyName"));
    auto* Value = FindFProperty<FStructProperty>(Function, TEXT("Value"));
    int32 Count = 0;
    for (TFieldIterator<FProperty> It(Function); It; ++It)
        if (It->HasAnyPropertyFlags(CPF_Parm))
        {
            ++Count;
            if (It->ArrayDim != 1 || It->HasAnyPropertyFlags(CPF_ReturnParm) ||
                (It->HasAnyPropertyFlags(CPF_OutParm) && !It->HasAnyPropertyFlags(CPF_ConstParm)))
                C->throw_error("collision profile adapter parameter contract changed");
        }
    if (Count != 3 || !Object || CastField<FClassProperty>(Object) || Object->PropertyClass != UObject::StaticClass() ||
        !Name || !Value || Value->Struct != FCollisionProfileName::StaticStruct())
        C->throw_error("collision profile adapter parameter types changed");
}
bool DispatchCollisionProfileProperty(FMarshalledCall& Call, das::Context* C)
{
    if (!IsCollisionProfilePropertyAdapter(Call.Function.Get()))
        return false;
    ValidateCollisionProfileAdapterSignature(Call.Function.Get(), C);
    RequireObjectContext(C);
    if (ConstructingObject || ActiveContext->bCollectingSubscriptions || ActiveContext->bEnding)
        C->throw_error(
            "collision profile property write requires gameplay outside ConstructionScript/bind_events/EndPlay");
    auto* Self = ResolveWorldContextObject(C);
    auto* World = Self->GetWorld();
    auto* SourceObject = FindFProperty<FObjectProperty>(Call.Function.Get(), TEXT("Object"));
    auto* Target = SourceObject->GetObjectPropertyValue_InContainer(Call.Frame->GetStructMemory());
    auto* Actor = Cast<AActor>(Target);
    auto* Component = Cast<UActorComponent>(Target);
    if (!IsValid(Target) || Target->IsTemplate() || (Actor && Actor->IsActorBeingDestroyed()) ||
        (Component && (Component->IsBeingDestroyed() || !IsValid(Component->GetOwner()) ||
                       Component->GetOwner()->IsActorBeingDestroyed())))
        C->throw_error("collision profile property requires a live non-template target");
    if (!IsValid(Self) || Self->IsTemplate() ||
        (Cast<UActorComponent>(Self) && CastChecked<UActorComponent>(Self)->IsBeingDestroyed()) || !World ||
        !World->IsGameWorld() || World->bIsTearingDown || Target->GetWorld() != World)
        C->throw_error("collision profile property target requires script owner's live game world");
    const auto Name = FindFProperty<FNameProperty>(
        Call.Function.Get(), TEXT("PropertyName"))->GetPropertyValue_InContainer(Call.Frame->GetStructMemory());
    if (Name.IsNone())
        C->throw_error("collision profile property name is empty");
    auto* Property = FindFProperty<FStructProperty>(Target->GetClass(), Name);
    if (!Property || Property->ArrayDim != 1 || Property->Struct != FCollisionProfileName::StaticStruct())
        C->throw_error("collision profile property must be a scalar FCollisionProfileName");
    if (!Property->HasAnyPropertyFlags(CPF_BlueprintVisible) ||
        Property->HasAnyPropertyFlags(CPF_BlueprintReadOnly | CPF_EditConst))
        C->throw_error("collision profile property is hidden or read-only");
    auto* Source = FindFProperty<FStructProperty>(Call.Function.Get(), TEXT("Value"));
    const auto Value =
        ReadNativePropertyValue(Source, Source->ContainerPtrToValuePtr<void>(Call.Frame->GetStructMemory()), C);
    FDasPropertyAccess Access;
    FString Error;
    if (!DescribeDasPropertyAccess(Property, Access, Error) ||
        !WriteDasObjectProperty(Property, Target, Access, false, Value, Error))
        C->throw_error(TCHAR_TO_UTF8(*Error));
    return true;
}
