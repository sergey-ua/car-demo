# System Design: Vehicle Cruise Control — Brake Override Safety Slice

**Feature Branch**: `001-cruise-brake-override`
**Created**: 2026-06-01
**Status**: Approved
**Source**: `specs/001-cruise-brake-override/v-model/requirements.md`
**Domain Overlay**: ISO 26262 (ASIL D — **Confirmed**, OQ-001 resolved 2026-06-02)

---

## Overview

The Brake Override Safety Slice is decomposed into eight system components. Six are
runtime software modules — each with a well-bounded responsibility and explicit
interfaces — one is a cross-cutting process subsystem covering lifecycle and
compliance obligations, and one is a supervisory module for non-Brake_Override fault
conditions.

The decomposition follows these principles:
- **Safety-first partitioning**: the state machine, the override response coordinator,
  and the longitudinal output interface are separate components so that each can be
  independently tested, independently verified, and independently reviewed for
  safety integrity.
- **Single responsibility**: no component owns both the detection of a condition and
  the full response to it; detection (SYS-004, SYS-005) and state management
  (SYS-001) are separated to support independence arguments.
- **Platform boundary isolation**: all platform input reads (SYS-004) and platform
  output writes (SYS-003, SYS-006, SYS-007) are confined to dedicated components,
  minimising the interface surface for cybersecurity and integrity analysis.

No component introduces a capability not traceable to a requirement. All 31 active
requirements are covered. Derived requirements are listed at the end of this document.

---

## ID Schema

- **System Component**: `SYS-NNN` — sequential identifier, never renumbered.
- **Parent Requirements**: Comma-separated `REQ-NNN` list per component (many-to-many).
- Example: `SYS-001` with Parent Requirements `REQ-001, REQ-004` — this component
  satisfies both requirements.

---


## Operational States

| STATE | Description | Cruise Active? |
|-------|-------------|---------------|
| CRUISE_STANDBY | Feature available but not actively controlling speed | No |
| CRUISE_ACTIVE | Feature holds longitudinal authority and actively controls speed | Yes |
| CRUISE_SUSPENDED | Cruise temporarily suspended; reactivation possible | No |
| CRUISE_CANCELLED | Cruise cancelled; driver must explicitly restart | No |
| CRUISE_FAULT | Self-diagnosed fault; no speed control permitted | No |

## Decomposition View (IEEE 1016 §5.1)

| SYS ID | Name | Description | Parent Requirements | Type |
|--------|------|-------------|---------------------|------|
| SYS-001 | Cruise State Machine | Implements the closed five-state machine (`Cruise_Standby`, `Cruise_Active`, `Cruise_Suspended`, `Cruise_Cancelled`, `Cruise_Fault`). Evaluates entry and exit conditions for each state, enforces permitted transitions, and maintains the reactivation guard after Brake_Override. Activation preconditions per REQ-018 (DR-001 resolved 2026-06-02): (1) speed in valid range, (2) Brake_Override not active, (3) RII not active, (4) CCF not active, (5) LCU not active. Receives condition signals from SYS-004 and SYS-005 and issues state-change events consumed by SYS-002, SYS-003, SYS-005, SYS-006, and SYS-007. | REQ-001, REQ-004, REQ-005, REQ-007, REQ-008, REQ-010, REQ-011, REQ-018, REQ-CN-004 | Module |
| SYS-002 | Brake Override Response Coordinator | Coordinates the complete response to a confirmed Brake_Override event in `Cruise_Active`: commands speed control cessation (via SYS-003), commands authority release (via SYS-003), triggers notification (via SYS-007), and requests event log persistence (via SYS-006), all within the timing budget governed by REQ-NF-001. Acts as the sequencing authority for the override response. | REQ-002, REQ-003, REQ-012, REQ-013, REQ-NF-001 | Module |
| SYS-003 | Longitudinal Speed Controller | Issues longitudinal speed control commands exclusively to the propulsion command interface (REQ-IF-001) during `Cruise_Active`. Executes immediate cessation on command from SYS-002 (Brake_Override), SYS-005 (LCU, CCF), or SYS-001 (state exit). Enforces the constraint that no speed control commands are issued outside of `Cruise_Active`. Operates solely on the platform propulsion command interface. | REQ-002, REQ-006, REQ-009, REQ-IF-001, REQ-CN-001 | Module |
| SYS-004 | Platform Input Monitor | Continuously monitors all five platform-provided inputs: vehicle speed, brake pedal status, driver command input, diagnostic status, and longitudinal control availability (REQ-016). Validates the integrity of brake pedal status and driver command input per REQ-IF-002. Derives the four named conditions (Brake_Override, Required_Input_Invalid, Longitudinal_Control_Unavailable, Cruise_Control_Fault) and publishes them to SYS-001, SYS-002, and SYS-005. Implements the cybersecurity input integrity checks required by REQ-CN-003. | REQ-016, REQ-IF-002, REQ-IF-005, REQ-IF-006, REQ-IF-007, REQ-CN-001, REQ-CN-003, REQ-CN-007 | Module |
| SYS-005 | Fault and Condition Supervisor | Monitors the Longitudinal_Control_Unavailable, Cruise_Control_Fault, and Required_Input_Invalid conditions as delivered by SYS-004. Triggers the appropriate cessation and state transition responses: requests speed control cessation from SYS-003 and state transitions from SYS-001. Handles all safety conditions other than Brake_Override, which is handled by SYS-002. | REQ-006, REQ-007, REQ-008, REQ-009, REQ-010, REQ-011 | Module |
| SYS-006 | Safety Event Logger | Writes event records to the platform local persistent event storage (REQ-IF-004) on every safety-relevant state transition. Guarantees all five required fields (timestamp, from-state, triggering condition, to-state, vehicle speed) are written before the transition is considered complete. Provides the reconstruction guarantee required by REQ-015. | REQ-013, REQ-014, REQ-015, REQ-017, REQ-IF-004 | Service |
| SYS-007 | Notification Dispatcher | Receives notification requests from SYS-002 on Brake_Override transitions and delivers notification events to the platform notification interface (REQ-IF-003). Enforces exactly-once delivery per event. | REQ-012, REQ-IF-003 | Service |
| SYS-008 | Safety Process and Compliance | Cross-cutting process subsystem. Represents the ISO 26262-inspired lifecycle obligations (REQ-CN-002), the ISO/SAE 21434 cybersecurity assessment process (REQ-CN-003), the componentized architecture constraint (REQ-CN-004), the verification-capable toolchain constraint (REQ-CN-005), and the X-VERSE virtual validation requirement (REQ-CN-006). Also carries REQ-NF-002 (ASIL lifecycle depth). This component is not a runtime module; it is the set of development process obligations that bound the design and implementation of all other components. | REQ-NF-002, REQ-CN-002, REQ-CN-003, REQ-CN-004, REQ-CN-005, REQ-CN-006 | Subsystem |

---

## Dependency View (IEEE 1016 §5.2)

### Dependency Table

| Source | Target | Relationship | Failure Impact on Source |
|--------|--------|-------------|--------------------------|
| SYS-002 | SYS-001 | Reads current state to gate Brake_Override response to `Cruise_Active` only | If SYS-001 fails and reports wrong state, SYS-002 may miss a Brake_Override event or fire erroneously |
| SYS-002 | SYS-003 | Commands speed control cessation and authority release | If SYS-003 fails to execute cessation, longitudinal control is not released — critical failure |
| SYS-002 | SYS-004 | Receives confirmed Brake_Override event | If SYS-004 fails to detect or deliver Brake_Override, SYS-002 never activates — critical failure |
| SYS-002 | SYS-006 | Requests event log persistence with transition data | If SYS-006 fails, audit record is lost; override response still proceeds |
| SYS-002 | SYS-007 | Requests notification dispatch | If SYS-007 fails, notification is not issued; safety response (cessation) still proceeds |
| SYS-001 | SYS-004 | Receives condition signals (Brake_Override, RII, LCU, CCF) | If SYS-004 fails, SYS-001 cannot detect safety conditions — critical failure |
| SYS-001 | SYS-005 | Receives state transition requests for LCU, CCF, RII conditions | If SYS-005 fails, non-Brake_Override conditions are not handled — safety-relevant failure |
| SYS-003 | SYS-001 | Reads state authorization (only issues commands in `Cruise_Active`) | If SYS-001 state is corrupted, SYS-003 may issue commands in wrong states |
| SYS-005 | SYS-004 | Receives LCU, CCF, RII condition signals | If SYS-004 fails, SYS-005 cannot detect fault conditions — safety-relevant failure |
| SYS-005 | SYS-001 | Issues state transition requests | If SYS-001 rejects transition, fault condition is not handled in state |
| SYS-005 | SYS-003 | Commands speed control cessation on LCU and CCF | If SYS-003 fails, speed control continues under fault — critical failure |
| SYS-006 | SYS-002 | Receives log event requests (Brake_Override transitions) | If SYS-002 fails to deliver event data, log record is incomplete |
| SYS-006 | SYS-005 | Receives log event requests (LCU, CCF, RII transitions) | If SYS-005 fails to deliver event data, log record is incomplete |
| SYS-007 | SYS-002 | Receives notification dispatch requests | If SYS-002 fails to send request, no notification is issued |

### Dependency Diagram

```
Platform Inputs
  (speed, brake, driver cmd, diag, LCU availability)
          │
          ▼
    ┌─────────────┐
    │  SYS-004    │  Platform Input Monitor
    │  (Monitor)  │  — validates, derives conditions
    └──────┬──────┘
           │ Brake_Override / RII / LCU / CCF signals
     ┌─────┼──────────────────┐
     │     │                  │
     ▼     ▼                  ▼
┌────────┐ ┌────────────────┐ ┌──────────────┐
│SYS-002 │ │   SYS-001      │ │   SYS-005    │
│Override│◄│ State Machine  │◄│ Fault Super  │
│Coord.  │ │ (authorises    │ │ (handles LCU │
└───┬────┘ │  speed ctrl)   │ │  CCF, RII)   │
    │      └────────────────┘ └──────┬───────┘
    │   state change events          │
    │           ┌────────────────────┘
    │           │ cessation cmds
    ▼           ▼
┌─────────────────────┐
│      SYS-003        │  Longitudinal Speed Controller
│  (propulsion cmd    │─────────────► Platform: Propulsion I/F
│   interface owner)  │
└─────────────────────┘

SYS-002 ──► SYS-006 (Safety Event Logger) ─────────► Platform: Event Storage
SYS-005 ──►     │
                └── writes before transition completes

SYS-002 ──► SYS-007 (Notification Dispatcher) ──────► Platform: Notification I/F
```

---

## Interface View (IEEE 1016 §5.3)

### External Interfaces (Platform Boundary)

| Component | Interface Name | Direction | Data | Integrity / Error Handling |
|-----------|---------------|-----------|------|---------------------------|
| SYS-004 | Vehicle Speed Input | Read | Current vehicle speed (physical unit and scaling TBD at design phase) | Plausibility check (range, rate of change); out-of-range → Required_Input_Invalid |
| SYS-004 | Brake Pedal Status Input | Read | Pedal applied / released state | Integrity verification (mechanism TBD at design, per REQ-CN-003); integrity failure → Required_Input_Invalid |
| SYS-004 | Driver Command Input | Read | Cruise activation / deactivation / target commands | Integrity verification (mechanism TBD at design, per REQ-CN-003); integrity failure → Required_Input_Invalid |
| SYS-004 | Diagnostic Status Input | Read | Platform fault codes including Cruise_Control_Fault | Validity check; invalid → ignored with diagnostic counter increment |
| SYS-004 | Longitudinal Control Availability | Read | Boolean: available / unavailable | No integrity mechanism specified; absence of signal treated as unavailable |
| SYS-003 | Propulsion Command Interface | Write | Longitudinal speed control commands; cessation command | Exclusive use per REQ-IF-001; no commands issued outside `Cruise_Active`; cessation on override/fault |
| SYS-007 | Notification Interface | Write | Override event notification | One notification per override event; no retransmission specified |
| SYS-006 | Local Persistent Event Storage | Write | StateTransitionEvent records (5 fields) | Write must complete before transition is finalised; storage-full handling TBD (see OQ-007) |

### Internal Interfaces (Inter-Component)

| Source | Target | Interface Name | Data | Contract |
|--------|--------|---------------|------|----------|
| SYS-004 | SYS-001 | Condition Signal Bus | Brake_Override, Required_Input_Invalid, LCU, CCF (boolean per condition) | Published on change; SYS-001 processes on every cycle |
| SYS-004 | SYS-002 | Brake_Override Event | Confirmed Brake_Override with vehicle speed snapshot | Delivered once per confirmed event; SYS-002 is the sole consumer |
| SYS-004 | SYS-005 | Fault Condition Signals | LCU, CCF, RII (boolean per condition) | Published on change; SYS-005 processes on every cycle |
| SYS-001 | SYS-003 | Speed Control Authorisation | Active / Inactive state (binary) | SYS-003 only issues speed commands when SYS-001 reports `Cruise_Active` |
| SYS-002 | SYS-003 | Cessation Command | Immediate cessation (no data payload) | Issued once on Brake_Override; SYS-003 executes within the timing budget (OQ-002) |
| SYS-002 | SYS-006 | Log Event Request | StateTransitionEvent: from=`Cruise_Active`, condition=`Brake_Override`, to=`<target state>`, speed=snapshot, timestamp | SYS-006 must acknowledge write before SYS-002 considers transition complete |
| SYS-002 | SYS-007 | Notification Request | Event type = Brake_Override override | Exactly one request per Brake_Override event |
| SYS-005 | SYS-001 | State Transition Request | Condition (LCU, CCF, RII) and requested target state | SYS-001 validates the transition is permitted before applying |
| SYS-005 | SYS-003 | Cessation Command | Immediate cessation (for LCU and CCF conditions) | Same contract as SYS-002 → SYS-003; executed independently |
| SYS-005 | SYS-006 | Log Event Request | StateTransitionEvent for LCU, CCF, RII transitions | Same contract as SYS-002 → SYS-006 |

---

## Data Design View (IEEE 1016 §5.4)

| Entity | Owner Component | Storage Medium | Protection at Rest | Protection in Transit | Retention Policy |
|--------|----------------|----------------|-------------------|-----------------------|-----------------|
| `CruiseControlState` | SYS-001 | In-memory (RAM); no persistence required | Memory region accessible only to SYS-001 (spatial isolation per FFI requirements) | Passed by value on internal interfaces; no external exposure | Discarded at component reset; no persistent copy |
| `ControlCondition` (Brake_Override, RII, LCU, CCF) | SYS-004 (derived) | In-memory; updated per monitoring cycle | Not persisted; derived from validated platform inputs only | Passed by value on internal interfaces | Discarded at next evaluation cycle |
| `StateTransitionEvent` | SYS-006 (persisted) | Platform local persistent event storage | Integrity protection of storage medium is a platform responsibility; this component writes complete records only | Integrity of write confirmed before transition completes (write acknowledgement required) | Retention policy subject to OQ-007; minimum: available for duration of test session |
| `ActivationPrecondition` (set) | SYS-001 | In-memory; re-evaluated from SYS-004 inputs each cycle | Same as `CruiseControlState` spatial isolation | Internal to SYS-001; not exposed to other components | Re-evaluated live; no persistence |
| Platform Input Snapshots (speed, brake status) | SYS-004 | In-memory; per-cycle capture | Not persisted; raw inputs validated before derivation | Validated before passing to derived conditions; raw values not forwarded externally | Discarded at next monitoring cycle; vehicle speed snapshot included in `StateTransitionEvent` |

---

## Safety-Critical Design Sections (ISO 26262 Overlay)

### Freedom from Interference (ISO 26262-6 §7.4.8)

All runtime components carry **confirmed** ASIL D (OQ-001 resolved 2026-06-02). The primary FFI concern is inter-component spatial and temporal isolation within the same ASIL domain. The table below documents required
isolation mechanisms; the exact implementation (e.g., memory protection unit
configuration, scheduling partitioning) is subject to confirmation at the architecture
and module design phases (OQ-004).

| Component | ASIL (Confirmed) | Spatial Isolation | Temporal Isolation | Communication Protection | Verification Method |
|-----------|-------------------|------------------|--------------------|--------------------------|---------------------|
| SYS-001 | ASIL D | Dedicated data segment for `CruiseControlState`; no write access from other components | Bounded execution time per scheduling period; execution budget to be defined when ASIL confirmed (OQ-001) | Condition signals received by value; no shared writable state with SYS-002, SYS-004, SYS-005 | Analysis (design review) + Inspection of memory map |
| SYS-002 | ASIL D | Dedicated data segment; no shared writable data with other runtime components | Same constraint as SYS-001; timing budget TBD (OQ-002) | Cessation command and log request interfaces are unidirectional (write-only from SYS-002's perspective) | Analysis + Test (timing measurement, ATP-NF-001-A/B) |
| SYS-003 | ASIL D | Dedicated data segment; propulsion interface write access exclusive to this component | Execution bounded to single scheduling slot per command | Authorisation received by value from SYS-001; cessation commands received from SYS-002 and SYS-005 by value | Analysis + Inspection (interface exclusivity) |
| SYS-004 | ASIL D | Platform input capture in dedicated data segment; derived condition outputs passed by value | Monitoring cycle must complete within one scheduling period | Input integrity failure mapped to `Required_Input_Invalid` before any condition is published | Analysis + Fault Injection Test (ATP-010-FI, ATP-011-FI) |
| SYS-005 | ASIL D | Dedicated data segment; fault condition state local to component | Bounded by scheduling period | Condition signals received by value from SYS-004; transition requests sent by value to SYS-001 | Analysis + Test |
| SYS-006 | ASIL D | Event record constructed in dedicated buffer; write to platform storage completes before buffer is released | Write must complete within the transition's timing budget | Write acknowledgement required; no partial records | Analysis + Test (ATP-013-B) |
| SYS-007 | ASIL D | Notification request buffer local to component | Non-blocking; if interface is temporarily unavailable, behaviour is TBD pending OQ-007 resolution | One notification per event; no re-send without new event | Analysis + Test (ATP-012-B) |

### Restricted Complexity (ISO 26262-6 §7.4.9)

The following complexity targets apply to the runtime components. These are design-phase
targets; conformance is verified during implementation and confirmed by static analysis
(per REQ-CN-005).

| Component | ASIL (Confirmed) | Cyclomatic Complexity Target | Nesting Depth Target | Notes |
|-----------|-------------------|-----------------------------|----------------------|-------|
| SYS-001 | ASIL D | ≤ 15 per function | ≤ 4 | State machine transition logic must be decomposed into per-condition handler functions |
| SYS-002 | ASIL D | ≤ 15 per function | ≤ 4 | Sequencing logic must not fold detection and response into a single function |
| SYS-003 | ASIL D | ≤ 10 per function | ≤ 3 | Output interface logic is expected to be simple; low threshold reinforces this |
| SYS-004 | ASIL D | ≤ 15 per function | ≤ 4 | Integrity checking functions may be table-driven to remain within complexity limits |
| SYS-005 | ASIL D | ≤ 15 per function | ≤ 4 | One handler function per condition (LCU, CCF, RII) is the recommended decomposition |
| SYS-006 | ASIL D | ≤ 10 per function | ≤ 3 | Logger is a service with a simple write path; low threshold reinforces linear flow |
| SYS-007 | ASIL D | ≤ 8 per function | ≤ 3 | Dispatcher has a single-path responsibility; very low threshold is appropriate |

### Safety Mechanisms Allocation (ISO 26262 Part 6 §6.5)

Safety mechanisms are allocated below from the mechanism tags in `requirements.md`.
Fault reaction time is the maximum elapsed time from fault detection to a confirmed safe
state; the exact budget for SYS-002's reaction time is pending OQ-002.

| Component | ASIL | Safety Mechanism | Type | Fault Addressed | Diagnostic Coverage | Fault Reaction Time |
|-----------|------|-----------------|------|-----------------|--------------------|--------------------|
| SYS-001 | ASIL D | State variable range check — verify state value is within the defined set of 5 before any transition | Prevention | State variable corruption (single-bit fault) | High (detects out-of-range state immediately) | One scheduling cycle |
| SYS-001 | ASIL D | Reactivation guard — refuse activation until all preconditions valid | Prevention | Unintended cruise re-engagement after override | Medium (depends on precondition evaluation completeness) | Immediate (evaluated every cycle) |
| SYS-002 | ASIL D | Override response sequencing — enforce cessation → authority release → log → notify order | Mitigation | Incomplete override response (missed step) | High (sequencing enforced by design; each step confirmed) | ≤ `<T_max>` from OQ-002 |
| SYS-002 | ASIL D | Response timing supervision — monitor elapsed time from Brake_Override detection to cessation | Detection | Delayed override response | High (measured each override event, compared to budget) | Immediate on budget expiry |
| SYS-003 | ASIL D | Exclusive interface guard — block speed commands if state is not `Cruise_Active` | Prevention | Speed control issued in wrong state | High (checked on every command) | Immediate |
| SYS-003 | ASIL D | Cessation command execution — override any in-progress command on cessation request | Mitigation | Partial speed correction continuing after override | High (command superseded unconditionally) | Within one control cycle |
| SYS-004 | ASIL D | Input integrity verification — validate brake pedal status and driver command inputs before condition derivation | Prevention | Corrupted or tampered input processed as valid Brake_Override | High (applied to all safety-critical inputs on every cycle) | One monitoring cycle |
| SYS-004 | ASIL D | Condition plausibility check — validate input values against defined ranges and rates of change | Detection | Sensor fault or out-of-range input mistaken for valid condition | Medium (range and rate checks; cross-channel not yet specified) | One monitoring cycle |
| SYS-005 | ASIL D | Fault condition detection and routing — detect CCF, LCU, RII and trigger appropriate component responses | Detection + Mitigation | Fault conditions not responded to | Medium (each condition independently detected; combinations tested via ATP-008-B) | One scheduling cycle |
| SYS-006 | ASIL D | Write-before-complete — event record write confirmed before transition state is finalised | Detection | Transition recorded but log entry absent (lost audit trail) | High (write confirmation is synchronous; verified by ATP-013-B) | Bounded to write completion time |

---

## Quality Attribute Coverage (ISO/IEC 25010:2023)

| Quality Characteristic | ISO/IEC 25010 Ref | Design Evidence |
|------------------------|-------------------|-----------------|
| Functional Suitability | §4.2.1 | All 31 REQs mapped to at least one SYS component; every SYS component is directly justified by a REQ |
| Reliability | §4.2.2 | Dependency View documents failure propagation for all inter-component paths; SYS-001 state variable range check prevents undefined states; SYS-003 cessation is unconditional on override/fault |
| Performance Efficiency | §4.2.3 | REQ-NF-001 (timing budget) carried by SYS-002; response timing supervision mechanism in SYS-002; timing budget pending OQ-002 — `[QUALITY GAP: ISO 25010 §4.2.3 — exact timing bound not yet specified; SYS-002 mechanism is defined but pass threshold is pending OQ-002]` |
| Security | §4.2.5 | SYS-004 implements input integrity verification and plausibility checks (REQ-CN-003); Data Design View documents isolation of in-memory data; platform storage integrity is a platform responsibility |
| Maintainability | §4.2.7 | Eight explicitly bounded components with single-responsibility ownership; Interface View defines explicit contracts for all inter-component communication; Restricted Complexity targets enforce testability |
| Safety | §4.2.9 | ISO 26262 safety mechanism allocation table covers all ASIL D runtime components; FFI analysis documents spatial and temporal isolation; safety mechanisms traceable to requirements MECHANISM tags |
| Compatibility | §4.2.4 | SYS-004 and SYS-003 are confined to the seven authorised platform interfaces (REQ-CN-001); no new hardware interfaces introduced |
| Flexibility | §4.2.8 | Componentized architecture (REQ-CN-004) supports independent component replacement; interface contracts isolate change propagation |

---

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total System Components | 8 (7 runtime + 1 process) |
| By Type | Module: 5 (SYS-001–005) · Service: 2 (SYS-006–007) · Subsystem: 1 (SYS-008) |
| REQ → SYS Forward Coverage | 31/31 (100%) |
| External Platform Interfaces | 8 (5 read, 3 write) |
| Internal Interfaces | 11 (see Interface View) |
| Dependency Relationships | 14 (see Dependency View) |
| Deprecated Components | 0 |
| Suspect Items | 0 |
| Quality Gaps Flagged | 1 (ISO 25010 §4.2.3 — timing bound pending OQ-002) |
| Derived Requirements Flagged | 2 (see below) |

---

## Derived Requirements

The following architectural needs were identified during decomposition. They are not
explicitly stated in `requirements.md` and must be resolved by the Safety Engineer
and Vehicle Software Architect before the system test plan is generated.

### DR-001: Activation Precondition Registry

**Architectural need**: SYS-001 (Cruise State Machine) enforces REQ-005 — it must
refuse reactivation until all activation preconditions are satisfied. This requires
a defined, finite set of preconditions. The requirements state the precondition set
is "to be defined during the design phase" (spec.md Assumptions). Without this set,
SYS-001 cannot be fully specified or tested.

**Resolution options**:
1. Add a new REQ defining the activation precondition set and assign it to SYS-001.
2. Confirm the preconditions are defined at a higher system level (outside this feature's scope) and document the external reference.
3. Defer until OQ-004 (component architecture) is confirmed.

### DR-002: Write-Failure Handling for Event Logger

**Architectural need**: SYS-006 (Safety Event Logger) requires a write acknowledgement
before transitions are finalised (per REQ-013). The requirements do not specify what
SYS-006 should do if the platform storage write fails or times out. This is needed to
determine whether a write failure constitutes a Cruise_Control_Fault or a handled
degradation.

**Resolution options**:
1. Add a new REQ specifying that a write failure triggers `Cruise_Control_Fault` (entering `Cruise_Fault`).
2. Add a new REQ specifying graceful degradation (transition proceeds without log entry, with a diagnostic counter increment).
3. Define the behaviour as a design decision in the architecture phase without a separate requirement, documenting the justification.

---

## Glossary

| Term | Definition |
|------|-----------|
| Cessation command | A command issued to the propulsion command interface that terminates active longitudinal speed control output |
| Condition signal | A boolean signal derived by SYS-004 indicating the presence or absence of a named condition (Brake_Override, RII, LCU, CCF) |
| FFI | Freedom from Interference — the property that a failure in one software component cannot corrupt the data, execution, or output of another |
| Spatial isolation | Separation of writable data between components, preventing one component from overwriting another's data |
| Temporal isolation | Separation of execution time between components, preventing one component's execution from delaying another's |
| State transition event | A persistent record of a safety-relevant state change; minimum content defined by REQ-014 |
| Write-before-complete | The property that an event record is confirmed written to storage before the state transition that generated it is considered finalised |
| LCU | Longitudinal_Control_Unavailable — the platform condition indicating longitudinal control is not available |
| CCF | Cruise_Control_Fault — the self-diagnosed fault condition derived from the diagnostic status input |
| RII | Required_Input_Invalid — the condition derived when a required input fails integrity or validity verification |
