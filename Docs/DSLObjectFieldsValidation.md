# Typed object field validation — 2026-10-07

UE 5.8.3 / pinned daScript 0.6.4, Win64 Development.

- TestDSLObjectFieldsPIE: succeeded=1, succeededWithWarnings=0, failed=0, notRun=0.
  Actor cycles, ActorComponent field, typed native getter through ProcessEvent,
  wrong-class refusal without mutation and null assignment.
- A concrete unrooted UCurveFloat survives Unreal GC solely through a Held
  UPROPERTY and is collected after clearing it. VM handles do not retain it.
- Structural post-compile fault restores remapped actor cycles and component
  identity. Stable-ID rename and retirement/restoration preserve references through
  repeated reinstance; retirement storage is consumed only after successful restore.
- Explicit Actor Destroy produces a successful null getter result and false
  validity, followed by EndPIE cleanup of all play contexts.
- TestDSLObjectFieldsAsset: eight separate processes Create/Reopen/Rename/
  ReopenRenamed/Retire/ReopenRetired/Restore/ReopenRestored PASS. Cycles, component
  identity, ordinary UCurveFloat asset and native-set read-only reference survive
  save/reopen and GC. Injected saved-scope rename rollback PASS. The Curve fixture
  is stored in its own ordinary asset package; migration scope restrictions were
  not broadened to multi-asset map packages.
- TestDSLLifecycleMigrationPIE and TestDSLStructuralRollbackAsset regressions PASS.
- TestEditor: 404 foundation checks, failures=0, PASS. New checks cover native
  type/null model roundtrip, unloaded Blueprint type refusal, unknown class,
  untyped initializer and read-only source write refusal.
- TestPackaged: native build/cook/stage/archive/launch, 404 foundation checks,
  failures=0, PASS. The new example script is included in the verified receipt.
- Disposable GUID sources cleaned up; fixture assets remain in DasFrameAssetProbe
  in the isolated smoke project. No user projects were migrated.

Fields currently have loaded native class identities and null declaring defaults.
Soft/weak/class/interface properties, script-defined property class identities,
non-null asset defaults and component construction are separate extensions.
