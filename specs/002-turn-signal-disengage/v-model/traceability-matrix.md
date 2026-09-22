# Traceability Matrix

**Generated**: 2026-09-22
**Source**: `specs/002-turn-signal-disengage/v-model/`

## Matrix A — Validation (User View)

| Requirement ID | Requirement Description | Test Case ID (ATP) | Validation Condition | Scenario ID (SCN) | Status |
|----------------|------------------------|--------------------|----------------------|--------------------|--------|
| **REQ-001** | The feature SHALL monitor the left and right turn signal input(s) provided by the existing EV prototype platform. `[ASIL D — Confirmed]` `[MECHANISM: Detection]` | ATP-001-A | Left and Right Turn Signal Inputs Monitored | SCN-001-A1 | ⬜ Untested |
| **REQ-002** | While the feature is in `Cruise_Active`, when either the left or the right turn signal is activated, the feature SHALL transition cruise control out of `Cruise_Active` within one control cycle. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | ATP-002-A | Left Turn Signal Triggers Disengagement | SCN-002-A1 | ⬜ Untested |
| | | ATP-002-B | Right Turn Signal Triggers Disengagement | SCN-002-B1 | ⬜ Untested |
| | | ATP-002-C | Both Signals Simultaneously — Hazard Lights | SCN-002-C1 | ⬜ Untested |
| | | ATP-002-D | Brief Tap Within a Single Control Cycle | SCN-002-D1 | ⬜ Untested |
| **REQ-003** | Following a transition out of `Cruise_Active` caused by turn signal activation under REQ-002, the feature SHALL transition cruise control to `Cruise_Cancelled`. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | ATP-003-A | Target State Is Cruise_Cancelled | SCN-003-A1 | ⬜ Untested |
| **REQ-004** | When cruise control transitions out of `Cruise_Active` due to a turn signal activation (REQ-002), the feature SHALL cease all longitudinal speed control output to the propulsion command interface as part of that same transition. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | ATP-004-A | Cessation Coincident With Transition | SCN-004-A1 | ⬜ Untested |
| | | ATP-004-B | Cessation During Active Speed Correction | SCN-004-B1 | ⬜ Untested |
| **REQ-005** | While cruise control is not in `Cruise_Active`, turn signal activation SHALL NOT cause this feature to take any action on the cruise control state. `[ASIL D — Confirmed]` `[MECHANISM: Prevention]` | ATP-005-A | No State Change Outside Cruise_Active — Negative Test | SCN-005-A1 | ⬜ Untested |
| **REQ-006** | Deactivation of the turn signal SHALL NOT, by itself, cause cruise control to transition to `Cruise_Active`; any resumption of `Cruise_Active` SHALL follow the existing cruise activation request and precondition process. `[ASIL D — Confirmed]` `[MECHANISM: Prevention]` `[REUSED: REQ-005, REQ-018]` | ATP-006-A | Deactivation Alone Does Not Resume Cruise_Active | SCN-006-A1 | ⬜ Untested |
| | | ATP-006-B | Reactivation Still Governed by Existing Guard | SCN-006-B1 | ⬜ Untested |
| **REQ-007** | Both the left and right turn signal being active simultaneously (hazard lights) SHALL be treated identically to a single turn signal activation for the purposes of REQ-002 through REQ-004. `[ASIL D — Confirmed]` | ATP-002-C | Both Signals Simultaneously — Hazard Lights | SCN-002-C1 | ⬜ Untested |
| | | ATP-007-A | No Distinct Hazard-Specific Handling | SCN-007-A1 | ⬜ Untested |
| **REQ-008** | If a turn signal activation is detected during a control cycle in which cruise control is `Cruise_Active`, the transition required by REQ-002 SHALL still occur even if the turn signal is deactivated again within that same control cycle. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | ATP-002-D | Brief Tap Within a Single Control Cycle | SCN-002-D1 | ⬜ Untested |
| | | ATP-008-A | Fault-Injection-Style Race Condition Test | SCN-008-A1 | ⬜ Untested |
| **REQ-009** | When a turn-signal-triggered disengagement occurs under REQ-002, the feature SHALL issue a notification event to the notification interface, consistent with REQ-012 in `specs/001-cruise-brake-override/`. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` `[REUSED: REQ-012]` | ATP-009-A | Notification Issued via Platform Interface | SCN-009-A1 | ⬜ Untested |
| **REQ-010** | When a turn-signal-triggered disengagement occurs under REQ-002, the feature SHALL persist an event record before the transition is considered complete, containing the same five fields as REQ-013/REQ-014 in `specs/001-cruise-brake-override/`, with the triggering condition field recording the turn signal side. `[ASIL D — Confirmed]` `[MECHANISM: Detection]` `[REUSED: REQ-013, REQ-014]` | ATP-010-A | Event Record Written Before Transition Complete | SCN-010-A1 | ⬜ Untested |
| | | ATP-010-B | All Five Fields Present, Triggering Condition Records Side | SCN-010-B1 | ⬜ Untested |
| | | ATP-010-B | All Five Fields Present, Triggering Condition Records Side | SCN-010-B2 | ⬜ Untested |
| **REQ-NF-001** | The elapsed time from detection of a turn signal activation while `Cruise_Active` to both transition out of `Cruise_Active` and cessation of longitudinal speed control output SHALL NOT exceed 100 milliseconds (same bound as REQ-NF-001 in `specs/001-cruise-brake-override/`). `[ASIL D — Confirmed]` `[REUSED: REQ-NF-001]` | ATP-NF-001-A | Latency Measurement Over Repeated Runs | SCN-NF-001-A1 | ⬜ Untested |
| | | ATP-NF-001-B | Worst-Case Execution Time Under Load | SCN-NF-001-B1 | ⬜ Untested |
| **REQ-IF-001** | The feature SHALL receive left and right turn signal activation status from a turn signal input provided by the existing EV prototype platform. `[ASIL D — Confirmed]` | ATP-IF-001-A | Trigger Derived Exclusively From Turn Signal Input | SCN-IF-001-A1 | ⬜ Untested |
| **REQ-IF-002** | The feature SHALL cease longitudinal speed control output exclusively through the propulsion command interface, reusing REQ-IF-001 in `specs/001-cruise-brake-override/`. `[ASIL D — Confirmed]` `[REUSED: REQ-IF-001]` | ATP-IF-002-A | No Second Output Channel Introduced | SCN-IF-002-A1 | ⬜ Untested |
| **REQ-IF-003** | The feature SHALL deliver the REQ-009 notification through the notification interface, reusing REQ-IF-003 in `specs/001-cruise-brake-override/`. `[ASIL D — Confirmed]` `[REUSED: REQ-IF-003]` | ATP-IF-003-A | Notification Delivered Through Existing Interface Only | SCN-IF-003-A1 | ⬜ Untested |
| **REQ-IF-004** | The feature SHALL write the REQ-010 event record to the local persistent event storage, reusing REQ-IF-004 in `specs/001-cruise-brake-override/`. `[ASIL D — Confirmed]` `[REUSED: REQ-IF-004]` | ATP-IF-004-A | Event Records Written to Existing Platform Storage Only | SCN-IF-004-A1 | ⬜ Untested |
| **REQ-CN-001** | The feature SHALL reuse, without modification, the five-state cruise control state machine, the reactivation guard, the propulsion command interface, the notification interface, and the local persistent event storage already defined in `specs/001-cruise-brake-override/`. | ATP-CN-001-A | No New State, State Machine, or Guard Introduced — Inspection | SCN-CN-001-A1 | ⬜ Untested |
| **REQ-CN-002** | Sensor-level fault or invalidity handling for the turn signal input itself is out of scope; such handling SHALL continue to be governed exclusively by the existing `Required_Input_Invalid` mechanism. | ATP-CN-002-A | Input Fault Governed Exclusively by Existing Mechanism — Inspection | SCN-CN-002-A1 | ⬜ Untested |
| **REQ-CN-003** | Activation preconditions for entering `Cruise_Active` are out of scope for this feature and SHALL continue to be governed exclusively by the existing activation precondition process (REQ-018 in `specs/001-cruise-brake-override/`). | ATP-CN-003-A | Turn Signal Active at Activation Request Governed by Existing Guard — Inspection | SCN-CN-003-A1 | ⬜ Untested |

### Matrix A Coverage

| Metric | Value |
|--------|-------|
| **Total Requirements** | 18 |
| **Total Test Cases (ATP)** | 25 |
| **Total Scenarios (SCN)** | 26 |
| **REQ → ATP Coverage** | 18/18 (100%) |
| **ATP → SCN Coverage** | 25/25 (100%) |

Note: REQ-007 and REQ-008 are covered jointly by dedicated test cases
(ATP-007-A, ATP-008-A) and by the relevant scenario of REQ-002's test cases
(ATP-002-C for hazard lights, ATP-002-D for the brief-tap edge case), since
those edge cases are direct restatements of REQ-002's behavior under a named
boundary condition rather than independent mechanisms.

## Matrix B — Verification (Architectural View)

**Source**: `specs/002-turn-signal-disengage/v-model/system-design.md` (SYS
decomposition) and `specs/002-turn-signal-disengage/v-model/system-test.md`
(STP/STS test plan). SYS-001 (Turn Signal Monitor) is the only new component
introduced by this feature; the five `001:SYS-NNN` rows are reused unmodified
from `specs/001-cruise-brake-override/` and are cross-referenced to their
existing, passing upstream `001:STP-*` test cases rather than re-verified here
(per REQ-CN-001's reuse-without-modification mandate — see system-test.md's
"reused, unmodified" section).

| Requirement ID | System Component (SYS) | Component Name | Test Case ID (STP) | Technique | Scenario ID (STS) | Status |
|----------------|------------------------|----------------|--------------------|-----------|--------------------|--------|
| **REQ-001** | SYS-001 | Turn Signal Monitor | STP-001-A | Interface Contract Testing (External) | STS-001-A1 | ⬜ Untested |
| | SYS-001 | Turn Signal Monitor | STP-001-A | Interface Contract Testing (External) | STS-001-A2 | ⬜ Untested |
| | SYS-001 | Turn Signal Monitor | STP-001-A | Interface Contract Testing (External) | STS-001-A3 | ⬜ Untested |
| **REQ-002** | SYS-001 | Turn Signal Monitor | STP-001-B | Interface Contract Testing (Internal) | STS-001-B1 | ⬜ Untested |
| | SYS-001 | Turn Signal Monitor | STP-001-B | Interface Contract Testing (Internal) | STS-001-B2 | ⬜ Untested |
| | SYS-001 | Turn Signal Monitor | STP-001-C | Interface Contract Testing (Internal) | STS-001-C1 | ⬜ Untested |
| | SYS-001 | Turn Signal Monitor | STP-001-C | Interface Contract Testing (Internal) | STS-001-C2 | ⬜ Untested |
| **REQ-003** | SYS-001 | Turn Signal Monitor | STP-001-C | Interface Contract Testing (Internal) | STS-001-C1 | ⬜ Untested |
| | 001:SYS-001 | Cruise State Machine | 001:STP-001-A | Interface Contract Testing (Internal) | 001:STS-001-A1 | ⬜ Untested |
| **REQ-004** | 001:SYS-003 | Longitudinal Speed Controller | 001:STP-003-B | Interface Contract Testing (Internal) | 001:STS-003-B1 | ⬜ Untested |
| **REQ-005** | SYS-001 | Turn Signal Monitor | STP-001-B | Interface Contract Testing (Internal) | STS-001-B2 | ⬜ Untested |
| **REQ-006** | SYS-001 | Turn Signal Monitor | STP-001-C | Interface Contract Testing (Internal) | STS-001-C2 | ⬜ Untested |
| | 001:SYS-001 | Cruise State Machine | 001:STP-001-A | Interface Contract Testing (Internal) | 001:STS-001-A1 | ⬜ Untested |
| **REQ-007** | SYS-001 | Turn Signal Monitor | STP-001-A | Interface Contract Testing (External) | STS-001-A3 | ⬜ Untested |
| **REQ-008** | SYS-001 | Turn Signal Monitor | STP-001-E | Boundary Value Analysis | STS-001-E1 | ⬜ Untested |
| | SYS-001 | Turn Signal Monitor | STP-001-E | Boundary Value Analysis | STS-001-E2 | ⬜ Untested |
| **REQ-009** | SYS-001 | Turn Signal Monitor | STP-001-D | Interface Contract Testing (Internal) | STS-001-D2 | ⬜ Untested |
| | 001:SYS-007 | Notification Dispatcher | 001:STP-007-A | Interface Contract Testing (External) | 001:STS-007-A1 | ⬜ Untested |
| | 001:SYS-007 | Notification Dispatcher | 001:STP-007-B | Interface Contract Testing (Internal) | 001:STS-007-B1 | ⬜ Untested |
| **REQ-010** | SYS-001 | Turn Signal Monitor | STP-001-D | Interface Contract Testing (Internal) | STS-001-D1 | ⬜ Untested |
| | 001:SYS-006 | Safety Event Logger | 001:STP-006-A | Interface Contract Testing (Internal) | 001:STS-006-A1 | ⬜ Untested |
| | 001:SYS-006 | Safety Event Logger | 001:STP-006-B | Boundary Value Analysis | 001:STS-006-B1 | ⬜ Untested |
| **REQ-NF-001** | SYS-001 | Turn Signal Monitor | STP-001-E | Boundary Value Analysis | STS-001-E1 | ⬜ Untested |
| **REQ-IF-001** | SYS-001 | Turn Signal Monitor | STP-001-A | Interface Contract Testing (External) | STS-001-A1 | ⬜ Untested |
| **REQ-IF-002** | 001:SYS-003 | Longitudinal Speed Controller | 001:STP-003-A | Interface Contract Testing (External) | 001:STS-003-A1 | ⬜ Untested |
| **REQ-IF-003** | SYS-001 | Turn Signal Monitor | STP-001-D | Interface Contract Testing (Internal) | STS-001-D2 | ⬜ Untested |
| | 001:SYS-007 | Notification Dispatcher | 001:STP-007-A | Interface Contract Testing (External) | 001:STS-007-A1 | ⬜ Untested |
| **REQ-IF-004** | SYS-001 | Turn Signal Monitor | STP-001-D | Interface Contract Testing (Internal) | STS-001-D1 | ⬜ Untested |
| | 001:SYS-006 | Safety Event Logger | 001:STP-006-A | Interface Contract Testing (Internal) | 001:STS-006-A1 | ⬜ Untested |
| **REQ-CN-001** | SYS-001 | Turn Signal Monitor | STP-001-B | Interface Contract Testing (Internal) | STS-001-B1 | ⬜ Untested |
| | SYS-001 | Turn Signal Monitor | STP-001-C | Interface Contract Testing (Internal) | STS-001-C1 | ⬜ Untested |
| **REQ-CN-002** | SYS-001 | Turn Signal Monitor | STP-001-G | Fault Injection | STS-001-G1 | ⬜ Untested |
| **REQ-CN-003** | SYS-001 | Turn Signal Monitor | STP-001-B | Interface Contract Testing (Internal) | STS-001-B2 | ⬜ Untested |
| | 001:SYS-001 | Cruise State Machine | 001:STP-001-A | Interface Contract Testing (Internal) | 001:STS-001-A1 | ⬜ Untested |

### Matrix B Coverage

| Metric | Value |
|--------|-------|
| **Total System Components (SYS, this feature)** | 1 new (SYS-001) + 5 upstream references (`001:SYS-NNN`) |
| **Total System Test Cases (STP, new)** | 7 (STP-001-A .. STP-001-G) |
| **Total System Scenarios (STS, new)** | 13 |
| **REQ → SYS Coverage** | 18/18 (100%) |
| **SYS → STP Coverage** | 1/1 new components (100%); all 5 upstream components cross-referenced to existing passing 001-slice STPs (not re-tested) |

### Open Items

None. Both Matrix A and Matrix B achieve 100% coverage for this feature:
every requirement in `specs/002-turn-signal-disengage/v-model/requirements.md`
has at least one linked acceptance Test Case (Matrix A) and at least one
linked system Test Case (Matrix B, either a new STP or a cross-referenced
upstream `001:STP-*`); every Test Case in both matrices has at least one
linked Scenario. The upstream open questions (OQ-001 ASIL D, OQ-002
T_max = 100 ms, OQ-003 `Cruise_Cancelled` target state) were already resolved
in the 001 slice and are inherited without re-opening.
