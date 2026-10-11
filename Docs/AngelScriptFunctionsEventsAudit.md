# Functions/events compatibility audit — 2026-10-11

Update: own helper calls, explicit BlueprintEvent dispatch and child-BP parent calls
are implemented and PIE-tested; see [method dispatch](DSLMethodDispatch.md) and
[validation](DSLMethodDispatchValidation.md). The table below records the earlier audit.

Scope: [Hazelight Functions and BlueprintEvents](https://angelscript.hazelight.se/scripting/functions-and-events/).
The direct page returned HTTP 429; its indexed text was available. This audit
covers that page, not AngelScript's full delegate/network/pure-function feature set.
No binding feature implementation was changed during this audit.

| Capability | Current DasForUE result |
|---|---|
| Plain functions | Global helpers work. A reflected method calling an unannotated class helper via self.Helper fails; plain class declarations alone do not establish usable UObject method dispatch. |
| Blueprint callable methods | Implemented and exercised in previous MethodValues/ActorComponent PIE suites. Explicit ue_function(BlueprintCallable) is required. Empty ue_function does not imply callability. |
| Parent reflected event override | Only fixed BeginPlay/Tick/EndPlay hooks exist. Arbitrary reflected parent-event override is absent. Hooks execute through native host lifecycle, with fixed Super ordering. |
| Child Blueprint overrides | Partial/untested. Generated methods are ordinary Blueprint function graphs; UE sets BlueprintEvent on these graphs. There is no separate DSL event/override policy or tested child-BP/DAS/parent-call contract. |
| Separate base logic and overridable hook | Not a supported end-to-end pattern yet: needs usable internal method calls and explicit virtual/event dispatch. ue_event subscribes to multicast delegates and is a different mechanism. |
| Reflected global functions | Not implemented. ue_function on a global function is rejected; no generated function-library host exists. |
| Script inheritance / virtual / Super | Not implemented for authored Unreal classes. Native daScript inheritance is rejected by ue_class; the host adapter only accepts direct Actor/ActorComponent parents. General daScript language inheritance is not UObject inheritance support. |
| Tooltips | Adjacent declaration comments now provide ToolTip metadata, including functions and interface stubs; explicit ToolTip overrides them. See [DSLCommentTooltips.md](DSLCommentTooltips.md). |
| Native override name aliases / IDE helpers | No general parent-event name resolver or UE override code-action/completion generation exists. Explicit binding aliases and the fixed lifecycle names are narrower facilities. |

## Evidence

- DasDSL.inl: FUEFunctionAnnotation requires a class/interface parent;
  FUEClassAnnotation initializes bCallable=false and rejects unsupported specifiers;
  its body visitor treats the self.Helper field as an unknown reflected property.
- DasAuthoring.cpp: ToActorSchema accepts only direct native Actor/ActorComponent
  hosts and refuses pure/static methods and authored parent IDs.
- DasBlueprintService.cpp: generated methods use AddFunctionGraph(..., true, nullptr).
- UE 5.8.3 BlueprintEditorUtils.h: CreateFunctionGraph marks user-created graph
  functions BlueprintCallable | BlueprintEvent | Public. Thus absence of a DSL
  BlueprintEvent flag does not prove absence of every standard BP override.
- DasProbeActor.cpp: Super::BeginPlay/Tick precede script hooks; the script EndPlay
  callback precedes Super::EndPlay. This is not explicit script Super dispatch.
- DSLMetadata accepts literal annotation values; the native CommentReader additionally supplies default ToolTip metadata from adjacent source comments.

Temporary compiler probes (files removed after use):

- logs/as-audit-globalhelper.log: success, plain global helper called from a reflected method.
- logs/as-audit-plainmember.log: rejection, Unknown reflected self property: Helper.
- logs/as-audit-default.log: rejection, empty ue_function requires BlueprintCallable.
- logs/as-audit-globalue.log: rejection, ue_function must be a class/interface member.

These probes were compile/model checks, not new runtime/child-Blueprint tests.
Existing PIE tests substantiate callable methods and lifecycle hooks only.

## Follow-up order

1. One lowering/dispatch model for own ordinary methods, reflected methods and
   lifecycle calls; script-only methods must not require Blueprint publication.
2. End-to-end child Blueprint override and parent-call tests on current graphs.
3. Explicit event/override kinds, signature matching, defaults and dispatch policy;
   distinguish multicast subscriptions from overridable reflected functions.
4. Authored class inheritance and parent-call semantics, including same-context
   method dispatch and reload compatibility.
5. Global reflected function libraries and optional callable-by-default policy.
6. Native event aliases and UE-aware editor assistance. Comment-derived tooltips are implemented.

The referenced integration also documents a limitation on C++ BlueprintNativeEvent
Super calls. Do not promise a stronger native-Super contract without dedicated
generated glue and validation.
