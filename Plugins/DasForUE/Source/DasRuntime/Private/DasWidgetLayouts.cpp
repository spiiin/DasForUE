#include "DasWidgetLayouts.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Widget.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Policies/CondensedJsonPrintPolicy.h"
bool ValidateDasWidgetLayout(UWidgetTree* Tree, const FDasWidgetLayoutUse& L, FString& Error,
                             FDasResolvedWidgetLayout* Resolved)
{
    Error.Empty();
    if (!Tree || L.Fields.IsEmpty() || L.Fields.Num() > 256)
    {
        Error = TEXT("Widget layout requires a widget tree and 1..256 fields");
        return false;
    }
    TArray<FString> Names;
    L.Fields.GetKeys(Names);
    Names.Sort();
    TMap<FName, TWeakObjectPtr<UWidget>> Fields;
    for (const auto& Name : Names)
    {
        auto* Expected = FindObject<UClass>(nullptr, *L.Fields[Name]);
        auto* Actual = Tree->FindWidget(FName(*Name));
        if (!Expected || !Expected->IsChildOf(UWidget::StaticClass()))
        {
            Error = FString::Printf(TEXT("Widget layout %s field %s has unsupported widget type %s"), *L.Name, *Name,
                                         *L.Fields[Name]);
            return false;
        }
        if (!Actual || !Actual->IsA(Expected))
        {
            Error = FString::Printf(TEXT("Widget layout %s field %s: expected %s, WBP contains %s"), *L.Name, *Name,
                                         *Expected->GetPathName(),
                                         Actual ? *Actual->GetClass()->GetPathName() : TEXT("<missing>"));
            return false;
        }
        if (Resolved)
            Fields.Add(FName(*Name), Actual);
    }
    if (Resolved)
        Resolved->Fields = MoveTemp(Fields);
    return true;
}
FString DasWidgetLayoutSpec(const FDasWidgetLayoutUse& L)
{
    auto J = MakeShared<FJsonObject>();
    J->SetStringField(TEXT("name"), L.Name);
    J->SetStringField(TEXT("widget_field"), L.WidgetField);
    J->SetStringField(TEXT("class_field"), L.ClassField);
    J->SetStringField(TEXT("known_class"), L.KnownClassPath);
    auto Fields = MakeShared<FJsonObject>();
    TArray<FString> Names;
    L.Fields.GetKeys(Names);
    Names.Sort();
    for (const auto& N : Names)
        Fields->SetStringField(N, L.Fields[N]);
    J->SetObjectField(TEXT("fields"), Fields);
    FString Out;
    auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&Out);
    FJsonSerializer::Serialize(J, Writer);
    return Out;
}
bool ParseDasWidgetLayout(const FString& Spec, FDasWidgetLayoutUse& Out, FString& Error)
{
    TSharedPtr<FJsonObject> J;
    if (Spec.Len() > 65536 || !FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Spec), J) || !J)
    {
        Error = TEXT("Invalid widget layout JSON");
        return false;
    }
    for (const auto& P : J->Values)
        if (P.Key != TEXT("name") && P.Key != TEXT("widget_field") &&
                                                   P.Key != TEXT("class_field") &&
                                                                 P.Key != TEXT("known_class") &&
                                                                               P.Key != TEXT("fields"))
        {
            Error = TEXT("Unknown widget layout key: ")+ FString(*P.Key);
            return false;
        }
    FDasWidgetLayoutUse L;
    const TSharedPtr<FJsonObject>* F = nullptr;
    if (!J->TryGetStringField(
            TEXT("name"), L.Name) || L.Name.IsEmpty() ||
            !J->TryGetStringField(
                TEXT("widget_field"), L.WidgetField) ||
                !J->TryGetStringField(TEXT("class_field"), L.ClassField) ||
                                      !J->TryGetStringField(TEXT("known_class"), L.KnownClassPath) ||
                                                            !J->TryGetObjectField(TEXT("fields"), F) || !F ||
                                                                                  (*F)->Values.IsEmpty() ||
                                                                                  (*F)->Values.Num() > 256)
    {
        Error = TEXT("Widget layout requires a name and 1..256 fields");
        return false;
    }
    TSet<FName> Unique;
    for (const auto& P : (*F)->Values)
    {
        FString Path;
        if (P.Key.IsEmpty() || P.Key.Len() >= NAME_SIZE || Unique.Contains(FName(*P.Key)) ||
            !P.Value->TryGetString(Path) || !Path.StartsWith(TEXT("/")))
        {
            Error = TEXT("Invalid or duplicate widget layout field: ")+ FString(*P.Key);
            return false;
        }
        Unique.Add(FName(*P.Key));
        L.Fields.Add(FString(*P.Key), Path);
    }
    Out = MoveTemp(L);
    Error.Empty();
    return true;
}
