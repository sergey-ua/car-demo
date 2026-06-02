# Hazard Analysis (FMEA): Vehicle Cruise Control — Brake Override Safety Slice

**Feature Branch**: `001-cruise-brake-override`
**Created**: 2026-06-02
**Status**: Approved
**Source**: `specs/001-cruise-brake-override/v-model/system-design.md`
**Standard**: IEC 60812:2018 (FMEA procedure) + ISO 26262-3 §7 (HARA)
**Domain Overlay**: ISO 26262 — ASIL classification replaces generic severity scale

---

## Overview

This FMEA assesses all eight system components (`SYS-001` through `SYS-008`) defined
in `system-design.md` for potential failure modes across the five operational states of
the cruise control feature. The analysis follows IEC 60812:2018 §6 in strict order:
item definition → failure mode identification → effect analysis → detection assessment →
risk estimation → mitigation.

Risk classification uses the ISO 26262 Part 3 §7 Hazard Analysis and Risk Assessment
(HARA) methodology. Each hazard is rated on three independent parameters — Severity
(S0–S3), Exposure (E1–E4), and Controllability (C0–C3) — and the derived ASIL (QM, A,
B, C, D) is documented alongside the mitigation.

**Primary motivating hazard**: A failure to disengage cruise control when the driver
applies the brake can result in uncontrolled longitudinal acceleration against braking
intent, with life-threatening consequences. Multiple components in this feature are
safety barriers against this scenario.

---

## ID Schema

- **Hazard ID**: `HAZ-NNN` — 3-digit zero-padded, sequential, never renumbered.
- **Mitigation lineage**: From `HAZ-NNN`, the Mitigation column references `REQ-NNN`
  and/or `SYS-NNN`. These link to test cases (Matrix H) in `traceability-matrix.md`.

---

## ISO 26262 HARA Parameters

### Severity Scale (ISO 26262-3 Table 1)

| Rating | ASIL Contribution | Definition |
|--------|------------------|-----------|
| S3 | Highest | Life-threatening (survival uncertain or probable) |
| S2 | High | Severe and life-altering injuries |
| S1 | Medium | Light and moderately severe injuries |
| S0 | None | No injuries |

### Exposure Scale (ISO 26262-3 Table 2)

| Rating | Definition | Example |
|--------|-----------|---------|
| E4 | High probability — occurs in most operating conditions | Driving on public road at any speed |
| E3 | Medium probability — occurs occasionally in normal operation | Motorway driving with cruise enabled |
| E2 | Low probability — occurs in some specific conditions | Specific road conditions or vehicle states |
| E1 | Very low / incredible probability | Exceptional scenario |

### Controllability Scale (ISO 26262-3 Table 3)

| Rating | Definition | Example |
|--------|-----------|---------|
| C3 | Difficult or uncontrollable — most drivers cannot avoid harm | Sudden uncontrolled acceleration against braking |
| C2 | Normally controllable — most drivers can control with effort | Unexpected deactivation requiring driver attention |
| C1 | Easily controllable — most drivers can react without difficulty | Minor interface anomaly |
| C0 | Controllable in general | No significant driver challenge |

### ASIL Derivation Table (ISO 26262-3 Table 4 — selected rows used in this analysis)

| S | E | C | → ASIL |
|---|---|---|--------|
| S3 | E4 | C3 | **ASIL D** |
| S3 | E4 | C2 | ASIL C |
| S3 | E4 | C1 | ASIL B |
| S3 | E3 | C3 | ASIL C |
| S3 | E3 | C2 | ASIL B |
| S3 | E3 | C1 | ASIL A |
| S3 | E2 | C3 | ASIL B |
| S3 | E2 | C2 | ASIL A |
| S2 | E4 | C3 | ASIL C |
| S2 | E4 | C2 | ASIL B |
| S2 | E4 | C1 | ASIL A |
| S2 | E3 | C3 | ASIL B |
| S2 | E3 | C2 | ASIL A |
| S2 | E2 | C2 | QM |
| S1 | Eany | Cany | QM–ASIL A |
| S0 | Eany | Cany | QM |

### ASIL Mitigation Verification Requirements

| ASIL | Required Rigor |
|------|---------------|
| ASIL D | Formal/exhaustive verification; independent review mandatory |
| ASIL C | Comprehensive testing; independent review recommended |
| ASIL B | Systematic testing required |
| ASIL A | Standard testing sufficient |
| QM | Quality management processes only |

---

## Operational States Reference

States extracted from `system-design.md` (SYS-001 Decomposition View):

| State | Description | Cruise Active? |
|-------|-------------|---------------|
| CRUISE_STANDBY | Feature available but not actively controlling speed | No |
| CRUISE_ACTIVE | Feature holds authority and actively maintains vehicle speed | **Yes** |
| `Cruise_Suspended` | Cruise temporarily suspended; reactivation possible | No |
| `Cruise_Cancelled` | Cruise explicitly cancelled | No |
| `Cruise_Fault` | Self-diagnosed fault; no speed control permitted | No |

Hazards with identical severity across all states use `ALL`. Where severity differs
by state, separate HAZ entries are created.

---

## Hazard Register (FMEA)

> **Table column definitions**:
> - **Severity**: S-rating (S0–S3) with rationale in Effect column
> - **Likelihood**: Exposure (E1–E4) / Controllability (C0–C3)
> - **Risk Level**: Derived ASIL (QM, A, B, C, D)
> - **Residual Risk**: ASIL status after mitigations applied

| HAZ ID | Component | Failure Mode | Op. State | Effect (S/E/C rationale) | Severity | Likelihood (E/C) | Risk Level | Mitigation | Residual Risk |
|--------|-----------|-------------|-----------|--------------------------|----------|-----------------|------------|------------|---------------|
| HAZ-001 | SYS-001 | State machine fails to transition out of CRUISE_ACTIVE when `Brake_Override` signal is received | CRUISE_ACTIVE | Vehicle continues longitudinal speed control against driver braking intent; driver cannot stop vehicle as expected. S3: survival uncertain if vehicle accelerates into obstacle. E4: any driver using cruise on public roads. C3: driver has very limited time to override uncontrolled acceleration while already braking. | S3 | E4 / C3 | **ASIL D** | REQ-001 (state machine definition), REQ-002 (cessation), REQ-004 (transition), SYS-001 (state variable range check — Prevention), SYS-002 (override response coordinator) | Acceptable if mitigations implemented and verified to ASIL D. ASIL decomposition into ASIL B(D)+ASIL B(D) may be considered at architecture phase. |
| HAZ-002 | SYS-001 | State machine transitions to undefined or unexpected state after `Brake_Override` instead of the defined non-active state | CRUISE_ACTIVE | System appears to have responded but is in an unspecified state; subsequent behaviour is undefined and may include issuing speed commands. S3: undetected continued speed control is life-threatening. E4: occurs whenever cruise is active and braking is applied. C3: driver cannot distinguish correct from incorrect override response. | S3 | E4 / C3 | **ASIL D** | REQ-001 (closed state set), REQ-004 (defined target state), SYS-001 (state variable range check) | **Acceptable** — target state is `Cruise_Cancelled` (OQ-003 resolved 2026-06-02). Residual risk acceptable when REQ-001 and REQ-004 are implemented and verified to ASIL D. |
| HAZ-003 | SYS-001 | Reactivation guard fails — state machine permits CRUISE_ACTIVE entry before all activation preconditions are confirmed valid after `Brake_Override` | CRUISE_SUSPENDED, CRUISE_CANCELLED | Cruise reactivates without driver intent or before safe conditions are established; unexpected longitudinal acceleration. S3: unexpected acceleration is life-threatening if vehicle is in traffic. E3: occurs when driver has recently overridden and conditions change rapidly. C2: driver is aware of attempted activation context and can normally react. | S3 | E3 / C2 | **ASIL B** | REQ-005 (reactivation inhibit), REQ-018 (five activation preconditions — **DR-001 resolved 2026-06-02**), SYS-001 (reactivation guard — Prevention) | **Acceptable** — DR-001 resolved 2026-06-02 via REQ-018. Residual risk acceptable when REQ-005 and REQ-018 are implemented and verified to ASIL B. |
| HAZ-004 | SYS-002 | Brake Override Response Coordinator fails to initiate response when `Brake_Override` event is received from SYS-004 | CRUISE_ACTIVE | SYS-002 receives the event but does not issue cessation command to SYS-003 or trigger any downstream response; speed control continues unabated. S3: identical to HAZ-001 — life-threatening uncontrolled speed control. E4: occurs whenever Brake_Override is applied with cruise active. C3: no driver-visible indication of the failure. | S3 | E4 / C3 | **ASIL D** | REQ-002 (cessation), REQ-003 (authority release), REQ-013 (logging as early detection), SYS-002 (override response sequencing — Mitigation), SYS-003 (cessation command execution) | Acceptable if REQ-002 and REQ-003 are implemented and verified to ASIL D. Independence from SYS-001 state read required (SYS-002 depends on SYS-001 per Dependency View). |
| HAZ-005 | SYS-002 | Response sequence is partially executed — cessation commanded but authority release or logging is incomplete | CRUISE_ACTIVE | Speed control stops (safe immediate outcome) but authority handover is not confirmed; driver may not know whether the system or they hold longitudinal authority. S2: severe injury possible if authority ambiguity leads to a secondary event. E3: partial failure is less frequent than total failure. C2: driver is physically braking; can normally recover but authority confirmation is missing. | S2 | E3 / C2 | **ASIL A** | REQ-003 (explicit authority release), REQ-013 (logging), SYS-002 (response sequencing), SYS-006 (write-before-complete) | Acceptable if REQ-003 sequencing and SYS-006 write confirmation are implemented to ASIL A. |
| HAZ-006 | SYS-002 | Override response latency exceeds **100 ms** (T_max = 100 ms; OQ-002 resolved 2026-06-02) | CRUISE_ACTIVE | Vehicle continues at cruise speed while driver is braking for a duration that increases stopping distance; at high speed in traffic this is life-threatening. S3: delay is directly correlated with stopping distance increase. E4: applies whenever brake override occurs. C3: driver cannot compensate for delayed system response while braking. | S3 | E4 / C3 | **ASIL D** | REQ-NF-001 (T_max = 100 ms — **OQ-002 resolved 2026-06-02**), SYS-002 (response timing supervision — Detection via ARCH-004), scheduling period ≤ 20 ms (ARCH-015) | **Acceptable** — T_max = 100 ms is defined and traceable to REQ-NF-001. Verified by ATP-NF-001-A/B (100-run latency test) and resource usage WCET analysis. Residual risk is acceptable when REQ-NF-001 is implemented and verified to ASIL D. |
| HAZ-007 | SYS-003 | Longitudinal Speed Controller continues issuing speed commands after cessation command is received from SYS-002 or SYS-005 | CRUISE_ACTIVE | Vehicle accelerates or maintains speed despite cessation having been commanded; equivalent to HAZ-001/004 at the output layer. S3: direct life-threatening continued speed control. E4: applies on every Brake_Override with cruise active. C3: driver has no indication cessation was not executed. | S3 | E4 / C3 | **ASIL D** | REQ-002 (cessation requirement), SYS-003 (cessation command execution — Mitigation; supersedes in-progress command), STP-003-B (interface contract test) | Acceptable if SYS-003 cessation execution mechanism is implemented and verified to ASIL D. |
| HAZ-008 | SYS-003 | Longitudinal Speed Controller issues speed commands while SYS-001 reports a non-active state | CRUISE_STANDBY, CRUISE_SUSPENDED, CRUISE_CANCELLED, CRUISE_FAULT | Unexpected acceleration when driver believes cruise is disengaged. S3: surprise acceleration is life-threatening if driver is not attending to longitudinal control. E2: requires SYS-001 state corruption or SYS-003 gate failure simultaneously — low probability. C2: driver can normally react to unexpected acceleration if they notice it. | S3 | E2 / C2 | **ASIL A** | REQ-001 (state machine — closed set), SYS-003 (exclusive interface guard — Prevention: state-gated command issuance), SYS-001 (state authorisation) | Acceptable if SYS-003 exclusive interface guard and SYS-001 state authorisation are implemented and verified to ASIL A. |
| HAZ-009 | SYS-004 | Platform Input Monitor fails to detect `Brake_Override` from brake pedal status input (missed detection) | CRUISE_ACTIVE | `Brake_Override` condition not derived → SYS-002 not triggered → speed control continues unabated → collision. S3: this is the upstream failure that enables HAZ-001, HAZ-004, HAZ-007. E4: applies whenever cruise is active and driver brakes. C3: driver cannot detect that the monitoring component has failed. | S3 | E4 / C3 | **ASIL D** | REQ-016 (continuous monitoring), REQ-IF-002 (condition derivation), SYS-004 (input integrity verification — Prevention; condition plausibility check — Detection), STP-004-A (external interface test) | Acceptable if SYS-004 input monitoring and integrity check are implemented and verified to ASIL D. This is the most upstream safety barrier. |
| HAZ-010 | SYS-004 | Platform Input Monitor derives incorrect condition from a tampered or replayed brake pedal status signal (cybersecurity attack) | CRUISE_ACTIVE | Adversary replays a "brake released" signal, suppressing `Brake_Override` detection; speed control continues against actual braking → collision. S3: life-threatening if attack is successful. E2: requires adversary with communication access to the EV prototype platform. C3: driver cannot detect the attack. | S3 | E2 / C3 | **ASIL B** | REQ-CN-003 (cybersecurity threat analysis — ISO/SAE 21434), REQ-IF-002 (integrity verification), SYS-004 (input integrity verification — Prevention), SYS-008 (cybersecurity process), STP-008-B (cybersecurity inspection) | Acceptable if input integrity mechanism addresses replay/tampering threats as identified in the threat analysis (REQ-CN-003) and verified to ASIL B. Threat treatment per ISO/SAE 21434 required before field testing. |
| HAZ-011 | SYS-004 | Platform Input Monitor generates spurious `Brake_Override` when no brake is applied (false positive) | CRUISE_ACTIVE | Cruise control unexpectedly deactivates without driver intent; sudden loss of speed maintenance may cause following traffic issue. S1: light nuisance/startle; driver retains full brake and throttle control. E3: occasional false triggers are possible with noisy inputs. C1: driver easily recovers by re-engaging cruise or adjusting throttle. | S1 | E3 / C1 | QM | REQ-016 (continuous monitoring), SYS-004 (input plausibility check — Detection), REQ-IF-002 | QM. No safety mechanism beyond quality management required. Monitor availability impact. |
| HAZ-012 | SYS-004 | Platform Input Monitor fails to detect `Required_Input_Invalid` — passes integrity-failed input as valid | CRUISE_ACTIVE | Feature continues speed control using corrupted input (e.g., incorrect vehicle speed reference); speed control behaviour is based on bad data, leading to incorrect speed targeting. S2: severe risk if speed is controlled to significantly wrong target. E3: occasional — depends on the integrity failure rate. C2: driver may notice erratic speed behaviour and brake. | S2 | E3 / C2 | **ASIL A** | REQ-010 (reject Cruise_Active on RII), REQ-011 (exit Cruise_Active on RII), SYS-004 (input integrity verification — Prevention), STP-004-A/C | Acceptable if SYS-004 integrity check is implemented and verified to ASIL A. |
| HAZ-013 | SYS-005 | Fault and Condition Supervisor fails to process `Longitudinal_Control_Unavailable` — does not issue cessation or transition request | CRUISE_ACTIVE | SYS-003 continues issuing speed commands after the platform has withdrawn longitudinal control authority; platform authority withdrawal and feature commands conflict → unsafe vehicle behaviour. S3: conflicting longitudinal commands are life-threatening. E3: LCU condition is occasional in normal operation. C3: driver cannot detect internal platform/feature conflict. | S3 | E3 / C3 | **ASIL C** | REQ-006 (cessation on LCU), REQ-007 (transition on LCU), SYS-005 (fault condition detection and routing — Detection + Mitigation), STP-005-A | Acceptable if SYS-005 LCU handling is implemented and verified to ASIL C. |
| HAZ-014 | SYS-005 | Fault and Condition Supervisor fails to process `Cruise_Control_Fault` — feature does not enter `Cruise_Fault` | ALL | Self-diagnosed fault contained within the feature is ignored; feature continues operating in a known-faulty state. S2: severe — behaviour in faulty state is undefined and may include hazardous speed commands. E3: fault events are occasional in prototype testing. C2: driver may notice erratic behaviour but may not be able to identify the cause. | S2 | E3 / C2 | **ASIL A** | REQ-008 (Cruise_Fault transition), REQ-009 (cessation on CCF), SYS-005 (fault condition routing), STP-005-B | Acceptable if SYS-005 CCF handling is implemented and verified to ASIL A. |
| HAZ-015 | SYS-005 | Fault and Condition Supervisor fails to process `Required_Input_Invalid` while CRUISE_ACTIVE — does not trigger state exit | CRUISE_ACTIVE | Feature continues in CRUISE_ACTIVE using inputs that have been flagged as invalid; speed control is based on corrupted or tampered data. S2: severe — speed control based on bad data. E3: occasional. C2: driver may notice speed anomaly. | S2 | E3 / C2 | **ASIL A** | REQ-010 (prevention), REQ-011 (exit on RII), SYS-005 (condition routing), SYS-004 (detection upstream), STP-005-A | Acceptable if SYS-005 RII routing is implemented and verified to ASIL A. |
| HAZ-016 | SYS-006 | Safety Event Logger fails to write event record before state transition completes | CRUISE_ACTIVE | State transition occurs without audit record; the safety-relevant event cannot be reconstructed post-incident, blocking root-cause analysis and safety evidence collection. S1: no direct injury from missing log entry; indirect risk to ongoing safety if root causes cannot be identified. E3: write failures are occasional (storage hardware, timing). C0: driver is unaffected directly by the logging failure. | S1 | E3 / C0 | **ASIL A** | REQ-013 (write-before-complete), REQ-017 (**DR-002 resolved 2026-06-02**: write failure → Cruise_Control_Fault → Cruise_Fault), SYS-006 (write-before-complete mechanism — Detection via ARCH-012), STP-006-A, STP-006-C (fault injection) | **Acceptable** — DR-002 resolved 2026-06-02: write failure escalates to Cruise_Control_Fault per REQ-017; no transition completes without a confirmed audit record. Residual risk is acceptable when REQ-013 and REQ-017 are implemented and verified to ASIL A. |
| HAZ-017 | SYS-006 | Safety Event Logger writes event record with one or more mandatory fields missing or null | ALL | Persisted record cannot support reconstruction (missing from-state, to-state, condition, timestamp, or vehicle speed); compliance evidence is incomplete; incident investigation is impeded. S1: indirect safety impact via audit gap. E3: occasional — field omission possible under error conditions. C0: driver unaffected directly. | S1 | E3 / C0 | **ASIL A** | REQ-014 (five-field minimum), SYS-006 (five-field record completeness validation), STP-006-B | Acceptable if SYS-006 field completeness check is implemented and verified to ASIL A. |
| HAZ-018 | SYS-007 | Notification Dispatcher fails to deliver override notification to the platform notification interface | CRUISE_ACTIVE | Driver interface is not informed that cruise has been overridden; driver may be momentarily uncertain about system state. S1: driver already braking and physical control is restored; notification is confirmatory. E3: notification failure is occasional. C1: driver physically braking has clear situational cues. | S1 | E3 / C1 | QM | REQ-012 (notification requirement), SYS-007 (notification dispatch), STP-007-A | QM. No safety mechanism beyond quality management required for notification delivery alone; physical cessation (HAZ-001 mitigations) is the primary safety barrier. |
| HAZ-019 | SYS-007 | Notification Dispatcher delivers duplicate notifications (more than one per `Brake_Override` event) | CRUISE_ACTIVE | Driver interface receives multiple override notifications for a single event; potential for driver distraction or confusion from unexpected repeated alerts. S1: minor confusion; driver retains physical control. E2: duplicate notification is an occasional edge case. C1: easily handled by driver. | S1 | E2 / C1 | QM | REQ-012 (exactly-once delivery), SYS-007 (exactly-once mechanism), STP-007-B | QM. No ASIL requirement for deduplication alone. |
| HAZ-020 | SYS-008 | Safety lifecycle artifacts (hazard analysis, requirements, design, implementation evidence, V&V records) are incomplete or unapproved at the time of controlled field testing | ALL | Controlled field testing proceeds with undiscovered or unmitigated hazards; residual risks not assessed; test personnel and road users are exposed to unknown risks. S2: severe — undetected hazards in a prototype on public or private roads. E3: lifecycle gaps are occasional in prototype development. C1: test programme management can halt testing once gaps are discovered. | S2 | E3 / C1 | QM–**ASIL A** | REQ-CN-002 (ISO 26262 lifecycle), REQ-NF-002 (ASIL lifecycle depth), SYS-008 (Safety Process and Compliance), STP-008-A | Acceptable if full lifecycle artifact set is reviewed and approved before field test authorisation per REQ-NF-002. OQ-001 (ASIL classification) resolution is a prerequisite. |
| HAZ-021 | SYS-008 | Cybersecurity threat analysis is absent, incomplete, or not incorporated into the design before field testing | ALL | Input integrity attack vector (e.g., replay of brake pedal status) remains unmitigated in the deployed feature; an adversary with communication access can suppress `Brake_Override` detection → uncontrolled acceleration → collision. S3: life-threatening if attack is successfully executed during road testing. E2: attack requires adversary with access to the EV prototype's communication interfaces. C3: driver cannot detect a silent integrity attack. | S3 | E2 / C3 | **ASIL B** | REQ-CN-003 (cybersecurity threat analysis), SYS-008 (cybersecurity process), SYS-004 (input integrity verification), STP-008-B | Acceptable if threat analysis is complete, all identified threats are treated (per ISO/SAE 21434), and SYS-004 integrity mechanisms address the treated threat classes. |

---

## Progressive Deepening Notes

Architecture design (`architecture-design.md`) is not yet present — architecture-level
hazard analysis has not been performed. When architecture design is generated, this
section will be populated with ARCH-level failure modes not visible at SYS level:
- Interface contract violations between ARCH modules
- Protocol timeout misconfigurations
- Race conditions on shared state (if applicable)

---

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total HAZ entries | 21 |
| SYS components analysed | 8 / 8 (100%) |
| Operational states used | 5 (CRUISE_STANDBY, CRUISE_ACTIVE, `Cruise_Suspended`, `Cruise_Cancelled`, `Cruise_Fault`) + Pre-deployment |
| Architecture-level hazards | 0 (architecture design not yet present) |

### Severity Distribution

| Severity | Count | HAZ IDs |
|----------|-------|---------|
| S3 (life-threatening) | 11 | HAZ-001, 002, 004, 006, 007, 009, 010, 013, 021 + subset of 003 |
| S2 (severe injuries) | 7 | HAZ-003 (partially), 005, 012, 014, 015, 017, 020 |
| S1 (light injuries) | 5 | HAZ-011, 016, 017, 018, 019 |
| S0 (no injuries) | 0 | — |

### ASIL Distribution (Risk Level)

| ASIL | Count | HAZ IDs |
|------|-------|---------|
| ASIL D | 5 | HAZ-001, HAZ-002, HAZ-006, HAZ-007, HAZ-009 |
| ASIL C | 2 | HAZ-004 (ASIL D), HAZ-013 |
| ASIL B | 3 | HAZ-003, HAZ-010, HAZ-021 |
| ASIL A | 8 | HAZ-005, HAZ-008, HAZ-012, HAZ-014, HAZ-015, HAZ-016, HAZ-017, HAZ-020 |
| QM | 3 | HAZ-011, HAZ-018, HAZ-019 |

*(Note: HAZ-004 is reclassified as ASIL D in the body — the ASIL C entry in the table above is an editorial artefact; see HAZ-004 row.)*

### Corrected ASIL Distribution

| ASIL | Count | HAZ IDs |
|------|-------|---------|
| **ASIL D** | **6** | HAZ-001, HAZ-002, HAZ-004, HAZ-006, HAZ-007, HAZ-009 |
| ASIL C | 1 | HAZ-013 |
| ASIL B | 3 | HAZ-003, HAZ-010, HAZ-021 |
| ASIL A | 8 | HAZ-005, HAZ-008, HAZ-012, HAZ-014, HAZ-015, HAZ-016, HAZ-017, HAZ-020 |
| QM | 3 | HAZ-011, HAZ-018, HAZ-019 |
| **Total** | **21** | |

### Open Items Flagged

| Issue | HAZ | Status | Impact |
|-------|-----|--------|--------|
| OQ-002 (timing budget) | HAZ-006 | **Resolved 2026-06-02** — T_max = 100 ms (REQ-NF-001) | HAZ-006 residual risk now acceptable; verify via ATP-NF-001-A/B |
| DR-002 (write-failure handling) | HAZ-016 | **Resolved 2026-06-02** — write failure → Cruise_Control_Fault (REQ-017) | HAZ-016 residual risk now acceptable; verify via STP-006-C |
| OQ-001 (ASIL classification) | All ASIL D entries | **Resolved 2026-06-02** — ASIL D confirmed | All ASIL tags updated to Confirmed |
| OQ-003 (target state) | HAZ-002 | **Resolved 2026-06-02** — `Cruise_Cancelled` | HAZ-002 testability unblocked; REQ-004, REQ-007, REQ-011 updated |
| DR-001 (activation preconditions) | HAZ-003 | **Resolved 2026-06-02** — five-condition minimum (REQ-018) | HAZ-003 residual risk now acceptable |
