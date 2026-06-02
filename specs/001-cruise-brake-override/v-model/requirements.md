# V-Model Requirements Specification: Vehicle Cruise Control — Brake Override Safety Slice

**Feature Branch**: `001-cruise-brake-override`
**Created**: 2026-06-01
**Status**: Approved
**Source**: `specs/001-cruise-brake-override/spec.md`

---

## Overview

During controlled prototype road trials, the Vehicle Cruise Control feature did not
always transition clearly to manual driver control when the driver applied the brake.
Some transitions were not logged with sufficient evidence to reconstruct the event.

This requirements specification formalises the traceable requirements for the Brake
Override Safety Slice: the software behaviour when a `Brake_Override` occurs while
cruise is in `Cruise_Active`. Requirements are extracted from `spec.md` under strict
translation rules — no requirement is invented that is not present in the source.

**Domain overlay**: ISO 26262 (road vehicles — functional safety)
**ASIL target** (from `v-model-config.yml`): D — **Confirmed** (OQ-001 resolved 2026-06-02). All safety requirements carry the tag
`[ASIL D — Confirmed]`.

**ISO 26262 tagging convention used throughout**:

| Tag | Meaning |
|-----|---------|
| `[ASIL D — Confirmed]` | **Confirmed** ASIL D — formal classification confirmed by hazard analysis (OQ-001 resolved 2026-06-02) |
| `[MECHANISM: Detection]` | Safety mechanism that detects a fault or safety-relevant event |
| `[MECHANISM: Prevention]` | Safety mechanism that prevents an unsafe state from being entered |
| `[MECHANISM: Mitigation]` | Safety mechanism that limits the effect of a fault or unsafe event |
| `[DERIVED]` | Requirement not directly traceable to an explicit source statement |

No `[DERIVED]` requirements are present in this version — all requirements trace
directly to `spec.md`.

---

## Requirements

### Functional Requirements

> **Note**: OQ-003 resolved 2026-06-02 — post-override non-active target state is `Cruise_Cancelled` (all three conditions: Brake_Override, LCU, RII).

| ID | Description | Priority | Rationale | Verification |
|----|-------------|----------|-----------|--------------|
| REQ-001 | The feature SHALL implement a closed state machine comprising exactly five discrete operational states — `Cruise_Standby`, `Cruise_Active`, `Cruise_Suspended`, `Cruise_Cancelled`, and `Cruise_Fault` — each with defined entry conditions, exit conditions, and permitted transitions. `[ASIL D — Confirmed]` `[MECHANISM: Prevention]` | P1 | Establishes the observable state model against which all other requirements are tested; no undefined state may be reachable. Source: FR-001 | Inspection |
| REQ-002 | When the feature is in `Cruise_Active` and `Brake_Override` is confirmed, the feature SHALL cease all longitudinal speed control output to the propulsion command interface. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | P1 | Core safety action — prevents the feature from continuing to accelerate or maintain speed when the driver asserts manual braking. Source: FR-002 / C-002 / US-1 | Test |
| REQ-003 | When `Brake_Override` is confirmed in `Cruise_Active`, the feature SHALL release longitudinal control authority to the driver or to an authorised vehicle function as the direct consequence of the cessation in REQ-002. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | P1 | Cessation of commands alone does not confirm authority handover; explicit release is required so that ambiguity in control ownership is eliminated. Source: FR-003 / C-002 / US-1 | Test, Demonstration |
| REQ-004 | Following `Brake_Override` in `Cruise_Active`, the feature SHALL transition to `Cruise_Cancelled`. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | P1 | OQ-003 resolved 2026-06-02: `Cruise_Cancelled` provides clear semantics (session ended, no inherited speed target) and simplifies the reactivation guard. Source: FR-004 / US-1 | Test |
| REQ-005 | Following a transition out of `Cruise_Active` due to `Brake_Override`, the feature SHALL refuse all cruise activation requests until every activation precondition is confirmed valid. `[ASIL D — Confirmed]` `[MECHANISM: Prevention]` | P1 | Prevents unintended reactivation of longitudinal speed control before the vehicle is in a safe state for cruise. Source: FR-005 / US-3 | Test |
| REQ-006 | When `Longitudinal_Control_Unavailable` is detected while the feature is in `Cruise_Active`, the feature SHALL cease all longitudinal speed control output to the propulsion command interface. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | P1 | If the platform withdraws longitudinal control availability, continued speed control commands would be unsafe. Source: FR-006 | Test |
| REQ-007 | When `Longitudinal_Control_Unavailable` is detected while the feature is in `Cruise_Active`, the feature SHALL transition to `Cruise_Cancelled`. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | P1 | Ensures a deterministic, safe mode when the platform retracts control authority. Source: FR-006 | Test |
| REQ-008 | When `Cruise_Control_Fault` is detected, the feature SHALL transition to the `Cruise_Fault` state regardless of its current operational state. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | P1 | Fault containment — `Cruise_Fault` is the designated state for self-diagnosed fault conditions; no other post-fault state is defined. Source: FR-006 / REQ-001 | Test |
| REQ-009 | When `Cruise_Control_Fault` is detected while the feature is in `Cruise_Active`, the feature SHALL cease all longitudinal speed control output to the propulsion command interface. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | P1 | A self-diagnosed fault must not leave speed control commands active; this requirement makes cessation explicit regardless of the transition in REQ-008. Source: FR-006 | Test |
| REQ-010 | The feature SHALL NOT transition to `Cruise_Active` when `Required_Input_Invalid` is detected. `[ASIL D — Confirmed]` `[MECHANISM: Prevention]` | P1 | Prevents the feature from entering active speed control on data that may be corrupted or manipulated. Source: FR-007 / US-4 | Test |
| REQ-011 | When `Required_Input_Invalid` is detected while the feature is in `Cruise_Active`, the feature SHALL transition to `Cruise_Cancelled`. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | P1 | Cruise_Active must not be maintained on untrustworthy inputs; this may indicate sensor corruption or a cybersecurity integrity failure. Source: FR-007 / US-4 | Test |
| REQ-012 | When `Brake_Override` causes a transition out of `Cruise_Active`, the feature SHALL issue a notification event to the notification interface provided by the EV prototype platform. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | P1 | Confirms to the driver and vehicle systems that cruise has been overridden and control has been returned; content and format are owned by the notification interface provider. Source: FR-008 / US-1 | Test |
| REQ-013 | When any safety-relevant state transition occurs, the feature SHALL persist an event record to the local persistent event storage provided by the EV prototype platform before the transition is considered complete. `[ASIL D — Confirmed]` `[MECHANISM: Detection]` | P1 | The triggering observation for this feature was that prior brake override transitions could not be reconstructed from the log; persistent records are required for safety audit and evidence. Source: FR-009 / US-2 | Test |
| REQ-014 | Each event record persisted under REQ-013 SHALL contain all five of the following fields: (1) event timestamp, (2) the state from which the transition occurred, (3) the condition that triggered the transition, (4) the state to which the transition occurred, (5) vehicle speed at the time of the event. `[ASIL D — Confirmed]` | P1 | These five fields are the minimum set that allows an auditor to reconstruct any transition without reference to any data source other than the log. Source: FR-010 / US-2 | Inspection, Test |
| REQ-015 | The event log records produced under REQ-013 and REQ-014 SHALL be sufficient for an auditor to reconstruct the complete state transition history of the feature from the log alone, without reference to any external data source. `[ASIL D — Confirmed]` | P1 | Directly addresses the test observation that prior transitions could not be reconstructed. Source: FR-011 / US-2 | Demonstration |
| REQ-016 | The feature SHALL continuously monitor all of the following inputs provided by the EV prototype platform: vehicle speed, brake pedal status, driver command input, diagnostic status, and longitudinal control availability. `[ASIL D — Confirmed]` `[MECHANISM: Detection]` | P1 | These five inputs are the minimum set required to detect all four named conditions (Brake_Override, Required_Input_Invalid, Longitudinal_Control_Unavailable, Cruise_Control_Fault). Source: FR-012 | Inspection, Test |
| REQ-017 | When the feature's event logging function fails to write an event record to the platform local persistent event storage, the feature SHALL treat the write failure as a `Cruise_Control_Fault` condition and transition to `Cruise_Fault`. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | P1 | DR-002 resolved 2026-06-02: a missing audit record during a safety-relevant transition is an unrecoverable evidence gap; entering `Cruise_Fault` is the appropriate safe state response under ASIL D. Source: DR-002 / SYS-006 / ARCH-012 | Test (STP-006-C — fault injection: write failure) |
| REQ-018 | Before permitting any transition from `Cruise_Standby` to `Cruise_Active`, the feature SHALL evaluate and confirm that all five of the following activation preconditions are simultaneously satisfied: (1) vehicle speed is within the defined activation speed range, (2) `Brake_Override` is not active, (3) `Required_Input_Invalid` is not active, (4) `Cruise_Control_Fault` is not active, (5) `Longitudinal_Control_Unavailable` is not active. If any precondition is not satisfied, the transition SHALL be refused. `[ASIL D — Confirmed]` `[MECHANISM: Prevention]` | P1 | DR-001 resolved 2026-06-02: defines the five activation preconditions evaluated by ARCH-001 (reactivation guard). Source: REQ-005 / SYS-001 / ARCH-001 | Test (STP-005-A/B/C) |

---

### Non-Functional Requirements

| ID | Description | Priority | Rationale | Verification |
|----|-------------|----------|-----------|--------------|
| REQ-NF-001 | The elapsed time from confirmation of `Brake_Override` to cessation of longitudinal speed control output (REQ-002) and release of control authority (REQ-003) SHALL not exceed **100 milliseconds**. `[ASIL D — Confirmed]` | P1 | OQ-002 resolved 2026-06-02: T_max = 100 ms confirmed by Safety Engineer and Vehicle Software Architect. Delayed handling is unacceptable per C-002; the 100 ms bound limits stopping-distance increase at motorway speeds and accommodates ASIL D cyclic scheduling with a 5× safety margin. Source: C-002 / FR-002 | Test (100 consecutive runs under worst-case platform load; all latencies ≤ 100 ms) |
| REQ-NF-002 | The development lifecycle for this feature SHALL conform to **ASIL D** per ISO 26262-6 (OQ-001 resolved 2026-06-02). Required methods include: MC/DC structural coverage (100%) for all software units; independent review of all safety-critical design and test artifacts; documented independence justification for each ASIL B(D)+B(D) decomposition pair (ARCH-001/002 and ARCH-003/004); and independent safety assessment of the complete lifecycle. `[ASIL D — Confirmed]` | P1 | OQ-001 resolved 2026-06-02: ASIL D confirmed as the formal safety integrity level. Source: C-003 / FR-013 / hazard-analysis.md | Inspection (independent safety lifecycle audit) |

---

### Interface Requirements

| ID | Description | Priority | Rationale | Verification |
|----|-------------|----------|-----------|--------------|
| REQ-IF-001 | The feature SHALL issue all longitudinal speed control commands exclusively through the propulsion command interface provided by the existing EV prototype platform. The feature SHALL NOT use any other interface to influence vehicle longitudinal motion. `[ASIL D — Confirmed]` | P1 | The propulsion command interface is the designated output channel; other channels are not within this feature's scope and using them would be unsafe. Source: C-001 / FR-012 | Inspection |
| REQ-IF-002 | The feature SHALL derive the `Brake_Override` and `Required_Input_Invalid` conditions from the brake pedal status and driver command inputs provided by the existing EV prototype platform. `[ASIL D — Confirmed]` `[MECHANISM: Detection]` | P1 | These platform inputs are the only authorised sources for the conditions that trigger safety-critical state transitions. Source: C-001 / FR-012 / US-4 | Inspection, Test |
| REQ-IF-003 | When issuing a notification under REQ-012, the feature SHALL deliver the notification through the notification interface provided by the existing EV prototype platform. `[ASIL D — Confirmed]` | P1 | The notification interface is the designated channel for driver and vehicle interface communication; content and format are outside this feature's scope. Source: C-001 / FR-008 | Test |
| REQ-IF-004 | When persisting event records under REQ-013, the feature SHALL write records to the local persistent event storage provided by the existing EV prototype platform. `[ASIL D — Confirmed]` | P1 | The platform already provides local persistent storage; the feature must use it rather than a separate or private store. Source: C-001 / FR-009 | Test |
| REQ-IF-005 | The feature SHALL receive all driver cruise activation and deactivation requests from the driver command input provided by the existing EV prototype platform. `[ASIL D — Confirmed]` | P1 | Driver command input is the authorised source for activation precondition checking and cruise control intent. Source: C-001 / FR-012 | Inspection, Test |
| REQ-IF-006 | The feature SHALL receive vehicle speed data from the vehicle speed input provided by the existing EV prototype platform. `[ASIL D — Confirmed]` | P1 | Vehicle speed is both an activation precondition input and a mandatory field in event records (REQ-014). Source: C-001 / FR-012 | Inspection, Test |
| REQ-IF-007 | The feature SHALL receive diagnostic status information from the diagnostic status input provided by the existing EV prototype platform, and SHALL use this as the source for determining the `Cruise_Control_Fault` condition. `[ASIL D — Confirmed]` `[MECHANISM: Detection]` | P1 | Diagnostic status drives the fault detection path to `Cruise_Fault`; using the platform's authorised diagnostic channel ensures consistency with other vehicle systems. Source: C-001 / FR-012 | Inspection, Test |

---

### Constraint Requirements

| ID | Description | Priority | Rationale | Verification |
|----|-------------|----------|-----------|--------------|
| REQ-CN-001 | The feature SHALL operate exclusively using the platform interfaces listed in REQ-IF-001 through REQ-IF-007. The feature SHALL NOT introduce new hardware interfaces, additional communication channels, or dependencies not provided by the existing EV prototype platform. | P1 | The EV prototype platform is established; introducing new hardware dependencies is out of scope and would require separate qualification activities. Source: C-001 / FR-016 | Inspection |
| REQ-CN-002 | The complete development lifecycle of this feature — hazard analysis, safety requirements, design, implementation, verification, and validation — SHALL produce artifacts in accordance with an ISO 26262-inspired functional safety process appropriate to the prototype development context. ASIL D has been confirmed as the feature's safety integrity level (OQ-001 resolved 2026-06-02). | P1 | ISO 26262-inspired lifecycle compliance is a hard stakeholder constraint and a prerequisite for controlled field testing approval. Source: C-003 / FR-013 | Inspection (safety lifecycle audit) |
| REQ-CN-003 | The cybersecurity-relevant interfaces and inputs used by this feature — specifically brake pedal status, driver command input, and vehicle communication interfaces — SHALL be subject to a threat analysis and treatment process following an ISO/SAE 21434-inspired approach. The threat treatment output SHALL be documented and incorporated into the feature design before implementation begins. | P1 | These inputs have a defined cybersecurity attack surface; US-4 confirms that integrity-compromised inputs must be handled safely, and this constraint mandates the supporting evidence. Source: C-004 / FR-014 / US-4 | Inspection |
| REQ-CN-004 | The feature SHALL be structured as a set of explicitly defined software components with documented boundaries, responsibilities, and interfaces, compatible with automotive-style embedded development. No specific middleware, operating environment, or vendor framework is prescribed. The component architecture is defined in `architecture-design.md` (16 ARCH modules; OQ-004 resolved). | P1 | Componentized architecture supports independent verification and traceability across the V-Model; it is required for ASIL-appropriate evidence generation. Source: C-005 / FR-016 | Inspection |
| REQ-CN-005 | The implementation language and toolchain used to develop this feature SHALL natively support: (a) static analysis, (b) coding-standard rule enforcement, and (c) automated unit and integration verification. The implementation language is **C** with the **MISRA C:2012** coding standard (OQ-005 resolved 2026-06-02). Static analysis tool selection is deferred to the build configuration phase. | P1 | Verification-capable tooling is a prerequisite for generating ASIL-appropriate evidence; this is a hard stakeholder constraint, not a recommended practice. Source: C-006 / FR-017 | Inspection |
| REQ-CN-006 | The verification and validation plan for this feature SHALL include at least one virtual validation activity using X-VERSE as the designated virtual validation environment. The specific X-VERSE scenario configuration is subject to definition (OQ-006 in spec.md). | P1 | X-VERSE is the designated downstream virtual validation environment; validation evidence from X-VERSE is required in the evidence package before controlled field testing. Source: C-007 / FR-015 | Inspection (evidence review) |
| REQ-CN-007 | The integrity verification mechanism applied by the Input Integrity Verifier (ARCH-008) to brake pedal status and driver command inputs SHALL implement CRC-16 error detection combined with a 4-bit rolling counter for replay detection. Inputs that fail the CRC check or present a non-sequential counter value SHALL be treated as `Required_Input_Invalid`. `[ASIL D — Confirmed]` `[MECHANISM: Prevention]` | P1 | OQ resolved 2026-06-02: CRC-16 provides ASIL D–appropriate protection against single-bit and burst errors; the 4-bit rolling counter prevents simple replay attacks identified by the ISO/SAE 21434 threat analysis. Source: REQ-CN-003 / REQ-IF-002 / ARCH-008 | Test (STP-004-A — integrity validation), Inspection (STP-008-B — cybersecurity verification) |

---

## Assumptions

- The existing EV prototype platform reliably delivers all inputs and interfaces
  referenced in REQ-IF-001 through REQ-IF-007. These platform services are consumed
  as-is; their qualification is outside the scope of this feature.
- `Cruise_Active` implies that the feature holds active longitudinal control authority
  at the point `Brake_Override` is detected.
- The exact non-active target state for `Brake_Override` (OQ-003) and the target
  states for `Longitudinal_Control_Unavailable` and `Required_Input_Invalid` exits
  will all be resolved together in the design phase; the same state or states may
  apply to all three conditions.
- Activation preconditions (the conditions that must be satisfied before `Cruise_Active`
  may be entered) will be formally defined during the design phase. REQ-005 requires
  they are enforced; their content is out of scope for this document.
- A cybersecurity threat analysis of brake pedal status, driver command input, and
  vehicle communication interfaces will be conducted as a parallel work item. Findings
  will be incorporated into the design before implementation.
- X-VERSE scenario configuration will be owned by the Test Engineer after the design
  is baselined; scenario details are outside this requirements document.
- The event log storage medium uses a **fixed-capacity oldest-first circular overwrite** policy (OQ-007 resolved 2026-06-02). When storage capacity is reached, the oldest entry is overwritten; no write failure is generated on a full-storage write. Write failures escalating per REQ-017 are reserved for I/O errors only.

---

## Dependencies

| Dependency | Type | Notes |
|------------|------|-------|
| EV prototype platform — vehicle speed input | Runtime input | Required by REQ-IF-006 and REQ-016 |
| EV prototype platform — brake pedal status | Runtime input | Required by REQ-IF-002 and REQ-016 |
| EV prototype platform — driver command input | Runtime input | Required by REQ-IF-002, REQ-IF-005, and REQ-016 |
| EV prototype platform — propulsion command interface | Runtime output | Required by REQ-IF-001; target for cessation in REQ-002, REQ-006, REQ-009 |
| EV prototype platform — braking command interface | Runtime interface | Provided by platform; usage within this feature to be defined at design phase |
| EV prototype platform — diagnostic status | Runtime input | Required by REQ-IF-007 and REQ-016 |
| EV prototype platform — notification interface | Runtime output | Required by REQ-IF-003; target for REQ-012 |
| EV prototype platform — local persistent event storage | Runtime output | Required by REQ-IF-004; target for REQ-013 |
| X-VERSE virtual validation environment | Validation | Required by REQ-CN-006; configuration owned by Test Engineer |
| Hazard analysis (to be produced) | Upstream safety artifact | Required to confirm ASIL level and satisfy REQ-NF-002 / REQ-CN-002 |
| Cybersecurity threat analysis (to be produced) | Upstream security artifact | Required to satisfy REQ-CN-003; inputs defined in US-4 |

---

## Glossary

| Term | Definition |
|------|-----------|
| `Brake_Override` | The condition in which the driver applies the brake pedal while `Cruise_Active` is the current state; detected from brake pedal status input |
| `Cruise_Active` | The operational state in which the feature holds longitudinal control authority and actively maintains vehicle speed toward a cruise target |
| `Cruise_Cancelled` | A defined non-active state indicating that cruise has been explicitly cancelled; exact use after `Brake_Override` is subject to OQ-003 |
| `Cruise_Control_Fault` | The condition indicating a self-diagnosed software or system fault within the cruise control feature; derived from diagnostic status input |
| `Cruise_Fault` | The operational state entered when `Cruise_Control_Fault` is detected; no longitudinal speed control is permitted in this state |
| `Cruise_Standby` | The operational state in which the feature is available but not actively controlling speed; activation preconditions may be checked here |
| `Cruise_Suspended` | A defined non-active state indicating that cruise has been temporarily suspended; exact use after `Brake_Override` is subject to OQ-003 |
| `Longitudinal_Control_Unavailable` | The condition indicating that the platform or an authorised vehicle function has retracted the availability of longitudinal control |
| `Required_Input_Invalid` | The condition indicating that one or more required inputs (brake pedal status, driver command, vehicle speed) has failed integrity or validity verification |
| ASIL | Automotive Safety Integrity Level — a risk classification defined by ISO 26262, ranging from A (lowest) to D (highest) |
| Safety mechanism | A software behaviour that detects, prevents, or mitigates a safety-relevant fault or hazardous event |
| Longitudinal control authority | The right of a software function to issue commands that accelerate, decelerate, or maintain vehicle speed |

---

**Total Requirements**: 34 (34 active, 0 deprecated)

**By category**: Functional: 18 | Non-Functional: 2 | Interface: 7 | Constraint: 7

**By priority**: P1: 31 | P2: 0 | P3: 0

**By verification method**: Test: 20 | Inspection: 21 | Analysis: 0 | Demonstration: 2
*(Several requirements have multiple verification methods; counts reflect all methods assigned)*

**Open flags**:
- `[NEEDS CLARIFICATION]`: 0 — all open questions resolved
- `[RESOLVED 2026-06-02]`: OQ-001 (ASIL D); OQ-002 (T_max = 100 ms); OQ-003 (Cruise_Cancelled); OQ-005 (C + MISRA C:2012); OQ-007 (oldest-first overwrite); DR-001 (5 activation preconditions via REQ-018); DR-002 (write failure → Cruise_Fault via REQ-017)
- `[DERIVED]`: 0
- `[CONFLICT]`: 0
- `[FEASIBILITY CONCERN]`: 0
