# Managed asynchronous asset loading

Added 2026-10-10. **Not built or tested**. Streamable Manager/UHT/source overloads,
callback delivery, asset loading/failure, cancellation/GC, PIE and packaging are pending.

```das
ue_asset_load_async("image",self.AssetPath,"OnLoaded")

[ue_function(BlueprintCallable)]
def OnLoaded(value:ObjectRef):void {
    self.Loaded = value
    self.Succeeded = ue_object_valid(value.handle)
}
```

Unreal Streamable Manager performs async package loading. There is no synchronous
wait and no DAS execution on a worker thread. Completion is routed through the
existing bounded game-thread proxy queue and consumed before calling user code.
This is an explicit callback API, not await/coroutine syntax.

API:
- ue_asset_load_async(name, object_path, callback): start/replace a request.
- ue_asset_is_pending(name): includes completion queued but not yet delivered.
- ue_asset_cancel(name): cancel/remove the current session's request by name.
- ue_asset_cancel_all(): count of this session's request records removed.

Class callbacks require an annotated instance method(value:typed native object):void.
The source callback type selects the expected native class in the load contract.
ObjectRef accepts any UObject; more-specific native wrappers select their own class.
Interface/authored class views and extra args/out/ref are not added here. Plain
exported callbacks use the low-level UObjectHandle generic contract; transparent
standalone source wrapper lowering remains separate.

Input must be a valid Unreal object path (at most 1024 characters), not a filesystem
or network URI. Names use existing 1..128-character session-local operation names;
requests are separate from timers/latent/input namespaces, capped at 64/session.
Owner's live game world and persistent session are required. Start is refused during
bind_events/EndPlay. The adapter does not create assets or alter project files.

The request initially starts stalled: its proxy/managed record is installed before
StartStalledHandle, preventing already-loaded completion from racing registration.
Handler/path/type/count and handle creation checks precede replacing an old request.
Private weak completion targets carry no DAS stack/context address. The streamable
handle retains loading state/assets until result delivery or cancellation.

Failure or wrong loaded class delivers a nullable result; wrong class logs a warning.
Store a successful result in an existing reflected UObject property if it should
stay referenced. Queued object pins keep it alive through callback delivery; DAS
wrappers themselves retain checked weak semantics. The proxy is pinned through its
native completion call even if the record is removed and the callback triggers GC.

Cancel suppresses this request's callback and releases/cancels its handle. It does
not promise to abort a package load shared with other engine requests. Revoked IDs
are ignored if a completion was already queued. EndPlay/session release/world cleanup/
shutdown/reinstancing/accepted reload cancel requests; rejected reload keeps old work.
No automatic request restoration or BeginPlay replay. Required handler contracts
remain until session release; callback errors use existing event error reporting.

Cook boundary: a string path alone does not ensure an asset is included in packaged
content. Configure existing soft/primary asset/cook rules or references through the
project's normal asset pipeline. This adapter has not been checked in a cooked build.

Example dsl_async_asset.das exposes an editable AssetPath and nullable Loaded result.
No concrete asset path/configuration or running result was created for this stage.

Still separate: structured error/result DSL, multiple assets/progress, cancellation
tokens/futures, soft-reference syntax, background task execution and await/coroutines.

Pending checks: build and RequestAsyncLoad template/API; already-loaded and cold/missing
assets; typed mismatch/null result; stalled registration ordering; replacement and
rearm; queued cancellation; exact handle release and shared-load behavior; result GC;
reentry/reload/owner/world teardown; actor/component examples, real PIE and packaged cook.
