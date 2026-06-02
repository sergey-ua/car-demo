# Integration Test Plan: Vehicle Cruise Control — Brake Override Safety Slice

**Feature Branch**: `001-cruise-brake-override`
**Created**: 2026-06-02
**Status**: Approved
**Source**: `specs/001-cruise-brake-override/v-model/architecture-design.md`
**Domain Overlay**: ISO 26262 (ASIL D — Confirmed)

---

## Overview

This plan verifies the **seams and handshakes between the 16 architecture modules**
defined in `architecture-design.md`. Integration tests target module-boundary contracts,
data transformation chains across module pairs, graceful failure at interface boundaries,
and concurrency interactions between modules executing within the same scheduling cycle.

These tests do NOT test internal module logic (that is unit-test scope) and do NOT test
user journeys (that is acceptance-test scope).

**Test strategy by architecture view:**
- **Interface Contract Testing** → Interface View (consumer-provider API compliance)
- **Data Flow Testing** → Data Flow View (transformation chain correctness)
- **Interface Fault Injection** → Interface + Process View (graceful failure at boundaries)
- **Concurrency & Race Condition Testing** → Process View (shared resource access, scheduling order)

**ISO 26262 ASIL D additions** (overlay active):
- SIL/HIL environment classification required per ISO 26262-6 §6.8
- Resource contention verification for all ASIL D module pairs sharing resources

---

## ID Schema

- **Integration Test Case**: `ITP-{NNN}-{X}` — NNN matches parent ARCH, X is a letter suffix
- **Integration Test Scenario**: `ITS-{NNN}-{X}{#}` — nested under parent ITP, numeric suffix
- Example: `ITS-003-A1` → Scenario 1 of ITP-003-A, verifying ARCH-003 boundary

---

## ISO 29119-4 Integration Test Techniques

| Technique | Architecture View | What It Tests |
|-----------|------------------|---------------|
| Interface Contract Testing | Interface View | Consumer-provider API compliance, data format, error response |
| Data Flow Testing | Data Flow View | End-to-end data transformation chain across module boundaries |
| Interface Fault Injection | Interface View + Process View | Malformed payloads, timeouts, error propagation at seams |
| Concurrency Testing | Process View | Scheduling order, concurrent resource access, race avoidance |

---

## Integration Tests

<!-- ════════════════════════════════════════════════════════ -->
<!-- ARCH-001: State Transition Evaluator                     -->
<!-- ════════════════════════════════════════════════════════ -->

### ARCH-001: State Transition Evaluator

#### Test Case: ITP-001-A (Interface Contract: ARCH-009 to ARCH-001 Condition Signal)
**Technique**: Interface Contract Testing
**Architecture View**: Interface View — internal interface ARCH-009 → ARCH-001
**Description**: Verify that when ARCH-009 publishes a condition signal, ARCH-001
correctly processes it and issues a corresponding state-write request to ARCH-002.
**Validation Condition**: A confirmed `Brake_Override` condition signal from ARCH-009
results in ARCH-001 issuing a `StateWrite {new_state=Cruise_Cancelled}` to ARCH-002
within one scheduling cycle.
**Expected Result**: ARCH-002 receives `StateWrite {Cruise_Cancelled}` from ARCH-001;
no other state write is issued for this cycle.

* **ITS-001-A1**
  * **Given** ARCH-001 holds `Cruise_Active` in ARCH-002 and ARCH-009 publishes a `Brake_Override` condition signal
  * **When** ARCH-001 processes the condition signal on the next scheduling cycle
  * **Then** ARCH-001 sends `StateWrite {new_state=Cruise_Cancelled}` to ARCH-002 and no other `StateWrite` is issued

* **ITS-001-A2**
  * **Given** ARCH-001 holds `Cruise_Standby` in ARCH-002 and ARCH-009 publishes `Required_Input_Invalid`
  * **When** ARCH-001 processes the condition signal
  * **Then** ARCH-001 does NOT send a `StateWrite` to ARCH-002 (no transition permitted from Standby on RII)

#### Test Case: ITP-001-B (Interface Fault Injection: Undefined Condition Code from ARCH-009)
**Technique**: Interface Fault Injection
**Architecture View**: Interface View — ARCH-009 → ARCH-001 condition signal boundary
**Description**: Verify that ARCH-001 handles receipt of an undefined condition code
(outside the four defined values) with `INVALID_CONDITION (E001)` and does not
evaluate a transition or corrupt ARCH-002.
**Validation Condition**: An undefined condition code arriving at ARCH-001 raises
`INVALID_CONDITION (E001)` and does not result in any `StateWrite` to ARCH-002.
**Expected Result**: No `StateWrite` to ARCH-002; `INVALID_CONDITION (E001)` is
observable on ARCH-001's diagnostic output.

* **ITS-001-B1**
  * **Given** ARCH-001 is in the active state and a test harness injects an undefined condition value (not in `{Brake_Override, RII, LCU, CCF}`) on the ARCH-009 → ARCH-001 condition signal interface
  * **When** ARCH-001 receives the undefined condition
  * **Then** ARCH-001 raises `INVALID_CONDITION (E001)` and sends zero `StateWrite` messages to ARCH-002 in that cycle

---

### ARCH-002: State Register

#### Test Case: ITP-002-A (Interface Contract: ARCH-001 to ARCH-002 State Write and Read)
**Technique**: Interface Contract Testing
**Architecture View**: Interface View — ARCH-001 ↔ ARCH-002
**Description**: Verify the write-then-read contract: ARCH-001 writes a valid state,
ARCH-002 confirms (`write_ok`), and a subsequent read from ARCH-005 or ARCH-003 returns
the newly written state.
**Validation Condition**: The state value read by ARCH-003 after a write is identical
to the value written by ARCH-001; the `write_ok` acknowledgement precedes the read.
**Expected Result**: Exact round-trip fidelity: write `Cruise_Cancelled` → `write_ok` → read `Cruise_Cancelled`.

* **ITS-002-A1**
  * **Given** ARCH-002 holds `Cruise_Active` and ARCH-001 sends `StateWrite {Cruise_Cancelled}`
  * **When** ARCH-002 processes the write and returns `write_ok` to ARCH-001
  * **Then** ARCH-003's subsequent read of ARCH-002 returns `Cruise_Cancelled`; no stale `Cruise_Active` value is returned

#### Test Case: ITP-002-B (Interface Fault Injection: Out-of-Range State Write to RANGE_VIOLATION)
**Technique**: Interface Fault Injection
**Architecture View**: Interface View — ARCH-001 → ARCH-002 write boundary
**Description**: Verify that ARCH-002 rejects a state value outside the five-state set,
raises `RANGE_VIOLATION (E010)`, and escalates `Cruise_Control_Fault` to ARCH-010 without
corrupting the stored state.
**Validation Condition**: A write of `0xFF` to ARCH-002 raises `RANGE_VIOLATION (E010)`,
delivers a `Cruise_Control_Fault` signal to ARCH-010, and leaves the existing state value
unchanged.
**Expected Result**: ARCH-002 state unchanged; `RANGE_VIOLATION (E010)` raised; ARCH-010
receives `Cruise_Control_Fault`.

* **ITS-002-B1**
  * **Given** ARCH-002 holds `Cruise_Active` and a test harness injects `StateWrite {0xFF}` on the ARCH-001 → ARCH-002 interface
  * **When** ARCH-002 evaluates the range check
  * **Then** ARCH-002 does not update its state, raises `RANGE_VIOLATION (E010)`, and the ARCH-010 input interface receives a `Cruise_Control_Fault` signal

---

### ARCH-003: Override Response Sequencer

#### Test Case: ITP-003-A (Interface Contract: Brake_Override Response Sequence)
**Technique**: Interface Contract Testing
**Architecture View**: Interface View — ARCH-003 output interfaces to ARCH-006, ARCH-011, ARCH-013
**Description**: Verify that ARCH-003 issues all three downstream interface calls in the
required sequence: cessation command to ARCH-006 first, then log event request to ARCH-011
(after `log_confirmed`), then notification to ARCH-013.
**Validation Condition**: The three calls are observed in the specified order; ARCH-003 does
not send the notification request before `log_confirmed` is received.
**Expected Result**: Interface trace shows: ARCH-006 cessation → ARCH-011 log request →
`log_confirmed` → ARCH-013 notification; no out-of-order delivery.

* **ITS-003-A1**
  * **Given** ARCH-002 reports `Cruise_Active` and ARCH-003 receives a `Brake_Override` event from ARCH-009; a trace harness monitors the ARCH-003 output interfaces
  * **When** ARCH-003 executes the override response sequence
  * **Then** the trace shows cessation command delivered to ARCH-006 before any message to ARCH-011, `log_confirmed` received before any message to ARCH-013, and exactly one notification request sent to ARCH-013

#### Test Case: ITP-003-B (Data Flow Testing: Brake_Override to Log Record Chain)
**Technique**: Data Flow Testing
**Architecture View**: Data Flow View — Flow 1 (stages 3-4) and Flow 2 (stage 3)
**Description**: Verify data fidelity across the ARCH-003 → ARCH-011 interface: the
`LogEventRequest` delivered to ARCH-011 carries the correct `from_state`, `condition`,
`to_state`, `vehicle_speed`, and `timestamp` values that ARCH-003 received from the
triggering event context.
**Validation Condition**: The `LogEventRequest` arriving at ARCH-011 has `from_state=Cruise_Active`,
`condition=Brake_Override`, `to_state=Cruise_Cancelled`, `vehicle_speed` matching the
platform input snapshot, and a non-null `timestamp`.
**Expected Result**: All five fields present and correct in the record delivered to ARCH-011.

* **ITS-003-B1**
  * **Given** a `Brake_Override` event carrying `vehicle_speed=90` arrives at ARCH-003 when ARCH-002 reports `Cruise_Active`; ARCH-001 confirms transition to `Cruise_Cancelled`
  * **When** ARCH-003 constructs and sends the `LogEventRequest` to ARCH-011
  * **Then** the `LogEventRequest` interface carries `from_state=Cruise_Active`, `condition=Brake_Override`, `to_state=Cruise_Cancelled`, `vehicle_speed=90`, and a non-null `timestamp`

---

### ARCH-004: Response Timing Supervisor

#### Test Case: ITP-004-A (Interface Contract: ARCH-009 Arm / ARCH-006 Stop Timer Protocol)
**Technique**: Interface Contract Testing
**Architecture View**: Interface View — ARCH-009 → ARCH-004 arm and ARCH-006 → ARCH-004 stop
**Description**: Verify the two-interface timer protocol: ARCH-004 arms on a `Brake_Override`
signal from ARCH-009 and stops when ARCH-006 delivers `cessation_complete`.
**Validation Condition**: ARCH-004 transitions from idle to armed on the ARCH-009 signal and
from armed to stopped on the ARCH-006 signal; `timing_ok` is emitted when elapsed time is
within 100 ms.
**Expected Result**: `timing_ok` emitted after `cessation_complete`; elapsed time ≤ 100 ms.

* **ITS-004-A1**
  * **Given** ARCH-004 is idle and ARCH-009 delivers a `Brake_Override` arm signal with a known start timestamp
  * **When** ARCH-006 delivers `cessation_complete` within 100 ms of the arm signal
  * **Then** ARCH-004 emits `timing_ok` to its caller; the measured elapsed time between arm and stop signals is ≤ 100 ms

#### Test Case: ITP-004-B (Interface Fault Injection: Stop Not Received to TIMING_EXCEEDED to ARCH-010)
**Technique**: Interface Fault Injection
**Architecture View**: Interface View — ARCH-006 → ARCH-004 stop signal boundary
**Description**: Verify that ARCH-004 raises `TIMING_EXCEEDED (E030)` and forwards a
`Cruise_Control_Fault` to ARCH-010 when the `cessation_complete` stop signal from ARCH-006
does not arrive within 100 ms.
**Validation Condition**: After 100 ms without a stop signal, ARCH-004 raises `TIMING_EXCEEDED (E030)`
and ARCH-010 receives a `Cruise_Control_Fault` escalation.
**Expected Result**: `TIMING_EXCEEDED (E030)` raised; ARCH-010 receives `Cruise_Control_Fault`;
no `timing_ok` emitted.

* **ITS-004-B1**
  * **Given** ARCH-004 is armed (timer started) and a test harness suppresses the `cessation_complete` signal from ARCH-006
  * **When** 100 ms elapse without a stop signal
  * **Then** ARCH-004 raises `TIMING_EXCEEDED (E030)` and the ARCH-010 input interface receives a `Cruise_Control_Fault` signal; ARCH-004 does not emit `timing_ok`

---

### ARCH-005: Speed Control Regulator

#### Test Case: ITP-005-A (Interface Contract: ARCH-002 Authorisation Gate to ARCH-006 Command)
**Technique**: Interface Contract Testing
**Architecture View**: Interface View — ARCH-002 → ARCH-005 authorisation and ARCH-005 → ARCH-006 command
**Description**: Verify that ARCH-005 delivers a speed command to ARCH-006 when and only when
ARCH-002 reports `Cruise_Active` on the authorisation interface.
**Validation Condition**: A speed command is present on the ARCH-005 → ARCH-006 interface when
ARCH-002 reports `Cruise_Active`; the command is absent (zero-command) when ARCH-002 reports
any other state.
**Expected Result**: Speed command present ↔ `Cruise_Active`; zero-command in all other states.

* **ITS-005-A1**
  * **Given** ARCH-002 reports `Cruise_Active` on its state-read interface and ARCH-005 has a non-zero speed target
  * **When** ARCH-005 evaluates the authorisation gate and generates a speed command
  * **Then** a non-zero speed command is delivered to the ARCH-005 → ARCH-006 interface

* **ITS-005-A2**
  * **Given** ARCH-002 reports `Cruise_Cancelled` on its state-read interface
  * **When** ARCH-005 evaluates the authorisation gate on the next cycle
  * **Then** a zero-command (suppression) is delivered to the ARCH-005 → ARCH-006 interface; no speed-maintaining command is present

---

### ARCH-006: Propulsion Interface Adapter

#### Test Case: ITP-006-A (Interface Contract (External): Cessation Command Execution and Acknowledgement)
**Technique**: Interface Contract Testing (External)
**Architecture View**: Interface View — ARCH-003/ARCH-010 → ARCH-006 → platform propulsion I/F
**Description**: Verify that ARCH-006 delivers a cessation command to the platform propulsion
interface and returns `cessation_complete` to the calling module (ARCH-003 or ARCH-010) after
the platform write is confirmed.
**Validation Condition**: The platform propulsion interface receives a cessation command; the
`cessation_complete` acknowledgement is returned to ARCH-003 after the platform write; no
speed command follows the cessation.
**Expected Result**: Platform propulsion interface log shows cessation command; ARCH-003 receives
`cessation_complete`; no subsequent speed commands from ARCH-006.

* **ITS-006-A1**
  * **Given** ARCH-006 has an active speed command on the propulsion interface and ARCH-003 sends a cessation command
  * **When** ARCH-006 delivers the cessation to the platform propulsion interface and the platform confirms the write
  * **Then** ARCH-003 receives `cessation_complete`; the platform propulsion interface log shows no further speed commands from ARCH-006

#### Test Case: ITP-006-B (Concurrency: Simultaneous Cessation from ARCH-003 and ARCH-010)
**Technique**: Concurrency & Race Condition Testing
**Architecture View**: Process View — concurrent access to ARCH-006 cessation interface
**Description**: Verify that when ARCH-003 and ARCH-010 both deliver cessation commands to
ARCH-006 within the same scheduling cycle, exactly one cessation command reaches the platform
propulsion interface with no duplicate or corrupted command.
**Validation Condition**: Exactly one cessation command is observable on the platform propulsion
interface; no malformed, partial, or duplicate cessation command is logged.
**Expected Result**: Single cessation command; both ARCH-003 and ARCH-010 receive `cessation_complete`; no command corruption.

* **ITS-006-B1**
  * **Given** a test harness delivers a cessation command from ARCH-003 and a cessation command from ARCH-010 to ARCH-006 within the same scheduling cycle
  * **When** ARCH-006 processes both concurrent cessation requests
  * **Then** exactly one cessation command is written to the platform propulsion interface; both ARCH-003 and ARCH-010 receive `cessation_complete` acknowledgements; no duplicate or malformed command is observable

---

### ARCH-007: Platform Input Reader

#### Test Case: ITP-007-A (Interface Contract (External): Raw Inputs Delivered to ARCH-008 and ARCH-009)
**Technique**: Interface Contract Testing (External)
**Architecture View**: Interface View — platform I/F → ARCH-007 → ARCH-008 and ARCH-009
**Description**: Verify that ARCH-007 reads all five platform inputs and delivers the brake
pedal status and driver command to ARCH-008, and all five raw values to ARCH-009, within one
scheduling cycle.
**Validation Condition**: After one scheduling tick from ARCH-015, ARCH-008 receives
`RawBrakePedalStatus` and `RawDriverCommand`, and ARCH-009 receives all five raw input values.
**Expected Result**: ARCH-008 and ARCH-009 each receive their expected raw inputs within one cycle.

* **ITS-007-A1**
  * **Given** the platform delivers known test values on all five input interfaces (brake status, driver command, vehicle speed, diagnostic status, LCU availability) and ARCH-015 has issued a scheduling tick to ARCH-007
  * **When** ARCH-007 reads all five platform inputs and distributes them
  * **Then** ARCH-008 receives the brake pedal and driver command raw values, and ARCH-009 receives all five raw values — all matching the known test values

#### Test Case: ITP-007-B (Interface Fault Injection: Platform Input Absent to READ_TIMEOUT to RII)
**Technique**: Interface Fault Injection
**Architecture View**: Interface View — platform I/F → ARCH-007 boundary
**Description**: Verify that ARCH-007 raises `READ_TIMEOUT (E050)` when a platform input
does not deliver within one scheduling cycle, and that ARCH-009 derives `Required_Input_Invalid`
from the resulting timeout.
**Validation Condition**: With one platform input absent for a full cycle, ARCH-007 raises
`READ_TIMEOUT (E050)` and ARCH-009 receives a signal that produces `Required_Input_Invalid`.
**Expected Result**: `READ_TIMEOUT (E050)` raised; ARCH-009 derives `Required_Input_Invalid`; no valid condition is derived from the timed-out input.

* **ITS-007-B1**
  * **Given** a test harness suppresses the brake pedal status platform input for one full scheduling cycle
  * **When** ARCH-007 attempts to read all five inputs and the brake pedal status is absent
  * **Then** ARCH-007 raises `READ_TIMEOUT (E050)` and the ARCH-009 input interface receives a signal that results in derivation of `Required_Input_Invalid`

---

### ARCH-008: Input Integrity Verifier

#### Test Case: ITP-008-A (Interface Contract: Valid Input to validity=TRUE Delivered to ARCH-009)
**Technique**: Interface Contract Testing
**Architecture View**: Interface View — ARCH-007 → ARCH-008 → ARCH-009
**Description**: Verify that a brake pedal status input passing the CRC-16 and counter check
at ARCH-008 is delivered to ARCH-009 with `validity=TRUE` and the original value intact.
**Validation Condition**: A valid raw input (correct CRC-16 and sequential counter) results in
`VerifiedInput {value, validity=TRUE}` on the ARCH-008 → ARCH-009 interface.
**Expected Result**: `validity=TRUE` and value unchanged on the output interface.

* **ITS-008-A1**
  * **Given** ARCH-007 delivers a brake pedal sample with a correct CRC-16 and an in-sequence rolling counter to ARCH-008
  * **When** ARCH-008 applies the CRC-16 and counter check
  * **Then** ARCH-008 delivers `VerifiedInput {value=<brake_value>, validity=TRUE}` to ARCH-009; the original value is not altered

#### Test Case: ITP-008-B (Data Flow Testing: Integrity-Failed Input to validity=FALSE to RII at ARCH-009)
**Technique**: Data Flow Testing
**Architecture View**: Data Flow View — Flow 1 (stage 2 → 3a)
**Description**: Verify the data flow from an integrity failure at ARCH-008 to `Required_Input_Invalid`
derivation at ARCH-009: an input with a CRC mismatch results in `validity=FALSE` at the
ARCH-008 → ARCH-009 boundary, which causes ARCH-009 to derive `Required_Input_Invalid`.
**Validation Condition**: CRC failure at ARCH-008 → `VerifiedInput {validity=FALSE}` at
ARCH-009 boundary → `Required_Input_Invalid` condition signal published by ARCH-009.
**Expected Result**: Correct condition `Required_Input_Invalid` published; no `Brake_Override` derived.

* **ITS-008-B1**
  * **Given** ARCH-007 delivers a brake pedal sample with a deliberate CRC mismatch (injected by test harness) to ARCH-008
  * **When** ARCH-008 fails the CRC-16 check and delivers `VerifiedInput {validity=FALSE}` to ARCH-009
  * **Then** ARCH-009 derives and publishes `Required_Input_Invalid`; no `Brake_Override` condition is derived from this input

---

### ARCH-009: Condition Deriver

#### Test Case: ITP-009-A (Interface Contract: Condition Published to All Registered Consumers)
**Technique**: Interface Contract Testing
**Architecture View**: Interface View — ARCH-009 → ARCH-001, ARCH-003, ARCH-004, ARCH-010
**Description**: Verify that when ARCH-009 derives `Brake_Override`, the condition signal is
delivered to all four registered consumers (ARCH-001, ARCH-003, ARCH-004, and the Brake_Override
event to ARCH-003 only) within one scheduling cycle.
**Validation Condition**: All four consumers receive the condition signal within one cycle of
derivation; ARCH-003 additionally receives the `BrakeOverrideEvent`; no consumer is skipped.
**Expected Result**: All four delivery interfaces show the condition signal; ARCH-003 additionally shows the event.

* **ITS-009-A1**
  * **Given** ARCH-008 delivers `VerifiedInput {value=applied, validity=TRUE}` to ARCH-009, and ARCH-002 reports `Cruise_Active`; a harness monitors all four ARCH-009 output interfaces
  * **When** ARCH-009 derives `Brake_Override` and publishes it
  * **Then** the condition signal is observable on ARCH-001, ARCH-003 (condition + event), ARCH-004, and ARCH-010 interfaces within one cycle; all four deliveries complete before the scheduling cycle ends

#### Test Case: ITP-009-B (Interface Contract: RII Condition Routing)
**Technique**: Interface Contract Testing
**Architecture View**: Interface View — ARCH-009 → ARCH-001, ARCH-010
**Description**: Verify that `Required_Input_Invalid` derived at ARCH-009 is routed to
ARCH-001 and ARCH-010, but NOT published as a `BrakeOverrideEvent` (RII is not Brake_Override).
**Validation Condition**: `Required_Input_Invalid` signal present on ARCH-001 and ARCH-010
interfaces; no `BrakeOverrideEvent` on the ARCH-003 event interface.
**Expected Result**: RII routed correctly; ARCH-003 receives no brake override event.

* **ITS-009-B1**
  * **Given** ARCH-008 delivers `VerifiedInput {validity=FALSE}` to ARCH-009
  * **When** ARCH-009 derives `Required_Input_Invalid`
  * **Then** ARCH-001 and ARCH-010 receive the `Required_Input_Invalid` condition signal; ARCH-003's `BrakeOverrideEvent` input interface receives nothing; ARCH-004's arm-timer interface receives nothing

---

### ARCH-010: Fault Condition Handler

#### Test Case: ITP-010-A (Interface Contract: LCU Condition to Cessation + Transition Requests)
**Technique**: Interface Contract Testing
**Architecture View**: Interface View — ARCH-009 → ARCH-010 → ARCH-006 and ARCH-001
**Description**: Verify that ARCH-010, on receiving `Longitudinal_Control_Unavailable` from
ARCH-009, delivers a cessation command to ARCH-006 and a transition request to ARCH-001,
in that order.
**Validation Condition**: Cessation command on ARCH-006 interface precedes transition request
on ARCH-001 interface; both delivered within one scheduling cycle.
**Expected Result**: ARCH-006 cessation precedes ARCH-001 transition request; both observed.

* **ITS-010-A1**
  * **Given** ARCH-009 publishes `Longitudinal_Control_Unavailable` to ARCH-010 while ARCH-002 reports `Cruise_Active`; a trace harness monitors both output interfaces
  * **When** ARCH-010 processes the LCU condition
  * **Then** the trace shows a cessation command delivered to ARCH-006 before the transition request delivered to ARCH-001; both are present within one scheduling cycle

#### Test Case: ITP-010-B (Interface Fault Injection: Transition Rejected by ARCH-001 to CCF Escalation)
**Technique**: Interface Fault Injection
**Architecture View**: Interface View — ARCH-010 → ARCH-001 transition request boundary
**Description**: Verify that when ARCH-001 rejects a transition request from ARCH-010 (simulating
a locked evaluator), ARCH-010 raises `TRANSITION_REJECTED (E070)` and escalates to CCF.
**Validation Condition**: Transition rejection raises `TRANSITION_REJECTED (E070)` and ARCH-010
delivers a `Cruise_Control_Fault` signal back to ARCH-010's own escalation path (which then
triggers ARCH-001 via fault condition routing, entering `Cruise_Fault`).
**Expected Result**: `TRANSITION_REJECTED (E070)` observable; CCF escalation initiated.

* **ITS-010-B1**
  * **Given** a test harness causes ARCH-001 to reject all transition requests and ARCH-010 has sent a valid LCU transition request
  * **When** ARCH-001 returns a rejection to ARCH-010
  * **Then** ARCH-010 raises `TRANSITION_REJECTED (E070)` and delivers `Cruise_Control_Fault` to its escalation output; the CCF escalation path is observable on the harness monitor

---

### ARCH-011: Event Record Builder

#### Test Case: ITP-011-A (Interface Contract: Log Event Request to 5-Field Record to ARCH-012)
**Technique**: Interface Contract Testing
**Architecture View**: Interface View — ARCH-003/ARCH-010 → ARCH-011 → ARCH-012
**Description**: Verify that ARCH-011 constructs a complete 5-field `StateTransitionEvent`
record from the `LogEventRequest` it receives and delivers it to ARCH-012.
**Validation Condition**: The `StateTransitionEvent` arriving at ARCH-012 contains all five
fields (timestamp, from_state, condition, to_state, vehicle_speed) populated with the values
from the original `LogEventRequest`.
**Expected Result**: 5-field record at ARCH-012 boundary; values match the request.

* **ITS-011-A1**
  * **Given** ARCH-003 sends `LogEventRequest {from=Cruise_Active, cond=Brake_Override, to=Cruise_Cancelled, speed=85, timestamp=T}` to ARCH-011
  * **When** ARCH-011 constructs the record and delivers it to ARCH-012
  * **Then** ARCH-012 receives `StateTransitionEvent` with `from_state=Cruise_Active`, `condition=Brake_Override`, `to_state=Cruise_Cancelled`, `vehicle_speed=85`, `timestamp=T`; all five fields are non-null

#### Test Case: ITP-011-B (Interface Fault Injection: Null Field in Request to FIELD_VALIDATION_FAILURE)
**Technique**: Interface Fault Injection
**Architecture View**: Interface View — ARCH-003 → ARCH-011 request boundary
**Description**: Verify that ARCH-011 raises `FIELD_VALIDATION_FAILURE (E080)` when the
`LogEventRequest` arrives with a null `from_state`, and that no partial record is forwarded
to ARCH-012.
**Validation Condition**: `FIELD_VALIDATION_FAILURE (E080)` raised; ARCH-012 receives no record.
**Expected Result**: No record delivered to ARCH-012; `FIELD_VALIDATION_FAILURE (E080)` observable.

* **ITS-011-B1**
  * **Given** a test harness delivers `LogEventRequest {from_state=NULL, cond=Brake_Override, to_state=Cruise_Cancelled, speed=85, timestamp=T}` to ARCH-011
  * **When** ARCH-011 validates the five fields
  * **Then** ARCH-011 raises `FIELD_VALIDATION_FAILURE (E080)` and delivers nothing to ARCH-012; the ARCH-012 input interface receives zero records for this request

---

### ARCH-012: Event Storage Adapter

#### Test Case: ITP-012-A (Interface Contract (External): Event Record to Platform Storage to write_confirmed)
**Technique**: Interface Contract Testing (External)
**Architecture View**: Interface View — ARCH-011 → ARCH-012 → platform event storage
**Description**: Verify that ARCH-012 writes the received `StateTransitionEvent` to the
platform event storage interface and returns `write_confirmed` to ARCH-003 or ARCH-010 only
after the platform confirms the write.
**Validation Condition**: Platform storage contains the written record; `write_confirmed` is
returned to the caller after — not before — the platform write succeeds.
**Expected Result**: Record in platform storage; `write_confirmed` returned; ordering confirmed by trace.

* **ITS-012-A1**
  * **Given** ARCH-011 delivers a complete `StateTransitionEvent` to ARCH-012 and a trace harness monitors the ARCH-012 → platform storage interface and the ARCH-012 → ARCH-003 return path
  * **When** ARCH-012 writes the record to platform storage and the platform returns a write success
  * **Then** the platform storage interface confirms the record is present; `write_confirmed` is returned to ARCH-003 after the platform success, not before

#### Test Case: ITP-012-B (Interface Fault Injection: Write Failure to CCF via ARCH-010)
**Technique**: Interface Fault Injection
**Architecture View**: Interface View — ARCH-012 → platform storage boundary + ARCH-012 → ARCH-010 escalation
**Description**: Verify that when the platform storage write fails, ARCH-012 raises
`WRITE_FAILURE (E090)`, escalates `Cruise_Control_Fault` to ARCH-010, and does NOT return
`write_confirmed` to the caller.
**Validation Condition**: `WRITE_FAILURE (E090)` raised; ARCH-010 receives `Cruise_Control_Fault`;
no `write_confirmed` returned to caller.
**Expected Result**: Failure path observable; CCF escalation triggered; caller does not receive success.

* **ITS-012-B1**
  * **Given** a test harness causes the platform event storage interface to return a write-failure response
  * **When** ARCH-012 attempts to write a `StateTransitionEvent` and receives the write failure
  * **Then** ARCH-012 raises `WRITE_FAILURE (E090)`, delivers `Cruise_Control_Fault` to ARCH-010, and returns nothing (no `write_confirmed`) to ARCH-003

---

### ARCH-013: Notification Publisher

#### Test Case: ITP-013-A (Interface Contract (External): Notification Request to Platform Interface)
**Technique**: Interface Contract Testing (External)
**Architecture View**: Interface View — ARCH-003 → ARCH-013 → platform notification I/F
**Description**: Verify that ARCH-013 delivers exactly one notification to the platform
notification interface per unique `event_id` from ARCH-003 and returns `delivered` to the caller.
**Validation Condition**: Platform notification interface receives exactly one event for a
given `event_id`; `delivered` is returned to ARCH-003.
**Expected Result**: One delivery; `delivered` returned; no duplicate on platform.

* **ITS-013-A1**
  * **Given** ARCH-003 sends `NotificationRequest {event_id=E001, event_type=Brake_Override}` to ARCH-013
  * **When** ARCH-013 delivers the notification to the platform notification interface
  * **Then** the platform notification interface receives exactly one event for `event_id=E001`; ARCH-003 receives `delivered`

#### Test Case: ITP-013-B (Interface Fault Injection: Duplicate Request to Exactly-Once Delivery)
**Technique**: Interface Fault Injection
**Architecture View**: Interface View — ARCH-003 → ARCH-013 deduplication boundary
**Description**: Verify that ARCH-013 deduplicates a repeated `NotificationRequest` carrying
the same `event_id`, delivering only one notification to the platform interface.
**Validation Condition**: Two identical `NotificationRequest {event_id=E001}` messages to
ARCH-013 result in exactly one platform notification delivery.
**Expected Result**: Platform interface shows one notification; second request is absorbed.

* **ITS-013-B1**
  * **Given** ARCH-003 sends `NotificationRequest {event_id=E001}` to ARCH-013 twice in rapid succession (simulating a duplicate delivery)
  * **When** ARCH-013 processes both requests
  * **Then** the platform notification interface receives exactly one delivery for `event_id=E001`; the second request is deduplicated and no second delivery occurs

---

### ARCH-014: Safety Lifecycle Evidence Set

#### Test Case: ITP-014-A (Inspection: Lifecycle Artifacts Complete and Approved)
**Technique**: Interface Contract Testing (Inspection)
**Architecture View**: Decomposition View — process artifacts implementing SYS-008 obligations
**Description**: Verify by inspection that all mandatory lifecycle artifacts for ASIL D are
present, reviewed, and approved, meeting the contract defined by ARCH-014 (SYS-008 obligations).
**Validation Condition**: Every mandatory category in the ASIL D checklist is present with
approved review status; zero missing or unapproved mandatory items.
**Expected Result**: All ASIL D lifecycle artifacts present and approved.

* **ITS-014-A1**
  * **Given** all V-Model development activities are complete and a lifecycle artifact checklist for ASIL D has been prepared
  * **When** the Compliance / Quality Officer inspects the artifact inventory against the ASIL D checklist
  * **Then** every mandatory artifact category — requirements, hazard analysis, architecture and module design, implementation evidence, test results, traceability matrix, X-VERSE output — is present with an approved review status; zero mandatory items are absent or unapproved

---

### ARCH-015: Scheduling Controller `[CROSS-CUTTING]`

#### Test Case: ITP-015-A (Interface Contract: Scheduling Tick Activates ARCH-007 at Cycle Start)
**Technique**: Interface Contract Testing
**Architecture View**: Interface View — ARCH-015 → ARCH-007 (activation_tick)
**Description**: Verify that ARCH-015 delivers exactly one `activation_tick` to ARCH-007 at
the start of each scheduling cycle, within the defined ≤ 20 ms period.
**Validation Condition**: ARCH-007 receives exactly one tick per cycle; inter-tick interval
is ≤ 20 ms over 100 consecutive cycles.
**Expected Result**: 100 consecutive ticks delivered; all inter-tick intervals ≤ 20 ms; no missed or duplicate ticks.

* **ITS-015-A1**
  * **Given** a trace harness records the timestamp of each `activation_tick` delivered to ARCH-007 and ARCH-015 is running for 100 consecutive cycles
  * **When** ARCH-015 delivers 100 consecutive scheduling ticks
  * **Then** all 100 inter-tick intervals are ≤ 20 ms; exactly one tick is delivered per cycle; no cycle produces zero or two ticks

#### Test Case: ITP-015-B (Concurrency Testing: Fixed Activation Order Within Cycle)
**Technique**: Concurrency & Race Condition Testing
**Architecture View**: Process View — Concurrency Model (ARCH-007 → ARCH-008 → ARCH-009 execution order)
**Description**: Verify that within a single scheduling cycle, ARCH-007 completes execution
before ARCH-008 begins, and ARCH-008 completes before ARCH-009 begins, as defined in the
Concurrency Model.
**Validation Condition**: Execution timestamps from a cycle trace show ARCH-007 completion
before ARCH-008 start, and ARCH-008 completion before ARCH-009 start, in all 50 test cycles.
**Expected Result**: Strict ordering ARCH-007 → ARCH-008 → ARCH-009 observed in all cycles; no out-of-order execution.

* **ITS-015-B1**
  * **Given** a timing trace harness records the start and end timestamps of ARCH-007, ARCH-008, and ARCH-009 execution slots within each cycle
  * **When** 50 consecutive scheduling cycles are executed
  * **Then** in all 50 cycles: ARCH-007 completion timestamp precedes ARCH-008 start timestamp, and ARCH-008 completion timestamp precedes ARCH-009 start timestamp

---

### ARCH-016: Watchdog Supervisor `[CROSS-CUTTING]`

#### Test Case: ITP-016-A (Interface Contract: Completion Signal Received to No Timeout)
**Technique**: Interface Contract Testing
**Architecture View**: Interface View — ARCH-NNN → ARCH-016 (completion_signal)
**Description**: Verify that when ARCH-001 through ARCH-010 deliver completion signals within
their allocated time slots, ARCH-016 does not raise `WATCHDOG_TIMEOUT (E120)`.
**Validation Condition**: All modules completing within their slot → zero `WATCHDOG_TIMEOUT` events
over 50 normal execution cycles.
**Expected Result**: Zero watchdog timeouts; all completion signals received in time.

* **ITS-016-A1**
  * **Given** ARCH-001 through ARCH-010 each complete their scheduled execution and deliver completion signals within their allocated time slots for 50 consecutive cycles
  * **When** ARCH-016 monitors all deadline slots across those 50 cycles
  * **Then** ARCH-016 raises zero `WATCHDOG_TIMEOUT` events; no `Cruise_Control_Fault` escalation is initiated by ARCH-016

#### Test Case: ITP-016-B (Interface Fault Injection: Module Misses Deadline to WATCHDOG_TIMEOUT to CCF)
**Technique**: Interface Fault Injection
**Architecture View**: Interface View — ARCH-NNN → ARCH-016 completion boundary
**Description**: Verify that when ARCH-001 (or any ASIL D module) fails to deliver its
completion signal before its deadline, ARCH-016 raises `WATCHDOG_TIMEOUT (E120)` and
delivers `Cruise_Control_Fault` to ARCH-010.
**Validation Condition**: Missing completion signal within one deadline window raises
`WATCHDOG_TIMEOUT (E120)` and ARCH-010 receives `Cruise_Control_Fault`.
**Expected Result**: `WATCHDOG_TIMEOUT (E120)` raised; CCF delivered to ARCH-010.

* **ITS-016-B1**
  * **Given** a test harness suppresses the completion signal from ARCH-001 for one scheduling slot
  * **When** ARCH-016 monitors the ARCH-001 deadline and no completion signal arrives
  * **Then** ARCH-016 raises `WATCHDOG_TIMEOUT (E120)` and delivers `Cruise_Control_Fault` to ARCH-010; the harness confirms the CCF on ARCH-010's input interface

---

## Test Harness and Mocking Strategy

### Mock and Stub Requirements

| Module Under Test | Mocked/Stubbed Dependencies | Mock Behaviour |
|------------------|-----------------------------|----------------|
| ARCH-001 | ARCH-002 (state register), ARCH-009 (condition source) | State register stub returns programmed values; condition injector delivers specific condition codes |
| ARCH-002 | ARCH-001 (write source), ARCH-010 (escalation target) | Write injector delivers programmed state values; escalation monitor captures CCF signals |
| ARCH-003 | ARCH-002 (state read), ARCH-006 (cessation sink), ARCH-011 (log sink), ARCH-013 (notify sink) | State stub returns `Cruise_Active`; sinks capture and acknowledge received messages |
| ARCH-004 | ARCH-009 (arm source), ARCH-006 (stop source), ARCH-010 (CCF target) | Arm injector; stop injector with configurable delay; CCF monitor |
| ARCH-005 | ARCH-002 (authorisation), ARCH-006 (command sink) | Authorisation stub returns programmed state; command sink captures speed values |
| ARCH-006 | ARCH-003/ARCH-010 (command sources), platform propulsion I/F | Platform propulsion stub returns configurable success/failure responses |
| ARCH-007 | Platform inputs (5 channels), ARCH-015 (tick source) | Platform stubs inject known test values or simulate absence; tick injector drives cycles |
| ARCH-008 | ARCH-007 (raw input source), ARCH-009 (verified output sink) | Raw input injector with configurable CRC pass/fail; output sink captures validity flags |
| ARCH-009 | ARCH-008 (verified inputs), ARCH-007 (other inputs), consumers (ARCH-001/003/004/010) | Input stubs; consumer monitors capture condition signals and event delivery |
| ARCH-010 | ARCH-009 (condition source), ARCH-006 (cessation sink), ARCH-001 (transition sink) | Condition injector; sinks configurable to accept or reject |
| ARCH-011 | ARCH-003/ARCH-010 (request sources), ARCH-012 (record sink) | Request injectors with configurable field values including null fields; record sink captures output |
| ARCH-012 | ARCH-011 (record source), platform storage I/F, ARCH-010 (CCF target) | Platform storage stub configurable to succeed or fail; CCF monitor |
| ARCH-013 | ARCH-003 (request source), platform notification I/F | Request injector with configurable event_id; platform notification stub |
| ARCH-015 | ARCH-007 (tick target), ARCH-016 (deadline target) | Tick receivers capture timestamps; deadline monitors verify timing |
| ARCH-016 | All ARCH-001–010 (completion sources), ARCH-010 (CCF target) | Completion suppressor (configurable by module); CCF output monitor |

### Test Data Management

- All test data is injected via harness stubs; no live platform hardware is required for SIL-mode tests
- Vehicle speed test values: a known reference value (e.g., 85 km/h from a calibrated harness source)
- Timestamps: driven by a test-harness clock to ensure reproducibility across runs
- CRC test vectors: pre-computed valid and invalid CRC-16 sequences for ARCH-008 injection
- State sequences: state register seeded by test setup, not by prior test run state

---

## Safety-Critical Integration Test Sections (ISO 26262 Overlay)

### SIL/HIL Test Environment Classification (ISO 26262-6 §6.8)

ASIL D integration tests with hardware-interfacing modules require HIL testing.
Pure-software module pairs may run in SIL with back-to-back comparison justification.

| Test ID | ASIL | Environment | Hardware Dependencies | Stubbed in SIL | Adaptation Note |
|---------|------|-------------|----------------------|----------------|-----------------|
| ITP-006-A | ASIL D | **HIL** | Platform propulsion command I/F | Propulsion bus and actuator stub | Requires real-time propulsion I/F or qualified HIL harness |
| ITP-007-A/B | ASIL D | **HIL** | Platform brake pedal, driver cmd, speed, diag, LCU inputs | All 5 input channels stubbed | Requires qualified platform input stimulus generator |
| ITP-012-A/B | ASIL D | **HIL** | Platform local persistent event storage | Storage read/write stub | Requires non-volatile storage interface or qualified stub |
| ITP-013-A/B | ASIL D | **HIL** | Platform notification interface | Notification bus stub | Requires platform notification receiver or qualified stub |
| ITP-001-A/B through ITP-005-A/B | ASIL D | **SIL** | None | All ARCH modules are pure software | Back-to-back SIL comparison recommended per ISO 26262-6 §6.9 Table 11 |
| ITP-008-A/B through ITP-011-A/B | ASIL D | **SIL** | None | All pure software | Back-to-back SIL comparison recommended |
| ITP-015-A/B, ITP-016-A/B | ASIL D | **SIL** | High-resolution timer | Scheduling clock from harness | Timing measurements require sub-millisecond resolution timer |
| ITP-014-A | ASIL D | Inspection | None | N/A | Document review; no execution environment required |

### Resource Contention Verification (ISO 26262-6 §7.4.11)

| Module Pair | ASIL | Shared Resource | Contention Scenario | Expected Resolution |
|-------------|------|-----------------|---------------------|---------------------|
| ARCH-003 ↔ ARCH-010 | ASIL B(D) + ASIL D | ARCH-006 cessation command interface | Both modules deliver cessation to ARCH-006 in the same cycle (ITP-006-B) | ARCH-006 serialises the two commands; exactly one cessation command reaches the platform; both callers receive `cessation_complete` |
| ARCH-003 ↔ ARCH-010 | ASIL B(D) + ASIL D | ARCH-011/ARCH-012 logging path | Both modules deliver log event requests to ARCH-011 in the same cycle | ARCH-011 queues or serialises the requests; both records are written in order; no record is lost |
| ARCH-015 scheduling slots | ASIL D (CROSS-CUTTING) | CPU scheduling time | All ARCH-001–010 require scheduling within the 20 ms cycle | Fixed-order scheduling per ARCH-015; ARCH-016 monitors deadline compliance |
| ARCH-002 state register | ASIL B(D) | State storage | ARCH-001 (write) and ARCH-003/005 (read) in same cycle | Write completes before reads are serviced; no torn read observed |

---

## V&V Coverage (IEEE 1012:2016 §5.6)

All 16 ARCH modules have at least one ITP. All key inter-module interfaces from the Interface
View are covered by at least one test case. No V&V gaps identified.

| ARCH | Interface(s) Exercised | ITP Coverage |
|------|----------------------|-------------|
| ARCH-001 | ARCH-009→ARCH-001 (condition), ARCH-001→ARCH-002 (write) | ITP-001-A, ITP-001-B |
| ARCH-002 | ARCH-001→ARCH-002 (write), ARCH-002→ARCH-001 (read), ARCH-002→ARCH-010 (CCF escalation) | ITP-002-A, ITP-002-B |
| ARCH-003 | ARCH-003→ARCH-006/011/013 (response sequence), ARCH-003→ARCH-011 (log data) | ITP-003-A, ITP-003-B |
| ARCH-004 | ARCH-009→ARCH-004 (arm), ARCH-006→ARCH-004 (stop), ARCH-004→ARCH-010 (timeout) | ITP-004-A, ITP-004-B |
| ARCH-005 | ARCH-002→ARCH-005 (authorisation), ARCH-005→ARCH-006 (command) | ITP-005-A |
| ARCH-006 | ARCH-003/010→ARCH-006 (cessation), ARCH-006→platform (write), ARCH-006→callers (ack) | ITP-006-A, ITP-006-B |
| ARCH-007 | Platform→ARCH-007 (5 inputs), ARCH-007→ARCH-008/009 (raw output) | ITP-007-A, ITP-007-B |
| ARCH-008 | ARCH-007→ARCH-008 (raw), ARCH-008→ARCH-009 (verified) | ITP-008-A, ITP-008-B |
| ARCH-009 | ARCH-009→ARCH-001/003/004/010 (conditions + event) | ITP-009-A, ITP-009-B |
| ARCH-010 | ARCH-009→ARCH-010 (fault), ARCH-010→ARCH-006 (cessation), ARCH-010→ARCH-001 (transition) | ITP-010-A, ITP-010-B |
| ARCH-011 | ARCH-003/010→ARCH-011 (request), ARCH-011→ARCH-012 (record) | ITP-011-A, ITP-011-B |
| ARCH-012 | ARCH-011→ARCH-012 (record), ARCH-012→platform (write), ARCH-012→ARCH-010 (CCF) | ITP-012-A, ITP-012-B |
| ARCH-013 | ARCH-003→ARCH-013 (request), ARCH-013→platform notification I/F | ITP-013-A, ITP-013-B |
| ARCH-014 | Process artifact completeness (inspection) | ITP-014-A |
| ARCH-015 | ARCH-015→ARCH-007 (tick), ARCH-015→all modules (order) | ITP-015-A, ITP-015-B |
| ARCH-016 | ARCH-NNN→ARCH-016 (completion), ARCH-016→ARCH-010 (CCF) | ITP-016-A, ITP-016-B |

**V&V gaps: None** — all 16 modules covered; all Interface View contracts exercised.

---

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total ARCH Modules | 16 |
| Total Integration Test Cases (ITP) | 32 |
| Total Integration Scenarios (ITS) | 36 |
| ARCH → ITP Coverage | 16/16 (100%) |
| ITP → ITS Coverage | 32/32 (100%) |
| V&V Gaps | 0 |

### Technique Distribution

| Technique | Count | Key Test Cases |
|-----------|-------|---------------|
| Interface Contract Testing (Internal) | 14 | ITP-001-A, 002-A, 003-A, 004-A, 005-A, 009-A/B, 010-A, 011-A, 015-A, 016-A |
| Interface Contract Testing (External) | 4 | ITP-006-A, 007-A, 012-A, 013-A |
| Interface Contract Testing (Inspection) | 1 | ITP-014-A |
| Data Flow Testing | 3 | ITP-003-B, 008-B, 011-B (boundary) |
| Interface Fault Injection | 8 | ITP-001-B, 002-B, 004-B, 007-B, 010-B, 012-B, 013-B, 016-B |
| Concurrency Testing | 2 | ITP-006-B, 015-B |
| **Total** | **32** | |

### ISO 26262 Safety Coverage

| Category | Count |
|----------|-------|
| HIL environment required | 4 test cases (ITP-006-A/B, 007-A/B, 012-A/B, 013-A/B) |
| SIL environment sufficient | 27 test cases |
| Inspection only | 1 test case (ITP-014-A) |
| Resource contention pairs covered | 4 |
