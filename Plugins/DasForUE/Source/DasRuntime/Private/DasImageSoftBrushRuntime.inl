// The normal generated frame retains signature/argument/construction checks.
// Only the two exact soft brush functions replace ProcessEvent with managed streaming.
bool DispatchImageSoftBrush(FMarshalledCall& Call, das::Context* C)
{
    auto* Image = Cast<UImage>(Call.Target.Get());
    if (!Image)
        return false;
    auto* Function = Call.Function.Get();
    if (!DasImageSoftBrushFunctionAllowed(Function))
    {
        if (Function &&
            (Function->GetName().StartsWith(TEXT("SetBrush")) || Function->GetFName() == TEXT("GetDynamicMaterial")))
            CancelDasImageSoftBrush(Image);
        return false;
    }
    auto* World = GameplayWorld(C);
    const bool Texture = Function->GetFName() == TEXT("SetBrushFromSoftTexture");
    auto* P = FindFProperty<FSoftObjectProperty>(Function, Texture ? TEXT("SoftTexture"): TEXT("SoftMaterial"));
    const auto Path =
        P->GetPropertyValue(P->ContainerPtrToValuePtr<void>(Call.Frame->GetStructMemory())).ToSoftObjectPath();
    bool Match = false;
    if (Texture)
    {
        auto* B = FindFProperty<FBoolProperty>(Function, TEXT("bMatchSize"));
        Match = B->GetPropertyValue_InContainer(Call.Frame->GetStructMemory());
    }
    FString Error;
    if (!ActiveContext->ImageBrushes.Begin(Image, World, Path, Texture, Match, Error))
        C->throw_error(TCHAR_TO_UTF8(*Error));
    return true;
}
char* ImageSoftBrushStatus(const FDasObjectHandle& H, das::Context* C)
{
    RequireObjectContext(C);
    auto* Image = Cast<UImage>(ResolveObject(H, C));
    if (!Image)
        C->throw_error("expected Image");
    auto* P = ActiveContext->ImageBrushes.Requests.Find(Image);
    const char* State = "none";
    if (P)
    {
        switch ((*P)->State)
        {
        case EDasImageSoftBrushState::Loading:
            State = "loading";
            break;
        case EDasImageSoftBrushState::Ready:
            State = "ready";
            break;
        case EDasImageSoftBrushState::Failed:
            State = "failed";
            break;
        case EDasImageSoftBrushState::Cancelled:
            State = "cancelled";
            break;
        }
    }
    return CopyDSLString(UTF8_TO_TCHAR(State), C);
}
char* ImageSoftBrushError(const FDasObjectHandle& H, das::Context* C)
{
    RequireObjectContext(C);
    auto* Image = Cast<UImage>(ResolveObject(H, C));
    if (!Image)
        C->throw_error("expected Image");
    auto* P = ActiveContext->ImageBrushes.Requests.Find(Image);
    return CopyDSLString(P ? (*P)->Error : TEXT(""), C);
}
