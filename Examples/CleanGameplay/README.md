# Clean gameplay example

An ordinary project with no plugin test opt-in. Gameplay lives in
`Content/Das/Scripts/collectible.das`: async mesh loading, component attachment,
rotation, a multicast event, score and a respawn timer. Press Space to collect.
C++ sets up the camera/light and forwards the input to the reflected DAS method.

Build CleanGameplayEditor, then run the editor with `-PrepareCleanGameplay` once
to generate the Blueprint and Arena map using the production authoring command.
For the first preparation, pass `/Engine/Maps/Entry` before `-PrepareCleanGameplay`
because the example map does not exist yet. For example:

```text
UnrealEditor.exe "CleanGameplay.uproject" /Engine/Maps/Entry -PrepareCleanGameplay
```

`Config/DefaultGame.ini` explicitly cooks the generated Blueprint folder and mesh
assets, since the native bootstrap loads its class by a string path.
Open the project and press Play, or launch a packaged build. Automated validation
uses `-CleanGameplayVerify`: collect twice and check the events, mesh and timers.
The verification mode belongs to this example, not to the plugin runtime.

`Scripts/TestCleanDistribution.ps1` in the development repository creates the
source archive and tests an extracted copy through the shared build agent.
