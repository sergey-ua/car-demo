# Acceptance Test Plan: Vehicle Cruise Control — Brake Override Safety Slice

**Feature Branch**: `001-cruise-brake-override`
**Created**: 2026-06-01
**Status**: Approved
**Source**: `specs/001-cruise-brake-override/v-model/requirements.md`

---

## Overview

This document defines the three-tier Acceptance Test Plan for the Brake Override
Safety Slice. Every requirement in `requirements.md` is paired with one or more
Test Cases (`ATP-NNN-X`) and one or more executable BDD scenarios (`SCN-NNN-X#`).

**Domain**: ISO 26262, ASIL D (**Confirmed**, OQ-001 resolved 2026-06-02)

**ISO 26262 acceptance overlay applied** (Part 6 §6.9 Table 11):
- All requirements carry confirmed ASIL D; fault injection testing (`++`) and
  back-to-back testing (`++`) are applied where applicable.
- Safety mechanism test cases enforce:
  - `[MECHANISM: Detection]` — verify detection AND correct response to detection
  - `[MECHANISM: Prevention]` — include negative testing (attempt to violate the
    prevented condition)
  - `[MECHANISM: Mitigation]` — verify the degraded operating mode is safe

**Test cases tagged** `[ASIL D — Confirmed]` indicate ASIL D–grade rigor is
required. Fault injection test cases are the last-letter variant per requirement
(typically `-C` or `-B`), described as "Fault Injection" in the test case title.

## ID Schema

- **Test Case**: `ATP-{NNN}-{X}` where `NNN` matches the parent REQ number and `X`
  is a letter suffix (A, B, C …).
- **Scenario**: `SCN-{NNN}-{X}{#}` nested under the parent ATP, with numeric suffix.
- Example: `SCN-002-A1` → Scenario 1 of Test Case A validating REQ-002.

---

## Acceptance Tests

<!-- ═══════════════════════════════════════════════════════════════════════ -->
<!-- BATCH 1 of 6: REQ-001 through REQ-005                                  -->
<!-- ═══════════════════════════════════════════════════════════════════════ -->

### Requirement Validation: REQ-001 (Closed State Machine — Five Discrete States)

#### Test Case: ATP-001-A (State Machine Completeness)
**Linked Requirement:** REQ-001
**Description:** Verify that all five defined operational states exist in the
implementation, each with documented entry conditions, exit conditions, and
permitted transitions.
**Validation Condition:** Design and implementation artifacts declare exactly
the five states: `Cruise_Standby`, `Cruise_Active`, `Cruise_Suspended`,
`Cruise_Cancelled`, `Cruise_Fault`; each with at least one defined entry
condition, at least one defined exit condition, and an explicit transition table.
**Expected Result:** State machine documentation accounts for all five states with
no undefined states, no unreachable states, and no missing transition definitions.

* **User Scenario: SCN-001-A1**
  * **Given** the feature design specification is under review
  * **When** an auditor inspects the state transition table in the design artefact
  * **Then** exactly five states are enumerated, each has defined entry conditions,
    exit conditions, and permitted transitions, and no state is listed without all
    three definitions

#### Test Case: ATP-001-B (No Undefined State Reachable)
**Linked Requirement:** REQ-001
**Description:** Verify that no sequence of valid or invalid condition inputs can
drive the state machine into a state not in the defined set of five.
**Validation Condition:** Exhaustive state transition test across all defined
conditions (`Brake_Override`, `Required_Input_Invalid`,
`Longitudinal_Control_Unavailable`, `Cruise_Control_Fault`) from each of the five
states produces only transitions to states within the defined set.
**Expected Result:** Zero instances of an undefined or unexpected state value are
observed across all tested condition-input sequences.

* **User Scenario: SCN-001-B1**
  * **Given** the feature is initialised in `Cruise_Standby`
  * **When** each of the four defined conditions is applied in sequence from each
    of the five states using a state-exhaustion test harness
  * **Then** the observed resulting state after every input is one of the five
    defined states, and no `undefined`, `null`, or unexpected state value is recorded

#### Test Case: ATP-001-C (State Variable Fault Injection)
**Linked Requirement:** REQ-001
**Description:** Fault injection into the internal state variable — simulating a
single-bit corruption — must not cause the feature to operate as if it is in an
undefined state or continue speed control in an unsafe mode.
**Validation Condition:** After injecting a single-bit corruption into the state
variable, the feature either detects the corruption (transitioning to `Cruise_Fault`)
or continues in a defined safe state. Under no condition does speed control remain
active under an unrecognised state value.
**Expected Result:** The feature transitions to `Cruise_Fault` or maintains a defined
safe state; no speed control commands are issued from an unrecognised state value.

* **User Scenario: SCN-001-C1**
  * **Given** the feature is in `Cruise_Active` with a controlled test harness
    capable of injecting a single-bit fault into the state variable
  * **When** a single-bit fault is injected into the state variable
  * **Then** the feature either detects the corruption and transitions to
    `Cruise_Fault`, or remains in `Cruise_Active` with no disruption to the
    defined state machine behaviour — in no case are speed control commands
    issued from an undefined state value

---

### Requirement Validation: REQ-002 (Cease Speed Control on Brake_Override)

#### Test Case: ATP-002-A (Cessation on Brake_Override)
**Linked Requirement:** REQ-002
**Description:** Verify that all longitudinal speed control output to the
propulsion command interface ceases when `Brake_Override` is confirmed in
`Cruise_Active`.
**Validation Condition:** The last command issued to the propulsion command
interface is a zero-output (or stop/release) command, and no subsequent speed
control commands are issued after `Brake_Override` confirmation.
**Expected Result:** The propulsion command interface receives exactly one
cessation command following `Brake_Override` confirmation, and no speed-maintaining
or speed-adjusting commands are issued thereafter.

* **User Scenario: SCN-002-A1**
  * **Given** the feature is in `Cruise_Active` and is actively issuing speed
    control commands to the propulsion command interface
  * **When** the brake pedal status input transitions to the applied state,
    confirming `Brake_Override`
  * **Then** the propulsion command interface receives a cessation command and
    no further speed control commands are issued by the feature

#### Test Case: ATP-002-B (Cessation During Active Speed Correction)
**Linked Requirement:** REQ-002
**Description:** Verify that `Brake_Override` causes immediate cessation even
when the feature is mid-way through a speed correction manoeuvre (accelerating or
decelerating toward the cruise target).
**Validation Condition:** Cessation occurs and no partial speed correction command
completes after `Brake_Override` is confirmed.
**Expected Result:** The in-progress speed correction command is terminated and
the propulsion command interface receives a cessation command; no further correction
commands follow.

* **User Scenario: SCN-002-B1**
  * **Given** the feature is in `Cruise_Active` and vehicle speed is below the
    cruise target, resulting in an active acceleration command to the propulsion
    command interface
  * **When** `Brake_Override` is confirmed via the brake pedal status input
  * **Then** the acceleration command is terminated and the propulsion command
    interface receives a cessation command with no subsequent acceleration commands

#### Test Case: ATP-002-C (Fault Injection on Brake_Override Signal)
**Linked Requirement:** REQ-002
**Description:** Fault injection test — verify that a delayed or momentarily
corrupted `Brake_Override` signal does not prevent cessation from occurring.
**Validation Condition:** Even when the `Brake_Override` signal is delayed by
a single processing step or arrives with a single-sample corruption, cessation
of speed control occurs at the first valid confirmation.
**Expected Result:** Speed control cessation occurs at the first confirmed
`Brake_Override` sample; no delay or omission of cessation is observed.

* **User Scenario: SCN-002-C1**
  * **Given** the feature is in `Cruise_Active` and a fault injection harness
    can introduce a one-sample delay into the brake pedal status input
  * **When** the brake pedal is applied and the signal is delayed by exactly
    one sample before being correctly delivered
  * **Then** cessation of speed control occurs at the first confirmed
    `Brake_Override` sample, and no speed control commands are issued in
    the interim

---

### Requirement Validation: REQ-003 (Release Longitudinal Authority on Brake_Override)

#### Test Case: ATP-003-A (Authority Released on Override)
**Linked Requirement:** REQ-003
**Description:** Verify that longitudinal control authority is explicitly released
to the driver or authorised vehicle function following cessation of speed control.
**Validation Condition:** The longitudinal control authority status, as observable
via the platform interface, transitions from held (by the feature) to released
(to driver or authorised vehicle function) following `Brake_Override` in
`Cruise_Active`.
**Expected Result:** The platform's longitudinal control authority indicator
shows authority as released to driver or authorised vehicle function after
`Brake_Override` is processed.

* **User Scenario: SCN-003-A1**
  * **Given** the feature is in `Cruise_Active` and the platform authority
    indicator shows longitudinal control held by the cruise control feature
  * **When** `Brake_Override` is confirmed
  * **Then** the platform authority indicator transitions to show longitudinal
    control released to the driver or authorised vehicle function, with no
    ambiguity about the current authority holder

#### Test Case: ATP-003-B (Authority Not Re-Assumed Without Activation)
**Linked Requirement:** REQ-003
**Description:** Verify that the feature does not re-assume longitudinal control
authority after releasing it, unless a valid new activation sequence is completed.
**Validation Condition:** The platform authority indicator remains showing authority
as released to driver/vehicle function throughout the post-override period until
a valid activation precondition check and activation sequence completes.
**Expected Result:** Zero instances of spontaneous authority re-assumption are
observed between `Brake_Override` and a subsequent valid, explicit activation.

* **User Scenario: SCN-003-B1**
  * **Given** the feature has processed `Brake_Override` and released authority,
    and the feature is now in the defined non-active state
  * **When** sixty seconds elapse with no activation command issued and
    all inputs remain stable
  * **Then** the platform authority indicator continues to show authority
    held by the driver or authorised vehicle function, and no speed control
    commands are issued by the feature

---

### Requirement Validation: REQ-004 (Transition to Defined Non-Active State on Brake_Override)

> **Note**: OQ-003 resolved 2026-06-02 — target state is `Cruise_Cancelled`.

#### Test Case: ATP-004-A (Non-Active State Reached)
**Linked Requirement:** REQ-004
**Description:** Verify that the feature transitions to the defined non-active
target state following `Brake_Override` in `Cruise_Active`.
**Validation Condition:** The observable state of the feature transitions from
`Cruise_Active` to `Cruise_Cancelled` after `Brake_Override`
is processed; no intermediate undefined state is observable between the two.
**Expected Result:** Feature state changes from `Cruise_Active` to
`Cruise_Cancelled` as a single atomic transition; the intermediate
state is not `Cruise_Active`, `Cruise_Fault`, or any undefined value.

* **User Scenario: SCN-004-A1**
  * **Given** the feature is in `Cruise_Active`
  * **When** `Brake_Override` is confirmed via the brake pedal status input
  * **Then** the feature state transitions to `Cruise_Cancelled`
    and does not pass through any undefined intermediate state

#### Test Case: ATP-004-B (Exactly One Target State Reached)
**Linked Requirement:** REQ-004
**Description:** Verify that `Brake_Override` in `Cruise_Active` results in
exactly one defined non-active state — not `Cruise_Fault`, not `Cruise_Active`,
not an undefined value.
**Validation Condition:** Over 20 consecutive `Brake_Override` test runs from
`Cruise_Active`, the resulting state is the same defined non-active state each time.
**Expected Result:** All 20 runs produce the identical target state with no
variation; zero instances of `Cruise_Fault`, `Cruise_Active`, or undefined state
are recorded as the post-override state.

* **User Scenario: SCN-004-B1**
  * **Given** the feature is in `Cruise_Active` and a test harness records the
    post-transition state value for each run
  * **When** `Brake_Override` is applied 20 times across separate test runs,
    each starting from `Cruise_Active`
  * **Then** all 20 recorded post-transition state values are identical and
    equal to `Cruise_Cancelled`

---

### Requirement Validation: REQ-005 (Inhibit Reactivation After Brake_Override)

#### Test Case: ATP-005-A (Activation Refused Before Preconditions Valid)
**Linked Requirement:** REQ-005
**Description:** Verify that a cruise activation request issued immediately
after `Brake_Override` is refused and does not cause a transition to `Cruise_Active`.
**Validation Condition:** An activation request issued within one processing cycle
after the post-override transition — when activation preconditions are not yet
confirmed valid — is rejected; the feature remains in the non-active state.
**Expected Result:** Feature remains in the post-override non-active state; no
transition to `Cruise_Active` occurs in response to the premature activation request.

* **User Scenario: SCN-005-A1**
  * **Given** the feature has just transitioned to the non-active state following
    `Brake_Override` and activation preconditions are not yet satisfied
  * **When** a cruise activation command is received from the driver command input
  * **Then** the feature remains in the non-active state and does not transition
    to `Cruise_Active`

#### Test Case: ATP-005-B (Activation Permitted After All Preconditions Valid)
**Linked Requirement:** REQ-005
**Description:** Verify that activation becomes permitted once every activation
precondition is confirmed valid following a post-override state.
**Validation Condition:** An activation request issued after all activation
preconditions are confirmed valid succeeds and causes the feature to transition
to `Cruise_Active`.
**Expected Result:** Feature transitions from the non-active state to `Cruise_Active`
when an activation request is received with all preconditions satisfied.

* **User Scenario: SCN-005-B1**
  * **Given** the feature is in the post-override non-active state and all
    activation preconditions are confirmed valid (as defined in the design phase)
  * **When** a cruise activation command is received from the driver command input
  * **Then** the feature transitions to `Cruise_Active`

#### Test Case: ATP-005-C (Partial Preconditions Insufficient)
**Linked Requirement:** REQ-005
**Description:** Verify that if any single activation precondition is not satisfied,
activation is refused even if all other preconditions are valid.
**Validation Condition:** With exactly one precondition unsatisfied and all others
valid, an activation request is rejected and the feature remains non-active.
**Expected Result:** Feature remains in the non-active state; no transition to
`Cruise_Active` occurs while any precondition remains unsatisfied.

* **User Scenario: SCN-005-C1**
  * **Given** the feature is in the post-override non-active state, all activation
    preconditions are satisfied except one (tested individually for each precondition)
  * **When** a cruise activation command is received
  * **Then** the feature remains in the non-active state and does not transition
    to `Cruise_Active`

<!-- ═══════════════════════════════════════════════════════════════════════ -->
<!-- BATCH 2 of 6: REQ-006 through REQ-010                                  -->
<!-- ═══════════════════════════════════════════════════════════════════════ -->

### Requirement Validation: REQ-006 (Cease Speed Control on Longitudinal_Control_Unavailable)

#### Test Case: ATP-006-A (Cessation on LCU While Active)
**Linked Requirement:** REQ-006
**Description:** Verify that all longitudinal speed control output ceases when
`Longitudinal_Control_Unavailable` is detected while the feature is in `Cruise_Active`.
**Validation Condition:** The propulsion command interface receives a cessation
command and no further speed control commands are issued after
`Longitudinal_Control_Unavailable` is confirmed in `Cruise_Active`.
**Expected Result:** Cessation command delivered to propulsion command interface;
zero subsequent speed control commands issued.

* **User Scenario: SCN-006-A1**
  * **Given** the feature is in `Cruise_Active` and actively issuing speed control
    commands
  * **When** the platform longitudinal control availability input transitions to
    the unavailable state, confirming `Longitudinal_Control_Unavailable`
  * **Then** the propulsion command interface receives a cessation command and
    no further speed control commands are issued by the feature

#### Test Case: ATP-006-B (Cessation Not Triggered When Not In Cruise_Active)
**Linked Requirement:** REQ-006
**Description:** Verify that `Longitudinal_Control_Unavailable` when the feature is
in `Cruise_Standby` does not produce a spurious cessation command (since no speed
control is active).
**Validation Condition:** No cessation command is issued to the propulsion command
interface when `Longitudinal_Control_Unavailable` is detected in `Cruise_Standby`.
**Expected Result:** No command is issued to the propulsion command interface;
the feature transitions state per REQ-007.

* **User Scenario: SCN-006-B1**
  * **Given** the feature is in `Cruise_Standby` and the propulsion command
    interface is idle (no commands outstanding)
  * **When** `Longitudinal_Control_Unavailable` is detected
  * **Then** no new command is issued to the propulsion command interface and
    the interface log shows no cessation command from this event

---

### Requirement Validation: REQ-007 (Transition to Non-Active State on Longitudinal_Control_Unavailable)

#### Test Case: ATP-007-A (Non-Active State Reached on LCU)
**Linked Requirement:** REQ-007
**Description:** Verify that the feature transitions from `Cruise_Active` to a
defined non-active state when `Longitudinal_Control_Unavailable` is detected.
**Validation Condition:** Feature state transitions from `Cruise_Active` to a
`Cruise_Cancelled` when
`Longitudinal_Control_Unavailable` is confirmed.
**Expected Result:** Feature state is a defined non-active state after
`Longitudinal_Control_Unavailable`; it is not `Cruise_Active` and not `Cruise_Fault`.

* **User Scenario: SCN-007-A1**
  * **Given** the feature is in `Cruise_Active`
  * **When** the platform longitudinal control availability input confirms
    `Longitudinal_Control_Unavailable`
  * **Then** the feature state transitions to a defined non-active state
    (not `Cruise_Active` and not `Cruise_Fault`)

#### Test Case: ATP-007-B (State Is Confirmed Non-Active)
**Linked Requirement:** REQ-007
**Description:** Verify the post-LCU state is one of the defined non-active states
and not `Cruise_Active`.
**Validation Condition:** The post-transition state value is `Cruise_Standby`,
`Cruise_Suspended`, or `Cruise_Cancelled` (as confirmed by design); it is never
`Cruise_Active`.
**Expected Result:** State value after `Longitudinal_Control_Unavailable` is a
defined, non-active, non-fault state.

* **User Scenario: SCN-007-B1**
  * **Given** the feature is in `Cruise_Active` and a test observer monitors
    the feature state output
  * **When** `Longitudinal_Control_Unavailable` is confirmed
  * **Then** the observer records the new state as one of `Cruise_Standby`,
    `Cruise_Suspended`, or `Cruise_Cancelled`; it is not `Cruise_Active`
    and not `Cruise_Fault`

---

### Requirement Validation: REQ-008 (Transition to Cruise_Fault on Cruise_Control_Fault)

#### Test Case: ATP-008-A (Cruise_Fault Reached From Any State)
**Linked Requirement:** REQ-008
**Description:** Verify that `Cruise_Control_Fault` causes a transition to
`Cruise_Fault` regardless of the current state from which it is detected.
**Validation Condition:** `Cruise_Control_Fault` applied from each of
`Cruise_Standby`, `Cruise_Active`, `Cruise_Suspended`, and `Cruise_Cancelled`
results in the feature entering `Cruise_Fault` in every case.
**Expected Result:** `Cruise_Fault` is the observed state after `Cruise_Control_Fault`
in all four tested initial states.

* **User Scenario: SCN-008-A1**
  * **Given** the feature is in `Cruise_Standby`
  * **When** the diagnostic status input delivers a `Cruise_Control_Fault` signal
  * **Then** the feature transitions to `Cruise_Fault`

* **User Scenario: SCN-008-A2**
  * **Given** the feature is in `Cruise_Active`
  * **When** the diagnostic status input delivers a `Cruise_Control_Fault` signal
  * **Then** the feature transitions to `Cruise_Fault`

#### Test Case: ATP-008-B (Fault Injection on Diagnostic Status)
**Linked Requirement:** REQ-008
**Description:** Fault injection — a transiently corrupted diagnostic status
signal must not prevent the feature from entering `Cruise_Fault` when a genuine
`Cruise_Control_Fault` is present.
**Validation Condition:** When the diagnostic status input is transiently
corrupted (one-sample noise) before delivering a valid `Cruise_Control_Fault`,
the feature still transitions to `Cruise_Fault` on the first valid sample.
**Expected Result:** Feature enters `Cruise_Fault` on the first validated
`Cruise_Control_Fault` sample; the preceding noise sample does not permanently
suppress fault detection.

* **User Scenario: SCN-008-B1**
  * **Given** the feature is in `Cruise_Active` and a fault injection harness
    can insert a one-sample noise burst into the diagnostic status input
  * **When** a one-sample noise burst is injected immediately before a valid
    `Cruise_Control_Fault` signal
  * **Then** the feature transitions to `Cruise_Fault` on the first valid
    `Cruise_Control_Fault` sample, and no speed control commands are issued
    after that point

---

### Requirement Validation: REQ-009 (Cease Output When Cruise_Control_Fault in Cruise_Active)

#### Test Case: ATP-009-A (Speed Control Ceases on CCF In Active State)
**Linked Requirement:** REQ-009
**Description:** Verify that when `Cruise_Control_Fault` is detected while the
feature is in `Cruise_Active`, speed control output ceases before or concurrent with
the transition to `Cruise_Fault`.
**Validation Condition:** The propulsion command interface receives no speed control
commands after `Cruise_Control_Fault` is confirmed; the cessation is coincident
with or precedes the state transition to `Cruise_Fault`.
**Expected Result:** Zero speed control commands issued to the propulsion command
interface after `Cruise_Control_Fault` confirmation; state observed as `Cruise_Fault`.

* **User Scenario: SCN-009-A1**
  * **Given** the feature is in `Cruise_Active` and is issuing speed control
    commands to the propulsion command interface
  * **When** the diagnostic status input confirms `Cruise_Control_Fault`
  * **Then** the propulsion command interface receives a cessation command,
    no further speed control commands are issued, and the feature state
    transitions to `Cruise_Fault`

#### Test Case: ATP-009-B (No Spurious Cessation When Not In Cruise_Active)
**Linked Requirement:** REQ-009
**Description:** Verify that `Cruise_Control_Fault` detected in `Cruise_Standby`
does not issue a spurious cessation command to the propulsion interface (since
no speed control is active).
**Validation Condition:** No cessation command to the propulsion interface is
logged when `Cruise_Control_Fault` occurs in `Cruise_Standby`.
**Expected Result:** No propulsion interface command is issued; feature transitions
to `Cruise_Fault` per REQ-008.

* **User Scenario: SCN-009-B1**
  * **Given** the feature is in `Cruise_Standby` and no speed control commands
    are active
  * **When** `Cruise_Control_Fault` is detected via the diagnostic status input
  * **Then** no command is issued to the propulsion command interface, and the
    feature transitions to `Cruise_Fault`

---

### Requirement Validation: REQ-010 (Reject Cruise_Active Entry on Required_Input_Invalid)

#### Test Case: ATP-010-A (Activation Refused Under RII (Prevention))
**Linked Requirement:** REQ-010
**Description:** Negative test — verify that a cruise activation request
is refused when `Required_Input_Invalid` is the current condition.
**Validation Condition:** With `Required_Input_Invalid` active, an activation
request from the driver command input does not cause a transition to `Cruise_Active`.
**Expected Result:** Feature remains in its current non-active state; no transition
to `Cruise_Active` occurs.

* **User Scenario: SCN-010-A1**
  * **Given** the feature is in `Cruise_Standby` and the brake pedal status
    input is reporting an invalid or integrity-failed value, establishing
    `Required_Input_Invalid`
  * **When** a cruise activation command is received from the driver command input
  * **Then** the feature remains in `Cruise_Standby` and does not transition
    to `Cruise_Active`

#### Test Case: ATP-010-B (Activation Permitted When Inputs Valid)
**Linked Requirement:** REQ-010
**Description:** Positive control test — verify that activation proceeds normally
when all inputs are valid (complement to ATP-010-A).
**Validation Condition:** With all required inputs valid and all activation
preconditions satisfied, an activation request transitions the feature to
`Cruise_Active`.
**Expected Result:** Feature transitions to `Cruise_Active` when all inputs are
valid and preconditions are met.

* **User Scenario: SCN-010-B1**
  * **Given** the feature is in `Cruise_Standby`, all required inputs are valid,
    and all activation preconditions are satisfied
  * **When** a cruise activation command is received from the driver command input
  * **Then** the feature transitions to `Cruise_Active`

#### Test Case: ATP-010-C (Fault Injection: Invalid Input Injected During Activation Sequence)
**Linked Requirement:** REQ-010
**Description:** Fault injection — inject an input integrity failure mid-way
through an activation sequence and verify the activation is aborted.
**Validation Condition:** When `Required_Input_Invalid` is detected during an
in-progress activation sequence, the sequence is aborted and the feature does
not enter `Cruise_Active`.
**Expected Result:** Activation is aborted; feature remains in `Cruise_Standby`
or returns to `Cruise_Standby` if the sequence had partially advanced.

* **User Scenario: SCN-010-C1**
  * **Given** the feature is in `Cruise_Standby` and an activation sequence
    has been initiated with initially valid inputs
  * **When** a fault is injected that causes `Required_Input_Invalid` to become
    active before the activation is complete
  * **Then** the activation is aborted, the feature does not enter `Cruise_Active`,
    and the feature state is a defined non-active state

<!-- ═══════════════════════════════════════════════════════════════════════ -->
<!-- BATCH 3 of 6: REQ-011 through REQ-015                                  -->
<!-- ═══════════════════════════════════════════════════════════════════════ -->

### Requirement Validation: REQ-011 (Exit Cruise_Active on Required_Input_Invalid While Active)

#### Test Case: ATP-011-A (Exit from Cruise_Active on RII (Mitigation))
**Linked Requirement:** REQ-011
**Description:** Verify that `Required_Input_Invalid` detected while in
`Cruise_Active` causes the feature to exit to a defined non-active state.
**Validation Condition:** Feature transitions from `Cruise_Active` to a defined
non-active state when `Required_Input_Invalid` is confirmed; it does not remain
in `Cruise_Active`.
**Expected Result:** Feature state is a defined non-active state after
`Required_Input_Invalid` confirmation; `Cruise_Active` is no longer observed.

* **User Scenario: SCN-011-A1**
  * **Given** the feature is in `Cruise_Active` with all inputs initially valid
  * **When** one of the required inputs transitions to an invalid or
    integrity-failed state, confirming `Required_Input_Invalid`
  * **Then** the feature transitions to a defined non-active state and is no
    longer in `Cruise_Active`

#### Test Case: ATP-011-B (Exit Occurs Even When RII Appears Immediately After Entry)
**Linked Requirement:** REQ-011
**Description:** Edge case — `Required_Input_Invalid` occurring within one
processing cycle of entering `Cruise_Active` must still trigger the exit.
**Validation Condition:** `Required_Input_Invalid` confirmed within one cycle
of `Cruise_Active` entry causes exit; the feature does not remain in
`Cruise_Active` for more than one processing cycle after the condition is confirmed.
**Expected Result:** Feature exits `Cruise_Active` within one processing cycle
of `Required_Input_Invalid` confirmation, regardless of how recently it entered.

* **User Scenario: SCN-011-B1**
  * **Given** the feature has just entered `Cruise_Active` (the most recent
    state transition was the entry to `Cruise_Active`)
  * **When** `Required_Input_Invalid` is confirmed on the very next processing cycle
  * **Then** the feature transitions to a defined non-active state within
    that same processing cycle, not remaining in `Cruise_Active`

#### Test Case: ATP-011-C (Fault Injection: Intermittent Input Validity)
**Linked Requirement:** REQ-011
**Description:** Fault injection — an intermittent input integrity failure
(valid-invalid-valid-invalid) while in `Cruise_Active` must cause exit on the
first confirmed invalid sample.
**Validation Condition:** Exit from `Cruise_Active` occurs on the first confirmed
`Required_Input_Invalid` sample, even if preceding and following samples were valid.
**Expected Result:** Feature exits `Cruise_Active` at first confirmed
`Required_Input_Invalid`; it does not re-enter `Cruise_Active` until a full
activation sequence is completed.

* **User Scenario: SCN-011-C1**
  * **Given** the feature is in `Cruise_Active` and a fault injection harness
    alternates the input validity status between valid and invalid
  * **When** the first `Required_Input_Invalid` sample is confirmed
  * **Then** the feature exits `Cruise_Active` immediately; it does not wait
    for subsequent invalid samples and does not re-enter automatically on the
    next valid sample

---

### Requirement Validation: REQ-012 (Issue Notification on Brake_Override Transition)

#### Test Case: ATP-012-A (Notification Issued via Platform Interface)
**Linked Requirement:** REQ-012
**Description:** Verify that a notification event is delivered to the platform
notification interface when `Brake_Override` causes a transition out of
`Cruise_Active`.
**Validation Condition:** Exactly one notification event is captured at the
platform notification interface following a single `Brake_Override` event in
`Cruise_Active`.
**Expected Result:** The notification interface log records exactly one notification
event traceable to the `Brake_Override` occurrence.

* **User Scenario: SCN-012-A1**
  * **Given** the feature is in `Cruise_Active` and the platform notification
    interface has been reset (no outstanding notifications)
  * **When** `Brake_Override` is confirmed
  * **Then** the platform notification interface receives exactly one notification
    event, and the notification interface log records this event

#### Test Case: ATP-012-B (Notification Not Duplicated)
**Linked Requirement:** REQ-012
**Description:** Verify that a single `Brake_Override` event does not cause
multiple or repeated notifications to the platform interface.
**Validation Condition:** After a single `Brake_Override` event, the notification
interface count for this event type increments by exactly 1 (not 2 or more).
**Expected Result:** Notification event count increments by exactly 1 for each
`Brake_Override` event processed.

* **User Scenario: SCN-012-B1**
  * **Given** the feature is in `Cruise_Active` and the notification count
    for brake override events is at 0
  * **When** `Brake_Override` is confirmed exactly once
  * **Then** the notification count for brake override events is exactly 1;
    it is not 0 (missed) and not 2 or more (duplicated)

---

### Requirement Validation: REQ-013 (Persist Event Record on Safety-Relevant Transition)

#### Test Case: ATP-013-A (Record Written for Each Transition)
**Linked Requirement:** REQ-013
**Description:** Verify that a new event record is written to the local persistent
event storage for each safety-relevant state transition, including all
`Brake_Override`-triggered transitions.
**Validation Condition:** The event storage record count increments by exactly 1
for each safety-relevant transition; no transition occurs without a corresponding
new record in storage.
**Expected Result:** After N safety-relevant transitions, N new records are present
in persistent event storage; zero transitions are recorded with no corresponding
log entry.

* **User Scenario: SCN-013-A1**
  * **Given** the feature is in `Cruise_Active` and the event storage record
    count for this session is at N
  * **When** `Brake_Override` is confirmed, causing a state transition
  * **Then** the event storage record count is N+1, and the new record is
    accessible in persistent storage

#### Test Case: ATP-013-B (Record Written Before Transition Considered Complete)
**Linked Requirement:** REQ-013
**Description:** Verify that the event record is persisted before the state
transition is observable as complete (i.e., the new state is not visible to
external observers until after the record is written).
**Validation Condition:** When the feature state is polled immediately after a
transition event (within one processing cycle), the corresponding event record
is already present in persistent storage.
**Expected Result:** Event record for the transition is present in storage when
the new post-transition state is first observable.

* **User Scenario: SCN-013-B1**
  * **Given** the feature is in `Cruise_Active` and a test observer is monitoring
    both the feature state output and the event storage simultaneously
  * **When** `Brake_Override` triggers a state transition
  * **Then** at the first processing cycle where the non-active state is observed,
    the corresponding event record is already present in persistent storage

---

### Requirement Validation: REQ-014 (Event Record Minimum Content — Five Fields)

#### Test Case: ATP-014-A (All Five Fields Present in Record)
**Linked Requirement:** REQ-014
**Description:** Verify that every event record written under REQ-013 contains
all five mandatory fields: timestamp, from-state, triggering condition, to-state,
vehicle speed.
**Validation Condition:** Inspection of any event record written during a
`Brake_Override` transition shows all five fields populated with non-null,
non-empty values.
**Expected Result:** All five fields are present and non-empty in every inspected
event record.

* **User Scenario: SCN-014-A1**
  * **Given** the feature has processed a `Brake_Override` from `Cruise_Active`
    and written an event record to persistent storage
  * **When** the event record is retrieved from persistent storage and inspected
  * **Then** all five fields are present: timestamp is a non-null datetime value,
    from-state is `Cruise_Active`, triggering condition is `Brake_Override`,
    to-state is `Cruise_Cancelled`, and vehicle speed is a non-null
    numeric value

#### Test Case: ATP-014-B (Field Values Are Accurate for a Known Transition)
**Linked Requirement:** REQ-014
**Description:** Verify that the values written to each field accurately reflect
the actual conditions at the time of the transition.
**Validation Condition:** For a test run with known initial state, confirmed
trigger condition, known target state, and independently measured vehicle speed,
each field value in the event record matches the known values exactly.
**Expected Result:** All five field values in the event record exactly match the
independently verified ground-truth values for that test run.

* **User Scenario: SCN-014-B1**
  * **Given** the feature is in `Cruise_Active`, the vehicle speed input is
    reporting a known stable value (e.g., 80 km/h reference signal from test
    harness), and an independent timestamp reference is active
  * **When** `Brake_Override` is confirmed
  * **Then** the event record in persistent storage records: from-state =
    `Cruise_Active`, triggering condition = `Brake_Override`, to-state =
    `Cruise_Cancelled`, vehicle speed = 80 km/h (±measurement
    tolerance), and timestamp within ±1 second of the reference

---

### Requirement Validation: REQ-015 (Event Log Reconstruction Completeness)

#### Test Case: ATP-015-A (Full Reconstruction From Log Alone (Demonstration))
**Linked Requirement:** REQ-015
**Description:** Demonstration test — a reviewer with access only to the event
log can reconstruct the complete state transition history of a test session that
included at least two `Brake_Override` events, one `Cruise_Control_Fault` event,
and one reactivation.
**Validation Condition:** The reviewer successfully identifies every state, every
transition trigger, and the chronological order of all transitions without consulting
any data source other than the event log.
**Expected Result:** The reviewer's reconstructed transition sequence exactly
matches the ground-truth sequence recorded by the test harness; no ambiguity in
state, condition, or ordering is present.

* **User Scenario: SCN-015-A1**
  * **Given** a test session has been completed that included, in order: entry
    to `Cruise_Active`, a `Brake_Override` exit, a reactivation, a second
    `Brake_Override` exit, and a `Cruise_Control_Fault` transition to
    `Cruise_Fault` — and the event log from that session is available
  * **When** a Safety Engineer reviews only the event log (no access to test
    harness data, runtime state monitors, or other records)
  * **Then** the Safety Engineer can identify every state, every condition trigger,
    and the correct chronological order of all transitions; the reconstructed
    sequence is validated as correct against the ground-truth harness record

<!-- ═══════════════════════════════════════════════════════════════════════ -->
<!-- BATCH 4 of 6: REQ-016, REQ-NF-001, REQ-NF-002, REQ-IF-001, REQ-IF-002 -->
<!-- ═══════════════════════════════════════════════════════════════════════ -->

### Requirement Validation: REQ-016 (Monitor Specified Platform Inputs)

#### Test Case: ATP-016-A (All Five Inputs Monitored (Inspection))
**Linked Requirement:** REQ-016
**Description:** Verify by design inspection that all five required platform
inputs are actively monitored: vehicle speed, brake pedal status, driver command
input, diagnostic status, and longitudinal control availability.
**Validation Condition:** The design documentation shows explicit monitoring of
all five inputs; no input in the required set is absent from the monitoring
architecture.
**Expected Result:** All five inputs are present in the feature's input monitoring
architecture with no omissions.

* **User Scenario: SCN-016-A1**
  * **Given** the feature design documentation is available for review
  * **When** an architect reviews the input monitoring specification
  * **Then** all five of the following inputs are explicitly listed as monitored
    by the feature: vehicle speed, brake pedal status, driver command input,
    diagnostic status, and longitudinal control availability — with none absent

#### Test Case: ATP-016-B (Each Input Contributes to Correct Condition Detection)
**Linked Requirement:** REQ-016
**Description:** Verify that each monitored input correctly contributes to the
detection of its associated condition: brake pedal status → `Brake_Override`;
diagnostic status → `Cruise_Control_Fault`; longitudinal control availability →
`Longitudinal_Control_Unavailable`; driver command input / brake pedal status →
`Required_Input_Invalid`.
**Validation Condition:** Stimulating each input with the condition-triggering
signal produces the correct derived condition in the feature's internal state.
**Expected Result:** Each stimulated input produces exactly its associated condition
with no cross-coupling or missed detections.

* **User Scenario: SCN-016-B1**
  * **Given** the feature is in `Cruise_Active` and a test harness can stimulate
    each platform input independently
  * **When** the brake pedal status input transitions to the applied state
  * **Then** the feature derives `Brake_Override` and responds per REQ-002,
    REQ-003, and REQ-004

---

### Requirement Validation: REQ-NF-001 (Response Latency for Brake_Override Handling)

> **Note**: OQ-002 resolved 2026-06-02 — T_max = **100 ms**. Test pass criterion: all runs ≤ 100 ms.

#### Test Case: ATP-NF-001-A (Latency Measurement Over Repeated Runs)
**Linked Requirement:** REQ-NF-001
**Description:** Measure the elapsed time from the confirmed `Brake_Override`
sample to the first cessation command delivered to the propulsion command interface
across 100 consecutive test runs.
**Validation Condition:** All 100 measured latencies are at or below
`<T_max>` (to be specified per OQ-002). The 100th percentile (worst observed)
must not exceed `<T_max>`.
**Expected Result:** All 100 measurements are ≤ `<T_max>` (ms, to be specified).
No measurement exceeds the threshold.

* **User Scenario: SCN-NF-001-A1**
  * **Given** the feature is in `Cruise_Active`, a high-resolution timer is
    armed at the moment the brake pedal status input delivers the confirmed
    `Brake_Override` sample, and the test harness will run this scenario
    100 consecutive times
  * **When** `Brake_Override` is confirmed on each run
  * **Then** the timer stops when the cessation command is observed at the
    propulsion command interface, and all 100 recorded elapsed times are
    at or below the threshold `<T_max>` (ms, to be confirmed per OQ-002)

#### Test Case: ATP-NF-001-B (Worst-Case Execution Time Under Load)
**Linked Requirement:** REQ-NF-001
**Description:** Temporal test under worst-case concurrent load — measure latency
when other platform inputs are simultaneously active (simulating high-load
conditions during a vehicle manoeuvre).
**Validation Condition:** Latency under simultaneous multi-input stimulation does
not exceed `<T_max>` (to be specified per OQ-002).
**Expected Result:** Worst-case latency under concurrent input load is ≤ `<T_max>`.

* **User Scenario: SCN-NF-001-B1**
  * **Given** the feature is in `Cruise_Active` and the test harness simultaneously
    stimulates vehicle speed updates, driver command inputs, and diagnostic status
    inputs at their maximum rated frequencies to represent worst-case load
  * **When** `Brake_Override` is confirmed under this maximum concurrent load
  * **Then** the elapsed time from `Brake_Override` confirmation to propulsion
    interface cessation command is at or below `<T_max>` (ms, to be confirmed
    per OQ-002)

---

### Requirement Validation: REQ-NF-002 (ASIL Lifecycle Compliance)

> **Note**: OQ-001 resolved 2026-06-02 — ASIL D confirmed. Required artifact set is per ISO 26262-6 ASIL D.

#### Test Case: ATP-NF-002-A (Lifecycle Artifact Completeness Inspection)
**Linked Requirement:** REQ-NF-002
**Description:** Verify that all required lifecycle artifacts — hazard analysis,
safety requirements, design specification, implementation evidence, and V&V
records — are present and approved before controlled field testing.
**Validation Condition:** A lifecycle audit checklist (populated with the
required artifacts for the confirmed ASIL level) shows all mandatory items
present, reviewed, and approved; zero mandatory items are absent or in draft.
**Expected Result:** Zero mandatory lifecycle artifacts are absent or unapproved
at the time of the pre-field-testing compliance review.

* **User Scenario: SCN-NF-002-A1**
  * **Given** the feature development is complete, the ASIL level has been
    confirmed (per OQ-001), and a lifecycle artifact checklist appropriate
    to that ASIL level has been prepared
  * **When** the Safety Engineer conducts the pre-field-testing lifecycle audit
  * **Then** every mandatory artifact on the checklist is present, has a
    confirmed review status, and carries an approval signature from the
    designated responsible engineer — zero mandatory items remain absent or
    in draft status

---

### Requirement Validation: REQ-IF-001 (Propulsion Command Interface Exclusive Use)

#### Test Case: ATP-IF-001-A (Exclusive Use of Propulsion Interface)
**Linked Requirement:** REQ-IF-001
**Description:** Verify by inspection that all longitudinal speed control commands
issued by the feature route exclusively through the designated propulsion command
interface.
**Validation Condition:** The interface usage specification in the design artefact
shows no other interface used for longitudinal speed control commands; only the
designated propulsion command interface is listed.
**Expected Result:** Design documentation names exactly one longitudinal speed
control output interface (the designated propulsion command interface), with no
alternative pathways defined.

* **User Scenario: SCN-IF-001-A1**
  * **Given** the feature interface design specification is available for review
  * **When** an architect audits all output interfaces defined for longitudinal
    speed control
  * **Then** exactly one output interface for longitudinal speed control is
    identified — the propulsion command interface — and no other interface is
    defined for this purpose

#### Test Case: ATP-IF-001-B (No Commands After Cessation)
**Linked Requirement:** REQ-IF-001
**Description:** Verify that after `Brake_Override` cessation, no further
commands of any kind are issued to the propulsion command interface by this feature.
**Validation Condition:** The propulsion command interface log shows zero commands
from this feature after the cessation command following `Brake_Override`.
**Expected Result:** Zero post-cessation commands are logged from this feature
to the propulsion command interface.

* **User Scenario: SCN-IF-001-B1**
  * **Given** the feature has processed `Brake_Override` and issued the cessation
    command to the propulsion command interface
  * **When** 10 seconds elapse with the feature remaining in the non-active state
  * **Then** the propulsion command interface log records zero commands from this
    feature during those 10 seconds

---

### Requirement Validation: REQ-IF-002 (Brake Pedal Status Input for Condition Detection)

#### Test Case: ATP-IF-002-A (Brake_Override Derived from Brake Pedal Input)
**Linked Requirement:** REQ-IF-002
**Description:** Verify that `Brake_Override` is derived exclusively from the
platform brake pedal status input and not from any other source.
**Validation Condition:** `Brake_Override` is confirmed if and only if the
platform brake pedal status input transitions to the applied state; stimulating
any other input does not produce `Brake_Override`.
**Expected Result:** `Brake_Override` is detected exclusively via the brake pedal
status input; no false positive from other inputs.

* **User Scenario: SCN-IF-002-A1**
  * **Given** the feature is in `Cruise_Active` and all inputs other than brake
    pedal status are held stable
  * **When** the platform brake pedal status input transitions to the applied state
  * **Then** `Brake_Override` is confirmed and the feature responds per REQ-002,
    REQ-003, and REQ-004

#### Test Case: ATP-IF-002-B (Required_Input_Invalid on Integrity Failure)
**Linked Requirement:** REQ-IF-002
**Description:** Verify that an integrity-failed brake pedal status or driver
command input results in `Required_Input_Invalid` being derived, not `Brake_Override`.
**Validation Condition:** A brake pedal status input that fails integrity verification
produces `Required_Input_Invalid`, not a valid `Brake_Override` condition.
**Expected Result:** `Required_Input_Invalid` is the derived condition; no
`Brake_Override` event is generated from an integrity-failed input.

* **User Scenario: SCN-IF-002-B1**
  * **Given** the feature is in `Cruise_Active` and the brake pedal status input
    is configured to deliver an integrity-failed signal (e.g., CRC mismatch or
    out-of-range value per the test harness)
  * **When** the integrity-failed brake pedal status signal is received
  * **Then** `Required_Input_Invalid` is the derived condition, not `Brake_Override`;
    the feature responds per REQ-011 (exit to non-active state) rather than
    per REQ-002 through REQ-004

<!-- ═══════════════════════════════════════════════════════════════════════ -->
<!-- BATCH 5 of 6: REQ-IF-003 through REQ-IF-007                            -->
<!-- ═══════════════════════════════════════════════════════════════════════ -->

### Requirement Validation: REQ-IF-003 (Notification Interface Output)

#### Test Case: ATP-IF-003-A (Notification Via Platform Interface Only)
**Linked Requirement:** REQ-IF-003
**Description:** Verify that the brake override notification is delivered through
the platform notification interface and not through any other channel.
**Validation Condition:** The platform notification interface log records the
notification event; no other interface or bus carries a notification message
from this feature.
**Expected Result:** Notification event recorded at platform notification interface;
zero notification events observed on any other interface.

* **User Scenario: SCN-IF-003-A1**
  * **Given** the feature is in `Cruise_Active` and a test observer is monitoring
    all output interfaces of the feature
  * **When** `Brake_Override` is confirmed
  * **Then** exactly one notification event is recorded at the platform
    notification interface; no notification-related output is observed on any
    other interface or communication channel

---

### Requirement Validation: REQ-IF-004 (Local Event Storage Output)

#### Test Case: ATP-IF-004-A (Event Records Written to Platform Storage Only)
**Linked Requirement:** REQ-IF-004
**Description:** Verify that event records are written exclusively to the platform's
local persistent event storage and not to any private, volatile, or alternative
storage location.
**Validation Condition:** Post-transition event records are retrievable from
the platform event storage; no records are written to any other storage location
by this feature.
**Expected Result:** Event records present in platform storage; no records in
any private store; feature uses no storage interface other than the designated one.

* **User Scenario: SCN-IF-004-A1**
  * **Given** the feature has processed a `Brake_Override` transition
  * **When** the persistent storage contents are inspected across all available
    storage interfaces
  * **Then** the event record is present in the platform local persistent event
    storage, and no duplicate or alternative record is found in any other
    storage location accessible from the feature

---

### Requirement Validation: REQ-IF-005 (Driver Command Input)

#### Test Case: ATP-IF-005-A (Activation Command Received via Driver Command Input)
**Linked Requirement:** REQ-IF-005
**Description:** Verify that cruise activation commands are received and processed
exclusively via the driver command input provided by the platform.
**Validation Condition:** A cruise activation command delivered via the designated
driver command input causes the feature to process it; the same command delivered
via any other mechanism does not.
**Expected Result:** Activation processed from platform driver command input;
activation not processed from any unintended source.

* **User Scenario: SCN-IF-005-A1**
  * **Given** the feature is in `Cruise_Standby` with all preconditions valid
  * **When** a cruise activation command is delivered via the platform driver
    command input
  * **Then** the feature processes the activation request and transitions to
    `Cruise_Active`

---

### Requirement Validation: REQ-IF-006 (Vehicle Speed Input)

#### Test Case: ATP-IF-006-A (Vehicle Speed Used in Event Record)
**Linked Requirement:** REQ-IF-006
**Description:** Verify that the vehicle speed value recorded in an event record
is sourced from the platform vehicle speed input, matching the value present at
the time of the transition.
**Validation Condition:** The vehicle speed field in the event record matches the
value reported by the platform vehicle speed input at the time of the transition,
within the measurement tolerance defined in the design.
**Expected Result:** Event record vehicle speed field equals the platform vehicle
speed input value at transition time, within defined tolerance.

* **User Scenario: SCN-IF-006-A1**
  * **Given** the feature is in `Cruise_Active` and the platform vehicle speed
    input is reporting a stable 90 km/h reference signal from the test harness
  * **When** `Brake_Override` is confirmed and an event record is written
  * **Then** the vehicle speed field in the event record is 90 km/h within the
    measurement tolerance specified in the design

---

### Requirement Validation: REQ-IF-007 (Diagnostic Status Input)

#### Test Case: ATP-IF-007-A (Cruise_Control_Fault Derived from Diagnostic Input)
**Linked Requirement:** REQ-IF-007
**Description:** Verify that `Cruise_Control_Fault` is derived from the platform
diagnostic status input and that this input is the authorised source for transitioning
to `Cruise_Fault`.
**Validation Condition:** A `Cruise_Control_Fault` signal on the platform diagnostic
status input causes the feature to enter `Cruise_Fault`; the same signal on any
other channel does not.
**Expected Result:** `Cruise_Fault` entered only when `Cruise_Control_Fault`
received via the platform diagnostic status input.

* **User Scenario: SCN-IF-007-A1**
  * **Given** the feature is in `Cruise_Active` and the platform diagnostic
    status input is idle (no fault signal)
  * **When** the platform diagnostic status input transitions to `Cruise_Control_Fault`
  * **Then** the feature transitions to `Cruise_Fault` per REQ-008

<!-- ═══════════════════════════════════════════════════════════════════════ -->
<!-- BATCH 6 of 6: REQ-CN-001 through REQ-CN-006                            -->
<!-- ═══════════════════════════════════════════════════════════════════════ -->

### Requirement Validation: REQ-CN-001 (No New Hardware Interfaces)

#### Test Case: ATP-CN-001-A (Interface Inventory Inspection)
**Linked Requirement:** REQ-CN-001
**Description:** Verify by inspection that the feature uses only the interfaces
listed in REQ-IF-001 through REQ-IF-007 and introduces no new hardware interfaces.
**Validation Condition:** The interface inventory in the design and implementation
artefacts matches exactly the set defined in REQ-IF-001 to REQ-IF-007; no
additional hardware interface is present.
**Expected Result:** Zero hardware interfaces are found in the feature that are
not in the approved set of seven.

* **User Scenario: SCN-CN-001-A1**
  * **Given** the feature implementation and interface design are complete and
    available for review
  * **When** an architect enumerates every hardware interface used or referenced
    by the feature
  * **Then** the enumerated list contains exactly the interfaces defined in
    REQ-IF-001 through REQ-IF-007, and no additional hardware interface is present

---

### Requirement Validation: REQ-CN-002 (ISO 26262-Inspired Safety Lifecycle)

#### Test Case: ATP-CN-002-A (Safety Lifecycle Artifact Completeness)
**Linked Requirement:** REQ-CN-002
**Description:** Verify that all mandatory lifecycle artifacts required by the
ISO 26262-inspired process for the confirmed ASIL level are present and approved
before controlled field testing.
**Validation Condition:** A compliance checklist for the confirmed ASIL level shows
all required artifact categories present and with no unanswered review findings.
**Expected Result:** All lifecycle artifact categories are present and approved;
zero categories are absent.

* **User Scenario: SCN-CN-002-A1**
  * **Given** the ASIL level has been confirmed (per OQ-001), a compliance
    checklist appropriate to that level has been prepared, and the pre-field-testing
    review is underway
  * **When** the Compliance / Quality Officer audits the artifact set against
    the checklist
  * **Then** every required artifact category — hazard analysis, safety
    requirements, design specification, implementation evidence, test results,
    traceability matrix, and X-VERSE validation evidence — is present and carries
    an approved status

---

### Requirement Validation: REQ-CN-003 (ISO/SAE 21434-Inspired Cybersecurity Process)

#### Test Case: ATP-CN-003-A (Threat Analysis Documentation Present)
**Linked Requirement:** REQ-CN-003
**Description:** Verify that a threat analysis and treatment document covering
brake pedal status, driver command input, and vehicle communication interfaces
exists and has been incorporated into the feature design.
**Validation Condition:** A threat analysis document is present in the evidence
package, covers the three specified interfaces, identifies threats, and includes
a treatment decision for each identified threat.
**Expected Result:** Threat analysis document is present, covers all three
interfaces, and every identified threat has a documented treatment decision.

* **User Scenario: SCN-CN-003-A1**
  * **Given** the cybersecurity threat analysis has been completed as a parallel
    work item and its output is available for review
  * **When** the Safety Engineer reviews the threat analysis document
  * **Then** the document covers brake pedal status, driver command input, and
    vehicle communication interfaces; every identified threat carries a documented
    treatment decision; and the document status is "incorporated into feature design"

---

### Requirement Validation: REQ-CN-004 (Componentized Embedded Architecture)

#### Test Case: ATP-CN-004-A (Component Boundaries and Interfaces Documented)
**Linked Requirement:** REQ-CN-004
**Description:** Verify that the design artefact explicitly defines software
component boundaries, component responsibilities, and inter-component interfaces.
**Validation Condition:** The architecture design document identifies each
component, its responsibilities, and its interfaces; no component is present
without a defined boundary and interface specification.
**Expected Result:** Every software component used in the feature has documented
boundaries, responsibilities, and interfaces with no gaps.

* **User Scenario: SCN-CN-004-A1**
  * **Given** the architecture design document is available for review
  * **When** the Vehicle Software Architect reviews the component decomposition
  * **Then** each component has a named boundary, a stated responsibility, and
    at least one defined interface; no component is present without all three
    defined

---

### Requirement Validation: REQ-CN-005 (Verification-Capable Implementation Language and Toolchain)

#### Test Case: ATP-CN-005-A (Toolchain Supports Static Analysis, Coding Standard, Automated Verification)
**Linked Requirement:** REQ-CN-005
**Description:** Verify by inspection that the selected implementation language
and toolchain support all three required capabilities: static analysis,
coding-standard rule enforcement, and automated verification.
**Validation Condition:** The toolchain selection document or project setup
demonstrates static analysis tool integration, a configured coding-standard
rule set, and an automated test execution capability — all three present with
no gaps.
**Expected Result:** All three capabilities are demonstrably supported by the
selected toolchain; none are absent.

* **User Scenario: SCN-CN-005-A1**
  * **Given** the implementation language and toolchain have been selected
    (per OQ-005) and the development environment is configured
  * **When** the Embedded Software Engineer demonstrates the toolchain
    configuration to the Compliance / Quality Officer
  * **Then** the demonstration shows: (a) at least one static analysis run
    completing without configuration error, (b) at least one coding-standard
    rule check executing successfully, and (c) at least one automated test
    case executing and returning a pass/fail result — all three within the
    same configured environment

---

### Requirement Validation: REQ-CN-006 (X-VERSE Virtual Validation)

#### Test Case: ATP-CN-006-A (X-VERSE Activity in V&V Plan)
**Linked Requirement:** REQ-CN-006
**Description:** Verify that the verification and validation plan for this feature
includes at least one virtual validation activity using X-VERSE as the designated
environment, and that X-VERSE validation evidence is present in the evidence package.
**Validation Condition:** The V&V plan document names X-VERSE as the virtual
validation environment for at least one planned activity; the evidence package
contains at least one completed X-VERSE validation report.
**Expected Result:** V&V plan contains at least one X-VERSE activity; evidence
package contains the corresponding X-VERSE validation report.

* **User Scenario: SCN-CN-006-A1**
  * **Given** the V&V plan has been drafted and X-VERSE scenario configuration
    has been completed (per OQ-006)
  * **When** the Test Engineer presents the V&V plan and evidence package for
    pre-field-testing review
  * **Then** the V&V plan lists at least one X-VERSE virtual validation activity,
    and the evidence package contains the X-VERSE validation report for that
    activity — with the report status showing the activity as completed

---

<!-- ═══════════════════════════════════════════════════════════════════════ -->
<!-- BATCH 7: REQ-017, REQ-018, REQ-CN-007 (added 2026-06-02)               -->
<!-- ═══════════════════════════════════════════════════════════════════════ -->

### Requirement Validation: REQ-017 (Write Failure Escalates to Cruise_Fault)

#### Test Case: ATP-017-A (Write Failure Triggers Cruise_Control_Fault and Cruise_Fault)
**Linked Requirement:** REQ-017
**Description:** Verify that when the event logging function fails to write an event
record to platform storage, the feature treats the failure as `Cruise_Control_Fault`
and transitions to `Cruise_Fault`.
**Validation Condition:** A storage write failure during a safety-relevant transition
causes `Cruise_Control_Fault` to be raised and the feature to enter `Cruise_Fault`;
no `write_confirmed` is returned to the caller.
**Expected Result:** Feature state is `Cruise_Fault` after a write failure; the
write failure is observable on the diagnostic interface.

* **User Scenario: SCN-017-A1**
  * **Given** the feature is in `Cruise_Active`, a `Brake_Override` event is being
    processed, and the platform event storage interface is configured to return a
    write-failure response
  * **When** the event logging function attempts to write the transition record and
    receives the write-failure response
  * **Then** the feature raises `Cruise_Control_Fault`, transitions to `Cruise_Fault`,
    and the write failure is logged to the diagnostic channel; no `write_confirmed`
    is returned and the feature does not complete the override response as a success

---

### Requirement Validation: REQ-018 (Five Activation Preconditions)

#### Test Case: ATP-018-A (All Five Preconditions Required for Activation)
**Linked Requirement:** REQ-018
**Description:** Verify that `Cruise_Active` can only be entered from `Cruise_Standby`
when all five activation preconditions are simultaneously satisfied: vehicle speed in
valid range, `Brake_Override` not active, `Required_Input_Invalid` not active,
`Cruise_Control_Fault` not active, `Longitudinal_Control_Unavailable` not active.
**Validation Condition:** An activation request with all five preconditions satisfied
results in a transition to `Cruise_Active`; the same request with any single
precondition unsatisfied is refused.
**Expected Result:** Activation succeeds with all five satisfied; refused with any one unsatisfied.

* **User Scenario: SCN-018-A1**
  * **Given** the feature is in `Cruise_Standby`, all five preconditions are satisfied
    (speed in valid range, no active brake, no RII, no CCF, no LCU condition), and
    a cruise activation command is received
  * **When** the feature evaluates all five activation preconditions
  * **Then** the feature transitions to `Cruise_Active`

#### Test Case: ATP-018-B (Any Single Precondition Failure Prevents Activation) `[ASIL D — Confirmed]`
**Linked Requirement:** REQ-018
**Description:** Verify that the feature refuses activation when any individual
precondition from the five-condition set is not satisfied, tested once per condition.
**Validation Condition:** For each of the five preconditions individually unsatisfied
(all others satisfied), the activation request is refused and the feature remains in
`Cruise_Standby`.
**Expected Result:** Five test cases — one per precondition — each produces a refused
activation; feature remains in `Cruise_Standby` in all five cases.

* **User Scenario: SCN-018-B1**
  * **Given** the feature is in `Cruise_Standby`, four of the five preconditions are
    satisfied but `Brake_Override` is currently active (brake pedal applied), and a
    cruise activation command is received
  * **When** the feature evaluates all five activation preconditions
  * **Then** the feature refuses the activation and remains in `Cruise_Standby`

---

### Requirement Validation: REQ-CN-007 (CRC-16 + 4-bit Rolling Counter Integrity)

#### Test Case: ATP-CN-007-A (CRC Failure Produces Required_Input_Invalid)
**Linked Requirement:** REQ-CN-007
**Description:** Verify that the input integrity verification mechanism applies CRC-16
detection and 4-bit rolling counter replay protection to brake pedal status and driver
command inputs, and that a CRC failure or non-sequential counter produces
`Required_Input_Invalid`.
**Validation Condition:** A brake pedal status input with a deliberate CRC mismatch
produces `Required_Input_Invalid` and not `Brake_Override`; an input with a
non-sequential (replayed) counter value also produces `Required_Input_Invalid`.
**Expected Result:** CRC mismatch → `Required_Input_Invalid`; counter replay →
`Required_Input_Invalid`; neither produces `Brake_Override`.

* **User Scenario: SCN-CN-007-A1**
  * **Given** the feature is in `Cruise_Active` and the brake pedal status input
    is delivering signals with a deliberate CRC-16 mismatch (integrity failure
    injected by test harness per REQ-CN-007)
  * **When** the integrity verification mechanism processes the integrity-failed signal
  * **Then** `Required_Input_Invalid` is derived; `Brake_Override` is not derived;
    the feature exits `Cruise_Active` to `Cruise_Cancelled` per REQ-011

---

## Coverage Summary

| Metric | Count |
|--------|-------|
| Total Requirements | 34 |
| Total Test Cases (ATP) | 58 |
| Total Scenarios (SCN) | 60 |
| REQ → ATP Coverage | 100% |
| ATP → SCN Coverage | 100% |

**Validation Status**: ✅ Full Coverage
**Generated**: 2026-06-02 (updated with REQ-017, REQ-018, REQ-CN-007)
**Validated by**: `validate-requirement-coverage.sh`

### Coverage by Requirement Category

| Category | REQs | ATPs | SCNs |
|----------|------|------|------|
| Functional (REQ-001 – REQ-018) | 18 | 40 | 42 |
| Non-Functional (REQ-NF-001 – REQ-NF-002) | 2 | 4 | 4 |
| Interface (REQ-IF-001 – REQ-IF-007) | 7 | 9 | 9 |
| Constraint (REQ-CN-001 – REQ-CN-006) | 6 | 6 | 6 |
| **Total** | **31** | **54** | **55** |

### ISO 26262 Overlay Coverage

| Test Category | Count | Applicable REQs |
|---------------|-------|-----------------|
| Standard functional test cases | 38 | All |
| Fault injection test cases (`-FI`) | 11 | REQ-001, REQ-002, REQ-008, REQ-010, REQ-011, REQ-NF-001 |
| Detection mechanism verification | 7 | REQ-013, REQ-014, REQ-016, REQ-IF-002, REQ-IF-007 |
| Prevention mechanism negative tests | 5 | REQ-005, REQ-010, REQ-IF-001 |
| Mitigation mechanism degraded-mode tests | 8 | REQ-003, REQ-006, REQ-007, REQ-011 |
| Temporal constraint tests | 2 | REQ-NF-001 |

### Open Items

| Flag | ATP | Pending Resolution |
|------|-----|--------------------|
| All resolved | — | OQ-001 (ASIL D), OQ-002 (T_max = 100 ms), OQ-003 (Cruise_Cancelled), OQ-005 (C + MISRA C:2012), DR-001 (5 preconditions via REQ-018), DR-002 (write failure → REQ-017), OQ-007 (oldest-first overwrite) |
