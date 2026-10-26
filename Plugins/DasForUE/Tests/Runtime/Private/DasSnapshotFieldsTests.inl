// Exercise prepared metadata against different storage, bounds and schemas.
{
    DasSnapshotFields::FPathCache Paths;
    auto* Type = Frozen.GetScriptStruct();
    DasSnapshotFields::TField<const uint8> Field{};
    FString Why;
    bool PathChecksOK = true;
    for (int32 I = 0; I < 20; ++I)
        PathChecksOK &= Paths.Find(Type, Frozen.GetMemory(), TEXT("Rows.0.Priority"), Field, Why) &&
                                   Field.Memory == &Probe.Rows[0].Priority;
    Expect(PathChecksOK && Paths.BuildCount() == 1 && Paths.LookupCount() == 2,
           TEXT("snapshot_path_reuses_parse_and_field_resolution"), Why);
    FInstancedStruct Copy = Frozen;
    DasSnapshotFields::TField<uint8> Mutable{};
    PathChecksOK = Paths.Find(Type, Copy.GetMutableMemory(), TEXT("Rows.0.Priority"), Mutable, Why);
    if (PathChecksOK)
        *Mutable.Memory = 37;
    Expect(PathChecksOK && Copy.Get<FDasSnapshotReadProbe>().Rows[0].Priority == 37 && Probe.Rows[0].Priority == 11 &&
               Paths.LookupCount() == 2,
           TEXT("snapshot_path_reuses_metadata_without_caching_storage"), Why);
    Copy.GetMutable<FDasSnapshotReadProbe>().Rows.Reset();
    const auto Previous = Mutable;
    PathChecksOK = !Paths.Find(Type, Copy.GetMutableMemory(), TEXT("Rows.0.Priority"), Mutable, Why);
    Expect(PathChecksOK && Why == TEXT("struct array index out of range") && Mutable.Memory == Previous.Memory &&
                                       Mutable.Property == Previous.Property && Paths.LookupCount() == 2,
                                       TEXT("snapshot_path_checks_current_bounds_and_preserves_failed_output"), Why);
    const TPair<const TCHAR*, const TCHAR*> BadPaths[] = {
        {TEXT(""), TEXT("empty struct field path")},
         {TEXT("Missing"), TEXT("struct field does not exist")},
          {TEXT("Rows.01.Priority"), TEXT("invalid struct array index")},
           {TEXT("Rows.-1.Priority"), TEXT("invalid struct array index")},
            {TEXT("Rows.10000.Priority"), TEXT("invalid struct array index")},
             {TEXT("Rows.0.Priority.Child"), TEXT("path crosses non-struct field")},
              {TEXT("Rows.1.Missing"), TEXT("struct array index out of range")},
               {TEXT("Nested.Positions.0.Missing"), TEXT("struct field does not exist")}};
    PathChecksOK = true;
    FString PathFailures;
    for (const auto& Bad : BadPaths)
        for (int32 I = 0; I < 2; ++I)
        {
            const bool Rejected = !Paths.Find(Type, Frozen.GetMemory(), Bad.Key, Field, Why);
            if (!Rejected || Why != Bad.Value)
            {
                PathChecksOK = false;
                PathFailures += FString::Printf(TEXT("[%s: expected %s; got %s] "), Bad.Key, Bad.Value, *Why);
            }
        }
    Expect(PathChecksOK, TEXT("snapshot_path_preserves_errors_on_cold_and_cached_access"), PathFailures);
    DasSnapshotFields::TField<const uint8> Terminal{};
    Expect(Paths.Find(Type, Frozen.GetMemory(), TEXT("Rows.0"), Terminal, Why) &&
                      Terminal.Memory == reinterpret_cast<const uint8*>(&Probe.Rows[0]) &&
                      CastField<FStructProperty>(Terminal.Property),
                      TEXT("snapshot_path_supports_terminal_array_element"), Why);
    for (int32 I = 0; I < DasSnapshotFields::FPathCache::MaxPaths + 5; ++I)
    {
        Paths.Find(Type, Frozen.GetMemory(), FString::Printf(TEXT("Missing%d"), I), Field, Why);
    }
    PathChecksOK = Paths.Num() <= DasSnapshotFields::FPathCache::MaxPaths;
    const int32 BeforeLong = Paths.Num();
    const FString LongPath = FString::ChrN(1025, TEXT('x'));
    for (int32 I = 0; I < 2; ++I)
        Paths.Find(Type, Frozen.GetMemory(), LongPath, Field, Why);
    Expect(PathChecksOK && Paths.Num() == BeforeLong,
           TEXT("snapshot_path_cache_is_bounded_and_long_paths_are_not_retained"));
}
#if WITH_EDITORONLY_DATA
{
    // Same UScriptStruct survives, but its FProperties are destroyed. This is
    // the engine's serial invalidation path used during structure recompiles.
    TStrongObjectPtr<UScriptStruct> Type(NewObject<UScriptStruct>());
    auto* Old = new FIntProperty(Type.Get(), TEXT("Value"));
    Type->AddCppProperty(Old);
    Type->Bind();
    Type->StaticLink(true);
    DasSnapshotFields::FPathCache Paths;
    FString Why;
    DasSnapshotFields::TField<uint8> Field{};
    bool PathChecksOK = false;
    {
        FStructOnScope Storage(Type.Get());
        PathChecksOK = Paths.Find(Type.Get(), Storage.GetStructMemory(), TEXT("Value"), Field, Why) &&
                                  CastField<FIntProperty>(Field.Property);
    }
    Type->DestroyChildPropertiesAndResetPropertyLinks();
    auto* Replacement = new FDoubleProperty(Type.Get(), TEXT("Value"));
    Type->AddCppProperty(Replacement);
    Type->Bind();
    Type->StaticLink(true);
    {
        FStructOnScope Storage(Type.Get());
        PathChecksOK &= Paths.Find(Type.Get(), Storage.GetStructMemory(), TEXT("Value"), Field, Why) &&
                                   Field.Property == Replacement && Paths.LookupCount() == 2;
    }
    Type->DestroyChildPropertiesAndResetPropertyLinks();
    Type->StaticLink(true);
    {
        FStructOnScope Storage(Type.Get());
        PathChecksOK &= !Paths.Find(Type.Get(), Storage.GetStructMemory(), TEXT("Value"), Field, Why) &&
                                    Why == TEXT("struct field does not exist");
    }
    Expect(PathChecksOK, TEXT("snapshot_path_invalidates_destroyed_and_retyped_fields"), Why);
}
#endif
