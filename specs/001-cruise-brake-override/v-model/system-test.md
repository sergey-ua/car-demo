# System Test Plan: Vehicle Cruise Control — Brake Override Safety Slice

**Feature Branch**: `001-cruise-brake-override`
**Created**: 2026-06-01
**Status**: Approved
**Source**: `specs/001-cruise-brake-override/v-model/system-design.md`
**Domain Overlay**: ISO 26262 (ASIL D — Confirmed)

---

## Overview

This plan verifies that all eight system components defined in `system-design.md`
behave as designed. System tests target the IEEE 1016 design views — they are
architectural verification activities, not user-journey validation. Every scenario
uses technical, component-oriented language.

**Test strategy by design view:**
- **Interface Contract Testing** → Interface View (external and internal interfaces)
- **Boundary Value Analysis** → Data Design View (field completeness, value ranges)
- **Equivalence Partitioning** → Data Design View (valid / invalid input classes)
- **Fault Injection** → Dependency View (failure propagation, degradation behaviour)

**ISO 26262 ASIL D additions** (overlay active):
- MC/DC structural coverage target: 100% per runtime component (SYS-001–007)
- Resource usage verification: WCET for SYS-002 (timing budget), stack depth for all runtime components
- Back-to-back testing: referenced for SYS-001, SYS-002, SYS-004 (highest risk components)

---

## ID Schema

- **System Test Case**: `STP-{NNN}-{X}` — NNN matches parent SYS, X is a letter suffix (A, B, C …)
- **System Test Scenario**: `STS-{NNN}-{X}{#}` — nested under parent STP with numeric suffix
- Example: `STS-002-A1` → Scenario 1 of Test Case A verifying SYS-002

---

## ISO 29119 Test Techniques Applied

| Technique | Test Cases | Design View Targeted |
|-----------|-----------|---------------------|
| Interface Contract Testing (External) | STP-003-A, STP-004-A, STP-006-A, STP-007-A | Interface View — external platform boundaries |
| Interface Contract Testing (Internal) | STP-001-A, STP-002-A, STP-003-B, STP-004-B, STP-005-A, STP-005-B, STP-007-B | Interface View — inter-component contracts |
| Boundary Value Analysis | STP-001-B, STP-003-C, STP-004-D, STP-006-B | Data Design View — field completeness, value ranges |
| Equivalence Partitioning | STP-004-C | Data Design View — valid / invalid input classes |
| Fault Injection | STP-001-C, STP-002-B, STP-002-C, STP-003-D, STP-005-C, STP-006-C | Dependency View — failure propagation |
| Inspection | STP-008-A, STP-008-B | Decomposition View — process and lifecycle obligations |

---

## System Tests

<!-- ══════════════════════════════════════════════════════════════════════ -->
<!-- SYS-001: Cruise State Machine                                          -->
<!-- ══════════════════════════════════════════════════════════════════════ -->

### SYS-001: Cruise State Machine

---

#### Test Case: STP-001-A (Condition Signal Contract from SYS-004)
**Technique**: Interface Contract Testing (Internal)
**Design View**: Interface View — internal interface SYS-004 → SYS-001
**Linked Component**: SYS-001
**Description**: Verify that SYS-001 correctly processes each of the four condition
signals delivered by SYS-004 and that the resulting state transition matches the
expected outcome for each condition.
**Validation Condition**: For each of the four conditions, the state transition
produced by SYS-001 after receiving the signal is the transition defined in the
state machine specification. No undefined or spurious transition occurs.
**Expected Result**: Each condition signal produces exactly its defined transition;
no condition produces an undefined resulting state.

* **System Scenario: STS-001-A1**
  * **Given** SYS-001 is in `Cruise_Active` and SYS-004 publishes a confirmed
    `Brake_Override` condition signal
  * **When** SYS-001 processes the condition signal on the next evaluation cycle
  * **Then** SYS-001 transitions away from `Cruise_Active` to the defined
    `Cruise_Cancelled` (OQ-003 resolved 2026-06-02); the resulting state value is not `Cruise_Active`
    and not an undefined value

* **System Scenario: STS-001-A2**
  * **Given** SYS-001 is in `Cruise_Standby` and SYS-004 publishes
    `Required_Input_Invalid`
  * **When** SYS-001 processes the condition signal
  * **Then** SYS-001 remains in `Cruise_Standby` and does not attempt a transition
    to `Cruise_Active`

---

#### Test Case: STP-001-B (State Machine Boundary Conditions)
**Technique**: Boundary Value Analysis
**Design View**: Data Design View — `CruiseControlState` value boundaries
**Linked Component**: SYS-001
**Description**: Verify that SYS-001 rejects any state value outside the five
defined valid states and that all five defined states are reachable.
**Validation Condition**: SYS-001 accepts exactly five valid state values and
rejects any value outside that set; all five states are entered during the
state exhaustion test.
**Expected Result**: State variable never holds a value outside
{`Cruise_Standby`, `Cruise_Active`, `Cruise_Suspended`, `Cruise_Cancelled`,
`Cruise_Fault`}; all five values are observed during the test suite.

* **System Scenario: STS-001-B1**
  * **Given** a state exhaustion harness drives SYS-001 through every defined
    condition from every defined state
  * **When** each valid condition is applied from each valid initial state
  * **Then** the resulting state after each transition is one of the five defined
    values; the harness records zero instances of an undefined or unexpected
    state value

* **System Scenario: STS-001-B2**
  * **Given** a fault injection harness injects a state value of `0xFF`
    (a value outside the five valid states) into SYS-001's state variable
  * **When** SYS-001 evaluates the state on its next cycle
  * **Then** SYS-001 detects the out-of-range value and transitions to
    `Cruise_Fault`; no speed control commands are issued in the cycle following
    the injection

---

#### Test Case: STP-001-C (Fault Injection — SYS-004 Signal Loss)
**Technique**: Fault Injection
**Design View**: Dependency View — SYS-001 depends on SYS-004 for condition signals
**Linked Component**: SYS-001
**Description**: Verify that SYS-001 behaves safely if SYS-004 stops delivering
condition signals (component failure or communication loss).
**Validation Condition**: SYS-001 does not remain in `Cruise_Active` indefinitely
when condition signals are absent for more than one monitoring cycle; it must either
hold in the current safe state or transition to a defined safe state.
**Expected Result**: SYS-001 does not enter an undefined state; if in `Cruise_Active`
when condition signals cease, it transitions to a defined non-active state within
the fault reaction time window.

* **System Scenario: STS-001-C1**
  * **Given** SYS-001 is in `Cruise_Active` and SYS-004 is delivering condition
    signals nominally
  * **When** SYS-004 stops delivering signals for two consecutive monitoring cycles
    (simulated by severing the condition signal interface)
  * **Then** SYS-001 transitions to a defined non-active state (not `Cruise_Fault`
    unless diagnostic status also indicates a fault); it does not remain in
    `Cruise_Active` with no condition signal input

---

<!-- ══════════════════════════════════════════════════════════════════════ -->
<!-- SYS-002: Brake Override Response Coordinator                           -->
<!-- ══════════════════════════════════════════════════════════════════════ -->

### SYS-002: Brake Override Response Coordinator

---

#### Test Case: STP-002-A (Override Response Sequence Contract)
**Technique**: Interface Contract Testing (Internal)
**Design View**: Interface View — internal interfaces SYS-002 → SYS-003, SYS-006, SYS-007
**Linked Component**: SYS-002
**Description**: Verify that SYS-002, upon receiving a confirmed `Brake_Override`
event from SYS-004, issues all three downstream commands in the required order:
(1) cessation command to SYS-003, (2) log event request to SYS-006,
(3) notification request to SYS-007.
**Validation Condition**: All three commands are issued; command 1 (cessation)
precedes command 3 (notification) in the execution trace; command 2 (log) is
confirmed written before the response sequence is marked complete.
**Expected Result**: Execution trace confirms three distinct inter-component
commands in correct order; the total elapsed time from event receipt to sequence
completion is within the timing budget (once OQ-002 is resolved).

* **System Scenario: STS-002-A1**
  * **Given** SYS-001 is in `Cruise_Active` and a test harness monitors all
    inter-component message exchanges on SYS-002's output interfaces
  * **When** SYS-004 delivers a confirmed `Brake_Override` event to SYS-002
  * **Then** the harness records, in sequence: (1) a cessation command to SYS-003,
    (2) a log event request to SYS-006 with acknowledgement returned before
    sequence completion, (3) a notification request to SYS-007 — with zero
    omissions and zero out-of-order deliveries

---

#### Test Case: STP-002-B (Fault Injection — SYS-003 Cessation Command Failure)
**Technique**: Fault Injection
**Design View**: Dependency View — SYS-002 depends on SYS-003 to execute cessation
**Linked Component**: SYS-002
**Description**: Verify SYS-002's behaviour when SYS-003 fails to acknowledge
a cessation command, simulating a longitudinal controller failure.
**Validation Condition**: SYS-002 detects the failure and escalates to
`Cruise_Control_Fault` (triggering SYS-001 to enter `Cruise_Fault`); it does not
silently complete the response sequence with an unacknowledged cessation.
**Expected Result**: SYS-002 escalates the unacknowledged cessation to a fault
condition; `Cruise_Fault` is entered; the failure is recorded in the event log.

* **System Scenario: STS-002-B1**
  * **Given** SYS-001 is in `Cruise_Active` and a fault injection harness suppresses
    SYS-003's acknowledgement of cessation commands (SYS-003 is unresponsive)
  * **When** SYS-002 issues a cessation command to SYS-003 in response to
    `Brake_Override`
  * **Then** SYS-002 detects the absence of acknowledgement, raises
    `Cruise_Control_Fault`, SYS-001 transitions to `Cruise_Fault`, and SYS-006
    records a `Cruise_Control_Fault` event with the escalation context

---

#### Test Case: STP-002-C (Fault Injection — SYS-006 Write Failure)
**Technique**: Fault Injection
**Design View**: Dependency View — SYS-002 depends on SYS-006 for event persistence
**Linked Component**: SYS-002
**Description**: Verify that SYS-002 handles a SYS-006 write failure without
silently completing the override response; the safety response (cessation and
authority release) must proceed regardless.
**Validation Condition**: Cessation and authority release proceed normally even
when SYS-006 reports a write failure; SYS-002 records the write failure as a
diagnostic event; DR-002 resolution determines whether `Cruise_Control_Fault`
is raised.
**Expected Result**: Cessation command is delivered to SYS-003 regardless of
SYS-006 outcome; write failure is observable via diagnostic interface.

* **System Scenario: STS-002-C1**
  * **Given** SYS-001 is in `Cruise_Active` and a fault injection harness causes
    SYS-006 to return a write-failure status on any write attempt
  * **When** SYS-002 processes a `Brake_Override` event and requests SYS-006 to
    persist the event record
  * **Then** SYS-003 still receives and executes the cessation command; SYS-007
    still receives and issues the notification; the write failure is logged to
    the diagnostic channel, not silently discarded

---

<!-- ══════════════════════════════════════════════════════════════════════ -->
<!-- SYS-003: Longitudinal Speed Controller                                 -->
<!-- ══════════════════════════════════════════════════════════════════════ -->

### SYS-003: Longitudinal Speed Controller

---

#### Test Case: STP-003-A (External Interface — Propulsion Command Exclusivity)
**Technique**: Interface Contract Testing (External)
**Design View**: Interface View — external interface SYS-003 → propulsion command I/F
**Linked Component**: SYS-003
**Description**: Verify that SYS-003 issues speed control commands exclusively to
the designated propulsion command interface and that no other interface carries
speed control commands from this component.
**Validation Condition**: During a full activation and override cycle, a bus monitor
records speed control commands only on the propulsion command interface; zero speed
control commands are observed on any other interface.
**Expected Result**: Propulsion command interface carries all speed commands; all
other interfaces carry zero speed control commands from SYS-003.

* **System Scenario: STS-003-A1**
  * **Given** SYS-001 is in `Cruise_Active`, SYS-003 is actively issuing speed
    control commands, and a bus monitor is recording all inter-component and
    platform interface traffic
  * **When** the full cycle of speed control active → `Brake_Override` → cessation
    is executed
  * **Then** the bus monitor records speed control commands only on the propulsion
    command interface port; zero speed control commands appear on any other
    monitored interface

---

#### Test Case: STP-003-B (Internal Interface — Cessation from SYS-002)
**Technique**: Interface Contract Testing (Internal)
**Design View**: Interface View — internal interface SYS-002 → SYS-003 (cessation)
**Linked Component**: SYS-003
**Description**: Verify that SYS-003 immediately executes a cessation command
received from SYS-002, terminating any in-progress speed control command
within one control cycle.
**Validation Condition**: The propulsion command interface receives a cessation
command within one control cycle of SYS-003 receiving SYS-002's cessation request;
no in-progress command completes after the cessation request is received.
**Expected Result**: Cessation on propulsion interface observed within one control
cycle; command log shows no speed command after the cessation timestamp.

* **System Scenario: STS-003-B1**
  * **Given** SYS-003 is in the middle of issuing an acceleration command to the
    propulsion command interface and a high-resolution trace is recording
    all propulsion interface events
  * **When** SYS-002 delivers a cessation command to SYS-003
  * **Then** the propulsion interface trace shows a cessation command within
    one control cycle of the SYS-002 request; the in-progress acceleration
    command is not completed after the cessation point

---

#### Test Case: STP-003-C (Boundary Value Analysis — State-Gated Command Issuance)
**Technique**: Boundary Value Analysis
**Design View**: Data Design View — `CruiseControlState` boundary for SYS-003
**Linked Component**: SYS-003
**Description**: Verify that SYS-003 issues speed control commands if and only if
SYS-001 reports `Cruise_Active`; test all four non-active states as the boundary
condition on the authorization gate.
**Validation Condition**: Zero speed control commands are issued to the propulsion
interface when SYS-001 is in any state other than `Cruise_Active`; commands are
issued normally when SYS-001 reports `Cruise_Active`.
**Expected Result**: Command issuance is gated strictly on `Cruise_Active`; no
commands issued in any of the four other states.

* **System Scenario: STS-003-C1**
  * **Given** SYS-001 is in each of the four non-active states in turn
    (`Cruise_Standby`, `Cruise_Suspended`, `Cruise_Cancelled`, `Cruise_Fault`)
  * **When** SYS-003 evaluates its authorisation signal from SYS-001 on each cycle
  * **Then** SYS-003 issues zero speed control commands to the propulsion interface
    in each non-active state; no propulsion command is emitted

---

#### Test Case: STP-003-D (Fault Injection — Concurrent Cessation from SYS-002 and SYS-005)
**Technique**: Fault Injection
**Design View**: Dependency View — SYS-003 receives cessation from both SYS-002
and SYS-005; concurrent commands must not corrupt the cessation
**Linked Component**: SYS-003
**Description**: Verify that SYS-003 correctly handles simultaneous cessation
commands arriving from both SYS-002 (Brake_Override path) and SYS-005
(Longitudinal_Control_Unavailable path) within the same control cycle.
**Validation Condition**: Exactly one cessation command is delivered to the
propulsion interface; no double-command or command corruption occurs; no
undefined behaviour results from the concurrent delivery.
**Expected Result**: Single cessation command on propulsion interface; no
malformed or duplicated commands; SYS-003 state remains consistent.

* **System Scenario: STS-003-D1**
  * **Given** SYS-001 is in `Cruise_Active` and a test harness triggers
    `Brake_Override` (via SYS-002) and `Longitudinal_Control_Unavailable`
    (via SYS-005) in the same scheduling cycle
  * **When** both cessation commands arrive at SYS-003 simultaneously
  * **Then** SYS-003 delivers exactly one cessation command to the propulsion
    interface; no duplicate, malformed, or partial command is emitted;
    SYS-003's internal state is consistent after the cycle

---

<!-- ══════════════════════════════════════════════════════════════════════ -->
<!-- SYS-004: Platform Input Monitor                                        -->
<!-- ══════════════════════════════════════════════════════════════════════ -->

### SYS-004: Platform Input Monitor

---

#### Test Case: STP-004-A (External Interface — Brake Pedal Status Integrity Validation)
**Technique**: Interface Contract Testing (External)
**Design View**: Interface View — external interface: brake pedal status input → SYS-004
**Linked Component**: SYS-004
**Description**: Verify that SYS-004 correctly validates the integrity of the brake
pedal status input — deriving `Brake_Override` from a valid applied signal and
`Required_Input_Invalid` from an integrity-failed signal.
**Validation Condition**: A valid applied signal produces `Brake_Override`; an
integrity-failed signal (CRC mismatch or out-of-range value per design) produces
`Required_Input_Invalid`, not `Brake_Override`.
**Expected Result**: Correct condition derived for valid and integrity-failed
brake pedal inputs with no cross-derivation.

* **System Scenario: STS-004-A1**
  * **Given** SYS-004 is receiving nominal platform inputs and the brake pedal
    status input is in the released state
  * **When** the brake pedal status input transitions to the applied state with
    a valid integrity check value
  * **Then** SYS-004 derives and publishes `Brake_Override`; it does not
    derive `Required_Input_Invalid`

* **System Scenario: STS-004-A2**
  * **Given** SYS-004 is receiving nominal platform inputs
  * **When** the brake pedal status input delivers a signal that fails the
    integrity check (integrity failure injected by test harness per design)
  * **Then** SYS-004 derives and publishes `Required_Input_Invalid`; it does
    not derive `Brake_Override`; the integrity failure is logged to the
    diagnostic channel

---

#### Test Case: STP-004-B (Internal Interface — Condition Publication Contract)
**Technique**: Interface Contract Testing (Internal)
**Design View**: Interface View — internal interfaces SYS-004 → SYS-001, SYS-002, SYS-005
**Linked Component**: SYS-004
**Description**: Verify that SYS-004 publishes each derived condition to all
required consumers and that condition signals are published on-change, not
at a fixed polling interval.
**Validation Condition**: Each condition signal is received by every registered
consumer within one monitoring cycle of derivation; no consumer is skipped; no
stale condition value is published when the condition has not changed.
**Expected Result**: All consumers receive the condition signal within one cycle
of its derivation; no consumer misses a condition publication.

* **System Scenario: STS-004-B1**
  * **Given** a test harness monitors the condition signal interfaces of SYS-001,
    SYS-002, and SYS-005 simultaneously
  * **When** SYS-004 derives a confirmed `Brake_Override` condition
  * **Then** the harness records the condition received at SYS-001, SYS-002,
    and SYS-005 within one monitoring cycle; no consumer receives the signal
    more than one cycle after derivation

---

#### Test Case: STP-004-C (Equivalence Partitioning — Input Validity Classes)
**Technique**: Equivalence Partitioning
**Design View**: Data Design View — valid vs. invalid input classes for platform inputs
**Linked Component**: SYS-004
**Description**: Verify SYS-004's handling of three input equivalence classes:
(A) fully valid inputs — all fields within range and integrity checks pass;
(B) integrity-failed inputs — range valid but integrity check fails;
(C) out-of-range inputs — value outside defined physical range.
**Validation Condition**: Class A produces the correct derived condition; classes B
and C both produce `Required_Input_Invalid`; no class produces an undefined condition.
**Expected Result**: Three distinct outcomes mapped exactly: class A → correct
condition; class B → `Required_Input_Invalid`; class C → `Required_Input_Invalid`.

* **System Scenario: STS-004-C1**
  * **Given** a test harness can present brake pedal status inputs from each
    of the three equivalence classes to SYS-004
  * **When** a class A (valid) applied brake pedal signal is presented
  * **Then** SYS-004 derives `Brake_Override` with no invalid-input indication

* **System Scenario: STS-004-C2**
  * **Given** the same harness as STS-004-C1
  * **When** a class B (integrity-failed) brake pedal signal is presented
  * **Then** SYS-004 derives `Required_Input_Invalid`; it does not derive
    `Brake_Override`

---

#### Test Case: STP-004-D (Boundary Value Analysis — Input Range Boundaries)
**Technique**: Boundary Value Analysis
**Design View**: Data Design View — platform input value ranges
**Linked Component**: SYS-004
**Description**: Verify SYS-004's behaviour at the exact minimum, maximum,
minimum–1, and maximum+1 values for the vehicle speed input, which contributes
to `Required_Input_Invalid` detection via the plausibility check.
**Validation Condition**: Values at minimum and maximum produce a valid speed
reading; values one unit below minimum or above maximum produce `Required_Input_Invalid`.
**Expected Result**: Boundary-exact values accepted; out-of-boundary values
produce `Required_Input_Invalid`.

* **System Scenario: STS-004-D1**
  * **Given** SYS-004 is monitoring all platform inputs and the vehicle speed
    input supports a defined minimum valid value `V_min`
  * **When** the vehicle speed input delivers exactly `V_min`
  * **Then** SYS-004 accepts the value as valid; no `Required_Input_Invalid`
    is derived from this input

* **System Scenario: STS-004-D2**
  * **Given** the same monitoring context as STS-004-D1
  * **When** the vehicle speed input delivers `V_min − 1` (one unit below minimum)
  * **Then** SYS-004 derives `Required_Input_Invalid`; the out-of-range value
    is not forwarded to other components as a valid speed reading

---

<!-- ══════════════════════════════════════════════════════════════════════ -->
<!-- SYS-005: Fault and Condition Supervisor                                -->
<!-- ══════════════════════════════════════════════════════════════════════ -->

### SYS-005: Fault and Condition Supervisor

---

#### Test Case: STP-005-A (Internal Interface — LCU Triggers Cessation and Transition)
**Technique**: Interface Contract Testing (Internal)
**Design View**: Interface View — SYS-005 → SYS-003 (cessation), SYS-005 → SYS-001 (transition)
**Linked Component**: SYS-005
**Description**: Verify that SYS-005, on receiving a `Longitudinal_Control_Unavailable`
condition from SYS-004 while SYS-001 is in `Cruise_Active`, issues both a cessation
command to SYS-003 and a state transition request to SYS-001.
**Validation Condition**: Both commands are issued; the cessation command precedes
the state transition request in the execution trace; SYS-001 transitions to a
defined non-active state after receiving the request.
**Expected Result**: Cessation command on SYS-003 interface; transition request
on SYS-001 interface; SYS-001 exits `Cruise_Active`.

* **System Scenario: STS-005-A1**
  * **Given** SYS-001 is in `Cruise_Active`, SYS-003 is issuing speed control
    commands, and a test harness monitors the SYS-005 output interfaces
  * **When** SYS-004 publishes `Longitudinal_Control_Unavailable` to SYS-005
  * **Then** the harness records SYS-005 issuing a cessation command to SYS-003
    followed by a transition request to SYS-001; SYS-001's state transitions to
    a defined non-active state within one scheduling cycle

---

#### Test Case: STP-005-B (Internal Interface — CCF Triggers Cruise_Fault)
**Technique**: Interface Contract Testing (Internal)
**Design View**: Interface View — SYS-005 → SYS-001 (Cruise_Fault transition request)
**Linked Component**: SYS-005
**Description**: Verify that SYS-005 issues a `Cruise_Fault` state transition
request to SYS-001 when SYS-004 delivers `Cruise_Control_Fault`, regardless of
the current state.
**Validation Condition**: SYS-001 receives a `Cruise_Fault` transition request
from SYS-005 in response to `Cruise_Control_Fault` from any of the five valid
initial states; SYS-001 transitions to `Cruise_Fault` in every case.
**Expected Result**: `Cruise_Fault` transition request issued from SYS-005;
`Cruise_Fault` state observed in SYS-001 for all five initial states.

* **System Scenario: STS-005-B1**
  * **Given** SYS-001 is in each of its five valid states in turn and a harness
    monitors the SYS-005 → SYS-001 interface
  * **When** SYS-004 delivers `Cruise_Control_Fault` to SYS-005 from each initial
    state
  * **Then** SYS-005 issues a `Cruise_Fault` transition request to SYS-001 in
    every case; SYS-001 transitions to `Cruise_Fault` in every case

---

#### Test Case: STP-005-C (Fault Injection — SYS-001 Rejects Transition Request)
**Technique**: Fault Injection
**Design View**: Dependency View — SYS-005 depends on SYS-001 to accept transition requests
**Linked Component**: SYS-005
**Description**: Verify SYS-005's behaviour when SYS-001 rejects a transition
request (e.g., because SYS-001 is itself in a failed or locked state).
**Validation Condition**: SYS-005 escalates the rejection to `Cruise_Control_Fault`
if SYS-001 does not accept a mandatory safety transition within the fault reaction
window; the escalation is logged.
**Expected Result**: SYS-005 detects the rejection; `Cruise_Control_Fault` is raised;
SYS-006 records the escalation event.

* **System Scenario: STS-005-C1**
  * **Given** SYS-001 is in `Cruise_Active` and a fault injection harness causes
    SYS-001 to reject all transition requests (simulating a locked state)
  * **When** SYS-004 delivers `Longitudinal_Control_Unavailable` and SYS-005
    issues the transition request to SYS-001
  * **Then** SYS-005 detects the rejection; `Cruise_Control_Fault` is escalated;
    SYS-006 records a fault event noting the rejected transition

---

<!-- ══════════════════════════════════════════════════════════════════════ -->
<!-- SYS-006: Safety Event Logger                                           -->
<!-- ══════════════════════════════════════════════════════════════════════ -->

### SYS-006: Safety Event Logger

---

#### Test Case: STP-006-A (External Interface — Event Record Write to Platform Storage)
**Technique**: Interface Contract Testing (External)
**Design View**: Interface View — external interface SYS-006 → platform local persistent storage
**Linked Component**: SYS-006
**Description**: Verify that SYS-006 writes each event record exclusively to the
platform local persistent event storage and that the write completes with an
acknowledged result before SYS-006 signals write-complete to SYS-002 or SYS-005.
**Validation Condition**: Written record is present in platform storage after each
safety-relevant transition; SYS-006 issues write-complete acknowledgement only
after the platform confirms the write; zero records are written to any other storage location.
**Expected Result**: Record in platform storage; write-complete acknowledged after
platform confirmation; zero records on any other medium.

* **System Scenario: STS-006-A1**
  * **Given** a safety-relevant state transition (Brake_Override from `Cruise_Active`)
    has been triggered and SYS-006 has received a log event request from SYS-002
  * **When** SYS-006 writes the event record to the platform storage interface
  * **Then** the record is retrievable from platform storage; SYS-006's
    write-complete acknowledgement to SYS-002 is issued only after the platform
    confirms the write; no record appears in any other storage interface

---

#### Test Case: STP-006-B (Boundary Value Analysis — Five-Field Record Completeness)
**Technique**: Boundary Value Analysis
**Design View**: Data Design View — `StateTransitionEvent` minimum five-field content
**Linked Component**: SYS-006
**Description**: Verify that every event record written by SYS-006 contains all
five mandatory fields, and that no field is null, empty, or at an invalid boundary.
The five boundary conditions are: (1) zero-value timestamp (invalid), (2) maximum
timestamp value (valid boundary), (3) null from-state (invalid), (4) null
to-state (invalid), (5) zero vehicle speed (valid boundary — vehicle may be stopped).
**Validation Condition**: Records with all five fields populated at valid values
are accepted; any record with a null, empty, or invalid-boundary field is flagged
by SYS-006 as a format violation before writing.
**Expected Result**: Five-field records with valid values are written; records
with any invalid field trigger a format violation diagnostic, not a silent write.

* **System Scenario: STS-006-B1**
  * **Given** a test harness delivers a fully populated log event request to
    SYS-006 (all five fields: non-null timestamp, valid from-state, valid
    condition, valid to-state, non-null vehicle speed)
  * **When** SYS-006 writes the event record to platform storage
  * **Then** the written record contains exactly five populated fields with
    no null or empty values; the record is retrievable with all five fields intact

* **System Scenario: STS-006-B2**
  * **Given** a test harness delivers a log event request with a null from-state
    field (a boundary violation)
  * **When** SYS-006 processes the request
  * **Then** SYS-006 raises a format violation diagnostic; it does not write a
    partial record to platform storage

---

#### Test Case: STP-006-C (Fault Injection — Platform Storage Write Failure)
**Technique**: Fault Injection
**Design View**: Dependency View — SYS-006 depends on platform storage availability
**Linked Component**: SYS-006
**Description**: Verify SYS-006's response to a platform storage write failure.
Per DR-002 (unresolved derived requirement), the exact escalation path is TBD;
this test verifies that the failure is observable and that SYS-006 does not
silently complete the write.
**Validation Condition**: SYS-006 reports the write failure to the diagnostic
interface; it does not signal write-complete to SYS-002 or SYS-005 on a failed
write; the state transition is not silently completed as if logging succeeded.
**Expected Result**: Write failure observable on diagnostic interface; write-complete
signal not issued; calling component (SYS-002 or SYS-005) receives a failure
notification.

* **System Scenario: STS-006-C1**
  * **Given** SYS-006 has received a log event request and a fault injection
    harness causes the platform storage write to return a failure status
  * **When** SYS-006 attempts to write the event record
  * **Then** SYS-006 reports the failure to the diagnostic interface; it does
    not issue a write-complete acknowledgement to the calling component;
    the calling component receives an explicit failure notification

---

<!-- ══════════════════════════════════════════════════════════════════════ -->
<!-- SYS-007: Notification Dispatcher                                       -->
<!-- ══════════════════════════════════════════════════════════════════════ -->

### SYS-007: Notification Dispatcher

---

#### Test Case: STP-007-A (External Interface — Notification Delivered to Platform)
**Technique**: Interface Contract Testing (External)
**Design View**: Interface View — external interface SYS-007 → platform notification I/F
**Linked Component**: SYS-007
**Description**: Verify that SYS-007 delivers notification events to the platform
notification interface and that each Brake_Override override event produces exactly
one notification delivery.
**Validation Condition**: The platform notification interface records exactly one
notification event per Brake_Override override event; zero notifications appear on
any other interface.
**Expected Result**: One notification on platform notification interface; zero on
any other interface.

* **System Scenario: STS-007-A1**
  * **Given** SYS-007 has received a notification request from SYS-002 and a
    bus monitor is recording all output interface traffic from SYS-007
  * **When** SYS-007 processes the notification request
  * **Then** exactly one notification event is delivered to the platform
    notification interface; the bus monitor records zero notification events on
    any other interface

---

#### Test Case: STP-007-B (Internal Interface — Exactly-Once Delivery)
**Technique**: Interface Contract Testing (Internal)
**Design View**: Interface View — internal interface SYS-002 → SYS-007
**Linked Component**: SYS-007
**Description**: Verify that SYS-007 delivers exactly one notification per
Brake_Override event, even if SYS-002 delivers the notification request more than
once (defensive against duplicate-request scenarios).
**Validation Condition**: If SYS-002 delivers two identical notification requests
for the same event (simulated duplicate), the platform notification interface
receives exactly one notification, not two.
**Expected Result**: Exactly one notification delivered regardless of request
duplication.

* **System Scenario: STS-007-B1**
  * **Given** a test harness causes SYS-002 to deliver two identical
    `Brake_Override` notification requests for the same event to SYS-007
  * **When** SYS-007 processes both requests
  * **Then** the platform notification interface records exactly one notification
    delivery; the second request is deduplicated or silently absorbed

---

<!-- ══════════════════════════════════════════════════════════════════════ -->
<!-- SYS-008: Safety Process and Compliance                                 -->
<!-- ══════════════════════════════════════════════════════════════════════ -->

### SYS-008: Safety Process and Compliance

---

#### Test Case: STP-008-A (Inspection — ISO 26262 Lifecycle Artifact Completeness)
**Technique**: Inspection
**Design View**: Decomposition View — process obligations per REQ-CN-002, REQ-NF-002
**Linked Component**: SYS-008
**Description**: Verify by inspection that all required lifecycle artifacts for the
confirmed ASIL level are present, reviewed, and approved before controlled field
testing. This is a process compliance check, not a runtime test.
**Validation Condition**: A lifecycle artifact checklist (populated for the confirmed
ASIL level per OQ-001) shows all mandatory categories present with no unapproved items.
**Expected Result**: Zero mandatory artifacts absent or unapproved; every required
category (hazard analysis, safety requirements, design, implementation, test results,
traceability matrix, X-VERSE output) shows approved status.

* **System Scenario: STS-008-A1**
  * **Given** the ASIL level has been confirmed (OQ-001 resolved), a lifecycle
    artifact checklist for that ASIL level has been prepared, and all development
    activities are complete
  * **When** the Compliance / Quality Officer reviews the artifact inventory
    against the checklist
  * **Then** every mandatory artifact category is present with an approved review
    status; the checklist shows zero absent or unapproved mandatory items

---

#### Test Case: STP-008-B (Inspection — Cybersecurity Threat Analysis Completeness)
**Technique**: Inspection
**Design View**: Decomposition View — cybersecurity process obligations per REQ-CN-003
**Linked Component**: SYS-008
**Description**: Verify by inspection that the cybersecurity threat analysis for
SYS-004's input interfaces (brake pedal status, driver command, vehicle communication)
is complete and that every identified threat has a documented treatment decision.
**Validation Condition**: The threat analysis document covers all three specified
interfaces; every identified threat entry has a treatment status of either
"treated" or "accepted with rationale"; zero threats are listed with an
unresolved treatment status.
**Expected Result**: Threat analysis document present, covering all three interfaces;
zero threats with unresolved treatment.

* **System Scenario: STS-008-B1**
  * **Given** the cybersecurity threat analysis has been completed as a parallel
    work item and its output document is available for inspection
  * **When** the Safety Engineer inspects the threat analysis against the three
    required interfaces (brake pedal status, driver command input, vehicle
    communication interfaces)
  * **Then** all three interfaces are covered; every identified threat entry has
    a treatment decision of "treated" or "accepted with documented rationale";
    no threat entry has an empty or unresolved treatment field

---

## Safety-Critical Test Sections (ISO 26262 Overlay)

### Structural Coverage Requirements (ISO 26262-6 §9.4.5)

All runtime components (SYS-001 through SYS-007) carry **confirmed** ASIL D (OQ-001 resolved 2026-06-02).
ISO 26262-6 §9.4.5 requires MC/DC (Modified Condition/Decision Coverage) for ASIL D.
These coverage targets are verified during implementation-phase testing (unit tests and
integration tests) and confirmed by the structural coverage measurement toolchain
(per REQ-CN-005).

| Component | ASIL | Coverage Target | Technique | Shortfall Justification |
|-----------|------|----------------|-----------|------------------------|
| SYS-001 | ASIL D | MC/DC 100% | Modified Condition/Decision Coverage | None — to be confirmed at implementation phase |
| SYS-002 | ASIL D | MC/DC 100% | MC/DC | None — timing measurement tests (STP-002-A) supplement coverage evidence |
| SYS-003 | ASIL D | MC/DC 100% | MC/DC | None — exclusive interface guard is a single-condition decision |
| SYS-004 | ASIL D | MC/DC 100% | MC/DC | None — integrity validation logic requires MC/DC per each check condition |
| SYS-005 | ASIL D | MC/DC 100% | MC/DC | None — per-condition handler decomposition (per complexity targets) supports MC/DC |
| SYS-006 | ASIL D | MC/DC 100% | MC/DC | None — write-before-complete path is linear; two branches (success / failure) |
| SYS-007 | ASIL D | MC/DC 100% | MC/DC | None — dispatcher has a single decision point (deduplicate / dispatch) |

### Resource Usage Verification (ISO 26262-6 §9.4.4)

Resource usage targets for WCET and stack depth. Exact thresholds are pending ASIL
confirmation (OQ-001) and architecture definition (OQ-004); measurement conditions
are specified here so test infrastructure can be prepared in advance.

| Component | ASIL | Resource | Measurement Method | Threshold | Measurement Condition |
|-----------|------|----------|-------------------|-----------|----------------------|
| SYS-002 | ASIL D | WCET | Instrumented measurement + static WCET analysis | `<T_max>` (OQ-002) | Worst-case platform load (all 5 inputs at maximum rate; concurrent LCU condition active) |
| SYS-001 | ASIL D | WCET | Static WCET analysis | To be defined at architecture phase (OQ-004) | State machine evaluation under all 4 conditions active simultaneously |
| SYS-004 | ASIL D | WCET | Instrumented measurement | To be defined at architecture phase | All 5 inputs changing within one monitoring cycle |
| SYS-001 | ASIL D | Max Stack Depth | Static stack analysis | To be defined at architecture phase | Maximum call depth from SYS-004 signal → SYS-001 transition evaluator |
| SYS-002 | ASIL D | Max Stack Depth | Static stack analysis | To be defined | Override response coordination call chain (SYS-002 → SYS-003 → SYS-006 → SYS-007) |
| SYS-003–007 | ASIL D | Max Stack Depth | Static stack analysis | To be defined per component | Worst-case call depth per component |

**Note**: Dynamic heap allocation is not permitted for ASIL D components; this table
omits heap metrics. If dynamic allocation is introduced during implementation, a
derived requirement must be raised and reviewed.

### Back-to-Back Testing (ISO 26262 Part 6 §6.9, Table 11)

Back-to-back testing is highly recommended (`++`) for ASIL D. The following
components are designated as back-to-back test candidates based on their safety
criticality and the complexity of their decision logic.

| Component | Reference Implementation | Test Implementation | Comparison Method | Tolerance |
|-----------|------------------------|--------------------|--------------------|-----------|
| SYS-001 | Formal state machine model (e.g., a state chart tool output or verified reference implementation) | Target embedded implementation | Automated trace comparison: given identical condition signal sequence, observed state sequence must be identical | Zero tolerance — any state divergence is a defect |
| SYS-002 | Model-based reference for override response sequence (e.g., derived from formal specification of REQ-002/003/004/012/013) | Target embedded implementation | Automated comparison of command sequence and timestamps | Zero tolerance on sequence order; timing tolerance = `<T_max>` (OQ-002) |
| SYS-004 | Reference implementation of integrity checker and condition derivation logic | Target embedded implementation | Automated comparison of derived conditions for identical input sequences | Zero tolerance — any condition divergence is a defect |

---

## V&V Coverage (IEEE 1012:2016)

Every active requirement must have at least one V&V activity at system test level.
The table below confirms coverage; the V&V activity type is Test (STP), Inspection,
or Analysis as appropriate.

| REQ | V&V Activity | Type | STP / Note |
|-----|-------------|------|-----------|
| REQ-001 | State exhaustion test; fault injection on state variable | Test | STP-001-A, STP-001-B |
| REQ-002 | Cessation command contract; boundary on state gate | Test | STP-002-A, STP-003-B, STP-003-C |
| REQ-003 | Override response sequence contract | Test | STP-002-A |
| REQ-004 | State machine transition from `Cruise_Active` on Brake_Override | Test | STP-001-A, STP-001-B |
| REQ-005 | Reactivation refused before preconditions valid (via SYS-001) | Test | STP-001-A, STP-001-B |
| REQ-006 | LCU triggers cessation via SYS-005 → SYS-003 | Test | STP-005-A, STP-003-B |
| REQ-007 | LCU triggers state transition via SYS-005 → SYS-001 | Test | STP-005-A, STP-001-A |
| REQ-008 | CCF triggers `Cruise_Fault` via SYS-005 → SYS-001 | Test | STP-005-B |
| REQ-009 | CCF cessation via SYS-005 → SYS-003 | Test | STP-005-B, STP-003-B |
| REQ-010 | RII prevents entry to `Cruise_Active` via SYS-001 | Test | STP-001-A |
| REQ-011 | RII exits `Cruise_Active` via SYS-004 → SYS-001 | Test | STP-004-A, STP-004-C |
| REQ-012 | Notification dispatch via SYS-002 → SYS-007 | Test | STP-002-A, STP-007-A, STP-007-B |
| REQ-013 | Event log persistence via SYS-002 → SYS-006 | Test | STP-002-A, STP-006-A |
| REQ-014 | Five-field record completeness | Test | STP-006-B |
| REQ-015 | Write-before-complete; reconstruction completeness | Test | STP-006-A, STP-006-B |
| REQ-016 | All 5 inputs monitored by SYS-004 | Test | STP-004-A, STP-004-B, STP-004-D |
| REQ-NF-001 | WCET measurement for SYS-002 (timing budget pending OQ-002) | Test | Resource Usage table (STP timing measurement) — `[QUALITY NOTE: exact pass threshold pending OQ-002]` |
| REQ-NF-002 | Lifecycle artifact completeness | Inspection | STP-008-A |
| REQ-IF-001 | Exclusive propulsion interface use | Test | STP-003-A |
| REQ-IF-002 | Brake pedal status and driver command integrity derivation | Test | STP-004-A, STP-004-C |
| REQ-IF-003 | Notification via platform notification interface | Test | STP-007-A |
| REQ-IF-004 | Event records written to platform storage | Test | STP-006-A |
| REQ-IF-005 | Driver command input monitored by SYS-004 | Test | STP-004-B |
| REQ-IF-006 | Vehicle speed input monitored by SYS-004 | Test | STP-004-D |
| REQ-IF-007 | Diagnostic status → `Cruise_Control_Fault` derivation | Test | STP-004-B, STP-005-B |
| REQ-CN-001 | No new hardware interfaces; SYS-003 and SYS-004 use only authorised platform I/Fs | Test | STP-003-A, STP-004-A |
| REQ-CN-002 | ISO 26262 lifecycle compliance | Inspection | STP-008-A |
| REQ-CN-003 | Cybersecurity threat analysis completeness | Inspection | STP-008-B, STP-004-A |
| REQ-CN-004 | Componentized architecture (inspected via design review) | Inspection | STP-008-A (design review gate) |
| REQ-CN-005 | Verification-capable toolchain | Inspection | STP-008-A (toolchain configuration gate) |
| REQ-CN-006 | X-VERSE virtual validation activity | Inspection | STP-008-A (V&V plan gate) |

**V&V gaps**: None — all 31 active requirements have at least one V&V activity.

---

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total SYS Components | 8 |
| Total Test Cases (STP) | 24 |
| Total System Scenarios (STS) | 31 |
| SYS → STP Coverage | 8/8 (100%) |
| STP → STS Coverage | 24/24 (100%) |
| REQ V&V Coverage | 31/31 (100%) |

### Technique Distribution

| Technique | STP Count |
|-----------|-----------|
| Interface Contract Testing (External) | 4 |
| Interface Contract Testing (Internal) | 7 |
| Boundary Value Analysis | 4 |
| Equivalence Partitioning | 1 |
| Fault Injection | 6 |
| Inspection | 2 |
| **Total** | **24** |

### ISO 26262 Safety Test Coverage

| Safety Test Category | Count |
|---------------------|-------|
| MC/DC structural coverage targets declared | 7 (all runtime components) |
| Resource usage tests (WCET, stack depth) | 8 rows in resource table |
| Back-to-back test pairs designated | 3 (SYS-001, SYS-002, SYS-004) |

### Open Items

| Flag | Test | Pending |
|------|------|---------|
| Timing threshold TBD | STP-002-A, Resource Usage (SYS-002 WCET) | OQ-002 — T_max must be defined |
| DR-002 escalation path TBD | STP-006-C | DR-002 — write-failure handling requires requirement resolution |
