# Actor + Component example

The current example creates an Actor Blueprint with an authored Component Blueprint
declared as a field. DSLComponentComposition.md describes the current syntax.

- dsl_actor_component_owner.das declares ComponentOwner, Score/Begins/Ticks/Ends,
  GetScore, ordinary lifecycle methods and @ue_component Behavior:ScoreComponent.
- ue_component_types.das provides the imported reference type for ScoreBehavior;
  it imports declarations rather than component execution, avoiding cyclic modules.
- dsl_component_behavior.das declares ScoreBehavior, its fields, Blueprint methods,
  class lifecycle/event methods and ue_owner(type<ComponentOwnerRef>) access.
- ue_actor_refs.das describes the owner's authored reference type.

The importer resolves the component source/type from the imported reference, creates
its Blueprint and adds an SCS node. It installs lifecycle source defaults on the
generated CDOs and refreshes native default-copy lists. Save Actor and Component
Blueprints with their map. No manual SCS node addition is needed by the example.

Each object owns its persistent VM/session. BeginPlay, real Tick delta, EndPlay,
owner multicast subscription and weak checked references use existing runtime
adapters. DestroyComponent/Actor and world/PIE cleanup revoke sessions/subscriptions.
The original actor_component_behavior.das remains a raw native-adapter example.

Category/ToolTip and component access/edit flags persist through Blueprint compilation
and save/reopen via SCS metadata/compiler replay. Property metadata uses separate
@Category/@ToolTip labels; it is not placed inside the @ue_property flag tuple.

TestActorComponent.ps1 checks Create/Reopen and optional cooked execution;
TestActorComponentPIE.ps1 uses actual editor play. Component structural migration,
native/SceneComponent attachments/defaults and component member dispatch remain
separate work. Current validation is in ComponentFieldsValidation.md; earlier
ActorComponentExampleValidation.md records the original native-component stage.

Direct component field access is supported; see [DSLComponentFieldAccess](DSLComponentFieldAccess.md).
