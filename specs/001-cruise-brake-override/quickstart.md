# Quickstart: Vehicle Cruise Control — Brake Override Safety Slice

<!-- v-model:traces source: v-model/acceptance-plan.md; version: v0.7.0 -->

**Branch**: `001-cruise-brake-override` | **Phase 1 output** | **Date**: 2026-06-02

Top acceptance scenarios for the three highest-priority user stories.
Full test plan: `v-model/acceptance-plan.md` (58 ATPs, 59 SCNs).

---

## Story 1 (P1): Brake Override While Cruise Active

**REQ-002, REQ-003, REQ-004, REQ-013** — core safety behaviour

**SCN-002-A1 — Cessation on Brake_Override**

- **Given** the feature is in `Cruise_Active` and is actively issuing speed control commands
- **When** the brake pedal status input transitions to the applied state, confirming `Brake_Override`
- **Then** the propulsion command interface receives a cessation command and no further speed control commands are issued

**SCN-004-A1 — Non-Active State Reached**

- **Given** the feature is in `Cruise_Active`
- **When** `Brake_Override` is confirmed
- **Then** the feature state transitions to `Cruise_Cancelled`; it does not pass through any undefined intermediate state

**SCN-013-B1 — Record Written Before Transition Considered Complete**

- **Given** the feature is in `Cruise_Active` and a test observer monitors both the feature state output and the event storage simultaneously
- **When** `Brake_Override` triggers a state transition
- **Then** at the first processing cycle where the non-active state is observed, the corresponding event record is already present in persistent storage

---

## Story 2 (P2): Audit and Event Reconstruction

**REQ-014, REQ-015** — safety evidence obligation

**SCN-015-A1 — Full Reconstruction From Log Alone**

- **Given** a test session has been completed that included entry to `Cruise_Active`, two `Brake_Override` exits, one reactivation, and one `Cruise_Control_Fault` transition
- **When** a Safety Engineer reviews only the event log (no access to test harness data)
- **Then** the Safety Engineer can identify every state, every condition trigger, and the correct chronological order; the reconstructed sequence is validated as correct against the ground-truth harness record

---

## Story 3 (P3): Reactivation Guard After Brake Override

**REQ-005, REQ-018** — safety guard

**SCN-005-A1 — Activation Refused Before Preconditions Valid**

- **Given** the feature has just transitioned to the non-active state following `Brake_Override` and activation preconditions are not yet satisfied
- **When** a cruise activation command is received from the driver command input
- **Then** the feature remains in the non-active state and does not transition to `Cruise_Active`

**SCN-018-B1 — Single Precondition Failure Prevents Activation**

- **Given** the feature is in `Cruise_Standby`, four of the five preconditions are satisfied but `Brake_Override` is currently active
- **When** a cruise activation command is received
- **Then** the feature refuses the activation and remains in `Cruise_Standby`

---

## Running the Tests

Refer to the four-tier test plan for test harness setup:

| Tier | Plan | Key mock requirement |
|------|------|---------------------|
| Acceptance | `v-model/acceptance-plan.md` | Platform inputs simulated by harness |
| System | `v-model/system-test.md` | SYS-level mocks (STP-006-C: storage fault injection) |
| Integration | `v-model/integration-test.md` | Platform HAL stubbed per module (SIL mode) |
| Unit | `v-model/unit-test.md` | All external calls stubbed; UTS uses Arrange/Act/Assert |
