# Whole self container member replacement

Container members inside an authored struct property can be replaced in a
ue_class method using daScript's clone operator:

```das
self.Settings.Items := items             // array<MethodPoint>
self.Settings.Weights := weights         // table<string; double>
self.Settings.Tags := tags               // table<string> set
```

The RHS keeps its natural source type and remains independent. Empty values
clear the member. Other fields of the root struct are preserved. Supported
container shapes are the existing snapshot shapes; this does not add new map
value types or direct arrays of enums/objects/math. Direct ue_class containers
are covered in DSLClassContainers.md.

The common typed leaf helper checks element/key/value types before packing
through existing standalone container carriers. The existing nested snapshot
replacement adapter now accepts a matching carrier's Values property for a
container member. It checks exact carrier and reflected property identity,
copies the owned native value, validates the whole root snapshot and commits.
There is no alternate container backend or live container view into UE memory.

Existing aggregate limits (4096 elements and 16 MiB of tracked native container
storage per snapshot) remain. An invalid candidate fails before the root is
published. Readonly roots reject clone assignment. Ordinary = is not used for
noncopyable daScript containers. This does not roll back earlier successful
statements in a method.

Self insert/erase/clear are described in
[DSLSelfContainerOperations.md](DSLSelfContainerOperations.md). Indexed
assignments still require a local copy and whole member/property replacement. Foreign-object nested writes likewise
remain separate. Existing registry/folder and import rules are unchanged.

Example: ValueTarget.ReplaceSelfContainers in dsl_method_value_target.das.
Validation: [DSLSelfContainerMembersValidation.md](DSLSelfContainerMembersValidation.md).

Update: supported indexed assignments, push/pop and typed-reference mutations are now described in [DSLIndexedContainers](DSLIndexedContainers.md). Historical scope limitations above apply to the earlier slice.
