# Feature Specification: Turn Signal Cruise Disengagement

**Feature Branch**: `002-turn-signal-disengage`

**Created**: 2026-09-22

**Status**: Draft

**Input**: User description: "While cruise control is ENGAGED, activation of either turn signal (left or right) SHALL transition cruise control to DISENGAGED within one control cycle. Rationale: a turn signal indicates an imminent maneuver (lane change, turn); cruise control holding a set speed during that maneuver is undesirable and inconsistent with driver intent, similar in spirit to the existing brake-override disengagement logic (REQ-001) already implemented in this repo (see specs/001-cruise-brake-override/)."

## User Scenarios & Testing *(mandatory)*

<!--
  IMPORTANT: User stories should be PRIORITIZED as user journeys ordered by importance.
  Each user story/journey must be INDEPENDENTLY TESTABLE - meaning if you implement just ONE of them,
  you should still have a viable MVP (Minimum Viable Product) that delivers value.
-->

### User Story 1 - Driver signals a lane change while cruise is active (Priority: P1)

While driving with cruise control engaged and holding a set speed, the driver activates
the turn signal (left or right) to indicate an intended lane change or turn. The driver
expects cruise control to relinquish speed-holding control promptly so that the driver
has full manual control of the vehicle during the maneuver.

**Why this priority**: This is the entire scope of the feature and delivers the complete
safety/usability value: cruise control must not continue holding a set speed while the
driver is signaling an imminent maneuver.

**Independent Test**: Can be fully tested by engaging cruise control, activating either
turn signal, and confirming that cruise control transitions out of the engaged state
within one control cycle, with longitudinal speed control output ceased.

**Acceptance Scenarios**:

1. **Given** cruise control is `Cruise_Active` and no turn signal is active, **When** the
   driver activates the left turn signal, **Then** the feature transitions cruise control
   out of `Cruise_Active` within one control cycle.
2. **Given** cruise control is `Cruise_Active` and no turn signal is active, **When** the
   driver activates the right turn signal, **Then** the feature transitions cruise control
   out of `Cruise_Active` within one control cycle.
3. **Given** cruise control is `Cruise_Active`, **When** a turn signal activation causes
   disengagement, **Then** the feature ceases all longitudinal speed control output before
   or at the moment the transition completes.

---

### User Story 2 - Turn signal activates while cruise is not engaged (Priority: P2)

While cruise control is in any state other than `Cruise_Active` (e.g., standby, already
cancelled, or in a fault state), the driver activates a turn signal for normal driving
purposes unrelated to cruise control.

**Why this priority**: Confirms the feature does not introduce unwanted side effects when
cruise is not actively controlling speed; it is a boundary condition of the primary
behavior, not a separate capability.

**Independent Test**: Can be fully tested by placing cruise control in a non-active state,
activating a turn signal, and confirming no unexpected state transition or side effect is
introduced by this feature.

**Acceptance Scenarios**:

1. **Given** cruise control is not `Cruise_Active`, **When** the driver activates a turn
   signal, **Then** this feature takes no action on the cruise control state.

---

### User Story 3 - Turn signal deactivates after disengagement (Priority: P3)

After a turn signal causes cruise control to disengage, the driver completes the maneuver
and deactivates (or the vehicle auto-cancels) the turn signal.

**Why this priority**: Clarifies that reactivation of cruise control is governed by the
existing reactivation guard (as with brake override) and is not automatically resumed
merely because the turn signal was turned off; this avoids surprising the driver with an
unrequested resumption of speed control.

**Independent Test**: Can be fully tested by triggering a turn-signal disengagement, then
turning the signal off, and confirming cruise control remains disengaged until the driver
explicitly re-requests cruise activation and all activation preconditions are satisfied.

**Acceptance Scenarios**:

1. **Given** cruise control was disengaged due to turn signal activation, **When** the
   turn signal is subsequently deactivated, **Then** cruise control remains in its
   disengaged state and does not automatically resume speed control.

---

### Edge Cases

- What happens if both turn signals are activated simultaneously (e.g., hazard lights)?
  The feature SHALL treat this the same as a single turn signal activation and disengage
  cruise control; hazard-specific handling is out of scope.
- What happens if the turn signal is activated and deactivated within a single control
  cycle (a very brief tap)? If the activation was detected during a control cycle in which
  cruise was `Cruise_Active`, disengagement SHALL still occur.
- What happens if a turn signal is already active at the moment the driver engages cruise
  control? Activation preconditions for engaging cruise control are governed by the
  existing brake-override feature's activation guard and are out of scope for this
  feature; this feature only governs disengagement of an already-engaged cruise control.
- What happens if the turn signal input itself is invalid or unavailable (sensor fault)?
  This is out of scope for this feature and is handled by existing input-validity
  handling (e.g., `Required_Input_Invalid`) in the Brake Override Safety Slice.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: The feature MUST monitor the left and right turn signal input(s) provided by
  the existing EV prototype platform.
- **FR-002**: While cruise control is in `Cruise_Active`, when either the left or the
  right turn signal is activated, the feature MUST transition cruise control out of
  `Cruise_Active` within one control cycle.
- **FR-003**: When cruise control transitions out of `Cruise_Active` due to a turn signal
  activation, the feature MUST cease all longitudinal speed control output to the
  propulsion command interface as part of that same transition.
- **FR-004**: While cruise control is not in `Cruise_Active`, turn signal activation MUST
  NOT cause this feature to take any action on the cruise control state.
- **FR-005**: Deactivation of the turn signal MUST NOT, by itself, cause cruise control to
  resume `Cruise_Active`; resumption MUST follow the existing cruise activation request and
  precondition process.
- **FR-006**: When a turn-signal-triggered disengagement occurs, the feature MUST issue a
  notification event and persist an event record, consistent with the notification and
  event-logging behavior already established for other disengagement causes in this
  system (e.g., brake override), so that the transition is auditable.

### Key Entities

- **Turn Signal Activation**: A driver-initiated input, provided by the existing EV
  prototype platform, indicating the left or right turn signal is active. Relevant
  attribute: which side (left/right) is active; for this feature's purposes, either side
  is treated identically.
- **Cruise Control State**: The existing cruise control state machine defined by the
  Brake Override Safety Slice (`Cruise_Standby`, `Cruise_Active`, `Cruise_Suspended`,
  `Cruise_Cancelled`, `Cruise_Fault`); this feature adds turn signal activation as an
  additional trigger causing transition out of `Cruise_Active`.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: 100% of turn signal activations that occur while cruise control is engaged
  result in cruise control disengaging within one control cycle, verified across
  repeated test trials.
- **SC-002**: 0% of turn signal activations that occur while cruise control is not engaged
  produce any observable change to cruise control state.
- **SC-003**: Every turn-signal-triggered disengagement produces exactly one corresponding
  auditable event record, allowing 100% reconstruction of the cause of disengagement from
  the event log alone.
- **SC-004**: Drivers report that cruise control reliably yields control during signaled
  lane changes or turns, with no perceptible delay between signaling and loss of
  cruise-held speed.

## Assumptions

- This feature is layered on top of, and depends on, the existing Vehicle Cruise Control
  Brake Override Safety Slice (`specs/001-cruise-brake-override/`); it reuses that
  feature's state machine, propulsion command interface, notification interface, and
  event storage rather than introducing new ones.
- "One control cycle" has the same meaning and platform-defined duration as used in the
  Brake Override Safety Slice's timing requirements; no new timing bound is introduced by
  this feature beyond disengagement occurring within a single cycle.
- The target post-disengagement state for a turn-signal-triggered transition is the same
  non-active state used for other non-fault disengagement causes in the existing state
  machine (i.e., `Cruise_Cancelled`), consistent with how brake override is handled.
- The turn signal input is provided as a reliable platform input; sensor-level fault
  handling for the turn signal input itself is covered by the platform's existing
  input-validity mechanisms and is not re-specified here.
- Hazard lights (both signals simultaneously) are treated as an ordinary turn signal
  activation for the purposes of this feature; no distinct behavior is defined for that
  case beyond what Edge Cases states.
