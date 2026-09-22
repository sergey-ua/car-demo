# V-Model Requirements Specification: Turn Signal Cruise Disengagement

**Feature Branch**: `002-turn-signal-disengage`
**Created**: 2026-09-22
**Status**: Draft
**Source**: `specs/002-turn-signal-disengage/spec.md`

---

## Overview

This feature layers on top of the existing Vehicle Cruise Control Brake Override
Safety Slice (`specs/001-cruise-brake-override/`). While cruise control is in
`Cruise_Active`, activation of either turn signal (left or right) indicates an
imminent driver-initiated maneuver (lane change or turn). Cruise control continuing
to hold a set speed during that maneuver is inconsistent with driver intent and
undesirable, mirroring in spirit the existing `Brake_Override` disengagement logic
(REQ-001/REQ-002 in `specs/001-cruise-brake-override/v-model/requirements.md`).

This requirements specification formalises the traceable requirements for turn
signal disengagement. Requirements are extracted from `spec.md` under strict
translation rules — no requirement is invented that is not present in the source.
Where this feature reuses interfaces, state values, or timing semantics already
defined and approved in the Brake Override Safety Slice, that reuse is called out
explicitly rather than re-derived.

**Domain overlay**: ISO 26262 (road vehicles — functional safety), consistent with
the Brake Override Safety Slice this feature depends on.
**ASIL target**: Inherited from `specs/001-cruise-brake-override/` — ASIL D
**Confirmed** (see that slice's `v-model-config.yml` / OQ-001). This feature adds a
new trigger condition into an already-classified safety-relevant state machine and
carries the same `[ASIL D — Confirmed]` tag; no independent hazard analysis is
performed by this document.

**ISO 26262 tagging convention used throughout** (identical to
`specs/001-cruise-brake-override/v-model/requirements.md`):

| Tag | Meaning |
|-----|---------|
| `[ASIL D — Confirmed]` | **Confirmed** ASIL D — inherited classification from the Brake Override Safety Slice hazard analysis |
| `[MECHANISM: Detection]` | Safety mechanism that detects a fault or safety-relevant event |
| `[MECHANISM: Prevention]` | Safety mechanism that prevents an unsafe state from being entered |
| `[MECHANISM: Mitigation]` | Safety mechanism that limits the effect of a fault or unsafe event |
| `[DERIVED]` | Requirement not directly traceable to an explicit source statement |
| `[REUSED]` | Requirement restates, without altering, a mechanism already specified and approved in `specs/001-cruise-brake-override/` |

No `[DERIVED]` requirements are present in this version — all requirements trace
directly to `spec.md`. Requirements tagged `[REUSED]` trace to the cited upstream
`REQ-*` in the Brake Override Safety Slice and do not redefine that mechanism.

---

## Requirements

### Functional Requirements

| ID | Description | Priority | Rationale | Verification |
|----|-------------|----------|-----------|--------------|
| REQ-001 | The feature SHALL monitor the left and right turn signal input(s) provided by the existing EV prototype platform. `[ASIL D — Confirmed]` `[MECHANISM: Detection]` | P1 | Establishes the input monitoring on which all other requirements in this feature depend. Source: FR-001 | Inspection |
| REQ-002 | While the feature (via the existing cruise control state machine defined in `specs/001-cruise-brake-override/`) is in `Cruise_Active`, when either the left or the right turn signal is activated, the feature SHALL transition cruise control out of `Cruise_Active` within one control cycle. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | P1 | Core behavior of this feature — a turn signal indicates an imminent maneuver during which cruise must not continue holding a set speed. Satisfies SC-001. Source: FR-002 / US-1 | Test |
| REQ-003 | Following a transition out of `Cruise_Active` caused by turn signal activation under REQ-002, the feature SHALL transition cruise control to `Cruise_Cancelled`. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | P1 | Per the Assumptions in spec.md, the target post-disengagement state for a turn-signal-triggered transition is the same non-active state (`Cruise_Cancelled`) used for other non-fault disengagement causes in the existing state machine (REQ-004 in the Brake Override Safety Slice), consistent with how `Brake_Override` is handled. Without this requirement, the target state of REQ-002's transition would be undefined and untestable. Source: Assumptions / FR-002 | Test |
| REQ-004 | When cruise control transitions out of `Cruise_Active` due to a turn signal activation (REQ-002), the feature SHALL cease all longitudinal speed control output to the propulsion command interface as part of that same transition. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | P1 | Cessation of speed control output must not lag the state transition; the driver must regain full manual control at the moment of disengagement. Satisfies SC-001. Source: FR-003 / US-1 | Test |
| REQ-005 | While cruise control is not in `Cruise_Active`, turn signal activation SHALL NOT cause this feature to take any action on the cruise control state. `[ASIL D — Confirmed]` `[MECHANISM: Prevention]` | P1 | Confirms the feature introduces no unwanted side effect when cruise is not actively controlling speed; a boundary condition of the primary behavior. Satisfies SC-002. Source: FR-004 / US-2 | Test |
| REQ-006 | Deactivation of the turn signal SHALL NOT, by itself, cause cruise control to transition to `Cruise_Active`; any resumption of `Cruise_Active` SHALL follow the existing cruise activation request and precondition process defined by REQ-005 and REQ-018 in `specs/001-cruise-brake-override/v-model/requirements.md`. `[ASIL D — Confirmed]` `[MECHANISM: Prevention]` `[REUSED: REQ-005, REQ-018]` | P1 | Prevents an unrequested resumption of speed control from surprising the driver; reactivation remains governed by the existing reactivation guard rather than being redefined here. Source: FR-005 / US-3 | Test |
| REQ-007 | Both the left and right turn signal being active simultaneously (e.g., hazard lights) SHALL be treated by the feature identically to a single turn signal activation for the purposes of REQ-002 through REQ-004; no distinct hazard-specific handling is implemented by this feature. `[ASIL D — Confirmed]` | P2 | Removes ambiguity for a named edge case in spec.md; without this requirement, behavior under simultaneous activation would be undefined. Source: Edge Cases (hazard lights) | Test |
| REQ-008 | If a turn signal activation is detected during a control cycle in which cruise control is `Cruise_Active`, the transition required by REQ-002 SHALL still occur even if the turn signal is deactivated again within that same control cycle. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | P2 | Removes ambiguity for a named edge case in spec.md (a very brief tap of the signal); prevents a race condition where a fast deactivation could suppress disengagement. Source: Edge Cases (brief tap) | Test |
| REQ-009 | When a turn-signal-triggered disengagement occurs under REQ-002, the feature SHALL issue a notification event to the notification interface provided by the existing EV prototype platform, consistent with the notification behavior already established for other disengagement causes (REQ-012 in `specs/001-cruise-brake-override/v-model/requirements.md`). `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` `[REUSED: REQ-012]` | P1 | Confirms to the driver and vehicle systems that cruise has been disengaged due to a turn signal; content and format are owned by the notification interface provider and are not redefined here. Source: FR-006 / US-1 | Test |
| REQ-010 | When a turn-signal-triggered disengagement occurs under REQ-002, the feature SHALL persist an event record to the local persistent event storage provided by the existing EV prototype platform before the transition is considered complete, containing the same five fields required by REQ-013 and REQ-014 in `specs/001-cruise-brake-override/v-model/requirements.md`, with the triggering condition field recording the turn signal side (left or right) that caused the transition. `[ASIL D — Confirmed]` `[MECHANISM: Detection]` `[REUSED: REQ-013, REQ-014]` | P1 | Makes the transition auditable — the source spec.md requires the transition be reconstructable from the event log alone, matching the existing evidentiary requirement for other disengagement causes. Satisfies SC-003. Source: FR-006 / US-1 | Test |

---

### Non-Functional Requirements

| ID | Description | Priority | Rationale | Verification |
|----|-------------|----------|-----------|--------------|
| REQ-NF-001 | The elapsed time from detection of a turn signal activation while `Cruise_Active` to both (a) transition out of `Cruise_Active` (REQ-002/REQ-003) and (b) cessation of longitudinal speed control output (REQ-004) SHALL NOT exceed the same maximum latency bound already defined for `Brake_Override` handling, REQ-NF-001 in `specs/001-cruise-brake-override/v-model/requirements.md` (100 milliseconds). `[ASIL D — Confirmed]` `[REUSED: REQ-NF-001]` | P1 | Per the Assumptions in spec.md, "one control cycle" has the same meaning and duration as used in the Brake Override Safety Slice's timing requirements; no new timing bound is introduced. Satisfies SC-001 and formalizes the qualitative claim in SC-004 ("no perceptible delay") as a measurable bound. Source: Assumptions / SC-001 / SC-004 | Test (100 consecutive runs under worst-case platform load; all latencies ≤ 100 ms) |

---

### Interface Requirements

| ID | Description | Priority | Rationale | Verification |
|----|-------------|----------|-----------|--------------|
| REQ-IF-001 | The feature SHALL receive left and right turn signal activation status from a turn signal input provided by the existing EV prototype platform. `[ASIL D — Confirmed]` | P1 | The turn signal input is the sole authorised source for the trigger condition in REQ-002; this feature introduces no new hardware interface. Source: FR-001 / C-001 (inherited constraint) | Inspection, Test |
| REQ-IF-002 | The feature SHALL cease longitudinal speed control output (REQ-004) exclusively through the propulsion command interface provided by the existing EV prototype platform, reusing the interface already specified as REQ-IF-001 in `specs/001-cruise-brake-override/v-model/requirements.md`. `[ASIL D — Confirmed]` `[REUSED: REQ-IF-001]` | P1 | This feature does not introduce a second output channel for longitudinal control; it reuses the existing designated channel. Source: Assumptions | Inspection |
| REQ-IF-003 | The feature SHALL deliver the notification required by REQ-009 through the notification interface provided by the existing EV prototype platform, reusing the interface already specified as REQ-IF-003 in `specs/001-cruise-brake-override/v-model/requirements.md`. `[ASIL D — Confirmed]` `[REUSED: REQ-IF-003]` | P1 | Notification delivery is not redefined by this feature; it uses the same designated channel as other disengagement causes. Source: FR-006 / Assumptions | Test |
| REQ-IF-004 | The feature SHALL write the event record required by REQ-010 to the local persistent event storage provided by the existing EV prototype platform, reusing the interface already specified as REQ-IF-004 in `specs/001-cruise-brake-override/v-model/requirements.md`. `[ASIL D — Confirmed]` `[REUSED: REQ-IF-004]` | P1 | Event persistence is not redefined by this feature; it uses the same designated store as other disengagement causes, preserving a single reconstructable audit trail. Source: FR-006 / Assumptions | Test |

---

### Constraint Requirements

| ID | Description | Priority | Rationale | Verification |
|----|-------------|----------|-----------|--------------|
| REQ-CN-001 | The feature SHALL reuse, without modification, the five-state cruise control state machine, the reactivation guard, the propulsion command interface, the notification interface, and the local persistent event storage already defined in `specs/001-cruise-brake-override/`. The feature SHALL NOT introduce a new cruise control state, a second state machine, or a separate reactivation guard. | P1 | This feature is explicitly layered on top of the Brake Override Safety Slice per the Assumptions in spec.md; introducing parallel or divergent state/guard logic would create two sources of truth for cruise control state and undermine the existing safety case. Source: Assumptions | Inspection |
| REQ-CN-002 | Sensor-level fault or invalidity handling for the turn signal input itself is out of scope for this feature; such handling SHALL continue to be governed exclusively by the existing `Required_Input_Invalid` mechanism defined in `specs/001-cruise-brake-override/`. This feature SHALL NOT implement a separate validity-checking mechanism for the turn signal input. | P2 | Named explicitly as out of scope in the Edge Cases section of spec.md; avoids duplicate or conflicting input-validity logic. Source: Edge Cases (turn signal input invalid/unavailable) | Inspection |
| REQ-CN-003 | Activation preconditions for entering `Cruise_Active` (including the case where a turn signal is already active at the moment the driver requests cruise activation) are out of scope for this feature and SHALL continue to be governed exclusively by the existing activation precondition process (REQ-018 in `specs/001-cruise-brake-override/v-model/requirements.md`). This feature governs only disengagement of an already-engaged cruise control. | P2 | Named explicitly as out of scope in the Edge Cases section of spec.md; scopes this feature to disengagement only and prevents overlap with the existing activation guard. Source: Edge Cases (turn signal active at engagement) | Inspection |

---

## Assumptions

- This feature depends on, and does not redefine, the cruise control state machine,
  propulsion command interface, notification interface, event storage, and
  reactivation guard already established in `specs/001-cruise-brake-override/`.
- "One control cycle" carries the same meaning and platform-defined duration used in
  the Brake Override Safety Slice's timing requirements (REQ-NF-001, 100 ms); no new
  timing bound is introduced by this feature.
- The target post-disengagement state for a turn-signal-triggered transition is
  `Cruise_Cancelled`, matching the resolved target state (OQ-003) used for other
  non-fault disengagement causes in the existing state machine.
- The turn signal input is a reliable platform input; sensor-level fault handling for
  the input itself is covered by the platform's existing input-validity mechanisms
  (`Required_Input_Invalid`) and is not re-specified here (REQ-CN-002).
- Hazard lights (both signals simultaneously) are treated as an ordinary turn signal
  activation; no distinct behavior beyond REQ-007 is defined.
- The ASIL D classification and the overall ISO 26262-inspired lifecycle obligations
  (REQ-NF-002 / REQ-CN-002 through REQ-CN-007 in `specs/001-cruise-brake-override/v-model/requirements.md`)
  are inherited by this feature because it modifies behavior of the same safety-relevant
  state machine; this document does not re-derive or restate those lifecycle
  constraints, but the inherited engineering rigor is assumed to apply.

---

## Dependencies

| Dependency | Type | Notes |
|------------|------|-------|
| EV prototype platform — turn signal input (left/right) | Runtime input | Required by REQ-001 and REQ-IF-001 |
| `specs/001-cruise-brake-override/` — cruise control state machine | Upstream feature | Required by REQ-002, REQ-003, REQ-005, REQ-006, REQ-CN-001 |
| `specs/001-cruise-brake-override/` — reactivation guard (REQ-005, REQ-018) | Upstream feature | Required by REQ-006, REQ-CN-003 |
| EV prototype platform — propulsion command interface | Runtime output | Required by REQ-004 and REQ-IF-002 |
| EV prototype platform — notification interface | Runtime output | Required by REQ-009 and REQ-IF-003 |
| EV prototype platform — local persistent event storage | Runtime output | Required by REQ-010 and REQ-IF-004 |
| `specs/001-cruise-brake-override/` — `Required_Input_Invalid` mechanism | Upstream feature | Required by REQ-CN-002 |

---

## Glossary

| Term | Definition |
|------|-----------|
| `Cruise_Active` | The operational state, defined in `specs/001-cruise-brake-override/`, in which the feature holds longitudinal control authority and actively maintains vehicle speed toward a cruise target |
| `Cruise_Cancelled` | A defined non-active state, defined in `specs/001-cruise-brake-override/`, indicating that cruise has been explicitly cancelled |
| Turn signal activation | A driver-initiated input, provided by the existing EV prototype platform, indicating the left or right turn signal is active; either side is treated identically by this feature |
| Control cycle | The platform-defined periodic execution interval of the cruise control feature; "one control cycle" carries the same duration as defined for the Brake Override Safety Slice |
| ASIL | Automotive Safety Integrity Level — a risk classification defined by ISO 26262, ranging from A (lowest) to D (highest) |

---

**Total Requirements**: 18 (18 active, 0 deprecated)

**By category**: Functional: 10 | Non-Functional: 1 | Interface: 4 | Constraint: 3

**By priority**: P1: 13 | P2: 4 | P3: 0

**By verification method**: Test: 13 | Inspection: 8
*(Several requirements have multiple verification methods; counts reflect all methods assigned)*

**Open flags**:
- `[NEEDS CLARIFICATION]`: 0
- `[CONFLICT]`: 0
- `[FEASIBILITY CONCERN]`: 0
- `[ADDED BY GUARD 1 — Constraint Absorption]`: 0 (REQ-CN-002 and REQ-CN-003 describe scope exclusions that are each paired with an explicit existing upstream mechanism, not an unbuilt capability; no missing functional requirement identified)
- `[ADDED BY GUARD 2 — SC Coverage]`: 1 — SC-004 ("no perceptible delay") had no directly corresponding requirement in a first draft; REQ-NF-001 was added, translating the qualitative claim into the existing 100 ms measurable bound inherited from the Brake Override Safety Slice
- `[ADDED BY GUARD 3 — Untestable Universal]`: 0 — no requirement in this document uses an unbounded universal quantifier ("never", "always", "at all times") without an accompanying finite, testable bound
