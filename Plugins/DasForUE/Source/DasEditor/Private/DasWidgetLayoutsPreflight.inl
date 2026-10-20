// Read-only preflight. It runs before the target Blueprint is created or modified.
static bool PreflightDasWidgetLayouts(const FDasDeclaredClass& C,UBlueprint* Existing,FString& Error){
 for(const auto& L:C.WidgetLayouts){TArray<UClass*> Candidates;
  auto AddPath=[&](const FString& Path){if(!Path.IsEmpty()){auto* K=LoadObject<UClass>(nullptr,*Path,nullptr,LOAD_NoWarn);if(K)Candidates.AddUnique(K);}};
  AddPath(L.KnownClassPath);
  if(!L.ClassField.IsEmpty()){
   if(Existing && Existing->GeneratedClass){auto* P=FindFProperty<FClassProperty>(Existing->GeneratedClass,*L.ClassField);if(P)if(auto* K=Cast<UClass>(P->GetObjectPropertyValue_InContainer(Existing->GeneratedClass->GetDefaultObject())))Candidates.AddUnique(K);}
   for(const auto& P:C.Properties)if(P.Name==L.ClassField && P.Default.Kind==TEXT("class"))AddPath(P.Default.String);
   for(const auto& P:C.PropertyOverrides)if(P.Name==L.ClassField && P.Default.Kind==TEXT("class"))AddPath(P.Default.String);
  }
  for(auto* Class:Candidates){auto* WidgetClass=Cast<UWidgetBlueprintGeneratedClass>(Class);if(!WidgetClass)continue;
   if(auto* Blueprint=Cast<UBlueprint>(Class->ClassGeneratedBy);Blueprint && Blueprint->Status!=BS_UpToDate){Error=TEXT("Compile widget Blueprint before Apply: ")+Blueprint->GetPathName();return false;}
   auto* Owning=WidgetClass->FindWidgetTreeOwningClass();if(!ValidateDasWidgetLayout(Owning?Owning->GetWidgetTreeArchetype():WidgetClass->GetWidgetTreeArchetype(),L,Error)){Error=TEXT("Widget layout Apply preflight for ")+Class->GetPathName()+TEXT(": ")+Error;return false;}
  }
 }return true;
}
