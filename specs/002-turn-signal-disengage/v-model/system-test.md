# System Test Plan: Turn Signal Cruise Disengagement

**Feature Branch**: `002-turn-signal-disengage`
**Created**: 2026-09-22
**Status**: Draft
**Source**: `specs/002-turn-signal-disengage/v-model/system-design.md`
**Domain Overlay**: ISO 26262 (ASIL D — Confirmed, inherited from `specs/001-cruise-brake-override/` OQ-001)
**Upstream System Test Reference**: `specs/001-cruise-brake-override/v-model/system-test.md` (the "001 slice")

---

## Overview

This plan verifies that the one new system component defined in this feature's
`system-design.md` — **SYS-001, Turn Signal Monitor** — behaves as designed,
and that its two new integration edges into the existing, unmodified 001-slice
architecture (`001:SYS-001` Cruise State Machine, `001:SYS-003` Longitudinal
Speed Controller, `001:SYS-006` Safety Event Logger, `001:SYS-007` Notification
Dispatcher, `001:SYS-004` Platform Input Monitor) are exercised correctly.

Per `system-design.md`'s "Note on upstream rows", the five `001:SYS-NNN`
components are **reused unmodified** — they were already system-tested in the
001 slice (`specs/001-cruise-brake-override/v-model/system-test.md`, STP-001-A
through STP-007-B). This plan does **not** re-derive test cases for their
unchanged internal behaviour. Instead, for each `001:SYS-NNN` reference row,
this plan either (a) writes a new test case scoped strictly to the **new
integration point** (the new call site or new condition value flowing through
an existing, unchanged contract), or (b) cross-references the exact upstream
STP ID(s) that already cover the unchanged internal behaviour.

**Test strategy by design view:**
- **Interface Contract Testing** → Interface View (the one new external
  interface, and the two new internal interfaces SYS-001 → `001:SYS-001`)
- **Boundary Value Analysis** → Data Design View (`TurnSignalCondition` /
  `DisengagementRequest` value boundaries; edge-detection timing boundary)
- **Fault Injection** → Dependency View (failure propagation across the two
  new dependency edges: SYS-001 → `001:SYS-001`, SYS-001 → `001:SYS-004`)

**ISO 26262 ASIL D additions** (overlay active, inherited):
- MC/DC structural coverage target: 100% for the one new runtime component (SYS-001)
- Resource usage verification: WCET and stack depth for SYS-001 (new call chain
  into `001:SYS-001` only; the four downstream reused components' resource
  budgets are unchanged — see 001 slice's Resource Usage table)
- No new back-to-back testing candidate is designated: SYS-001's decision logic
  (edge detection + single state-gated boolean) is materially simpler than the
  001 slice's back-to-back candidates (SYS-001/002/004 in that document); this
  is a design judgment, not an open item

---

## ID Schema

- **System Test Case**: `STP-{NNN}-{X}` — NNN matches this document's own
  feature-local `SYS-NNN` numbering (starting at SYS-001, per
  `system-design.md`'s ID Schema), X is a letter suffix (A, B, C …)
- **System Test Scenario**: `STS-{NNN}-{X}{#}` — nested under parent STP with
  numeric suffix
- **Upstream Cross-Reference**: `001:STP-{NNN}-{X}` — refers to a test case
  already defined and passed in `specs/001-cruise-brake-override/v-model/system-test.md`,
  covering the unchanged internal behaviour of an `001:SYS-NNN` component; this
  plan does not restate or renumber those test cases
- Example: `STS-001-A1` → Scenario 1 of Test Case A verifying this feature's SYS-001

---

## ISO 29119 Test Techniques Applied

| Technique | Test Cases | Design View Targeted |
|-----------|-----------|---------------------|
| Interface Contract Testing (External) | STP-001-A | Interface View — external platform boundary (Turn Signal Input) |
| Interface Contract Testing (Internal) | STP-001-B, STP-001-C, STP-001-D | Interface View — new internal interfaces SYS-001 → `001:SYS-001`, and new call-site/condition-value flow into `001:SYS-006` / `001:SYS-007` via `001:SYS-001` |
| Boundary Value Analysis | STP-001-E | Data Design View — `TurnSignalCondition` / edge-detection timing boundary |
| Fault Injection | STP-001-F, STP-001-G | Dependency View — SYS-001 → `001:SYS-001` and SYS-001 → `001:SYS-004` failure propagation |

---

## System Tests

<!-- ══════════════════════════════════════════════════════════════════════ -->
<!-- SYS-001: Turn Signal Monitor (new component)                           -->
<!-- ══════════════════════════════════════════════════════════════════════ -->

### SYS-001: Turn Signal Monitor

---

#### Test Case: STP-001-A (External Interface — Turn Signal Input)
**Technique**: Interface Contract Testing (External)
**Design View**: Interface View — external interface Turn Signal Input → SYS-001
**Linked Component**: SYS-001
**Linked Requirements**: REQ-001, REQ-007, REQ-IF-001
**Description**: Verify that SYS-001 correctly reads left, right, and
simultaneous (hazard) turn signal activation status from the platform-provided
Turn Signal Input and derives a single boolean "turn signal active" condition
that treats all three cases identically.
**Validation Condition**: Left-only, right-only, and simultaneous activation
each produce "turn signal active" = true with no distinct internal handling;
no other input pattern produces a false-positive "turn signal active" condition.
**Expected Result**: SYS-001 derives "turn signal active" = true for all three
qualifying input patterns and false otherwise; no distinction is observable
downstream between left, right, and hazard activation.

* **System Scenario: STS-001-A1**
  * **Given** SYS-001 is monitoring the Turn Signal Input and no signal is active
  * **When** the left turn signal input transitions to active
  * **Then** SYS-001 derives "turn signal active" = true

* **System Scenario: STS-001-A2**
  * **Given** the same monitoring context as STS-001-A1
  * **When** the right turn signal input transitions to active
  * **Then** SYS-001 derives "turn signal active" = true, indistinguishable in
    outcome from STS-001-A1 except for the recorded triggering side

* **System Scenario: STS-001-A3**
  * **Given** the same monitoring context as STS-001-A1
  * **When** both the left and right turn signal inputs transition to active
    simultaneously (hazard lights)
  * **Then** SYS-001 derives "turn signal active" = true; no distinct
    hazard-specific code path or output is observed (REQ-007)

---

#### Test Case: STP-001-B (Internal Interface — Cruise State Read Contract)
**Technique**: Interface Contract Testing (Internal)
**Design View**: Interface View — internal interface SYS-001 → `001:SYS-001` (Cruise State Read)
**Linked Component**: SYS-001
**Linked Requirements**: REQ-002, REQ-005, REQ-CN-003
**Description**: Verify that SYS-001 reads the current cruise state from
`001:SYS-001` by value (read-only) each control cycle and gates its
disengagement request strictly on `Cruise_Active`, taking no action at all in
any of the four non-active states. This test exercises only the new read edge
into `001:SYS-001`; the correctness of `001:SYS-001`'s own state values is
covered by `001:STP-001-B` (State Machine Boundary Conditions) in the 001
slice and is not re-verified here.
**Validation Condition**: A turn signal activation edge while `001:SYS-001`
reports `Cruise_Active` produces exactly one disengagement request; the same
edge while `001:SYS-001` reports any of its other four states produces zero
requests and zero other observable action from SYS-001.
**Expected Result**: Disengagement request issuance is gated strictly on
`Cruise_Active`; SYS-001 has no write access to `001:SYS-001`'s state (read-only
by design).

* **System Scenario: STS-001-B1**
  * **Given** `001:SYS-001` reports `Cruise_Active` and a bus monitor records
    all traffic on the SYS-001 → `001:SYS-001` interfaces
  * **When** a turn signal activation edge is detected by SYS-001
  * **Then** the bus monitor records exactly one disengagement request issued
    to `001:SYS-001`, carrying the triggering side

* **System Scenario: STS-001-B2**
  * **Given** `001:SYS-001` reports each of its four non-active states in turn
    (`Cruise_Standby`, `Cruise_Suspended`, `Cruise_Cancelled`, `Cruise_Fault`)
  * **When** a turn signal activation edge is detected by SYS-001 in each state
  * **Then** SYS-001 issues zero disengagement requests and takes no other
    action in each of the four non-active states (REQ-005)

---

#### Test Case: STP-001-C (Internal Interface — Disengagement Request Drives Existing Transition)
**Technique**: Interface Contract Testing (Internal)
**Design View**: Interface View — internal interface SYS-001 → `001:SYS-001` (Disengagement Request)
**Linked Component**: SYS-001
**Linked Requirements**: REQ-002, REQ-003, REQ-006, REQ-CN-001
**Description**: Verify that a disengagement request from SYS-001 drives
`001:SYS-001`'s existing, unmodified `Cruise_Active` → `Cruise_Cancelled`
transition mechanics — the same target-state resolution and reactivation guard
already verified for other disengagement causes in `001:STP-001-A`
(Condition Signal Contract) — without SYS-001 introducing any new transition
target, guard, or second state machine. This test verifies only the new
trigger edge; `001:SYS-001`'s internal transition correctness for a given
trigger is covered by `001:STP-001-A`/`001:STP-001-B` and is not re-verified here.
**Validation Condition**: `001:SYS-001` transitions from `Cruise_Active` to
`Cruise_Cancelled` within one control cycle of receiving a disengagement
request; deactivation of the turn signal alone produces no reactivation
request of any kind.
**Expected Result**: Exactly one transition to `Cruise_Cancelled` per
qualifying disengagement request; no request is generated on turn signal
deactivation.

* **System Scenario: STS-001-C1**
  * **Given** `001:SYS-001` is in `Cruise_Active` and SYS-001 has detected a
    qualifying turn signal activation edge
  * **When** SYS-001 issues the disengagement request to `001:SYS-001`
  * **Then** `001:SYS-001` transitions to `Cruise_Cancelled` within one control
    cycle, using its existing transition mechanics (no new state or guard
    introduced by SYS-001)

* **System Scenario: STS-001-C2**
  * **Given** `001:SYS-001` is in `Cruise_Cancelled` following a turn-signal-
    triggered transition and the turn signal subsequently deactivates
  * **When** SYS-001 observes the deactivation edge
  * **Then** SYS-001 issues no request of any kind to `001:SYS-001`; any
    resumption of `Cruise_Active` remains governed exclusively by
    `001:SYS-001`'s existing reactivation guard (REQ-006), which is verified
    independently by `001:STP-001-A` in the 001 slice

---

#### Test Case: STP-001-D (Internal Interface — New Condition Value on Existing Log/Notify Contracts)
**Technique**: Interface Contract Testing (Internal)
**Design View**: Interface View — `001:SYS-001` → `001:SYS-006` (Log Event Request) and `001:SYS-001` → `001:SYS-007` (Notification Request), exercised with the new `Turn_Signal` condition value
**Linked Component**: SYS-001 (as the origin of the new condition value); `001:SYS-006`, `001:SYS-007` (as unmodified consumers)
**Linked Requirements**: REQ-009, REQ-010, REQ-IF-003, REQ-IF-004
**Description**: Verify that when `001:SYS-001` completes a turn-signal-caused
transition, it invokes its existing, unmodified log and notification contracts
with the triggering-condition field populated as the turn signal side
(left/right), and that `001:SYS-006` / `001:SYS-007` handle this new condition
value using their existing, already-verified mechanics. The write-before-complete
guarantee, five-field record format, and exactly-once notification delivery are
**not** re-verified here — see `001:STP-006-A`, `001:STP-006-B`, `001:STP-007-A`,
`001:STP-007-B` in the 001 slice for those unchanged contracts. This test
verifies only that the new condition value is correctly populated and flows
through unmodified.
**Validation Condition**: The persisted `StateTransitionEvent` record's
triggering-condition field equals the turn signal side that caused the
transition; the notification event type correctly identifies a turn-signal
disengagement; both occur via the existing contracts with no format deviation.
**Expected Result**: One log record and one notification per qualifying
transition, both correctly tagged with the turn signal cause; no new contract,
field, or delivery mechanic is introduced.

* **System Scenario: STS-001-D1**
  * **Given** a turn-signal-triggered transition from `Cruise_Active` to
    `Cruise_Cancelled` has occurred with the left turn signal as the triggering
    cause
  * **When** `001:SYS-001` issues its log event request to `001:SYS-006`
  * **Then** the persisted `StateTransitionEvent` record's triggering-condition
    field reads `Turn_Signal` (left), using the existing five-field record
    format verified unchanged by `001:STP-006-B`

* **System Scenario: STS-001-D2**
  * **Given** the same transition as STS-001-D1
  * **When** `001:SYS-001` issues its notification request to `001:SYS-007`
  * **Then** exactly one notification event is delivered via the existing
    notification interface, identifying the disengagement as turn-signal-caused;
    exactly-once delivery is unchanged and verified independently by
    `001:STP-007-B`

---

#### Test Case: STP-001-E (Boundary Value Analysis — Activation Edge Within One Control Cycle)
**Technique**: Boundary Value Analysis
**Design View**: Data Design View — `TurnSignalCondition` activation-edge timing boundary
**Linked Component**: SYS-001
**Linked Requirements**: REQ-008, REQ-NF-001
**Description**: Verify that SYS-001 still detects a qualifying activation edge
— and still issues the disengagement request within the inherited 100 ms
latency bound (REQ-NF-001, reused from the 001 slice's REQ-NF-001) — even when
the turn signal deactivates again within the same control cycle it activated in
(the "brief tap" boundary condition named in REQ-008).
**Validation Condition**: An activation that is both asserted and deasserted
within a single control cycle still produces exactly one disengagement request
(while `Cruise_Active`), delivered within the 100 ms bound; an activation held
for zero cycles (never sampled as active) produces no request.
**Expected Result**: Edge detection is robust to the minimum-duration boundary;
no missed disengagement due to a fast tap; the 100 ms bound is met at this
boundary condition, not just in the steady-state case.

* **System Scenario: STS-001-E1**
  * **Given** `001:SYS-001` is in `Cruise_Active` and a test harness can assert
    and deassert the turn signal input within a single control cycle boundary
  * **When** the harness asserts the turn signal input and deasserts it again
    before the next control cycle boundary (a brief tap)
  * **Then** SYS-001 still detects the activation edge and issues exactly one
    disengagement request to `001:SYS-001`, delivered within 100 ms of the tap
    (REQ-NF-001)

* **System Scenario: STS-001-E2**
  * **Given** the same harness as STS-001-E1
  * **When** the turn signal input is never sampled as active within any
    control cycle (a signal glitch shorter than the sampling boundary itself,
    outside SYS-001's detection capability)
  * **Then** SYS-001 issues zero disengagement requests for that non-event;
    this boundary is explicitly outside SYS-001's responsibility per its
    edge-detection design (sensor-level fidelity is a platform concern, not
    re-verified here per REQ-CN-002)

---

#### Test Case: STP-001-F (Fault Injection — `001:SYS-001` State Read Failure)
**Technique**: Fault Injection
**Design View**: Dependency View — SYS-001 depends on `001:SYS-001` for cruise state
**Linked Component**: SYS-001
**Linked Requirements**: REQ-002, REQ-005
**Description**: Verify SYS-001's fail-safe behaviour when the read of
`001:SYS-001`'s current state is stale or fails (per the Dependency View's
"Failure Impact on Source" column: SYS-001 may withhold a required request or
issue one when not applicable).
**Validation Condition**: On a stale or failed state read, SYS-001 does not
emit a disengagement request (fail-safe: no action rather than spurious
action); it does not fabricate a `Cruise_Active` assumption.
**Expected Result**: Zero disengagement requests issued on a stale/failed
state read; no spurious action taken.

* **System Scenario: STS-001-F1**
  * **Given** SYS-001 is monitoring the turn signal input and a fault injection
    harness causes the read of `001:SYS-001`'s current state to return a stale
    or failed result
  * **When** a turn signal activation edge is detected during the same cycle
    as the failed state read
  * **Then** SYS-001 issues zero disengagement requests for that cycle; it
    does not assume `Cruise_Active` and does not act on the edge until a valid
    state read is obtained

---

#### Test Case: STP-001-G (Fault Injection — `001:SYS-004` Input Validity Boundary)
**Technique**: Fault Injection
**Design View**: Dependency View — SYS-001 relies on `001:SYS-004`'s existing `Required_Input_Invalid` mechanism (informational/scope-boundary dependency)
**Linked Component**: SYS-001
**Linked Requirements**: REQ-CN-002
**Description**: Verify that SYS-001 does **not** implement a parallel
validity-checking mechanism for the turn signal input, and that a sensor-level
fault on that input is handled exclusively by `001:SYS-004`'s existing
`Required_Input_Invalid` mechanism (already verified by `001:STP-004-A` in the
001 slice) rather than by any logic internal to SYS-001. This test verifies
the scope boundary itself, not `001:SYS-004`'s internal detection logic.
**Validation Condition**: When `001:SYS-004` reports `Required_Input_Invalid`
for the turn signal input, SYS-001 exhibits no independent validity-checking
behaviour of its own (e.g., no separate error state, no separate diagnostic
path) — inspection of SYS-001's implementation confirms zero duplicate
validity logic.
**Expected Result**: Exactly one validity-checking mechanism exists for the
turn signal input (`001:SYS-004`'s, unmodified); SYS-001 accepts the input as
reliable per its documented Assumptions and takes no independent fault-handling
action.

* **System Scenario: STS-001-G1**
  * **Given** `001:SYS-004` reports `Required_Input_Invalid` for the turn
    signal input (sensor-level fault, injected by test harness per the existing
    `001:STP-004-A` fault scenario)
  * **When** SYS-001 continues its monitoring cycle during the invalid-input
    condition
  * **Then** SYS-001 exhibits no separate error handling, diagnostic output, or
    validity-checking logic of its own for this condition; a design-time
    inspection of SYS-001 confirms no duplicate mechanism exists (REQ-CN-002)

---

### `001:SYS-001` Cruise State Machine, `001:SYS-003` Longitudinal Speed Controller, `001:SYS-006` Safety Event Logger, `001:SYS-007` Notification Dispatcher, `001:SYS-004` Platform Input Monitor (reused, unmodified)

These five components are **not redesigned** by this feature (per
`system-design.md`'s "Note on upstream rows") and are **not re-tested at the
system level** beyond the new integration points already exercised above
(STP-001-B through STP-001-G). Their unchanged internal behaviour continues to
be covered by the following pre-existing, passing test cases in
`specs/001-cruise-brake-override/v-model/system-test.md`:

| Upstream Component | Unchanged Internal Behaviour | Covered By (001 slice) |
|---------------------|-------------------------------|--------------------------|
| `001:SYS-001` Cruise State Machine | Five-state machine, target-state resolution, reactivation guard | `001:STP-001-A`, `001:STP-001-B`, `001:STP-001-C` |
| `001:SYS-003` Longitudinal Speed Controller | Cessation mechanics, exclusive propulsion command interface | `001:STP-003-A`, `001:STP-003-B`, `001:STP-003-C`, `001:STP-003-D` |
| `001:SYS-006` Safety Event Logger | Write-before-complete guarantee, five-field record format | `001:STP-006-A`, `001:STP-006-B`, `001:STP-006-C` |
| `001:SYS-007` Notification Dispatcher | Exactly-once delivery contract, notification interface | `001:STP-007-A`, `001:STP-007-B` |
| `001:SYS-004` Platform Input Monitor | `Required_Input_Invalid` sensor-level fault detection | `001:STP-004-A`, `001:STP-004-B`, `001:STP-004-C`, `001:STP-004-D` |

No new STP is raised for these components' internal behaviour; raising one
would duplicate already-passing verification evidence and contradict
REQ-CN-001's reuse-without-modification mandate.

---

## Safety-Critical Test Sections (ISO 26262 Overlay)

### Structural Coverage Requirements (ISO 26262-6 §9.4.5)

SYS-001 carries **confirmed** ASIL D, inherited from the 001 slice's OQ-001
resolution. The four reused components retain the MC/DC coverage targets
already declared in the 001 slice's system-test.md (`001:SYS-001`,
`001:SYS-003`, `001:SYS-004`, `001:SYS-006`, `001:SYS-007` rows); those targets
are unchanged and are not restated as new obligations here.

| Component | ASIL | Coverage Target | Technique | Shortfall Justification |
|-----------|------|----------------|-----------|------------------------|
| SYS-001 (Turn Signal Monitor) | ASIL D | MC/DC 100% | Modified Condition/Decision Coverage | None — decision logic is bounded to edge detection + single state-gate condition, consistent with the 001 slice's Module-tier complexity threshold |
| `001:SYS-001`, `001:SYS-003`, `001:SYS-004`, `001:SYS-006`, `001:SYS-007` *(reused)* | ASIL D | MC/DC 100% (unchanged) | MC/DC | Unchanged — see 001 slice's Structural Coverage table; no new decision paths introduced into these components by this feature |

### Resource Usage Verification (ISO 26262-6 §9.4.4)

| Component | ASIL | Resource | Measurement Method | Threshold | Measurement Condition |
|-----------|------|----------|-------------------|-----------|----------------------|
| SYS-001 | ASIL D | WCET | Instrumented measurement + static WCET analysis | Must fit within the inherited 100 ms end-to-end bound (REQ-NF-001) alongside the existing 001-slice call chain into `001:SYS-006`/`001:SYS-007` | Turn signal edge detected simultaneously with worst-case platform load on all other 001-slice inputs |
| SYS-001 | ASIL D | Max Stack Depth | Static stack analysis | To be defined at implementation phase | Maximum call depth from Turn Signal Input read → disengagement request emission |

**Note**: Dynamic heap allocation is not permitted for ASIL D components,
consistent with the 001 slice; SYS-001 introduces no persistent data entity
and no heap allocation (per `system-design.md`'s Data Design View).

---

## V&V Coverage (IEEE 1012:2016)

Every one of this feature's 18 active requirements has at least one V&V
activity at system test level, either a new STP (for SYS-001) or a
cross-referenced upstream `001:STP-*` (for behaviour owned by a reused
component).

| REQ | V&V Activity | Type | STP / Note |
|-----|-------------|------|-----------|
| REQ-001 | Turn signal input monitoring, all activation patterns | Test | STP-001-A |
| REQ-002 | Disengagement request gated on `Cruise_Active`; drives existing transition | Test | STP-001-B, STP-001-C |
| REQ-003 | Transition target resolves to `Cruise_Cancelled` via existing mechanics | Test | STP-001-C, `001:STP-001-A` |
| REQ-004 | Cessation of longitudinal control output as part of the same transition | Test | `001:STP-003-B` (unchanged cessation mechanics) |
| REQ-005 | No action taken while cruise is not `Cruise_Active` | Test | STP-001-B |
| REQ-006 | Deactivation produces no reactivation request; reactivation guard unchanged | Test | STP-001-C, `001:STP-001-A` |
| REQ-007 | Left/right/hazard treated identically | Test | STP-001-A |
| REQ-008 | Brief-tap edge still detected within the control cycle | Test | STP-001-E |
| REQ-009 | Notification issued via existing notification contract | Test | STP-001-D, `001:STP-007-A`, `001:STP-007-B` |
| REQ-010 | Event record persisted with turn signal triggering condition | Test | STP-001-D, `001:STP-006-A`, `001:STP-006-B` |
| REQ-NF-001 | 100 ms latency bound met at the brief-tap boundary | Test | STP-001-E, Resource Usage table |
| REQ-IF-001 | Turn signal input is the sole source for the trigger condition | Test, Inspection | STP-001-A |
| REQ-IF-002 | Propulsion command interface reused exclusively, unmodified | Inspection | `001:STP-003-A` |
| REQ-IF-003 | Notification interface reused, unmodified | Test | STP-001-D, `001:STP-007-A` |
| REQ-IF-004 | Event storage interface reused, unmodified | Test | STP-001-D, `001:STP-006-A` |
| REQ-CN-001 | No new state, guard, or duplicate interface introduced | Inspection | STP-001-B, STP-001-C, upstream cross-reference table |
| REQ-CN-002 | No parallel validity mechanism implemented by SYS-001 | Inspection, Test | STP-001-G |
| REQ-CN-003 | Activation preconditions remain exclusively `001:SYS-001`'s responsibility | Inspection | STP-001-B, `001:STP-001-A` |

**V&V gaps**: None — all 18 active requirements have at least one V&V activity
(new STP or cross-referenced upstream `001:STP-*`).

---

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total SYS Components (this document, new) | 1 (SYS-001) |
| Total Upstream Reference Components (`001:SYS-NNN`, reused unmodified) | 5 |
| Total New Test Cases (STP) | 7 (STP-001-A through STP-001-G) |
| Total New System Scenarios (STS) | 13 (STS-001-A1..A3, B1..B2, C1..C2, D1..D2, E1..E2, F1, G1) |
| SYS → STP Coverage (new component) | 1/1 (100%) — SYS-001 has 7 test cases |
| STP → STS Coverage | 7/7 (100%) — every STP has ≥1 nested STS |
| REQ V&V Coverage | 18/18 (100%) — every REQ-001..010, REQ-NF-001, REQ-IF-001..004, REQ-CN-001..003 has ≥1 V&V activity (new STP and/or upstream `001:STP-*` cross-reference) |
| Upstream Components Re-Tested at System Level | 0 — all reused components' internal behaviour covered by pre-existing, passing 001-slice STPs (cross-referenced, not restated) |

### Technique Distribution

| Technique | STP Count |
|-----------|-----------|
| Interface Contract Testing (External) | 1 |
| Interface Contract Testing (Internal) | 3 |
| Boundary Value Analysis | 1 |
| Fault Injection | 2 |
| **Total (new)** | **7** |

### ISO 26262 Safety Test Coverage

| Safety Test Category | Count |
|---------------------|-------|
| MC/DC structural coverage targets declared (new component) | 1 (SYS-001) |
| Resource usage tests declared (new component) | 2 rows (WCET, stack depth for SYS-001) |
| Back-to-back test pairs designated (new) | 0 — design judgment; SYS-001's decision logic is simpler than the 001 slice's designated candidates (see Overview) |

### Uncovered Components

None. SYS-001 (the only new component) has 7 test cases. All 5 upstream
`001:SYS-NNN` reference rows are covered either by a new integration-point test
case in this document or by an explicit cross-reference to the exact upstream
STP ID(s) that already verify their unchanged internal behaviour.

### Open Items

None raised by this test plan. All open questions referenced in this feature's
upstream dependencies (OQ-001 ASIL D, OQ-002 T_max = 100 ms, OQ-003
`Cruise_Cancelled` target state) were already resolved in the 001 slice
(`specs/001-cruise-brake-override/v-model/requirements.md`, "Open flags" /
"`[RESOLVED 2026-06-02]`" note) and are inherited here without re-opening.

---

**Next step**: Run `/speckit.v-model.trace` to build the full bidirectional
traceability matrix, incorporating this document's STP/STS IDs alongside the
cross-referenced upstream `001:STP-*` IDs.
