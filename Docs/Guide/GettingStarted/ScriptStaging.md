# Script staging

Normal packages include only plugin modules listed in
`Plugins/DasForUE/Content/Das/RuntimeScripts.txt`, and `.das` files in the bundled
`daslib` standard library. Unlisted fixtures, generated `dsl_registered_*` scripts,
the build-time AOT emitter, JSON catalogs, Markdown and backup files are excluded.
The manifest includes transitive imports. Some compatibility binding declarations
(for example `ue_native_hash_probe`) are still required by the checked-in generated
native module; these are type views, not executable test scenarios. Removing these
declarations requires regenerating the shared native surface separately.

Project-owned `Content/Das/Scripts/**/*.das` are staged. Keep project tests outside
that gameplay directory. Project JSON or other data is not implicitly staged:
list exact project-relative paths in `Config/DasProjectData.txt`.

To ship a plugin example or extra module, list exact paths relative to the plugin's
`Content/Das` in your project's `Config/DasPluginScripts.txt`, one per line. Include
its imports and data dependencies too. Comments start with `#`. Missing paths,
wildcards, absolute paths and escaping paths fail the build. Existing projects that
relied on implicit plugin examples must opt in or copy their gameplay into the project.

Only test projects should create `Config/DasIncludeTestFixtures.txt` containing
exactly `include-plugin-test-fixtures`. This opts into all plugin Scripts `.das`
and `.json` fixtures, including dynamically created fixtures needed by saved test
maps. DasForUESmoke explicitly opts in for both Development and Shipping tests.
This marker must not be copied to a release project. It does not enable compiler,
editor or security features. Editor source lookup is unchanged.

New runtime modules must be added to RuntimeScripts.txt. Queue the `script-staging`
suite through the build agent: it compiles and executes the actual C# selector,
checks transitive local imports and tests exclusion, explicit additions, invalid
paths, missing files and test opt-in. A normal release and a smoke package intentionally
have different script inventories. Changes to the directory inventory may require
rebuilding UBT's target makefile; the packaged test runner already invalidates it.

After creating an optional manifest or changing test opt-in for an already built target, regenerate its UBT makefile (clean the target's Intermediate build cache) before packaging. Edits to existing manifests are tracked as ExternalDependencies. The packaged smoke runner already clears its target makefile.
