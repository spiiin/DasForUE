# DasForUE source distribution

Win64 source integration for Unreal Engine 5.8.3. This is a source bundle with
prebuilt daScript static libraries, not a precompiled Marketplace plugin.
Keep `Plugins/DasForUE` and `ThirdParty/daScript` at the same root: the module rules
resolve their dependencies relative to that root. Visual Studio C++ tools and a
matching Unreal Engine installation are required.

`Examples/CleanGameplay` is a standalone project without test opt-ins. See its
README for preparation and use. The archive deliberately excludes plugin tests,
fixtures, diagnostic scripts, generated build products and logs. Generated C++
bindings under Source are production sources and are included.

`distribution-manifest.json` records every delivered file and its SHA-256 hash.
Third-party license texts are included alongside the corresponding dependencies.
