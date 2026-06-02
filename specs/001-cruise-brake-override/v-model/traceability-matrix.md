# Traceability Matrix

**Generated**: 2026-06-02
**Source**: `specs/001-cruise-brake-override/v-model/`

## Matrix A — Validation (User View)

| Requirement ID | Requirement Description | Test Case ID (ATP) | Validation Condition | Scenario ID (SCN) | Status |
|----------------|------------------------|--------------------|----------------------|--------------------|--------|
| **REQ-001** | The feature SHALL implement a closed state machine comprising exactly five discrete operational states — `Cruise_Standby`, `Cruise_Active`, `Cruise_Suspended`, `Cruise_Cancelled`, and `Cruise_Fault` — each with defined entry conditions, exit conditions, and permitted transitions. `[ASIL D — Confirmed]` `[MECHANISM: Prevention]` | ATP-001-A | State Machine Completeness | SCN-001-A1 | ⬜ Untested |
| | | ATP-001-B | No Undefined State Reachable | SCN-001-B1 | ⬜ Untested |
| | | ATP-001-C | State Variable Fault Injection | SCN-001-C1 | ⬜ Untested |
| **REQ-002** | When the feature is in `Cruise_Active` and `Brake_Override` is confirmed, the feature SHALL cease all longitudinal speed control output to the propulsion command interface. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | ATP-002-A | Cessation on Brake_Override | SCN-002-A1 | ⬜ Untested |
| | | ATP-002-B | Cessation During Active Speed Correction | SCN-002-B1 | ⬜ Untested |
| | | ATP-002-C | Fault Injection on Brake_Override Signal | SCN-002-C1 | ⬜ Untested |
| **REQ-003** | When `Brake_Override` is confirmed in `Cruise_Active`, the feature SHALL release longitudinal control authority to the driver or to an authorised vehicle function as the direct consequence of the cessation in REQ-002. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | ATP-003-A | Authority Released on Override | SCN-003-A1 | ⬜ Untested |
| | | ATP-003-B | Authority Not Re-Assumed Without Activation | SCN-003-B1 | ⬜ Untested |
| **REQ-004** | Following `Brake_Override` in `Cruise_Active`, the feature SHALL transition to `Cruise_Cancelled`. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | ATP-004-A | Non-Active State Reached | SCN-004-A1 | ⬜ Untested |
| | | ATP-004-B | Exactly One Target State Reached | SCN-004-B1 | ⬜ Untested |
| **REQ-005** | Following a transition out of `Cruise_Active` due to `Brake_Override`, the feature SHALL refuse all cruise activation requests until every activation precondition is confirmed valid. `[ASIL D — Confirmed]` `[MECHANISM: Prevention]` | ATP-005-A | Activation Refused Before Preconditions Valid | SCN-005-A1 | ⬜ Untested |
| | | ATP-005-B | Activation Permitted After All Preconditions Valid | SCN-005-B1 | ⬜ Untested |
| | | ATP-005-C | Partial Preconditions Insufficient | SCN-005-C1 | ⬜ Untested |
| **REQ-006** | When `Longitudinal_Control_Unavailable` is detected while the feature is in `Cruise_Active`, the feature SHALL cease all longitudinal speed control output to the propulsion command interface. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | ATP-006-A | Cessation on LCU While Active | SCN-006-A1 | ⬜ Untested |
| | | ATP-006-B | Cessation Not Triggered When Not In Cruise_Active | SCN-006-B1 | ⬜ Untested |
| **REQ-007** | When `Longitudinal_Control_Unavailable` is detected while the feature is in `Cruise_Active`, the feature SHALL transition to `Cruise_Cancelled`. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | ATP-007-A | Non-Active State Reached on LCU | SCN-007-A1 | ⬜ Untested |
| | | ATP-007-B | State Is Confirmed Non-Active | SCN-007-B1 | ⬜ Untested |
| **REQ-008** | When `Cruise_Control_Fault` is detected, the feature SHALL transition to the `Cruise_Fault` state regardless of its current operational state. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | ATP-008-A | Cruise_Fault Reached From Any State | SCN-008-A1 | ⬜ Untested |
| | | ATP-008-A | Cruise_Fault Reached From Any State | SCN-008-A2 | ⬜ Untested |
| | | ATP-008-B | Fault Injection on Diagnostic Status | SCN-008-B1 | ⬜ Untested |
| **REQ-009** | When `Cruise_Control_Fault` is detected while the feature is in `Cruise_Active`, the feature SHALL cease all longitudinal speed control output to the propulsion command interface. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | ATP-009-A | Speed Control Ceases on CCF In Active State | SCN-009-A1 | ⬜ Untested |
| | | ATP-009-B | No Spurious Cessation When Not In Cruise_Active | SCN-009-B1 | ⬜ Untested |
| **REQ-010** | The feature SHALL NOT transition to `Cruise_Active` when `Required_Input_Invalid` is detected. `[ASIL D — Confirmed]` `[MECHANISM: Prevention]` | ATP-010-A | Activation Refused Under RII (Prevention | SCN-010-A1 | ⬜ Untested |
| | | ATP-010-B | Activation Permitted When Inputs Valid | SCN-010-B1 | ⬜ Untested |
| | | ATP-010-C | Fault Injection: Invalid Input Injected During Activation Sequence | SCN-010-C1 | ⬜ Untested |
| **REQ-011** | When `Required_Input_Invalid` is detected while the feature is in `Cruise_Active`, the feature SHALL transition to `Cruise_Cancelled`. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | ATP-011-A | Exit from Cruise_Active on RII (Mitigation | SCN-011-A1 | ⬜ Untested |
| | | ATP-011-B | Exit Occurs Even When RII Appears Immediately After Entry | SCN-011-B1 | ⬜ Untested |
| | | ATP-011-C | Fault Injection: Intermittent Input Validity | SCN-011-C1 | ⬜ Untested |
| **REQ-012** | When `Brake_Override` causes a transition out of `Cruise_Active`, the feature SHALL issue a notification event to the notification interface provided by the EV prototype platform. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | ATP-012-A | Notification Issued via Platform Interface | SCN-012-A1 | ⬜ Untested |
| | | ATP-012-B | Notification Not Duplicated | SCN-012-B1 | ⬜ Untested |
| **REQ-013** | When any safety-relevant state transition occurs, the feature SHALL persist an event record to the local persistent event storage provided by the EV prototype platform before the transition is considered complete. `[ASIL D — Confirmed]` `[MECHANISM: Detection]` | ATP-013-A | Record Written for Each Transition | SCN-013-A1 | ⬜ Untested |
| | | ATP-013-B | Record Written Before Transition Considered Complete | SCN-013-B1 | ⬜ Untested |
| **REQ-014** | Each event record persisted under REQ-013 SHALL contain all five of the following fields: (1) event timestamp, (2) the state from which the transition occurred, (3) the condition that triggered the transition, (4) the state to which the transition occurred, (5) vehicle speed at the time of the event. `[ASIL D — Confirmed]` | ATP-014-A | All Five Fields Present in Record | SCN-014-A1 | ⬜ Untested |
| | | ATP-014-B | Field Values Are Accurate for a Known Transition | SCN-014-B1 | ⬜ Untested |
| **REQ-015** | The event log records produced under REQ-013 and REQ-014 SHALL be sufficient for an auditor to reconstruct the complete state transition history of the feature from the log alone, without reference to any external data source. `[ASIL D — Confirmed]` | ATP-015-A | Full Reconstruction From Log Alone (Demonstration | SCN-015-A1 | ⬜ Untested |
| **REQ-016** | The feature SHALL continuously monitor all of the following inputs provided by the EV prototype platform: vehicle speed, brake pedal status, driver command input, diagnostic status, and longitudinal control availability. `[ASIL D — Confirmed]` `[MECHANISM: Detection]` | ATP-016-A | All Five Inputs Monitored (Inspection | SCN-016-A1 | ⬜ Untested |
| | | ATP-016-B | Each Input Contributes to Correct Condition Detection | SCN-016-B1 | ⬜ Untested |
| **REQ-017** | When the feature's event logging function fails to write an event record to the platform local persistent event storage, the feature SHALL treat the write failure as a `Cruise_Control_Fault` condition and transition to `Cruise_Fault`. `[ASIL D — Confirmed]` `[MECHANISM: Mitigation]` | ATP-017-A | Write Failure Triggers Cruise_Control_Fault and Cruise_Fault | SCN-017-A1 | ⬜ Untested |
| **REQ-018** | Before permitting any transition from `Cruise_Standby` to `Cruise_Active`, the feature SHALL evaluate and confirm that all five of the following activation preconditions are simultaneously satisfied: (1) vehicle speed is within the defined activation speed range, (2) `Brake_Override` is not active, (3) `Required_Input_Invalid` is not active, (4) `Cruise_Control_Fault` is not active, (5) `Longitudinal_Control_Unavailable` is not active. If any precondition is not satisfied, the transition SHALL be refused. `[ASIL D — Confirmed]` `[MECHANISM: Prevention]` | ATP-018-A | All Five Preconditions Required for Activation | SCN-018-A1 | ⬜ Untested |
| | | ATP-018-B | Any Single Precondition Failure Prevents Activation | SCN-018-B1 | ⬜ Untested |
| **REQ-CN-001** | The feature SHALL operate exclusively using the platform interfaces listed in REQ-IF-001 through REQ-IF-007. The feature SHALL NOT introduce new hardware interfaces, additional communication channels, or dependencies not provided by the existing EV prototype platform. | ATP-CN-001-A | Interface Inventory Inspection | SCN-CN-001-A1 | ⬜ Untested |
| **REQ-CN-002** | The complete development lifecycle of this feature — hazard analysis, safety requirements, design, implementation, verification, and validation — SHALL produce artifacts in accordance with an ISO 26262-inspired functional safety process appropriate to the prototype development context. ASIL D has been confirmed as the feature's safety integrity level (OQ-001 resolved 2026-06-02). | ATP-CN-002-A | Safety Lifecycle Artifact Completeness | SCN-CN-002-A1 | ⬜ Untested |
| **REQ-CN-003** | The cybersecurity-relevant interfaces and inputs used by this feature — specifically brake pedal status, driver command input, and vehicle communication interfaces — SHALL be subject to a threat analysis and treatment process following an ISO/SAE 21434-inspired approach. The threat treatment output SHALL be documented and incorporated into the feature design before implementation begins. | ATP-CN-003-A | Threat Analysis Documentation Present | SCN-CN-003-A1 | ⬜ Untested |
| **REQ-CN-004** | The feature SHALL be structured as a set of explicitly defined software components with documented boundaries, responsibilities, and interfaces, compatible with automotive-style embedded development. No specific middleware, operating environment, or vendor framework is prescribed. The component architecture is defined in `architecture-design.md` (16 ARCH modules; OQ-004 resolved). | ATP-CN-004-A | Component Boundaries and Interfaces Documented | SCN-CN-004-A1 | ⬜ Untested |
| **REQ-CN-005** | The implementation language and toolchain used to develop this feature SHALL natively support: (a) static analysis, (b) coding-standard rule enforcement, and (c) automated unit and integration verification. The implementation language is **C** with the **MISRA C:2012** coding standard (OQ-005 resolved 2026-06-02). Static analysis tool selection is deferred to the build configuration phase. | ATP-CN-005-A | Toolchain Supports Static Analysis, Coding Standard, Automated Verification | SCN-CN-005-A1 | ⬜ Untested |
| **REQ-CN-006** | The verification and validation plan for this feature SHALL include at least one virtual validation activity using X-VERSE as the designated virtual validation environment. The specific X-VERSE scenario configuration is subject to definition (OQ-006 in spec.md). | ATP-CN-006-A | X-VERSE Activity in V&V Plan | SCN-CN-006-A1 | ⬜ Untested |
| **REQ-CN-007** | The integrity verification mechanism applied by the Input Integrity Verifier (ARCH-008) to brake pedal status and driver command inputs SHALL implement CRC-16 error detection combined with a 4-bit rolling counter for replay detection. Inputs that fail the CRC check or present a non-sequential counter value SHALL be treated as `Required_Input_Invalid`. `[ASIL D — Confirmed]` `[MECHANISM: Prevention]` | ATP-CN-007-A | CRC Failure Produces Required_Input_Invalid | SCN-CN-007-A1 | ⬜ Untested |
| **REQ-IF-001** | The feature SHALL issue all longitudinal speed control commands exclusively through the propulsion command interface provided by the existing EV prototype platform. The feature SHALL NOT use any other interface to influence vehicle longitudinal motion. `[ASIL D — Confirmed]` | ATP-IF-001-A | Exclusive Use of Propulsion Interface | SCN-IF-001-A1 | ⬜ Untested |
| | | ATP-IF-001-B | No Commands After Cessation | SCN-IF-001-B1 | ⬜ Untested |
| **REQ-IF-002** | The feature SHALL derive the `Brake_Override` and `Required_Input_Invalid` conditions from the brake pedal status and driver command inputs provided by the existing EV prototype platform. `[ASIL D — Confirmed]` `[MECHANISM: Detection]` | ATP-IF-002-A | Brake_Override Derived from Brake Pedal Input | SCN-IF-002-A1 | ⬜ Untested |
| | | ATP-IF-002-B | Required_Input_Invalid on Integrity Failure | SCN-IF-002-B1 | ⬜ Untested |
| **REQ-IF-003** | When issuing a notification under REQ-012, the feature SHALL deliver the notification through the notification interface provided by the existing EV prototype platform. `[ASIL D — Confirmed]` | ATP-IF-003-A | Notification Via Platform Interface Only | SCN-IF-003-A1 | ⬜ Untested |
| **REQ-IF-004** | When persisting event records under REQ-013, the feature SHALL write records to the local persistent event storage provided by the existing EV prototype platform. `[ASIL D — Confirmed]` | ATP-IF-004-A | Event Records Written to Platform Storage Only | SCN-IF-004-A1 | ⬜ Untested |
| **REQ-IF-005** | The feature SHALL receive all driver cruise activation and deactivation requests from the driver command input provided by the existing EV prototype platform. `[ASIL D — Confirmed]` | ATP-IF-005-A | Activation Command Received via Driver Command Input | SCN-IF-005-A1 | ⬜ Untested |
| **REQ-IF-006** | The feature SHALL receive vehicle speed data from the vehicle speed input provided by the existing EV prototype platform. `[ASIL D — Confirmed]` | ATP-IF-006-A | Vehicle Speed Used in Event Record | SCN-IF-006-A1 | ⬜ Untested |
| **REQ-IF-007** | The feature SHALL receive diagnostic status information from the diagnostic status input provided by the existing EV prototype platform, and SHALL use this as the source for determining the `Cruise_Control_Fault` condition. `[ASIL D — Confirmed]` `[MECHANISM: Detection]` | ATP-IF-007-A | Cruise_Control_Fault Derived from Diagnostic Input | SCN-IF-007-A1 | ⬜ Untested |
| **REQ-NF-001** | The elapsed time from confirmation of `Brake_Override` to cessation of longitudinal speed control output (REQ-002) and release of control authority (REQ-003) SHALL not exceed **100 milliseconds**. `[ASIL D — Confirmed]` | ATP-NF-001-A | Latency Measurement Over Repeated Runs | SCN-NF-001-A1 | ⬜ Untested |
| | | ATP-NF-001-B | Worst-Case Execution Time Under Load | SCN-NF-001-B1 | ⬜ Untested |
| **REQ-NF-002** | The development lifecycle for this feature SHALL conform to **ASIL D** per ISO 26262-6 (OQ-001 resolved 2026-06-02). Required methods include: MC/DC structural coverage (100%) for all software units; independent review of all safety-critical design and test artifacts; documented independence justification for each ASIL B(D)+B(D) decomposition pair (ARCH-001/002 and ARCH-003/004); and independent safety assessment of the complete lifecycle. `[ASIL D — Confirmed]` | ATP-NF-002-A | Lifecycle Artifact Completeness Inspection | SCN-NF-002-A1 | ⬜ Untested |

### Matrix A Coverage

| Metric | Value |
|--------|-------|
| **Total Requirements** | 34 |
| **Total Test Cases (ATP)** | 58 |
| **Total Scenarios (SCN)** | 59 |
| **REQ → ATP Coverage** | 34/34 (100%) |
| **ATP → SCN Coverage** | 58/58 (100%) |

## Matrix B — Verification (Architectural View)

| Requirement ID | System Component (SYS) | Component Name | Test Case ID (STP) | Technique | Scenario ID (STS) | Status |
|----------------|------------------------|----------------|--------------------|-----------|--------------------|--------|
| **REQ-001** | SYS-001 | Cruise State Machine | STP-001-A | Interface Contract Testing (Internal) | STS-001-A1 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-A | Interface Contract Testing (Internal) | STS-001-A2 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-B | Boundary Value Analysis | STS-001-B1 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-B | Boundary Value Analysis | STS-001-B2 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-C | Fault Injection | STS-001-C1 | ⬜ Untested |
| **REQ-002** | SYS-002 | Brake Override Response Coordinator | STP-002-A | Interface Contract Testing (Internal) | STS-002-A1 | ⬜ Untested |
| | SYS-002 | Brake Override Response Coordinator | STP-002-B | Fault Injection | STS-002-B1 | ⬜ Untested |
| | SYS-002 | Brake Override Response Coordinator | STP-002-C | Fault Injection | STS-002-C1 | ⬜ Untested |
| | SYS-003 | Longitudinal Speed Controller | STP-003-A | Interface Contract Testing (External) | STS-003-A1 | ⬜ Untested |
| | SYS-003 | Longitudinal Speed Controller | STP-003-B | Interface Contract Testing (Internal) | STS-003-B1 | ⬜ Untested |
| | SYS-003 | Longitudinal Speed Controller | STP-003-C | Boundary Value Analysis | STS-003-C1 | ⬜ Untested |
| | SYS-003 | Longitudinal Speed Controller | STP-003-D | Fault Injection | STS-003-D1 | ⬜ Untested |
| **REQ-003** | SYS-002 | Brake Override Response Coordinator | STP-002-A | Interface Contract Testing (Internal) | STS-002-A1 | ⬜ Untested |
| | SYS-002 | Brake Override Response Coordinator | STP-002-B | Fault Injection | STS-002-B1 | ⬜ Untested |
| | SYS-002 | Brake Override Response Coordinator | STP-002-C | Fault Injection | STS-002-C1 | ⬜ Untested |
| **REQ-004** | SYS-001 | Cruise State Machine | STP-001-A | Interface Contract Testing (Internal) | STS-001-A1 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-A | Interface Contract Testing (Internal) | STS-001-A2 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-B | Boundary Value Analysis | STS-001-B1 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-B | Boundary Value Analysis | STS-001-B2 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-C | Fault Injection | STS-001-C1 | ⬜ Untested |
| **REQ-005** | SYS-001 | Cruise State Machine | STP-001-A | Interface Contract Testing (Internal) | STS-001-A1 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-A | Interface Contract Testing (Internal) | STS-001-A2 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-B | Boundary Value Analysis | STS-001-B1 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-B | Boundary Value Analysis | STS-001-B2 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-C | Fault Injection | STS-001-C1 | ⬜ Untested |
| **REQ-006** | SYS-003 | Longitudinal Speed Controller | STP-003-A | Interface Contract Testing (External) | STS-003-A1 | ⬜ Untested |
| | SYS-003 | Longitudinal Speed Controller | STP-003-B | Interface Contract Testing (Internal) | STS-003-B1 | ⬜ Untested |
| | SYS-003 | Longitudinal Speed Controller | STP-003-C | Boundary Value Analysis | STS-003-C1 | ⬜ Untested |
| | SYS-003 | Longitudinal Speed Controller | STP-003-D | Fault Injection | STS-003-D1 | ⬜ Untested |
| | SYS-005 | Fault and Condition Supervisor | STP-005-A | Interface Contract Testing (Internal) | STS-005-A1 | ⬜ Untested |
| | SYS-005 | Fault and Condition Supervisor | STP-005-B | Interface Contract Testing (Internal) | STS-005-B1 | ⬜ Untested |
| | SYS-005 | Fault and Condition Supervisor | STP-005-C | Fault Injection | STS-005-C1 | ⬜ Untested |
| **REQ-007** | SYS-001 | Cruise State Machine | STP-001-A | Interface Contract Testing (Internal) | STS-001-A1 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-A | Interface Contract Testing (Internal) | STS-001-A2 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-B | Boundary Value Analysis | STS-001-B1 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-B | Boundary Value Analysis | STS-001-B2 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-C | Fault Injection | STS-001-C1 | ⬜ Untested |
| | SYS-005 | Fault and Condition Supervisor | STP-005-A | Interface Contract Testing (Internal) | STS-005-A1 | ⬜ Untested |
| | SYS-005 | Fault and Condition Supervisor | STP-005-B | Interface Contract Testing (Internal) | STS-005-B1 | ⬜ Untested |
| | SYS-005 | Fault and Condition Supervisor | STP-005-C | Fault Injection | STS-005-C1 | ⬜ Untested |
| **REQ-008** | SYS-001 | Cruise State Machine | STP-001-A | Interface Contract Testing (Internal) | STS-001-A1 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-A | Interface Contract Testing (Internal) | STS-001-A2 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-B | Boundary Value Analysis | STS-001-B1 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-B | Boundary Value Analysis | STS-001-B2 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-C | Fault Injection | STS-001-C1 | ⬜ Untested |
| | SYS-005 | Fault and Condition Supervisor | STP-005-A | Interface Contract Testing (Internal) | STS-005-A1 | ⬜ Untested |
| | SYS-005 | Fault and Condition Supervisor | STP-005-B | Interface Contract Testing (Internal) | STS-005-B1 | ⬜ Untested |
| | SYS-005 | Fault and Condition Supervisor | STP-005-C | Fault Injection | STS-005-C1 | ⬜ Untested |
| **REQ-009** | SYS-003 | Longitudinal Speed Controller | STP-003-A | Interface Contract Testing (External) | STS-003-A1 | ⬜ Untested |
| | SYS-003 | Longitudinal Speed Controller | STP-003-B | Interface Contract Testing (Internal) | STS-003-B1 | ⬜ Untested |
| | SYS-003 | Longitudinal Speed Controller | STP-003-C | Boundary Value Analysis | STS-003-C1 | ⬜ Untested |
| | SYS-003 | Longitudinal Speed Controller | STP-003-D | Fault Injection | STS-003-D1 | ⬜ Untested |
| | SYS-005 | Fault and Condition Supervisor | STP-005-A | Interface Contract Testing (Internal) | STS-005-A1 | ⬜ Untested |
| | SYS-005 | Fault and Condition Supervisor | STP-005-B | Interface Contract Testing (Internal) | STS-005-B1 | ⬜ Untested |
| | SYS-005 | Fault and Condition Supervisor | STP-005-C | Fault Injection | STS-005-C1 | ⬜ Untested |
| **REQ-010** | SYS-001 | Cruise State Machine | STP-001-A | Interface Contract Testing (Internal) | STS-001-A1 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-A | Interface Contract Testing (Internal) | STS-001-A2 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-B | Boundary Value Analysis | STS-001-B1 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-B | Boundary Value Analysis | STS-001-B2 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-C | Fault Injection | STS-001-C1 | ⬜ Untested |
| | SYS-005 | Fault and Condition Supervisor | STP-005-A | Interface Contract Testing (Internal) | STS-005-A1 | ⬜ Untested |
| | SYS-005 | Fault and Condition Supervisor | STP-005-B | Interface Contract Testing (Internal) | STS-005-B1 | ⬜ Untested |
| | SYS-005 | Fault and Condition Supervisor | STP-005-C | Fault Injection | STS-005-C1 | ⬜ Untested |
| **REQ-011** | SYS-001 | Cruise State Machine | STP-001-A | Interface Contract Testing (Internal) | STS-001-A1 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-A | Interface Contract Testing (Internal) | STS-001-A2 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-B | Boundary Value Analysis | STS-001-B1 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-B | Boundary Value Analysis | STS-001-B2 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-C | Fault Injection | STS-001-C1 | ⬜ Untested |
| | SYS-005 | Fault and Condition Supervisor | STP-005-A | Interface Contract Testing (Internal) | STS-005-A1 | ⬜ Untested |
| | SYS-005 | Fault and Condition Supervisor | STP-005-B | Interface Contract Testing (Internal) | STS-005-B1 | ⬜ Untested |
| | SYS-005 | Fault and Condition Supervisor | STP-005-C | Fault Injection | STS-005-C1 | ⬜ Untested |
| **REQ-012** | SYS-002 | Brake Override Response Coordinator | STP-002-A | Interface Contract Testing (Internal) | STS-002-A1 | ⬜ Untested |
| | SYS-002 | Brake Override Response Coordinator | STP-002-B | Fault Injection | STS-002-B1 | ⬜ Untested |
| | SYS-002 | Brake Override Response Coordinator | STP-002-C | Fault Injection | STS-002-C1 | ⬜ Untested |
| | SYS-007 | Notification Dispatcher | STP-007-A | Interface Contract Testing (External) | STS-007-A1 | ⬜ Untested |
| | SYS-007 | Notification Dispatcher | STP-007-B | Interface Contract Testing (Internal) | STS-007-B1 | ⬜ Untested |
| **REQ-013** | SYS-002 | Brake Override Response Coordinator | STP-002-A | Interface Contract Testing (Internal) | STS-002-A1 | ⬜ Untested |
| | SYS-002 | Brake Override Response Coordinator | STP-002-B | Fault Injection | STS-002-B1 | ⬜ Untested |
| | SYS-002 | Brake Override Response Coordinator | STP-002-C | Fault Injection | STS-002-C1 | ⬜ Untested |
| | SYS-006 | Safety Event Logger | STP-006-A | Interface Contract Testing (External) | STS-006-A1 | ⬜ Untested |
| | SYS-006 | Safety Event Logger | STP-006-B | Boundary Value Analysis | STS-006-B1 | ⬜ Untested |
| | SYS-006 | Safety Event Logger | STP-006-B | Boundary Value Analysis | STS-006-B2 | ⬜ Untested |
| | SYS-006 | Safety Event Logger | STP-006-C | Fault Injection | STS-006-C1 | ⬜ Untested |
| **REQ-014** | SYS-006 | Safety Event Logger | STP-006-A | Interface Contract Testing (External) | STS-006-A1 | ⬜ Untested |
| | SYS-006 | Safety Event Logger | STP-006-B | Boundary Value Analysis | STS-006-B1 | ⬜ Untested |
| | SYS-006 | Safety Event Logger | STP-006-B | Boundary Value Analysis | STS-006-B2 | ⬜ Untested |
| | SYS-006 | Safety Event Logger | STP-006-C | Fault Injection | STS-006-C1 | ⬜ Untested |
| **REQ-015** | SYS-006 | Safety Event Logger | STP-006-A | Interface Contract Testing (External) | STS-006-A1 | ⬜ Untested |
| | SYS-006 | Safety Event Logger | STP-006-B | Boundary Value Analysis | STS-006-B1 | ⬜ Untested |
| | SYS-006 | Safety Event Logger | STP-006-B | Boundary Value Analysis | STS-006-B2 | ⬜ Untested |
| | SYS-006 | Safety Event Logger | STP-006-C | Fault Injection | STS-006-C1 | ⬜ Untested |
| **REQ-016** | SYS-004 | Platform Input Monitor | STP-004-A | Interface Contract Testing (External) | STS-004-A1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-A | Interface Contract Testing (External) | STS-004-A2 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-B | Interface Contract Testing (Internal) | STS-004-B1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-C | Equivalence Partitioning | STS-004-C1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-C | Equivalence Partitioning | STS-004-C2 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-D | Boundary Value Analysis | STS-004-D1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-D | Boundary Value Analysis | STS-004-D2 | ⬜ Untested |
| **REQ-017** | SYS-006 | Safety Event Logger | STP-006-A | Interface Contract Testing (External) | STS-006-A1 | ⬜ Untested |
| | SYS-006 | Safety Event Logger | STP-006-B | Boundary Value Analysis | STS-006-B1 | ⬜ Untested |
| | SYS-006 | Safety Event Logger | STP-006-B | Boundary Value Analysis | STS-006-B2 | ⬜ Untested |
| | SYS-006 | Safety Event Logger | STP-006-C | Fault Injection | STS-006-C1 | ⬜ Untested |
| **REQ-018** | SYS-001 | Cruise State Machine | STP-001-A | Interface Contract Testing (Internal) | STS-001-A1 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-A | Interface Contract Testing (Internal) | STS-001-A2 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-B | Boundary Value Analysis | STS-001-B1 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-B | Boundary Value Analysis | STS-001-B2 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-C | Fault Injection | STS-001-C1 | ⬜ Untested |
| **REQ-CN-001** | SYS-003 | Longitudinal Speed Controller | STP-003-A | Interface Contract Testing (External) | STS-003-A1 | ⬜ Untested |
| | SYS-003 | Longitudinal Speed Controller | STP-003-B | Interface Contract Testing (Internal) | STS-003-B1 | ⬜ Untested |
| | SYS-003 | Longitudinal Speed Controller | STP-003-C | Boundary Value Analysis | STS-003-C1 | ⬜ Untested |
| | SYS-003 | Longitudinal Speed Controller | STP-003-D | Fault Injection | STS-003-D1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-A | Interface Contract Testing (External) | STS-004-A1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-A | Interface Contract Testing (External) | STS-004-A2 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-B | Interface Contract Testing (Internal) | STS-004-B1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-C | Equivalence Partitioning | STS-004-C1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-C | Equivalence Partitioning | STS-004-C2 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-D | Boundary Value Analysis | STS-004-D1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-D | Boundary Value Analysis | STS-004-D2 | ⬜ Untested |
| **REQ-CN-002** | SYS-008 | Safety Process and Compliance | STP-008-A | Inspection | STS-008-A1 | ⬜ Untested |
| | SYS-008 | Safety Process and Compliance | STP-008-B | Inspection | STS-008-B1 | ⬜ Untested |
| **REQ-CN-003** | SYS-004 | Platform Input Monitor | STP-004-A | Interface Contract Testing (External) | STS-004-A1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-A | Interface Contract Testing (External) | STS-004-A2 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-B | Interface Contract Testing (Internal) | STS-004-B1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-C | Equivalence Partitioning | STS-004-C1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-C | Equivalence Partitioning | STS-004-C2 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-D | Boundary Value Analysis | STS-004-D1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-D | Boundary Value Analysis | STS-004-D2 | ⬜ Untested |
| | SYS-008 | Safety Process and Compliance | STP-008-A | Inspection | STS-008-A1 | ⬜ Untested |
| | SYS-008 | Safety Process and Compliance | STP-008-B | Inspection | STS-008-B1 | ⬜ Untested |
| **REQ-CN-004** | SYS-001 | Cruise State Machine | STP-001-A | Interface Contract Testing (Internal) | STS-001-A1 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-A | Interface Contract Testing (Internal) | STS-001-A2 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-B | Boundary Value Analysis | STS-001-B1 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-B | Boundary Value Analysis | STS-001-B2 | ⬜ Untested |
| | SYS-001 | Cruise State Machine | STP-001-C | Fault Injection | STS-001-C1 | ⬜ Untested |
| | SYS-008 | Safety Process and Compliance | STP-008-A | Inspection | STS-008-A1 | ⬜ Untested |
| | SYS-008 | Safety Process and Compliance | STP-008-B | Inspection | STS-008-B1 | ⬜ Untested |
| **REQ-CN-005** | SYS-008 | Safety Process and Compliance | STP-008-A | Inspection | STS-008-A1 | ⬜ Untested |
| | SYS-008 | Safety Process and Compliance | STP-008-B | Inspection | STS-008-B1 | ⬜ Untested |
| **REQ-CN-006** | SYS-008 | Safety Process and Compliance | STP-008-A | Inspection | STS-008-A1 | ⬜ Untested |
| | SYS-008 | Safety Process and Compliance | STP-008-B | Inspection | STS-008-B1 | ⬜ Untested |
| **REQ-CN-007** | SYS-004 | Platform Input Monitor | STP-004-A | Interface Contract Testing (External) | STS-004-A1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-A | Interface Contract Testing (External) | STS-004-A2 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-B | Interface Contract Testing (Internal) | STS-004-B1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-C | Equivalence Partitioning | STS-004-C1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-C | Equivalence Partitioning | STS-004-C2 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-D | Boundary Value Analysis | STS-004-D1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-D | Boundary Value Analysis | STS-004-D2 | ⬜ Untested |
| **REQ-IF-001** | SYS-003 | Longitudinal Speed Controller | STP-003-A | Interface Contract Testing (External) | STS-003-A1 | ⬜ Untested |
| | SYS-003 | Longitudinal Speed Controller | STP-003-B | Interface Contract Testing (Internal) | STS-003-B1 | ⬜ Untested |
| | SYS-003 | Longitudinal Speed Controller | STP-003-C | Boundary Value Analysis | STS-003-C1 | ⬜ Untested |
| | SYS-003 | Longitudinal Speed Controller | STP-003-D | Fault Injection | STS-003-D1 | ⬜ Untested |
| **REQ-IF-002** | SYS-004 | Platform Input Monitor | STP-004-A | Interface Contract Testing (External) | STS-004-A1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-A | Interface Contract Testing (External) | STS-004-A2 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-B | Interface Contract Testing (Internal) | STS-004-B1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-C | Equivalence Partitioning | STS-004-C1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-C | Equivalence Partitioning | STS-004-C2 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-D | Boundary Value Analysis | STS-004-D1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-D | Boundary Value Analysis | STS-004-D2 | ⬜ Untested |
| **REQ-IF-003** | SYS-007 | Notification Dispatcher | STP-007-A | Interface Contract Testing (External) | STS-007-A1 | ⬜ Untested |
| | SYS-007 | Notification Dispatcher | STP-007-B | Interface Contract Testing (Internal) | STS-007-B1 | ⬜ Untested |
| **REQ-IF-004** | SYS-006 | Safety Event Logger | STP-006-A | Interface Contract Testing (External) | STS-006-A1 | ⬜ Untested |
| | SYS-006 | Safety Event Logger | STP-006-B | Boundary Value Analysis | STS-006-B1 | ⬜ Untested |
| | SYS-006 | Safety Event Logger | STP-006-B | Boundary Value Analysis | STS-006-B2 | ⬜ Untested |
| | SYS-006 | Safety Event Logger | STP-006-C | Fault Injection | STS-006-C1 | ⬜ Untested |
| **REQ-IF-005** | SYS-004 | Platform Input Monitor | STP-004-A | Interface Contract Testing (External) | STS-004-A1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-A | Interface Contract Testing (External) | STS-004-A2 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-B | Interface Contract Testing (Internal) | STS-004-B1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-C | Equivalence Partitioning | STS-004-C1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-C | Equivalence Partitioning | STS-004-C2 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-D | Boundary Value Analysis | STS-004-D1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-D | Boundary Value Analysis | STS-004-D2 | ⬜ Untested |
| **REQ-IF-006** | SYS-004 | Platform Input Monitor | STP-004-A | Interface Contract Testing (External) | STS-004-A1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-A | Interface Contract Testing (External) | STS-004-A2 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-B | Interface Contract Testing (Internal) | STS-004-B1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-C | Equivalence Partitioning | STS-004-C1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-C | Equivalence Partitioning | STS-004-C2 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-D | Boundary Value Analysis | STS-004-D1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-D | Boundary Value Analysis | STS-004-D2 | ⬜ Untested |
| **REQ-IF-007** | SYS-004 | Platform Input Monitor | STP-004-A | Interface Contract Testing (External) | STS-004-A1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-A | Interface Contract Testing (External) | STS-004-A2 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-B | Interface Contract Testing (Internal) | STS-004-B1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-C | Equivalence Partitioning | STS-004-C1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-C | Equivalence Partitioning | STS-004-C2 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-D | Boundary Value Analysis | STS-004-D1 | ⬜ Untested |
| | SYS-004 | Platform Input Monitor | STP-004-D | Boundary Value Analysis | STS-004-D2 | ⬜ Untested |
| **REQ-NF-001** | SYS-002 | Brake Override Response Coordinator | STP-002-A | Interface Contract Testing (Internal) | STS-002-A1 | ⬜ Untested |
| | SYS-002 | Brake Override Response Coordinator | STP-002-B | Fault Injection | STS-002-B1 | ⬜ Untested |
| | SYS-002 | Brake Override Response Coordinator | STP-002-C | Fault Injection | STS-002-C1 | ⬜ Untested |
| **REQ-NF-002** | SYS-008 | Safety Process and Compliance | STP-008-A | Inspection | STS-008-A1 | ⬜ Untested |
| | SYS-008 | Safety Process and Compliance | STP-008-B | Inspection | STS-008-B1 | ⬜ Untested |

### Matrix B Coverage

| Metric | Value |
|--------|-------|
| **Total System Components (SYS)** | 8 |
| **Total System Test Cases (STP)** | 24 |
| **Total System Scenarios (STS)** | 30 |
| **REQ → SYS Coverage** | 34/34 (100%) |
| **SYS → STP Coverage** | 8/8 (100%) |

## Matrix C — Integration Verification (Module Boundary View)

| System Component (SYS) | Parent REQs | Architecture Module (ARCH) | Module Name | Test Case ID (ITP) | Technique | Scenario ID (ITS) | Status |
|------------------------|-------------|---------------------------|-------------|--------------------|-----------|--------------------|--------|
| SYS-001 (REQ-001, REQ-004, REQ-005, REQ-007, REQ-008, REQ-010, REQ-011, REQ-018, REQ-CN-004) | REQ-001, REQ-004, REQ-005, REQ-007, REQ-008, REQ-010, REQ-011, REQ-018, REQ-CN-004 | ARCH-001 | State Transition Evaluator | ITP-001-A | Interface Contract Testing | ITS-001-A1 | ⬜ Untested |
| SYS-001 (REQ-001, REQ-004, REQ-005, REQ-007, REQ-008, REQ-010, REQ-011, REQ-018, REQ-CN-004) | REQ-001, REQ-004, REQ-005, REQ-007, REQ-008, REQ-010, REQ-011, REQ-018, REQ-CN-004 | ARCH-001 | State Transition Evaluator | ITP-001-A | Interface Contract Testing | ITS-001-A2 | ⬜ Untested |
| SYS-001 (REQ-001, REQ-004, REQ-005, REQ-007, REQ-008, REQ-010, REQ-011, REQ-018, REQ-CN-004) | REQ-001, REQ-004, REQ-005, REQ-007, REQ-008, REQ-010, REQ-011, REQ-018, REQ-CN-004 | ARCH-001 | State Transition Evaluator | ITP-001-B | Interface Fault Injection | ITS-001-B1 | ⬜ Untested |
| SYS-001 (REQ-001, REQ-004, REQ-005, REQ-007, REQ-008, REQ-010, REQ-011, REQ-018, REQ-CN-004) | REQ-001, REQ-004, REQ-005, REQ-007, REQ-008, REQ-010, REQ-011, REQ-018, REQ-CN-004 | ARCH-002 | State Register | ITP-002-A | Interface Contract Testing | ITS-002-A1 | ⬜ Untested |
| SYS-001 (REQ-001, REQ-004, REQ-005, REQ-007, REQ-008, REQ-010, REQ-011, REQ-018, REQ-CN-004) | REQ-001, REQ-004, REQ-005, REQ-007, REQ-008, REQ-010, REQ-011, REQ-018, REQ-CN-004 | ARCH-002 | State Register | ITP-002-B | Interface Fault Injection | ITS-002-B1 | ⬜ Untested |
| SYS-002 (REQ-002, REQ-003, REQ-012, REQ-013, REQ-NF-001) | REQ-002, REQ-003, REQ-012, REQ-013, REQ-NF-001 | ARCH-003 | Override Response Sequencer | ITP-003-A | Interface Contract Testing | ITS-003-A1 | ⬜ Untested |
| SYS-002 (REQ-002, REQ-003, REQ-012, REQ-013, REQ-NF-001) | REQ-002, REQ-003, REQ-012, REQ-013, REQ-NF-001 | ARCH-003 | Override Response Sequencer | ITP-003-B | Data Flow Testing | ITS-003-B1 | ⬜ Untested |
| SYS-002 (REQ-002, REQ-003, REQ-012, REQ-013, REQ-NF-001) | REQ-002, REQ-003, REQ-012, REQ-013, REQ-NF-001 | ARCH-004 | Response Timing Supervisor | ITP-004-A | Interface Contract Testing | ITS-004-A1 | ⬜ Untested |
| SYS-002 (REQ-002, REQ-003, REQ-012, REQ-013, REQ-NF-001) | REQ-002, REQ-003, REQ-012, REQ-013, REQ-NF-001 | ARCH-004 | Response Timing Supervisor | ITP-004-B | Interface Fault Injection | ITS-004-B1 | ⬜ Untested |
| SYS-003 (REQ-002, REQ-006, REQ-009, REQ-IF-001, REQ-CN-001) | REQ-002, REQ-006, REQ-009, REQ-IF-001, REQ-CN-001 | ARCH-005 | Speed Control Regulator | ITP-005-A | Interface Contract Testing | ITS-005-A1 | ⬜ Untested |
| SYS-003 (REQ-002, REQ-006, REQ-009, REQ-IF-001, REQ-CN-001) | REQ-002, REQ-006, REQ-009, REQ-IF-001, REQ-CN-001 | ARCH-005 | Speed Control Regulator | ITP-005-A | Interface Contract Testing | ITS-005-A2 | ⬜ Untested |
| SYS-003 (REQ-002, REQ-006, REQ-009, REQ-IF-001, REQ-CN-001) | REQ-002, REQ-006, REQ-009, REQ-IF-001, REQ-CN-001 | ARCH-006 | Propulsion Interface Adapter | ITP-006-A | Interface Contract Testing (External) | ITS-006-A1 | ⬜ Untested |
| SYS-003 (REQ-002, REQ-006, REQ-009, REQ-IF-001, REQ-CN-001) | REQ-002, REQ-006, REQ-009, REQ-IF-001, REQ-CN-001 | ARCH-006 | Propulsion Interface Adapter | ITP-006-B | Concurrency & Race Condition Testing | ITS-006-B1 | ⬜ Untested |
| SYS-004 (REQ-016, REQ-IF-002, REQ-IF-005, REQ-IF-006, REQ-IF-007, REQ-CN-001, REQ-CN-003, REQ-CN-007) | REQ-016, REQ-IF-002, REQ-IF-005, REQ-IF-006, REQ-IF-007, REQ-CN-001, REQ-CN-003, REQ-CN-007 | ARCH-007 | Platform Input Reader | ITP-007-A | Interface Contract Testing (External) | ITS-007-A1 | ⬜ Untested |
| SYS-004 (REQ-016, REQ-IF-002, REQ-IF-005, REQ-IF-006, REQ-IF-007, REQ-CN-001, REQ-CN-003, REQ-CN-007) | REQ-016, REQ-IF-002, REQ-IF-005, REQ-IF-006, REQ-IF-007, REQ-CN-001, REQ-CN-003, REQ-CN-007 | ARCH-007 | Platform Input Reader | ITP-007-B | Interface Fault Injection | ITS-007-B1 | ⬜ Untested |
| SYS-004 (REQ-016, REQ-IF-002, REQ-IF-005, REQ-IF-006, REQ-IF-007, REQ-CN-001, REQ-CN-003, REQ-CN-007) | REQ-016, REQ-IF-002, REQ-IF-005, REQ-IF-006, REQ-IF-007, REQ-CN-001, REQ-CN-003, REQ-CN-007 | ARCH-008 | Input Integrity Verifier | ITP-008-A | Interface Contract Testing | ITS-008-A1 | ⬜ Untested |
| SYS-004 (REQ-016, REQ-IF-002, REQ-IF-005, REQ-IF-006, REQ-IF-007, REQ-CN-001, REQ-CN-003, REQ-CN-007) | REQ-016, REQ-IF-002, REQ-IF-005, REQ-IF-006, REQ-IF-007, REQ-CN-001, REQ-CN-003, REQ-CN-007 | ARCH-008 | Input Integrity Verifier | ITP-008-B | Data Flow Testing | ITS-008-B1 | ⬜ Untested |
| SYS-004 (REQ-016, REQ-IF-002, REQ-IF-005, REQ-IF-006, REQ-IF-007, REQ-CN-001, REQ-CN-003, REQ-CN-007) | REQ-016, REQ-IF-002, REQ-IF-005, REQ-IF-006, REQ-IF-007, REQ-CN-001, REQ-CN-003, REQ-CN-007 | ARCH-009 | Condition Deriver | ITP-009-A | Interface Contract Testing | ITS-009-A1 | ⬜ Untested |
| SYS-004 (REQ-016, REQ-IF-002, REQ-IF-005, REQ-IF-006, REQ-IF-007, REQ-CN-001, REQ-CN-003, REQ-CN-007) | REQ-016, REQ-IF-002, REQ-IF-005, REQ-IF-006, REQ-IF-007, REQ-CN-001, REQ-CN-003, REQ-CN-007 | ARCH-009 | Condition Deriver | ITP-009-B | Interface Contract Testing | ITS-009-B1 | ⬜ Untested |
| SYS-005 (REQ-006, REQ-007, REQ-008, REQ-009, REQ-010, REQ-011) | REQ-006, REQ-007, REQ-008, REQ-009, REQ-010, REQ-011 | ARCH-010 | Fault Condition Handler | ITP-010-A | Interface Contract Testing | ITS-010-A1 | ⬜ Untested |
| SYS-005 (REQ-006, REQ-007, REQ-008, REQ-009, REQ-010, REQ-011) | REQ-006, REQ-007, REQ-008, REQ-009, REQ-010, REQ-011 | ARCH-010 | Fault Condition Handler | ITP-010-B | Interface Fault Injection | ITS-010-B1 | ⬜ Untested |
| SYS-006 (REQ-013, REQ-014, REQ-015, REQ-017, REQ-IF-004) | REQ-013, REQ-014, REQ-015, REQ-017, REQ-IF-004 | ARCH-011 | Event Record Builder | ITP-011-A | Interface Contract Testing | ITS-011-A1 | ⬜ Untested |
| SYS-006 (REQ-013, REQ-014, REQ-015, REQ-017, REQ-IF-004) | REQ-013, REQ-014, REQ-015, REQ-017, REQ-IF-004 | ARCH-011 | Event Record Builder | ITP-011-B | Interface Fault Injection | ITS-011-B1 | ⬜ Untested |
| SYS-006 (REQ-013, REQ-014, REQ-015, REQ-017, REQ-IF-004) | REQ-013, REQ-014, REQ-015, REQ-017, REQ-IF-004 | ARCH-012 | Event Storage Adapter | ITP-012-A | Interface Contract Testing (External) | ITS-012-A1 | ⬜ Untested |
| SYS-006 (REQ-013, REQ-014, REQ-015, REQ-017, REQ-IF-004) | REQ-013, REQ-014, REQ-015, REQ-017, REQ-IF-004 | ARCH-012 | Event Storage Adapter | ITP-012-B | Interface Fault Injection | ITS-012-B1 | ⬜ Untested |
| SYS-007 (REQ-012, REQ-IF-003) | REQ-012, REQ-IF-003 | ARCH-013 | Notification Publisher | ITP-013-A | Interface Contract Testing (External) | ITS-013-A1 | ⬜ Untested |
| SYS-007 (REQ-012, REQ-IF-003) | REQ-012, REQ-IF-003 | ARCH-013 | Notification Publisher | ITP-013-B | Interface Fault Injection | ITS-013-B1 | ⬜ Untested |
| SYS-008 (REQ-NF-002, REQ-CN-002, REQ-CN-003, REQ-CN-004, REQ-CN-005, REQ-CN-006) | REQ-NF-002, REQ-CN-002, REQ-CN-003, REQ-CN-004, REQ-CN-005, REQ-CN-006 | ARCH-014 | Safety Lifecycle Evidence Set | ITP-014-A | Interface Contract Testing (Inspection) | ITS-014-A1 | ⬜ Untested |
| N/A (Cross-Cutting) | — | ARCH-015 | Scheduling Controller | ITP-015-A | Interface Contract Testing | ITS-015-A1 | ⬜ Untested |
| N/A (Cross-Cutting) | — | ARCH-015 | Scheduling Controller | ITP-015-B | Concurrency & Race Condition Testing | ITS-015-B1 | ⬜ Untested |
| N/A (Cross-Cutting) | — | ARCH-016 | Watchdog Supervisor | ITP-016-A | Interface Contract Testing | ITS-016-A1 | ⬜ Untested |
| N/A (Cross-Cutting) | — | ARCH-016 | Watchdog Supervisor | ITP-016-B | Interface Fault Injection | ITS-016-B1 | ⬜ Untested |

### Matrix C Coverage

| Metric | Value |
|--------|-------|
| **Total Architecture Modules (ARCH)** | 16 |
| **Total Cross-Cutting Modules** | 2 |
| **Total Integration Test Cases (ITP)** | 30 |
| **Total Integration Scenarios (ITS)** | 32 |
| **SYS → ARCH Coverage** | 8/8 (100%) |
| **ARCH → ITP Coverage** | 16/16 (100%) |

### Uncovered Requirements (REQ without ATP)

None — full coverage.

### Orphaned Test Cases (ATP without valid REQ)

None — all tests trace to requirements.

### Uncovered Requirements — System Level (REQ without SYS)

None — full coverage.

### Orphaned System Test Cases (STP without valid SYS)

None — all system tests trace to components.

### Uncovered System Components — Architecture Level (SYS without ARCH)

None — full coverage.

### Orphaned Integration Test Cases (ITP without valid ARCH)

None — all integration tests trace to modules.

## Matrix D — Implementation Verification (Module View)

| Architecture Module (ARCH) | Parent System | Module Design (MOD) | Module Name | Test Case ID (UTP) | Technique | Scenario ID (UTS) | Status |
|---------------------------|---------------|---------------------|-------------|--------------------|-----------|--------------------|--------|
| ARCH-001 (SYS-001) | SYS-001 | MOD-001 | cruise_eval_transition | UTP-001-A | Statement & Branch Coverage | UTS-001-A1 | ⬜ Untested |
| ARCH-001 (SYS-001) | SYS-001 | MOD-001 | cruise_eval_transition | UTP-001-A | Statement & Branch Coverage | UTS-001-A2 | ⬜ Untested |
| ARCH-001 (SYS-001) | SYS-001 | MOD-001 | cruise_eval_transition | UTP-001-A | Statement & Branch Coverage | UTS-001-A3 | ⬜ Untested |
| ARCH-001 (SYS-001) | SYS-001 | MOD-001 | cruise_eval_transition | UTP-001-B | Equivalence Partitioning | UTS-001-B1 | ⬜ Untested |
| ARCH-001 (SYS-001) | SYS-001 | MOD-002 | cruise_check_preconditions | UTP-002-A | Statement & Branch Coverage + MC/DC Coverage | UTS-002-A1 | ⬜ Untested |
| ARCH-001 (SYS-001) | SYS-001 | MOD-002 | cruise_check_preconditions | UTP-002-A | Statement & Branch Coverage + MC/DC Coverage | UTS-002-A2 | ⬜ Untested |
| ARCH-001 (SYS-001) | SYS-001 | MOD-002 | cruise_check_preconditions | UTP-002-A | Statement & Branch Coverage + MC/DC Coverage | UTS-002-A3 | ⬜ Untested |
| ARCH-001 (SYS-001) | SYS-001 | MOD-002 | cruise_check_preconditions | UTP-002-A | Statement & Branch Coverage + MC/DC Coverage | UTS-002-A4 | ⬜ Untested |
| ARCH-002 (SYS-001) | SYS-001 | MOD-003 | cruise_state_register | UTP-003-A | Equivalence Partitioning + Variable-Level Fault Injection | UTS-003-A1 | ⬜ Untested |
| ARCH-002 (SYS-001) | SYS-001 | MOD-003 | cruise_state_register | UTP-003-A | Equivalence Partitioning + Variable-Level Fault Injection | UTS-003-A2 | ⬜ Untested |
| ARCH-002 (SYS-001) | SYS-001 | MOD-003 | cruise_state_register | UTP-003-B | Statement & Branch Coverage | UTS-003-B1 | ⬜ Untested |
| ARCH-003 (SYS-002) | SYS-002 | MOD-004 | cruise_sequence_override | UTP-004-A | Statement & Branch Coverage | UTS-004-A1 | ⬜ Untested |
| ARCH-003 (SYS-002) | SYS-002 | MOD-004 | cruise_sequence_override | UTP-004-A | Statement & Branch Coverage | UTS-004-A2 | ⬜ Untested |
| ARCH-003 (SYS-002) | SYS-002 | MOD-004 | cruise_sequence_override | UTP-004-A | Statement & Branch Coverage | UTS-004-A3 | ⬜ Untested |
| ARCH-004 (SYS-002) | SYS-002 | MOD-005 | cruise_timing_supervisor | UTP-005-A | State Transition Testing | UTS-005-A1 | ⬜ Untested |
| ARCH-004 (SYS-002) | SYS-002 | MOD-005 | cruise_timing_supervisor | UTP-005-A | State Transition Testing | UTS-005-A2 | ⬜ Untested |
| ARCH-004 (SYS-002) | SYS-002 | MOD-005 | cruise_timing_supervisor | UTP-005-A | State Transition Testing | UTS-005-A3 | ⬜ Untested |
| ARCH-004 (SYS-002) | SYS-002 | MOD-005 | cruise_timing_supervisor | UTP-005-A | State Transition Testing | UTS-005-A4 | ⬜ Untested |
| ARCH-004 (SYS-002) | SYS-002 | MOD-005 | cruise_timing_supervisor | UTP-005-A | State Transition Testing | UTS-005-A5 | ⬜ Untested |
| ARCH-004 (SYS-002) | SYS-002 | MOD-005 | cruise_timing_supervisor | UTP-005-B | Boundary Value Analysis | UTS-005-B1 | ⬜ Untested |
| ARCH-004 (SYS-002) | SYS-002 | MOD-005 | cruise_timing_supervisor | UTP-005-B | Boundary Value Analysis | UTS-005-B2 | ⬜ Untested |
| ARCH-004 (SYS-002) | SYS-002 | MOD-005 | cruise_timing_supervisor | UTP-005-B | Boundary Value Analysis | UTS-005-B3 | ⬜ Untested |
| ARCH-005 (SYS-003) | SYS-003 | MOD-006 | cruise_generate_speed_cmd | UTP-006-A | Statement & Branch Coverage | UTS-006-A1 | ⬜ Untested |
| ARCH-005 (SYS-003) | SYS-003 | MOD-006 | cruise_generate_speed_cmd | UTP-006-A | Statement & Branch Coverage | UTS-006-A2 | ⬜ Untested |
| ARCH-006 (SYS-003) | SYS-003 | MOD-007 | cruise_propulsion_write | UTP-007-A | Equivalence Partitioning + Statement & Branch Coverage | UTS-007-A1 | ⬜ Untested |
| ARCH-006 (SYS-003) | SYS-003 | MOD-007 | cruise_propulsion_write | UTP-007-A | Equivalence Partitioning + Statement & Branch Coverage | UTS-007-A2 | ⬜ Untested |
| ARCH-006 (SYS-003) | SYS-003 | MOD-007 | cruise_propulsion_write | UTP-007-A | Equivalence Partitioning + Statement & Branch Coverage | UTS-007-A3 | ⬜ Untested |
| ARCH-007 (SYS-004) | SYS-004 | MOD-008 | cruise_read_platform_inputs | UTP-008-A | Statement & Branch Coverage + Strict Isolation | UTS-008-A1 | ⬜ Untested |
| ARCH-007 (SYS-004) | SYS-004 | MOD-008 | cruise_read_platform_inputs | UTP-008-A | Statement & Branch Coverage + Strict Isolation | UTS-008-A2 | ⬜ Untested |
| ARCH-007 (SYS-004) | SYS-004 | MOD-008 | cruise_read_platform_inputs | UTP-008-A | Statement & Branch Coverage + Strict Isolation | UTS-008-A3 | ⬜ Untested |
| ARCH-008 (SYS-004) | SYS-004 | MOD-009 | cruise_verify_integrity | UTP-009-A | Statement & Branch Coverage | UTS-009-A1 | ⬜ Untested |
| ARCH-008 (SYS-004) | SYS-004 | MOD-009 | cruise_verify_integrity | UTP-009-A | Statement & Branch Coverage | UTS-009-A2 | ⬜ Untested |
| ARCH-008 (SYS-004) | SYS-004 | MOD-009 | cruise_verify_integrity | UTP-009-A | Statement & Branch Coverage | UTS-009-A3 | ⬜ Untested |
| ARCH-008 (SYS-004) | SYS-004 | MOD-009 | cruise_verify_integrity | UTP-009-B | Boundary Value Analysis | UTS-009-B1 | ⬜ Untested |
| ARCH-008 (SYS-004) | SYS-004 | MOD-009 | cruise_verify_integrity | UTP-009-B | Boundary Value Analysis | UTS-009-B2 | ⬜ Untested |
| ARCH-008 (SYS-004) | SYS-004 | MOD-009 | cruise_verify_integrity | UTP-009-C | MC/DC Coverage | UTS-009-C1 | ⬜ Untested |
| ARCH-008 (SYS-004) | SYS-004 | MOD-009 | cruise_verify_integrity | UTP-009-C | MC/DC Coverage | UTS-009-C2 | ⬜ Untested |
| ARCH-008 (SYS-004) | SYS-004 | MOD-009 | cruise_verify_integrity | UTP-009-C | MC/DC Coverage | UTS-009-C3 | ⬜ Untested |
| ARCH-009 (SYS-004) | SYS-004 | MOD-010 | cruise_derive_conditions | UTP-010-A | Equivalence Partitioning + Statement & Branch Coverage | UTS-010-A1 | ⬜ Untested |
| ARCH-009 (SYS-004) | SYS-004 | MOD-010 | cruise_derive_conditions | UTP-010-A | Equivalence Partitioning + Statement & Branch Coverage | UTS-010-A2 | ⬜ Untested |
| ARCH-009 (SYS-004) | SYS-004 | MOD-010 | cruise_derive_conditions | UTP-010-A | Equivalence Partitioning + Statement & Branch Coverage | UTS-010-A3 | ⬜ Untested |
| ARCH-009 (SYS-004) | SYS-004 | MOD-010 | cruise_derive_conditions | UTP-010-A | Equivalence Partitioning + Statement & Branch Coverage | UTS-010-A4 | ⬜ Untested |
| ARCH-010 (SYS-005) | SYS-005 | MOD-011 | cruise_handle_fault | UTP-011-A | Statement & Branch Coverage + Equivalence Partitioning | UTS-011-A1 | ⬜ Untested |
| ARCH-010 (SYS-005) | SYS-005 | MOD-011 | cruise_handle_fault | UTP-011-A | Statement & Branch Coverage + Equivalence Partitioning | UTS-011-A2 | ⬜ Untested |
| ARCH-010 (SYS-005) | SYS-005 | MOD-011 | cruise_handle_fault | UTP-011-A | Statement & Branch Coverage + Equivalence Partitioning | UTS-011-A3 | ⬜ Untested |
| ARCH-011 (SYS-006) | SYS-006 | MOD-012 | cruise_build_event_record | UTP-012-A | Statement & Branch Coverage + Boundary Value Analysis | UTS-012-A1 | ⬜ Untested |
| ARCH-011 (SYS-006) | SYS-006 | MOD-012 | cruise_build_event_record | UTP-012-A | Statement & Branch Coverage + Boundary Value Analysis | UTS-012-A2 | ⬜ Untested |
| ARCH-011 (SYS-006) | SYS-006 | MOD-012 | cruise_build_event_record | UTP-012-A | Statement & Branch Coverage + Boundary Value Analysis | UTS-012-A3 | ⬜ Untested |
| ARCH-011 (SYS-006) | SYS-006 | MOD-012 | cruise_build_event_record | UTP-012-A | Statement & Branch Coverage + Boundary Value Analysis | UTS-012-A4 | ⬜ Untested |
| ARCH-012 (SYS-006) | SYS-006 | MOD-013 | cruise_write_event_record | UTP-013-A | Statement & Branch Coverage + Strict Isolation | UTS-013-A1 | ⬜ Untested |
| ARCH-012 (SYS-006) | SYS-006 | MOD-013 | cruise_write_event_record | UTP-013-A | Statement & Branch Coverage + Strict Isolation | UTS-013-A2 | ⬜ Untested |
| ARCH-012 (SYS-006) | SYS-006 | MOD-013 | cruise_write_event_record | UTP-013-A | Statement & Branch Coverage + Strict Isolation | UTS-013-A3 | ⬜ Untested |
| ARCH-013 (SYS-007) | SYS-007 | MOD-014 | cruise_publish_notification | UTP-014-A | Statement & Branch Coverage + Equivalence Partitioning | UTS-014-A1 | ⬜ Untested |
| ARCH-013 (SYS-007) | SYS-007 | MOD-014 | cruise_publish_notification | UTP-014-A | Statement & Branch Coverage + Equivalence Partitioning | UTS-014-A2 | ⬜ Untested |
| ARCH-013 (SYS-007) | SYS-007 | MOD-014 | cruise_publish_notification | UTP-014-B | Boundary Value Analysis | UTS-014-B1 | ⬜ Untested |
| ARCH-014 (SYS-008) | SYS-008 | MOD-015 | lifecycle_evidence_manifest | UTP-015-A | Inspection | UTS-015-A1 | ⬜ Untested |
| ARCH-015 ([CROSS-CUTTING]) | [CROSS-CUTTING] | MOD-016 | scheduling_controller | UTP-016-A | State Transition Testing | UTS-016-A1 | ⬜ Untested |
| ARCH-015 ([CROSS-CUTTING]) | [CROSS-CUTTING] | MOD-016 | scheduling_controller | UTP-016-A | State Transition Testing | UTS-016-A2 | ⬜ Untested |
| ARCH-015 ([CROSS-CUTTING]) | [CROSS-CUTTING] | MOD-016 | scheduling_controller | UTP-016-B | Statement & Branch Coverage | UTS-016-B1 | ⬜ Untested |
| ARCH-016 ([CROSS-CUTTING]) | [CROSS-CUTTING] | MOD-017 | watchdog_supervisor | UTP-017-A | State Transition Testing | UTS-017-A1 | ⬜ Untested |
| ARCH-016 ([CROSS-CUTTING]) | [CROSS-CUTTING] | MOD-017 | watchdog_supervisor | UTP-017-A | State Transition Testing | UTS-017-A2 | ⬜ Untested |
| ARCH-016 ([CROSS-CUTTING]) | [CROSS-CUTTING] | MOD-017 | watchdog_supervisor | UTP-017-A | State Transition Testing | UTS-017-A3 | ⬜ Untested |
| ARCH-016 ([CROSS-CUTTING]) | [CROSS-CUTTING] | MOD-017 | watchdog_supervisor | UTP-017-B | Boundary Value Analysis | UTS-017-B1 | ⬜ Untested |
| ARCH-016 ([CROSS-CUTTING]) | [CROSS-CUTTING] | MOD-017 | watchdog_supervisor | UTP-017-B | Boundary Value Analysis | UTS-017-B2 | ⬜ Untested |

### Matrix D Coverage

| Metric | Value |
|--------|-------|
| **Total Module Designs (MOD)** | 17 |
| **External Modules** | 0 |
| **Testable Modules** | 17 |
| **Total Unit Test Cases (UTP)** | 25 |
| **Total Unit Scenarios (UTS)** | 64 |
| **ARCH → MOD Coverage** | 16/16 (100%) |
| **MOD → UTP Coverage** | 17/17 (100%) |

## Matrix H — Hazard Traceability

| HAZ ID | Mitigation | Verification | Status |
|--------|-----------|-------------|--------|
| HAZ-001 | REQ-001 | ATP-001-C ATP-001-B ATP-001-A | ⬜ Pending |
| | REQ-002 | ATP-002-A ATP-002-B ATP-002-C | ⬜ Pending |
| | REQ-004 | ATP-004-B ATP-004-A | ⬜ Pending |
| | SYS-001 | STP-001-A STP-001-B STP-001-C | ⬜ Pending |
| | SYS-002 | STP-002-A STP-002-B STP-002-C | ⬜ Pending |
| HAZ-002 | REQ-001 | ATP-001-C ATP-001-B ATP-001-A | ⬜ Pending |
| | REQ-004 | ATP-004-B ATP-004-A | ⬜ Pending |
| | SYS-001 | STP-001-A STP-001-B STP-001-C | ⬜ Pending |
| HAZ-003 | REQ-005 | ATP-005-C ATP-005-B ATP-005-A | ⬜ Pending |
| | REQ-018 | ATP-018-A ATP-018-B | ⬜ Pending |
| | SYS-001 | STP-001-A STP-001-B STP-001-C | ⬜ Pending |
| HAZ-004 | REQ-002 | ATP-002-A ATP-002-B ATP-002-C | ⬜ Pending |
| | REQ-003 | ATP-003-A ATP-003-B | ⬜ Pending |
| | REQ-013 | ATP-013-B ATP-013-A | ⬜ Pending |
| | SYS-002 | STP-002-A STP-002-B STP-002-C | ⬜ Pending |
| | SYS-003 | STP-003-A STP-003-B STP-003-C STP-003-D | ⬜ Pending |
| HAZ-005 | REQ-003 | ATP-003-A ATP-003-B | ⬜ Pending |
| | REQ-013 | ATP-013-B ATP-013-A | ⬜ Pending |
| | SYS-002 | STP-002-A STP-002-B STP-002-C | ⬜ Pending |
| | SYS-006 | STP-006-A STP-006-B STP-006-C | ⬜ Pending |
| HAZ-006 | REQ-NF-001 | ATP-NF-001-B ATP-NF-001-A | ⬜ Pending |
| | SYS-002 | STP-002-A STP-002-B STP-002-C | ⬜ Pending |
| HAZ-007 | REQ-002 | ATP-002-A ATP-002-B ATP-002-C | ⬜ Pending |
| | SYS-003 | STP-003-A STP-003-B STP-003-C STP-003-D | ⬜ Pending |
| HAZ-008 | REQ-001 | ATP-001-C ATP-001-B ATP-001-A | ⬜ Pending |
| | SYS-003 | STP-003-A STP-003-B STP-003-C STP-003-D | ⬜ Pending |
| | SYS-001 | STP-001-A STP-001-B STP-001-C | ⬜ Pending |
| HAZ-009 | REQ-016 | ATP-016-B ATP-016-A | ⬜ Pending |
| | REQ-IF-002 | ATP-IF-002-A ATP-IF-002-B | ⬜ Pending |
| | SYS-004 | STP-004-A STP-004-B STP-004-C STP-004-D | ⬜ Pending |
| HAZ-010 | REQ-CN-003 | ATP-CN-003-A | ⬜ Pending |
| | REQ-IF-002 | ATP-IF-002-A ATP-IF-002-B | ⬜ Pending |
| | SYS-004 | STP-004-A STP-004-B STP-004-C STP-004-D | ⬜ Pending |
| | SYS-008 | STP-008-A STP-008-B | ⬜ Pending |
| HAZ-011 | REQ-016 | ATP-016-B ATP-016-A | ⬜ Pending |
| | SYS-004 | STP-004-A STP-004-B STP-004-C STP-004-D | ⬜ Pending |
| | REQ-IF-002 | ATP-IF-002-A ATP-IF-002-B | ⬜ Pending |
| HAZ-012 | REQ-010 | ATP-010-A ATP-010-C ATP-010-B | ⬜ Pending |
| | REQ-011 | ATP-011-A ATP-011-B ATP-011-C | ⬜ Pending |
| | SYS-004 | STP-004-A STP-004-B STP-004-C STP-004-D | ⬜ Pending |
| HAZ-013 | REQ-006 | ATP-006-A ATP-006-B | ⬜ Pending |
| | REQ-007 | ATP-007-A ATP-007-B | ⬜ Pending |
| | SYS-005 | STP-005-A STP-005-B STP-005-C | ⬜ Pending |
| HAZ-014 | REQ-008 | ATP-008-B ATP-008-A | ⬜ Pending |
| | REQ-009 | ATP-009-B ATP-009-A | ⬜ Pending |
| | SYS-005 | STP-005-A STP-005-B STP-005-C | ⬜ Pending |
| HAZ-015 | REQ-010 | ATP-010-A ATP-010-C ATP-010-B | ⬜ Pending |
| | REQ-011 | ATP-011-A ATP-011-B ATP-011-C | ⬜ Pending |
| | SYS-005 | STP-005-A STP-005-B STP-005-C | ⬜ Pending |
| | SYS-004 | STP-004-A STP-004-B STP-004-C STP-004-D | ⬜ Pending |
| HAZ-016 | REQ-013 | ATP-013-B ATP-013-A | ⬜ Pending |
| | REQ-017 | ATP-017-A | ⬜ Pending |
| | SYS-006 | STP-006-A STP-006-B STP-006-C | ⬜ Pending |
| HAZ-017 | REQ-014 | ATP-014-A ATP-014-B | ⬜ Pending |
| | SYS-006 | STP-006-A STP-006-B STP-006-C | ⬜ Pending |
| HAZ-018 | REQ-012 | ATP-012-B ATP-012-A | ⬜ Pending |
| | SYS-007 | STP-007-A STP-007-B | ⬜ Pending |
| HAZ-019 | REQ-012 | ATP-012-B ATP-012-A | ⬜ Pending |
| | SYS-007 | STP-007-A STP-007-B | ⬜ Pending |
| HAZ-020 | REQ-CN-002 | ATP-CN-002-A | ⬜ Pending |
| | REQ-NF-002 | ATP-NF-002-A | ⬜ Pending |
| | SYS-008 | STP-008-A STP-008-B | ⬜ Pending |
| HAZ-021 | REQ-CN-003 | ATP-CN-003-A | ⬜ Pending |
| | SYS-008 | STP-008-A STP-008-B | ⬜ Pending |
| | SYS-004 | STP-004-A STP-004-B STP-004-C STP-004-D | ⬜ Pending |

### Matrix H Coverage

| Metric | Value |
|--------|-------|
| **Total Hazards (HAZ)** | 21 |
| **HAZ with Verification** | 21/21 (100%) |

## Audit Notes

- **Matrix generated by**: `build-matrix.sh` (deterministic regex parser)
- **Source documents**: `requirements.md`, `acceptance-plan.md`, `system-design.md`, `system-test.md`, `architecture-design.md`, `integration-test.md`, `module-design.md`, `unit-test.md`, `hazard-analysis.md`
- **Last validated**: 2026-06-02

---

## Baseline Information

| Property | Value |
|----------|-------|
| Matrix Generated | 2026-06-02 (final — all five matrices complete) |
| Requirements Source | `specs/001-cruise-brake-override/v-model/requirements.md` |
| Requirements | 34 active, all open questions resolved |
| Acceptance Plan Source | `specs/001-cruise-brake-override/v-model/acceptance-plan.md` |
| System Design Source | `specs/001-cruise-brake-override/v-model/system-design.md` |
| System Test Source | `specs/001-cruise-brake-override/v-model/system-test.md` |
| Architecture Design Source | `specs/001-cruise-brake-override/v-model/architecture-design.md` |
| Integration Test Source | `specs/001-cruise-brake-override/v-model/integration-test.md` |
| Module Design Source | `specs/001-cruise-brake-override/v-model/module-design.md` |
| Unit Test Source | `specs/001-cruise-brake-override/v-model/unit-test.md` |
| Hazard Analysis Source | `specs/001-cruise-brake-override/v-model/hazard-analysis.md` |
| Validation Tool | `build-matrix.sh` (deterministic — not AI-generated) |
| Git Commit | uncommitted changes |
| Domain Overlay | ISO 26262 (ASIL D — Confirmed) |
