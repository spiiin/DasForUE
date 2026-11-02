# Typed reference property event validation

2026-11-03, UE 5.8.3, shared build agent.

- `rogue-events-v2-20261103`: `dsl-event-checks` PASS, 59 checks. Added native/authored reference-property validation, wrong source type/payload rejection, repeated add/remove, synchronous delivery from an external component and source detachment on listener Destroy. Existing queued reload and dynamic reload/release checks pass. The isolated commandlet now prepares gameplay programs and editor reload candidates explicitly after the loading optimization.
- Final per-suite results: effects 6 groups PASS (`rogue-events-pie-v3-20261103`); Overwhelm 7 groups and point impact 3 groups PASS (`rogue-events-pie-v4-20261103`). The shared port runner waits for the actual gameplay Character before accessing its input fields, allowing the transient PIE spectator. The new hit-count assertion distinguishes all three physical hits from the two damage events counted by Overwhelm before stun.

The compiler now validates reflected object-reference property sources for dynamic event operators using their declared native/authored type. Existing runtime source lookup, exact handler/delegate checks, weak session tracking and lifecycle guards remain in use; no runtime or extern changes were required.

The port uses BindEvents for permanent health/status notifications and symbolic ue_event names for native events. Health mutation remains synchronous; permanent notifications use queued delivery and automatic declarative reload replacement. Overwhelm uses synchronous, session-owned +=/-= only while running and detaches before retarget/stop. The base target invokes one virtual post-health hook; combat tests assert exactly one hit count increment per health event. Removed-action collection, regrant, lethal cleanup and timer teardown pass.
