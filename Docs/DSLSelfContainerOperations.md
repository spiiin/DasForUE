# Self container operations

Supported array/map/set members of authored self struct properties can be
mutated with these daScript operations:

```das
insert(self.Settings.Tags, "ready")
insert(self.Settings.Weights, "score", 8.5lf)
let removed = erase(self.Settings.Tags, "ready")   // bool
let removedMap = erase(self.Settings.Weights, "score") // bool
erase(self.Settings.Samples, 0)                  // array: void
clear(self.Settings.Samples)
clear(self.Settings.Weights)
clear(self.Settings.Tags)
```

insert supports set(table<K>) and supported map(table<K;V>) members. erase
supports table/set keys (bool result) and array int indices, optionally with
an int count (void result). clear supports all existing container shapes.
Key/value argument types are exact source types. Array operation indices/counts
currently use int, not the additional daScript int64 overloads.

Each generated helper evaluates call arguments once before reading the native
member, unpacks an owned source container, invokes the standard daScript operation,
then packs/validates and commits the modified member through the existing root
snapshot path. Table erase publishes its bool result after a successful commit.
Sibling fields remain intact. There are no live iterators or mutable aliases
into UE memory and no second container mutation backend.

Readonly roots/members and incompatible keys/values reject. Existing identity,
snapshot and size-limit validation remains. A failed operation does not publish
its candidate; previous successful statements in a method are not rolled back.

Indexed assignments, push/pop, map insert_clone and typed-reference mutations
were added in DSLIndexedContainers.md. Live iterators, consuming moves, new
container shapes remain outside this adapter scope. Direct class declarations
are covered in DSLClassContainers.md.
Existing explicit registry/import rules remain.

Example: ValueTarget.MutateSelfContainers in dsl_method_value_target.das.
Validation: [DSLSelfContainerOperationsValidation.md](DSLSelfContainerOperationsValidation.md).

Update: supported indexed assignments, push/pop and typed-reference mutations are now described in [DSLIndexedContainers](DSLIndexedContainers.md). Historical scope limitations above apply to the earlier slice.
