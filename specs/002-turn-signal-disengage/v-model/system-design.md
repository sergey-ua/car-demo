# System Design: Turn Signal Cruise Disengagement

**Feature Branch**: `002-turn-signal-disengage`
**Created**: 2026-09-22
**Status**: Draft
**Source**: `specs/002-turn-signal-disengage/v-model/requirements.md`
**Domain Overlay**: ISO 26262 (ASIL D — **Confirmed**, inherited from `specs/001-cruise-brake-override/` OQ-001; no independent hazard analysis performed by this document)
**Upstream Design Reference**: `specs/001-cruise-brake-override/v-model/system-design.md` (the "001 slice")

---

## Overview

This feature adds exactly **one new runtime component** to the system: a Turn
Signal Monitor that detects turn signal activation and drives disengagement of
an already-`Cruise_Active` cruise control. Per REQ-CN-001, this feature does
**not** re-design, re-implement, or duplicate any of the following components
already approved in the 001 slice's `system-design.md`:

- **SYS-001 (001 slice) — Cruise State Machine**: owns the five-state machine,
  including the `Cruise_Cancelled` target state and the reactivation guard.
- **SYS-003 (001 slice) — Longitudinal Speed Controller**: owns the exclusive
  write path to the propulsion command interface.
- **SYS-006 (001 slice) — Safety Event Logger**: owns writes to local
  persistent event storage.
- **SYS-007 (001 slice) — Notification Dispatcher**: owns delivery to the
  notification interface.
- **SYS-004 (001 slice) — Platform Input Monitor**: owns `Required_Input_Invalid`
  and all other platform input-validity handling (REQ-CN-002).

To avoid ID collision with the upstream document while keeping this feature's
design self-contained and independently traceable, this document uses a
**fresh, feature-local `SYS-NNN` numbering sequence starting at SYS-001**,
exactly as the 001 slice's own document does for its own scope. Any reference
to an upstream component is written explicitly as `001:SYS-NNN` throughout
this document (e.g., `001:SYS-001`) to make the boundary between "designed
here" and "reused unmodified" unambiguous. This mirrors the convention implied
by the requirements document's `[REUSED: REQ-NNN]` tags: reuse is declared,
not re-derived. **Numbering convention decision**: SYS numbering restarts
per feature-design-document (not globally), because each `system-design.md`
is the authoritative decomposition for its own feature's requirements.md only;
cross-feature reuse is expressed via the `001:` prefix rather than by importing
the upstream component into this feature's own ID space.

The decomposition follows the same principles as the 001 slice:
- **Single new responsibility boundary**: turn signal detection and the
  decision to request disengagement are the only new logic introduced;
  everything downstream of "request disengagement" is delegated to existing,
  already-verified upstream components.
- **No duplicate state, guard, or interface ownership**: the new component
  is a pure producer of disengagement requests into `001:SYS-001` and a pure
  consumer of the existing propulsion, notification, and event-storage paths
  via `001:SYS-003`, `001:SYS-007`, and `001:SYS-006` respectively (REQ-CN-001).
- **Platform boundary isolation preserved**: the only new platform-facing
  interface is the turn signal input itself (REQ-IF-001); all other platform
  interfaces used by this feature are the existing ones already owned by 001
  slice components.

No component introduces a capability not traceable to a requirement in this
feature's `requirements.md`. All 18 active requirements are covered by at
least one SYS component defined or referenced in this document.

---

## ID Schema

- **System Component**: `SYS-NNN` — sequential identifier local to this
  feature's design document, never renumbered.
- **Upstream (001 slice) Component Reference**: `001:SYS-NNN` — refers to a
  component defined and approved in `specs/001-cruise-brake-override/v-model/system-design.md`.
  This feature does not redefine, extend, or take ownership of any `001:SYS-NNN`
  component; it only documents how its own new component integrates with them.
- **Parent Requirements**: Comma-separated `REQ-NNN` list per component
  (many-to-many).
- Example: `SYS-001` with Parent Requirements `REQ-001, REQ-002, REQ-007, REQ-008`
  — this component satisfies all four requirements.

---

## Decomposition View (IEEE 1016 §5.1)

| SYS ID | Name | Description | Parent Requirements | Type |
|--------|------|-------------|---------------------|------|
| SYS-001 | Turn Signal Monitor | Continuously monitors the left and right turn signal input(s) provided by the existing EV prototype platform (REQ-IF-001). Derives a single boolean "turn signal active" condition, treating left, right, and simultaneous (hazard) activation identically (REQ-007). Detects activation edges within a control cycle such that a brief tap that deactivates within the same cycle is still detected (REQ-008). Reads the current cruise state from `001:SYS-001` to gate its output: emits a disengagement request to `001:SYS-001` only while cruise is `Cruise_Active` (REQ-002); while cruise is not `Cruise_Active`, produces no output and takes no action at all (REQ-005). Deactivation of the turn signal produces no request of any kind — it is not this component's role to request or influence reactivation, which remains exclusively governed by `001:SYS-001`'s reactivation guard (REQ-006). Does not perform sensor-level validity/fault checking of the turn signal input itself; that remains the responsibility of the existing `Required_Input_Invalid` mechanism in `001:SYS-004` (REQ-CN-002). Does not evaluate cruise activation preconditions; that remains the responsibility of `001:SYS-001` per REQ-018 (REQ-CN-003). | REQ-001, REQ-002, REQ-005, REQ-006, REQ-007, REQ-008, REQ-IF-001, REQ-NF-001, REQ-CN-001, REQ-CN-002, REQ-CN-003 | Module |
| 001:SYS-001 *(reused, unmodified)* | Cruise State Machine | **Not redesigned by this feature.** Accepts a new trigger input — the disengagement request from SYS-001 (this feature) — as an additional cause for the existing `Cruise_Active` → `Cruise_Cancelled` transition, using the transition mechanics, target-state resolution, and reactivation guard already specified and approved in the 001 slice. This feature adds no new state, no new transition target, and no new guard logic (REQ-003, REQ-CN-001). | REQ-003, REQ-006, REQ-CN-001, REQ-CN-003 | Module *(upstream — see note)* |
| 001:SYS-003 *(reused, unmodified)* | Longitudinal Speed Controller | **Not redesigned by this feature.** Ceases longitudinal speed control output to the propulsion command interface as part of the same transition triggered by SYS-001 (this feature)'s disengagement request, using the existing cessation mechanics already specified for other disengagement causes (REQ-004, REQ-IF-002). | REQ-004, REQ-IF-002, REQ-CN-001 | Module *(upstream — see note)* |
| 001:SYS-006 *(reused, unmodified)* | Safety Event Logger | **Not redesigned by this feature.** Persists the event record required by REQ-010 using the existing five-field record format and write-before-complete guarantee already specified in the 001 slice, with the triggering-condition field populated by SYS-001 (this feature) to record which side (left/right) caused the transition. | REQ-010, REQ-IF-004, REQ-CN-001 | Service *(upstream — see note)* |
| 001:SYS-007 *(reused, unmodified)* | Notification Dispatcher | **Not redesigned by this feature.** Delivers the notification event required by REQ-009 through the existing notification interface and exactly-once delivery contract already specified in the 001 slice. | REQ-009, REQ-IF-003, REQ-CN-001 | Service *(upstream — see note)* |
| 001:SYS-004 *(reused, unmodified)* | Platform Input Monitor | **Not redesigned by this feature.** Continues to own the `Required_Input_Invalid` mechanism; any sensor-level fault or invalidity condition on the turn signal input is handled here and only here, per REQ-CN-002. This feature's SYS-001 does not implement a parallel validity mechanism. | REQ-CN-002 | Module *(upstream — see note)* |

**Note on upstream rows**: The five `001:SYS-NNN` rows above are included in
this Decomposition View **only** to make the many-to-many REQ↔SYS mapping for
this feature's requirements.md complete and auditable (every REQ must appear
against at least one SYS row). They are not new design output of this
document — their authoritative description, dependency behavior, interfaces,
data design, FFI treatment, and complexity budget remain exactly as specified
in `specs/001-cruise-brake-override/v-model/system-design.md` and are not
restated or altered here.

---

## Dependency View (IEEE 1016 §5.2)

### Dependency Table

| Source | Target | Relationship | Failure Impact on Source |
|--------|--------|-------------|--------------------------|
| SYS-001 | 001:SYS-001 | Reads current cruise state to gate its disengagement request to `Cruise_Active` only (REQ-002, REQ-005); issues a disengagement request that `001:SYS-001` processes using its existing transition mechanics (REQ-003) | If `001:SYS-001` fails or reports the wrong state, SYS-001 may withhold a required request (missed disengagement, REQ-002 violation) or issue one when not applicable |
| SYS-001 | 001:SYS-004 | Relies on `001:SYS-004`'s existing `Required_Input_Invalid` mechanism to cover sensor-level faults on the turn signal input (REQ-CN-002); SYS-001 does not itself validate the input | If `001:SYS-004` fails to detect an invalid turn signal input, SYS-001 may act on a faulty signal — this risk is explicitly accepted and scoped out per REQ-CN-002 |
| 001:SYS-001 | 001:SYS-003 | *(unchanged from 001 slice)* State exit triggers cessation authorization | *(unchanged from 001 slice)* |
| 001:SYS-001 | 001:SYS-006 | *(new call site, existing contract)* Turn-signal-caused transition triggers a log event request using the same contract already used for Brake_Override transitions, with triggering condition = turn signal side | If `001:SYS-006` fails, the audit record is lost for this cause; the disengagement itself still proceeds (same failure-tolerance posture as the existing Brake_Override path) |
| 001:SYS-001 | 001:SYS-007 | *(new call site, existing contract)* Turn-signal-caused transition triggers a notification request using the same contract already used for other disengagement causes | If `001:SYS-007` fails, notification is not issued; the safety-relevant disengagement still proceeds |

Dependency edges among the five `001:SYS-NNN` components themselves (e.g.
`001:SYS-005` ↔ `001:SYS-001`, `001:SYS-004` ↔ `001:SYS-005`, etc.) are
unchanged from the 001 slice and are not restated here; see that document's
Dependency View for the full existing graph.

### Dependency Diagram

```
                         Existing EV Prototype Platform
                    (turn signal input — left, right)
                                   │
                                   ▼
                          ┌──────────────────┐
                          │      SYS-001      │   Turn Signal Monitor
                          │  (this feature)   │   — detects L/R/hazard,
                          │                    │     edge-detects brief taps,
                          └────────┬───────────┘     gates on cruise state
                                   │
              reads state         │  disengagement request
         ┌─────────────────────┐  │  (only while Cruise_Active)
         │                     ▼  ▼
         │            ┌──────────────────────┐
         │            │     001:SYS-001       │  Cruise State Machine
         └───────────►│  Cruise State Machine │  (existing, unmodified)
                       │  (existing, ASIL D)   │
                       └───┬────────┬──────────┘
                           │        │
             cessation cmd │        │ log + notify requests
                           ▼        ▼
                  ┌─────────────┐  ┌───────────────────────┐
                  │ 001:SYS-003 │  │ 001:SYS-006 / SYS-007 │
                  │ Longitudinal│  │ Event Logger /        │
                  │ Speed Ctrl  │  │ Notification Dispatch │
                  │ (existing)  │  │ (existing)             │
                  └──────┬──────┘  └───────────┬────────────┘
                         ▼                      ▼
              Platform: Propulsion I/F   Platform: Event Storage /
                                          Notification I/F

Sensor-fault handling for the turn signal input itself: 001:SYS-004
(Required_Input_Invalid, existing, unmodified) — out of scope for SYS-001
per REQ-CN-002; not shown as a control-flow dependency because SYS-001
performs no validity checking of its own.
```

---

## Interface View (IEEE 1016 §5.3)

### External Interfaces (Platform Boundary)

| Component | Interface Name | Protocol / Direction | Input | Output | Error Handling |
|-----------|----------------|-----------------------|-------|--------|----------------|
| SYS-001 | Turn Signal Input | Read (platform-provided) | Left/right turn signal activation status | Internal "turn signal active" condition, consumed only by SYS-001 itself | No independent validity mechanism implemented here; sensor-level invalidity is exclusively handled by the existing `001:SYS-004` `Required_Input_Invalid` mechanism (REQ-CN-002) — SYS-001 treats the input as reliable per this feature's Assumptions |
| 001:SYS-003 *(reused)* | Propulsion Command Interface | Write | Cessation command (from `001:SYS-001`, triggered indirectly by SYS-001's request) | Longitudinal speed control cessation | Unchanged from 001 slice — see that document |
| 001:SYS-007 *(reused)* | Notification Interface | Write | Notification request (from `001:SYS-001`) | Turn-signal disengagement notification event | Unchanged from 001 slice — see that document |
| 001:SYS-006 *(reused)* | Local Persistent Event Storage | Write | StateTransitionEvent (5 fields; triggering condition = turn signal side) | Persisted audit record | Unchanged write-before-complete contract from 001 slice — see that document |

This feature introduces exactly **one** new external interface (Turn Signal
Input, REQ-IF-001) and reuses three existing external interfaces unmodified
(REQ-IF-002, REQ-IF-003, REQ-IF-004).

### Internal Interfaces (Inter-Component)

| Source | Target | Interface Name | Protocol | Data Format | Error Handling |
|--------|--------|-----------------|----------|--------------|-----------------|
| SYS-001 | 001:SYS-001 | Cruise State Read | Synchronous read, per cycle | Current cruise state enum | If read fails/is stale, SYS-001 must not emit a disengagement request (fail-safe: no action rather than spurious action) |
| SYS-001 | 001:SYS-001 | Disengagement Request | Event, emitted only while `Cruise_Active` and a turn signal edge is detected | Request with triggering side (left/right/both) | Delivered once per qualifying activation edge within a control cycle (REQ-008); `001:SYS-001` applies its existing transition validation before acting |
| 001:SYS-001 | 001:SYS-006 | Log Event Request | Same contract as existing 001 slice `001:SYS-002 → 001:SYS-006` / `001:SYS-005 → 001:SYS-006` paths | StateTransitionEvent: from=`Cruise_Active`, condition=`Turn_Signal` (side), to=`Cruise_Cancelled`, speed snapshot, timestamp | `001:SYS-006` must acknowledge write before transition is finalised (unchanged contract) |
| 001:SYS-001 | 001:SYS-007 | Notification Request | Same contract as existing 001 slice paths | Event type = Turn_Signal disengagement | Exactly one request per qualifying transition (unchanged contract) |

No new internal interface is introduced between any pair of `001:SYS-NNN`
components; only SYS-001's two new edges into `001:SYS-001` are added by this
feature, and `001:SYS-001`'s existing downstream calls to `001:SYS-006` /
`001:SYS-007` are exercised with a new condition value rather than a new
contract.

---

## Data Design View (IEEE 1016 §5.4)

| Entity | Owner Component | Storage | Protection at Rest | Protection in Transit | Retention |
|--------|-----------------|---------|---------------------|------------------------|-----------|
| `TurnSignalCondition` (left/right/both, active edge) | SYS-001 | In-memory (RAM); no persistence required | Memory region accessible only to SYS-001 (spatial isolation, consistent with 001 slice's ASIL D FFI treatment) | Passed by value on the internal Disengagement Request interface to `001:SYS-001` | Discarded at next monitoring cycle; not itself persisted |
| `DisengagementRequest` (triggering side) | SYS-001 (produced) / 001:SYS-001 (consumed) | In-memory; transient event | Not persisted directly; consumed synchronously by `001:SYS-001` | Passed by value; single delivery per qualifying edge | Discarded once consumed by `001:SYS-001` |
| `StateTransitionEvent` (turn-signal cause) | 001:SYS-006 *(reused, unmodified)* | Platform local persistent event storage | Unchanged from 001 slice — storage medium integrity is a platform responsibility | Unchanged write-before-complete contract from 001 slice | Unchanged retention policy from 001 slice |
| `CruiseControlState` | 001:SYS-001 *(reused, unmodified)* | In-memory (RAM) | Unchanged from 001 slice | Read by SYS-001 by value only; no write access granted to SYS-001 | Unchanged from 001 slice |

SYS-001 introduces no new persistent data entity. It introduces one new
transient in-memory entity (`TurnSignalCondition`) and one new transient
inter-component event (`DisengagementRequest`); both are consistent in
protection posture with the equivalent transient entities already documented
in the 001 slice (e.g., `ControlCondition`).

---

## Safety-Critical Design Sections (ISO 26262 Overlay)

Per the requirements document's Assumptions, this feature inherits the
**confirmed ASIL D** classification and the ISO 26262-inspired lifecycle
obligations from the 001 slice without re-deriving them. The following
sections apply that inherited classification to the one new component
introduced here; the four reused components carry the ASIL D treatment
already documented in `specs/001-cruise-brake-override/v-model/system-design.md`
unchanged.

### Freedom from Interference (ISO 26262-6 §7.4.8)

| Component | ASIL Rating | Isolation Mechanism | Verification Method |
|-----------|-------------|----------------------|-----------------------|
| SYS-001 (Turn Signal Monitor) | ASIL D (Confirmed, inherited) | Dedicated data segment for `TurnSignalCondition`; read-only access to `001:SYS-001`'s state (no write access granted); disengagement requests sent by value, unidirectional (SYS-001 → `001:SYS-001` only) | Analysis (design review) + Inspection of interface exclusivity (SYS-001 has no write path into `001:SYS-001`'s state) |
| 001:SYS-001, 001:SYS-003, 001:SYS-006, 001:SYS-007, 001:SYS-004 *(reused)* | ASIL D (Confirmed) | Unchanged — see 001 slice's Freedom from Interference table | Unchanged — see 001 slice |

### Restricted Complexity (ISO 26262-6 §7.4.9)

| Component | Complexity Metric | Value (Target) | Threshold | Status |
|-----------|---------------------|-----------------|-----------|--------|
| SYS-001 (Turn Signal Monitor) | Cyclomatic Complexity | ≤ 12 per function (target) | ≤ 15 per function (consistent with the 001 slice's Module-tier threshold) | Within threshold (design-phase target; conformance confirmed by static analysis per the inherited REQ-CN-005-equivalent obligation) |
| SYS-001 (Turn Signal Monitor) | Nesting Depth | ≤ 3 (target) | ≤ 4 (consistent with the 001 slice's Module-tier threshold) | Within threshold |
| 001:SYS-NNN *(reused)* | — | — | — | Unchanged — see 001 slice's Restricted Complexity table |

---

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total System Components (this document) | 1 new (SYS-001) + 5 upstream references (`001:SYS-001`, `001:SYS-003`, `001:SYS-004`, `001:SYS-006`, `001:SYS-007`) |
| By Type (new only) | Module: 1 (SYS-001) |
| REQ → SYS Forward Coverage | 18/18 (100%) — every REQ-001..010, REQ-NF-001, REQ-IF-001..004, REQ-CN-001..003 appears in the Decomposition View's Parent Requirements column against SYS-001 and/or an `001:SYS-NNN` reference row |
| New External Platform Interfaces | 1 (Turn Signal Input, read) |
| Reused External Platform Interfaces | 3 (propulsion command write, notification write, event storage write — all unmodified) |
| New Internal Interfaces | 2 (SYS-001 → `001:SYS-001` state read; SYS-001 → `001:SYS-001` disengagement request) |
| Dependency Relationships (new) | 2 (SYS-001 → `001:SYS-001`; SYS-001 → `001:SYS-004`, informational/scope-boundary only) |
| Components Redesigned From Scratch | 0 (REQ-CN-001 compliance: no new state machine, no second reactivation guard, no duplicate interfaces) |
| Deprecated Components | 0 |
| Suspect Items | 0 |
| Derived Requirements Flagged | 0 |

### Requirement → Component Traceability Detail

| REQ ID | Satisfied By |
|--------|---------------|
| REQ-001 | SYS-001 |
| REQ-002 | SYS-001, 001:SYS-001 |
| REQ-003 | 001:SYS-001 |
| REQ-004 | 001:SYS-003 |
| REQ-005 | SYS-001 |
| REQ-006 | SYS-001, 001:SYS-001 |
| REQ-007 | SYS-001 |
| REQ-008 | SYS-001 |
| REQ-009 | 001:SYS-007 |
| REQ-010 | 001:SYS-006 |
| REQ-NF-001 | SYS-001 |
| REQ-IF-001 | SYS-001 |
| REQ-IF-002 | 001:SYS-003 |
| REQ-IF-003 | 001:SYS-007 |
| REQ-IF-004 | 001:SYS-006 |
| REQ-CN-001 | SYS-001, 001:SYS-001, 001:SYS-003 |
| REQ-CN-002 | SYS-001, 001:SYS-004 |
| REQ-CN-003 | SYS-001, 001:SYS-001 |

All 18 requirements map to at least one SYS component. No component has an
empty Parent Requirements field.

---

## Derived Requirements

No derived requirements were identified during this decomposition. The single
new component (SYS-001) has a well-bounded responsibility — turn signal
detection and disengagement-request emission — fully traceable to
REQ-001/002/005/006/007/008/IF-001/NF-001 and the constraint requirements
REQ-CN-001..003. All downstream behavior (state transition mechanics, output
cessation, logging, notification, reactivation guard, sensor-fault handling)
is explicitly delegated to already-approved 001 slice components per
REQ-CN-001 through REQ-CN-003, and required no new architectural invention.

---

## Glossary

| Term | Definition |
|------|-----------|
| `001:SYS-NNN` | Notation used in this document to reference a system component defined and approved in `specs/001-cruise-brake-override/v-model/system-design.md`; indicates the component is reused unmodified, not redesigned by this feature |
| Turn signal activation | A driver-initiated input, provided by the existing EV prototype platform, indicating the left or right turn signal is active; either side, or both simultaneously (hazard lights), is treated identically by SYS-001 |
| Disengagement request | The event SYS-001 emits to `001:SYS-001` when a qualifying turn signal activation edge is detected while cruise is `Cruise_Active`; carries the triggering side for downstream logging |
| Activation edge | The detected onset of a turn signal activation within a control cycle; detected even if the signal deactivates again within that same cycle (REQ-008) |
| `Cruise_Active` | The operational state, defined in `specs/001-cruise-brake-override/`, in which cruise holds longitudinal control authority and actively maintains vehicle speed |
| `Cruise_Cancelled` | A defined non-active state, defined in `specs/001-cruise-brake-override/`, indicating cruise has been explicitly cancelled; the target state for a turn-signal-triggered transition (REQ-003) |
| `Required_Input_Invalid` | The existing platform-level input-validity condition, owned by `001:SYS-004`, that governs sensor-level fault handling for the turn signal input; not re-implemented by this feature (REQ-CN-002) |
| ASIL | Automotive Safety Integrity Level — a risk classification defined by ISO 26262, ranging from A (lowest) to D (highest); this feature inherits ASIL D (Confirmed) from the 001 slice |
