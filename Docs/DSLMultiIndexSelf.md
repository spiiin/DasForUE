# Multi-level self mutations

Supported authored structs and containers can be traversed through more than one
index in a ue_class method. There is no need to clone and assign the root manually:

```das
self.Groups[0].Scores[1l] += 3.0lf
self.Groups[0l].Weights["score"] = 8.0lf
self.Groups[0].Points[0l].Mode = MethodState.Idle
self.Groups[0].Points[0].Position.x += 4.0lf

push(self.Groups[0].Scores, 2.0lf)
push_clone(self.Groups[0].Points, MethodPoint())
insert(self.Groups[0].Tags, "ready")
let removed = erase(self.Groups[0].Tags, "ready")
pop(self.Groups[0].Scores)
clear(self.Groups[0].Points)

var replacement : array<double>
push(replacement, 11.0lf)
self.Groups[0].Scores := replacement
```

Groups is an array of ue_struct values containing Scores/Points/Weights/Tags.
Existing declaration rules still apply: this does not add array<array<T>>, maps
of structs or other new native container shapes.

The compiler captures each index/key and supplied value as a helper argument,
reads one owned root snapshot, reconstructs the full local lvalue, then packs and
publishes the root. Indices are evaluated once; failures do not publish the local
candidate. Earlier statements and argument side effects are not rolled back.

Array indices use ordinary daScript generic int|int64 parameters independently
at every level, rather than generating all combinations of overloads. Map keys
and assigned leaf values keep exact declared types. Readonly roots and crossed
member annotations reject mutations. Copy/clone and compound arithmetic keep
their usual local daScript rules; consuming move remains unsupported.

Operations under an index use the same traversal and commit path as assignments.
Non-indexed field operations retain their existing snapshot adapter. Typed foreign
references continue to use their existing compiler-resolved traversal.

Live iterators and native-member mutable function arguments still require local
copies. This stage changes access syntax, not declaration/default migration policy.

Fixture: ValueTarget.MultiIndexContainers and MethodGroup in ue_method_values.das.
Validation: DSLMultiIndexSelfValidation.md.
