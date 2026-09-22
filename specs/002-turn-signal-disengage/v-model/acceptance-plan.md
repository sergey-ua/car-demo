# Acceptance Test Plan: Turn Signal Cruise Disengagement

**Feature Branch**: `002-turn-signal-disengage`
**Created**: 2026-09-22
**Status**: Draft
**Source**: `specs/002-turn-signal-disengage/v-model/requirements.md`

---

## Overview

This document defines the three-tier Acceptance Test Plan for the Turn Signal
Cruise Disengagement feature. Every requirement in `requirements.md` is paired
with one or more Test Cases (`ATP-NNN-X`) and one or more executable BDD
scenarios (`SCN-NNN-X#`).

This feature is layered on top of the Vehicle Cruise Control Brake Override
Safety Slice (`specs/001-cruise-brake-override/`) and reuses that slice's state
machine, reactivation guard, propulsion command interface, notification
interface, and event storage. Where a requirement here is tagged `[REUSED: ...]`
in `requirements.md`, the corresponding test case verifies only the
turn-signal-specific behavior added by this feature; it does not re-verify the
upstream mechanism, which already carries its own acceptance coverage in
`specs/001-cruise-brake-override/v-model/acceptance-plan.md`.

**Domain**: ISO 26262, ASIL D (**Confirmed** — inherited from
`specs/001-cruise-brake-override/`, see that slice's OQ-001).

**ISO 26262 acceptance overlay applied** (Part 6 §6.9 Table 11), consistent
with the sibling slice:
- All requirements carry confirmed ASIL D.
- Safety mechanism test cases enforce:
  - `[MECHANISM: Detection]` — verify detection AND correct response to detection
  - `[MECHANISM: Prevention]` — include negative testing (attempt to violate the
    prevented condition)
  - `[MECHANISM: Mitigation]` — verify the degraded operating mode is safe

## ID Schema

- **Test Case**: `ATP-{NNN}-{X}` where `NNN` matches the parent REQ number and `X`
  is a letter suffix (A, B, C …).
- **Scenario**: `SCN-{NNN}-{X}{#}` nested under the parent ATP, with numeric suffix.
- Example: `SCN-002-A1` → Scenario 1 of Test Case A validating REQ-002.

---

## Acceptance Tests

### Requirement Validation: REQ-001 (Monitor Turn Signal Inputs)

#### Test Case: ATP-001-A (Left and Right Turn Signal Inputs Monitored)
**Linked Requirement:** REQ-001
**Description:** Verify by design inspection that both the left and right turn
signal inputs provided by the existing EV prototype platform are actively
monitored by the feature.
**Validation Condition:** The design documentation shows explicit monitoring of
both the left and right turn signal inputs; neither is absent from the
monitoring architecture.
**Expected Result:** Both turn signal inputs are present in the feature's input
monitoring architecture with no omissions.

* **User Scenario: SCN-001-A1**
  * **Given** the feature design documentation is available for review
  * **When** an architect reviews the input monitoring specification
  * **Then** both the left turn signal input and the right turn signal input are
    explicitly listed as monitored by the feature

---

### Requirement Validation: REQ-002 (Transition Out of Cruise_Active on Turn Signal)

#### Test Case: ATP-002-A (Left Turn Signal Triggers Disengagement)
**Linked Requirement:** REQ-002
**Description:** Verify that activating the left turn signal while `Cruise_Active`
causes the feature to transition cruise control out of `Cruise_Active` within
one control cycle.
**Validation Condition:** The observable cruise control state transitions out of
`Cruise_Active` within one control cycle of the left turn signal activation.
**Expected Result:** State is no longer `Cruise_Active` at or before the end of
the control cycle following activation.

* **User Scenario: SCN-002-A1**
  * **Given** cruise control is `Cruise_Active` and no turn signal is active
  * **When** the driver activates the left turn signal
  * **Then** the feature transitions cruise control out of `Cruise_Active`
    within one control cycle

#### Test Case: ATP-002-B (Right Turn Signal Triggers Disengagement)
**Linked Requirement:** REQ-002
**Description:** Verify that activating the right turn signal while
`Cruise_Active` causes the same transition as the left turn signal (REQ-002
applies identically to either side).
**Validation Condition:** The observable cruise control state transitions out of
`Cruise_Active` within one control cycle of the right turn signal activation.
**Expected Result:** State is no longer `Cruise_Active` at or before the end of
the control cycle following activation.

* **User Scenario: SCN-002-B1**
  * **Given** cruise control is `Cruise_Active` and no turn signal is active
  * **When** the driver activates the right turn signal
  * **Then** the feature transitions cruise control out of `Cruise_Active`
    within one control cycle

#### Test Case: ATP-002-C (Both Signals Simultaneously — Hazard Lights)
**Linked Requirement:** REQ-002, REQ-007
**Description:** Verify that simultaneous activation of both turn signals
(hazard lights) is treated identically to a single turn signal activation and
still triggers disengagement within one control cycle.
**Validation Condition:** With both turn signals activated simultaneously from
`Cruise_Active`, the feature transitions out of `Cruise_Active` within one
control cycle, exactly as for a single-signal activation.
**Expected Result:** Disengagement occurs within one control cycle; no distinct
hazard-specific handling or delay is observed.

* **User Scenario: SCN-002-C1**
  * **Given** cruise control is `Cruise_Active` and neither turn signal is active
  * **When** the driver activates both the left and right turn signals
    simultaneously (hazard lights)
  * **Then** the feature transitions cruise control out of `Cruise_Active`
    within one control cycle, identically to a single turn signal activation

#### Test Case: ATP-002-D (Brief Tap Within a Single Control Cycle)
**Linked Requirement:** REQ-002, REQ-008
**Description:** Edge case — verify that a turn signal activated and then
deactivated within the same control cycle in which `Cruise_Active` was current
still causes the disengagement transition to occur.
**Validation Condition:** When the turn signal is detected active during a
control cycle in which cruise control is `Cruise_Active`, and is deactivated
again before the end of that same cycle, the transition out of `Cruise_Active`
still occurs.
**Expected Result:** The disengagement transition occurs even though the signal
is no longer active by the time the transition completes; no race condition
suppresses disengagement.

* **User Scenario: SCN-002-D1**
  * **Given** cruise control is `Cruise_Active` and a test harness can assert
    and de-assert the turn signal input within a single control cycle
  * **When** the left turn signal is briefly activated and then deactivated
    within one control cycle while cruise control is `Cruise_Active`
  * **Then** the feature still transitions cruise control out of `Cruise_Active`
    within one control cycle, and the transition is not suppressed by the
    signal's subsequent deactivation

---

### Requirement Validation: REQ-003 (Transition to Cruise_Cancelled)

#### Test Case: ATP-003-A (Target State Is Cruise_Cancelled)
**Linked Requirement:** REQ-003
**Description:** Verify that the transition out of `Cruise_Active` caused by
turn signal activation under REQ-002 lands specifically in `Cruise_Cancelled`,
not any other non-active state.
**Validation Condition:** Over 20 consecutive turn-signal-triggered
disengagement runs from `Cruise_Active`, the resulting state is
`Cruise_Cancelled` every time.
**Expected Result:** All 20 runs produce `Cruise_Cancelled`; zero instances of
`Cruise_Fault`, `Cruise_Active`, `Cruise_Standby`, `Cruise_Suspended`, or an
undefined value are recorded as the post-transition state.

* **User Scenario: SCN-003-A1**
  * **Given** cruise control is `Cruise_Active` and a test harness records the
    post-transition state value for each run
  * **When** a turn signal is activated 20 times across separate test runs,
    each starting from `Cruise_Active`
  * **Then** all 20 recorded post-transition state values are identical and
    equal to `Cruise_Cancelled`

---

### Requirement Validation: REQ-004 (Cease Speed Control Output on Turn Signal Disengagement)

#### Test Case: ATP-004-A (Cessation Coincident With Transition)
**Linked Requirement:** REQ-004
**Description:** Verify that all longitudinal speed control output to the
propulsion command interface ceases as part of the same transition triggered by
turn signal activation.
**Validation Condition:** The last command issued to the propulsion command
interface following turn signal activation from `Cruise_Active` is a
cessation/stop command, and no further speed control commands are issued after
that point.
**Expected Result:** The propulsion command interface receives exactly one
cessation command following the turn-signal-triggered transition; no
speed-maintaining or speed-adjusting commands follow.

* **User Scenario: SCN-004-A1**
  * **Given** the feature is in `Cruise_Active` and is actively issuing speed
    control commands to the propulsion command interface
  * **When** the driver activates a turn signal, causing disengagement per
    REQ-002
  * **Then** the propulsion command interface receives a cessation command and
    no further speed control commands are issued by the feature

#### Test Case: ATP-004-B (Cessation During Active Speed Correction)
**Linked Requirement:** REQ-004
**Description:** Verify that turn signal activation causes immediate cessation
even when the feature is mid-way through a speed correction manoeuvre.
**Validation Condition:** Cessation occurs and no partial speed correction
command completes after turn signal activation is detected in `Cruise_Active`.
**Expected Result:** The in-progress speed correction command is terminated and
the propulsion command interface receives a cessation command; no further
correction commands follow.

* **User Scenario: SCN-004-B1**
  * **Given** the feature is in `Cruise_Active` and vehicle speed is below the
    cruise target, resulting in an active acceleration command to the
    propulsion command interface
  * **When** the driver activates a turn signal
  * **Then** the acceleration command is terminated and the propulsion command
    interface receives a cessation command with no subsequent acceleration
    commands

---

### Requirement Validation: REQ-005 (No Action When Not in Cruise_Active)

#### Test Case: ATP-005-A (No State Change Outside Cruise_Active — Negative Test)
**Linked Requirement:** REQ-005
**Description:** Negative test — verify that turn signal activation while
cruise control is in any state other than `Cruise_Active` produces no state
change or side effect attributable to this feature.
**Validation Condition:** With cruise control in `Cruise_Standby`,
`Cruise_Suspended`, `Cruise_Cancelled`, and `Cruise_Fault` in turn, activating a
turn signal produces zero observable state transitions and zero propulsion
command interface activity from this feature.
**Expected Result:** State remains unchanged in all four tested non-active
states; no cessation command, notification, or event record is produced by
this feature as a result of the turn signal input alone.

* **User Scenario: SCN-005-A1**
  * **Given** cruise control is in `Cruise_Standby` (repeated for
    `Cruise_Suspended`, `Cruise_Cancelled`, and `Cruise_Fault`) and no turn
    signal is active
  * **When** the driver activates a turn signal
  * **Then** this feature takes no action on the cruise control state: the
    state is unchanged, no cessation command is issued, no notification is
    issued, and no event record is written by this feature

---

### Requirement Validation: REQ-006 (Turn Signal Deactivation Does Not Cause Reactivation)

#### Test Case: ATP-006-A (Deactivation Alone Does Not Resume Cruise_Active)
**Linked Requirement:** REQ-006
**Description:** Verify that deactivating the turn signal after a
turn-signal-triggered disengagement does not, by itself, cause cruise control
to transition back to `Cruise_Active`.
**Validation Condition:** Following a turn-signal-triggered transition to
`Cruise_Cancelled`, deactivating the turn signal — with no cruise activation
command issued and no activation preconditions re-evaluated as satisfied by
driver request — leaves the feature in `Cruise_Cancelled`.
**Expected Result:** Zero instances of spontaneous transition to
`Cruise_Active` are observed following turn signal deactivation alone.

* **User Scenario: SCN-006-A1**
  * **Given** cruise control was disengaged to `Cruise_Cancelled` due to turn
    signal activation, and no cruise activation command has been issued
  * **When** the turn signal is subsequently deactivated
  * **Then** cruise control remains in `Cruise_Cancelled` and does not
    automatically resume `Cruise_Active`

#### Test Case: ATP-006-B (Reactivation Still Governed by Existing Guard) `[REUSED: REQ-005, REQ-018 in specs/001-cruise-brake-override/]`
**Linked Requirement:** REQ-006
**Description:** Verify that resumption of `Cruise_Active` after a
turn-signal-triggered disengagement follows the same activation request and
five-precondition process already verified for the upstream slice
(`ATP-005-B`, `ATP-018-A` in `specs/001-cruise-brake-override/v-model/acceptance-plan.md`);
this test case checks only that the turn-signal-disengaged state is not
special-cased to bypass or additionally restrict that existing process.
**Validation Condition:** From `Cruise_Cancelled` reached via turn signal
disengagement, an explicit driver activation request issued once all five
existing activation preconditions are satisfied results in a transition to
`Cruise_Active`, with no additional turn-signal-specific precondition imposed
or existing precondition bypassed.
**Expected Result:** Feature transitions to `Cruise_Active` under the same
conditions as any other `Cruise_Cancelled` reactivation; no turn-signal-specific
gating logic is observed.

* **User Scenario: SCN-006-B1**
  * **Given** cruise control is in `Cruise_Cancelled` due to a prior
    turn-signal-triggered disengagement, the turn signal is now deactivated,
    and all five activation preconditions (per REQ-018 in
    `specs/001-cruise-brake-override/`) are satisfied
  * **When** the driver issues a cruise activation command
  * **Then** the feature transitions to `Cruise_Active`, exactly as it would
    following a `Cruise_Cancelled` reached by any other disengagement cause

---

### Requirement Validation: REQ-007 (Hazard Lights Treated as Ordinary Activation)

#### Test Case: ATP-007-A (No Distinct Hazard-Specific Handling)
**Linked Requirement:** REQ-007
**Description:** Verify by inspection and test that no hazard-light-specific
code path or behavior exists beyond ordinary single-signal handling; covered
functionally by ATP-002-C. This test case additionally confirms no distinct
notification content or event record field is produced for the simultaneous
case.
**Validation Condition:** The event record and notification produced for a
simultaneous-signal (hazard) disengagement are structurally identical in
format to those produced for a single-signal disengagement, differing only in
the recorded triggering-condition value if applicable.
**Expected Result:** No hazard-specific record format, notification content, or
timing difference is observed relative to single-signal disengagement.

* **User Scenario: SCN-007-A1**
  * **Given** two test runs are performed — one with a single turn signal
    activated and one with both turn signals activated simultaneously — each
    from `Cruise_Active`
  * **When** the resulting event records and notifications from both runs are
    compared
  * **Then** both records follow the same format with no additional or
    differently-structured fields for the hazard-light case

---

### Requirement Validation: REQ-008 (Brief Tap Still Triggers Disengagement)

#### Test Case: ATP-008-A (Fault-Injection-Style Race Condition Test)
**Linked Requirement:** REQ-008
**Description:** Fault-injection-style test — verify that a turn signal
detected active for only a single control cycle (activated and deactivated
within that cycle) does not race against or suppress the disengagement
transition required by REQ-002. Functionally identical to ATP-002-D; retained
as a separate test case because REQ-008 is independently traceable to a named
edge case in `spec.md`.
**Validation Condition:** Across 20 repeated brief-tap trials, disengagement
occurs in all 20 trials with no instance of the transition being skipped or
delayed beyond one control cycle.
**Expected Result:** 20/20 trials show disengagement to `Cruise_Cancelled`
within one control cycle; zero suppressed transitions.

* **User Scenario: SCN-008-A1**
  * **Given** cruise control is `Cruise_Active` and a test harness can assert
    and de-assert a turn signal input within a single control cycle, repeated
    across 20 separate trials
  * **When** a turn signal is briefly activated and deactivated within one
    control cycle on each trial
  * **Then** all 20 trials show cruise control transitioning to
    `Cruise_Cancelled` within one control cycle of the activation

---

### Requirement Validation: REQ-009 (Notification on Turn-Signal-Triggered Disengagement) `[REUSED: REQ-012 in specs/001-cruise-brake-override/]`

#### Test Case: ATP-009-A (Notification Issued via Platform Interface)
**Linked Requirement:** REQ-009
**Description:** Verify that a notification event is delivered to the platform
notification interface when a turn-signal-triggered disengagement occurs,
reusing the same notification interface and delivery mechanism already
verified in `ATP-012-A` of the upstream slice; this test case checks that the
turn-signal trigger correctly invokes that existing mechanism.
**Validation Condition:** Exactly one notification event is captured at the
platform notification interface following a single turn-signal-triggered
disengagement.
**Expected Result:** The notification interface log records exactly one
notification event traceable to the turn signal disengagement occurrence.

* **User Scenario: SCN-009-A1**
  * **Given** the feature is in `Cruise_Active` and the platform notification
    interface has been reset (no outstanding notifications)
  * **When** a turn signal activation causes disengagement per REQ-002
  * **Then** the platform notification interface receives exactly one
    notification event, and the notification interface log records this event

---

### Requirement Validation: REQ-010 (Persist Event Record With Triggering Side) `[REUSED: REQ-013, REQ-014 in specs/001-cruise-brake-override/]`

#### Test Case: ATP-010-A (Event Record Written Before Transition Complete)
**Linked Requirement:** REQ-010
**Description:** Verify that an event record is persisted to local persistent
event storage before the turn-signal-triggered transition is considered
complete, reusing the same persistence timing guarantee already verified in
`ATP-013-B` of the upstream slice.
**Validation Condition:** When the feature state is polled immediately after a
turn-signal-triggered transition (within one processing cycle), the
corresponding event record is already present in persistent storage.
**Expected Result:** Event record for the transition is present in storage when
the new post-transition state (`Cruise_Cancelled`) is first observable.

* **User Scenario: SCN-010-A1**
  * **Given** the feature is in `Cruise_Active` and a test observer is
    monitoring both the feature state output and the event storage
    simultaneously
  * **When** a turn signal activation triggers a state transition
  * **Then** at the first processing cycle where `Cruise_Cancelled` is
    observed, the corresponding event record is already present in
    persistent storage

#### Test Case: ATP-010-B (All Five Fields Present, Triggering Condition Records Side)
**Linked Requirement:** REQ-010
**Description:** Verify that the event record contains all five mandatory
fields (timestamp, from-state, triggering condition, to-state, vehicle speed)
per the reused format from REQ-013/REQ-014 of the upstream slice, and that the
triggering-condition field specifically records which side (left or right)
activated.
**Validation Condition:** Inspection of an event record written for a
left-signal-triggered transition shows the triggering-condition field
recording "left turn signal" (or the equivalent defined value); a separate
right-signal trial records "right turn signal"; all five fields are non-null
and non-empty in both records.
**Expected Result:** All five fields are present and non-empty; the
triggering-condition field accurately distinguishes left from right across the
two trials.

* **User Scenario: SCN-010-B1**
  * **Given** the feature is in `Cruise_Active` with vehicle speed reporting a
    known stable value from the test harness
  * **When** the left turn signal is activated, triggering disengagement, and
    the event record is retrieved from persistent storage
  * **Then** all five fields are present: timestamp is a non-null datetime
    value, from-state is `Cruise_Active`, triggering condition identifies the
    left turn signal, to-state is `Cruise_Cancelled`, and vehicle speed
    matches the known value within tolerance

* **User Scenario: SCN-010-B2**
  * **Given** the feature is in `Cruise_Active` with vehicle speed reporting a
    known stable value from the test harness
  * **When** the right turn signal is activated, triggering disengagement, and
    the event record is retrieved from persistent storage
  * **Then** all five fields are present and the triggering condition field
    identifies the right turn signal, distinguishing it from the left-signal
    case in SCN-010-B1

---

### Requirement Validation: REQ-NF-001 (Latency Bound — 100 ms) `[REUSED: REQ-NF-001 in specs/001-cruise-brake-override/]`

#### Test Case: ATP-NF-001-A (Latency Measurement Over Repeated Runs)
**Linked Requirement:** REQ-NF-001
**Description:** Measure the elapsed time from a confirmed turn signal
activation sample (while `Cruise_Active`) to both (a) the state transition out
of `Cruise_Active` and (b) the first cessation command delivered to the
propulsion command interface, across 100 consecutive test runs, using the same
100 ms bound already confirmed for the upstream slice's `Brake_Override`
handling (OQ-002).
**Validation Condition:** All 100 measured latencies for both (a) and (b) are
at or below 100 ms; the worst observed latency does not exceed 100 ms.
**Expected Result:** All 100 measurements for both the state transition and the
cessation command are ≤ 100 ms. No measurement exceeds the threshold.

* **User Scenario: SCN-NF-001-A1**
  * **Given** the feature is in `Cruise_Active`, a high-resolution timer is
    armed at the moment a turn signal input is confirmed active, and the test
    harness will run this scenario 100 consecutive times
  * **When** a turn signal activation is confirmed on each run
  * **Then** the timer stops when both the state transition out of
    `Cruise_Active` and the cessation command at the propulsion command
    interface are observed, and all 100 recorded elapsed times for each are at
    or below 100 ms

#### Test Case: ATP-NF-001-B (Worst-Case Execution Time Under Load)
**Linked Requirement:** REQ-NF-001
**Description:** Temporal test under worst-case concurrent load — measure
latency when other platform inputs are simultaneously active.
**Validation Condition:** Latency under simultaneous multi-input stimulation
does not exceed 100 ms.
**Expected Result:** Worst-case latency under concurrent input load is ≤
100 ms.

* **User Scenario: SCN-NF-001-B1**
  * **Given** the feature is in `Cruise_Active` and the test harness
    simultaneously stimulates vehicle speed updates, driver command inputs,
    and diagnostic status inputs at their maximum rated frequencies to
    represent worst-case load
  * **When** a turn signal activation is confirmed under this maximum
    concurrent load
  * **Then** the elapsed time from activation confirmation to both the state
    transition and the propulsion interface cessation command is at or below
    100 ms

---

### Requirement Validation: REQ-IF-001 (Turn Signal Input Source)

#### Test Case: ATP-IF-001-A (Trigger Derived Exclusively From Turn Signal Input)
**Linked Requirement:** REQ-IF-001
**Description:** Verify that the REQ-002 trigger condition is derived
exclusively from the platform turn signal input and not from any other source.
**Validation Condition:** Turn-signal-triggered disengagement occurs if and
only if the platform left or right turn signal input transitions to active;
stimulating any other input does not produce this feature's disengagement
behavior.
**Expected Result:** Disengagement under this feature is triggered exclusively
via the turn signal input; no false positive from other inputs.

* **User Scenario: SCN-IF-001-A1**
  * **Given** the feature is in `Cruise_Active` and all inputs other than the
    turn signal inputs are held stable
  * **When** an unrelated platform input changes (e.g., vehicle speed updates
    within normal range) with no turn signal activation
  * **Then** no turn-signal-triggered disengagement occurs; the feature only
    disengages under this requirement when the turn signal input itself
    transitions to active

---

### Requirement Validation: REQ-IF-002 (Propulsion Command Interface Reuse) `[REUSED: REQ-IF-001 in specs/001-cruise-brake-override/]`

#### Test Case: ATP-IF-002-A (No Second Output Channel Introduced)
**Linked Requirement:** REQ-IF-002
**Description:** Verify by inspection that this feature's cessation of
longitudinal speed control output (REQ-004) routes exclusively through the
existing propulsion command interface already designated in
`specs/001-cruise-brake-override/` (REQ-IF-001), with no second or alternate
output channel introduced by this feature.
**Validation Condition:** The interface usage specification for this feature
names no output interface for longitudinal control cessation other than the
existing propulsion command interface.
**Expected Result:** Design documentation shows exactly one longitudinal
control output interface used by this feature — the existing propulsion
command interface — with no alternative pathway added.

* **User Scenario: SCN-IF-002-A1**
  * **Given** the feature interface design specification is available for
    review
  * **When** an architect audits every output interface this feature uses for
    longitudinal speed control cessation
  * **Then** exactly one output interface is identified — the existing
    propulsion command interface from `specs/001-cruise-brake-override/` — and
    no additional interface is introduced

---

### Requirement Validation: REQ-IF-003 (Notification Interface Reuse) `[REUSED: REQ-IF-003 in specs/001-cruise-brake-override/]`

#### Test Case: ATP-IF-003-A (Notification Delivered Through Existing Interface Only)
**Linked Requirement:** REQ-IF-003
**Description:** Verify that the REQ-009 notification is delivered through the
existing platform notification interface and not through any other channel.
**Validation Condition:** The platform notification interface log records the
turn-signal-disengagement notification; no other interface or bus carries this
notification.
**Expected Result:** Notification event recorded at platform notification
interface; zero notification events observed on any other interface for this
trigger.

* **User Scenario: SCN-IF-003-A1**
  * **Given** the feature is in `Cruise_Active` and a test observer is
    monitoring all output interfaces of the feature
  * **When** a turn signal activation triggers disengagement
  * **Then** exactly one notification event is recorded at the platform
    notification interface; no notification-related output is observed on any
    other interface or communication channel

---

### Requirement Validation: REQ-IF-004 (Event Storage Reuse) `[REUSED: REQ-IF-004 in specs/001-cruise-brake-override/]`

#### Test Case: ATP-IF-004-A (Event Records Written to Existing Platform Storage Only)
**Linked Requirement:** REQ-IF-004
**Description:** Verify that REQ-010 event records are written exclusively to
the existing platform local persistent event storage and not to any private,
volatile, or alternative storage location.
**Validation Condition:** Post-transition event records for turn-signal
disengagements are retrievable from the existing platform event storage; no
records are written to any other storage location by this feature.
**Expected Result:** Event records present in the existing platform storage; no
records in any private store.

* **User Scenario: SCN-IF-004-A1**
  * **Given** the feature has processed a turn-signal-triggered disengagement
    transition
  * **When** the persistent storage contents are inspected across all
    available storage interfaces
  * **Then** the event record is present in the existing platform local
    persistent event storage, and no duplicate or alternative record is found
    in any other storage location accessible from the feature

---

### Requirement Validation: REQ-CN-001 (Reuse of Existing State Machine, Guard, and Interfaces — No Parallel Logic)

#### Test Case: ATP-CN-001-A (No New State, State Machine, or Guard Introduced — Inspection)
**Linked Requirement:** REQ-CN-001
**Description:** Verify by inspection that the feature introduces no new cruise
control state, no second state machine, and no separate reactivation guard,
and instead reuses without modification the five-state machine, reactivation
guard, propulsion command interface, notification interface, and event storage
from `specs/001-cruise-brake-override/`.
**Validation Condition:** The design and implementation artifacts for this
feature reference the existing state machine, guard, and interfaces by name
with no redefinition; the state enumeration remains exactly the five states
already defined (`Cruise_Standby`, `Cruise_Active`, `Cruise_Suspended`,
`Cruise_Cancelled`, `Cruise_Fault`).
**Expected Result:** Zero new states, state machines, or reactivation guards
are found; all cited mechanisms trace to the existing upstream definitions.

* **User Scenario: SCN-CN-001-A1**
  * **Given** the design artifacts for this feature and for
    `specs/001-cruise-brake-override/` are both available for review
  * **When** an architect compares the state enumeration, reactivation guard,
    and interface definitions referenced by this feature against the upstream
    slice
  * **Then** this feature's artifacts reference the identical five-state
    machine, the identical reactivation guard, and the identical interfaces
    with no new or divergent definitions introduced

---

### Requirement Validation: REQ-CN-002 (Turn Signal Input Fault Handling Out of Scope)

#### Test Case: ATP-CN-002-A (Input Fault Governed Exclusively by Existing Mechanism — Inspection)
**Linked Requirement:** REQ-CN-002
**Description:** Verify by inspection that this feature implements no separate
validity-checking mechanism for the turn signal input, and that turn signal
input faults continue to be handled exclusively by the existing
`Required_Input_Invalid` mechanism from `specs/001-cruise-brake-override/`.
**Validation Condition:** The design artifacts show no turn-signal-specific
input validity logic; a turn signal input fault is handled only via the
existing `Required_Input_Invalid` condition path.
**Expected Result:** Zero turn-signal-specific validity mechanisms are found in
the design; input faults route exclusively through the existing mechanism.

* **User Scenario: SCN-CN-002-A1**
  * **Given** the feature is in `Cruise_Active` and the turn signal input is
    delivering an integrity-failed or unavailable signal
  * **When** a Safety Engineer reviews how the fault is handled
  * **Then** the fault is handled exclusively via the existing
    `Required_Input_Invalid` mechanism (causing exit to a non-active state per
    the upstream slice's REQ-011), and no turn-signal-specific fault-handling
    code path is present

---

### Requirement Validation: REQ-CN-003 (Activation Preconditions Out of Scope)

#### Test Case: ATP-CN-003-A (Turn Signal Active at Activation Request Governed by Existing Guard — Inspection)
**Linked Requirement:** REQ-CN-003
**Description:** Verify by inspection and test that this feature defines no
activation precondition of its own, and that a turn signal already active at
the moment of a cruise activation request is handled exclusively by the
existing five-precondition activation process (REQ-018 in
`specs/001-cruise-brake-override/`), which does not include turn signal state
as a precondition.
**Validation Condition:** With a turn signal already active and all five
existing activation preconditions satisfied, a cruise activation request
succeeds exactly as it would with no turn signal active; this feature adds no
sixth precondition.
**Expected Result:** Activation outcome is identical regardless of turn signal
state, confirming this feature imposes no additional precondition.

* **User Scenario: SCN-CN-003-A1**
  * **Given** cruise control is in `Cruise_Standby`, a turn signal is already
    active, and all five existing activation preconditions (per REQ-018) are
    satisfied
  * **When** the driver issues a cruise activation command
  * **Then** the feature transitions to `Cruise_Active`, identically to the
    case where no turn signal is active, confirming this feature does not
    gate activation on turn signal state

---

## Coverage Summary

| Metric | Count |
|--------|-------|
| Total Requirements | 18 |
| Total Test Cases (ATP) | 25 |
| Total Scenarios (SCN) | 26 |
| REQ → ATP Coverage | 100% |
| ATP → SCN Coverage | 100% |

**Validation Status**: ✅ Full Coverage (manually verified — no deterministic
coverage script exists in this repository; verification performed by cross-
checking every REQ-* ID in `requirements.md` against the "Linked Requirement"
field of every ATP below, and every ATP against at least one SCN)
**Generated**: 2026-09-22

### Coverage by Requirement Category

| Category | REQs | ATPs | SCNs |
|----------|------|------|------|
| Functional (REQ-001 – REQ-010) | 10 | 16 | 17 |
| Non-Functional (REQ-NF-001) | 1 | 2 | 2 |
| Interface (REQ-IF-001 – REQ-IF-004) | 4 | 4 | 4 |
| Constraint (REQ-CN-001 – REQ-CN-003) | 3 | 3 | 3 |
| **Total** | **18** | **25** | **26** |

### Requirement → Test Case Traceability Index

| Requirement | Test Case(s) |
|-------------|---------------|
| REQ-001 | ATP-001-A |
| REQ-002 | ATP-002-A, ATP-002-B, ATP-002-C, ATP-002-D |
| REQ-003 | ATP-003-A |
| REQ-004 | ATP-004-A, ATP-004-B |
| REQ-005 | ATP-005-A |
| REQ-006 | ATP-006-A, ATP-006-B |
| REQ-007 | ATP-002-C, ATP-007-A |
| REQ-008 | ATP-002-D, ATP-008-A |
| REQ-009 | ATP-009-A |
| REQ-010 | ATP-010-A, ATP-010-B |
| REQ-NF-001 | ATP-NF-001-A, ATP-NF-001-B |
| REQ-IF-001 | ATP-IF-001-A |
| REQ-IF-002 | ATP-IF-002-A |
| REQ-IF-003 | ATP-IF-003-A |
| REQ-IF-004 | ATP-IF-004-A |
| REQ-CN-001 | ATP-CN-001-A |
| REQ-CN-002 | ATP-CN-002-A |
| REQ-CN-003 | ATP-CN-003-A |

### Success Criteria → Acceptance Coverage

| Success Criterion | Covered By |
|--------------------|------------|
| SC-001 (100% of turn signal activations while engaged disengage within one control cycle) | ATP-002-A, ATP-002-B, ATP-004-A, ATP-NF-001-A, ATP-NF-001-B |
| SC-002 (0% of turn signal activations while not engaged produce any state change) | ATP-005-A |
| SC-003 (every disengagement produces exactly one auditable, reconstructable event record) | ATP-010-A, ATP-010-B |
| SC-004 (no perceptible delay — formalized as the 100 ms bound in REQ-NF-001) | ATP-NF-001-A, ATP-NF-001-B |

### ISO 26262 Overlay Coverage

| Test Category | Count | Applicable REQs |
|---------------|-------|-----------------|
| Standard functional test cases | 20 | All |
| Negative / boundary tests | 2 | REQ-005, REQ-CN-002 (inspection) |
| Race-condition / brief-tap tests | 2 | REQ-002, REQ-008 |
| Reused-mechanism confirmation tests (do not re-verify upstream) | 6 | REQ-006, REQ-009, REQ-010, REQ-IF-002, REQ-IF-003, REQ-IF-004 |
| Temporal constraint tests | 2 | REQ-NF-001 |

### Open Items

| Flag | ATP | Pending Resolution |
|------|-----|--------------------|
| None | — | No open questions remain; all requirements trace to `spec.md` and inherit resolved values (100 ms bound, `Cruise_Cancelled` target state) from `specs/001-cruise-brake-override/` |

**Note on validation tooling**: No `validate-requirement-coverage.sh` or
equivalent deterministic script exists in this repository (verified — searched
the full repo tree). Coverage above was verified manually by direct
cross-reference between this document and `requirements.md`. If such a script
is added in a later step, it should be run against this document and any gap
it surfaces should be resolved by revising this file rather than deferring to
implementation.
