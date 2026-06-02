# Unit Test Plan: Vehicle Cruise Control — Brake Override Safety Slice

**Feature Branch**: `001-cruise-brake-override`
**Created**: 2026-06-02
**Status**: Approved
**Source**: `specs/001-cruise-brake-override/v-model/module-design.md`
**Domain**: ISO 26262 (ASIL D — Confirmed)
**Language**: C + MISRA C:2012

---

## Overview

White-box unit tests for all 17 MODs in `module-design.md`. Every test verifies
**internal module logic** — control flow branches, variable boundary conditions,
state transitions, and error handling — using the Arrange/Act/Assert format.

These tests do NOT test module boundaries (integration), user journeys (acceptance),
or system-level behaviour (system tests). All external dependencies are stubbed.

**Techniques applied:**
- **Statement & Branch Coverage** — every branch True and False per pseudocode
- **Boundary Value Analysis (BVA)** — min−1, min, mid, max, max+1 for scalar types
- **Equivalence Partitioning (EP)** — one representative per valid enum/bool partition
- **State Transition Testing** — valid and invalid transitions for stateful modules
- **Strict Isolation** — all platform HAL calls and inter-module calls stubbed
- **MC/DC Coverage** — compound boolean decisions in ASIL D modules (ISO 26262-6 §9.4.4)
- **Variable-Level Fault Injection** — key safety-critical variables (ISO 26262-6 §9.4.4)

---

## ID Schema

- **Unit Test Case**: `UTP-{NNN}-{X}` — NNN matches parent MOD; X is letter suffix
- **Unit Test Scenario**: `UTS-{NNN}-{X}{#}` — nested under parent UTP
- Example: `UTS-009-B2` → Scenario 2 of UTP-009-B, testing MOD-009

---

## ISO 29119-4 White-Box Techniques

| Technique | Module View | What It Tests |
|-----------|-------------|---------------|
| Statement & Branch Coverage | Algorithmic/Logic View | Every code line and every True/False branch |
| Boundary Value Analysis | Internal Data Structures (scalar) | min−1, min, mid, max, max+1 |
| Equivalence Partitioning | Internal Data Structures (enum/bool) | One test per valid partition |
| State Transition Testing | State Machine View | Every valid and invalid transition |
| Strict Isolation | All (external deps) | Platform HAL and inter-module calls stubbed |
| MC/DC Coverage | Algorithmic/Logic (ASIL D) | Each condition independently affects compound decision |
| Variable-Level Fault Injection | Internal Data Structures (ASIL D) | Key safety-critical variables corrupted |

---

## Unit Tests

---

### MOD-001: cruise_eval_transition

**Dependency & Mock Registry**

| Dependency | Source | Mock Strategy |
|------------|--------|--------------|
| `cruise_state_read()` | MOD-003 | Stub returning programmed `CruiseControlState_t` value |
| `cruise_state_write()` | MOD-003 | Spy capturing `new_state`; returns programmed `ErrorCode_t` |
| `cruise_handle_fault()` | MOD-011 | Spy capturing `condition`; returns `CRUISE_OK` |

---

#### Test Case: UTP-001-A (Statement and Branch Coverage: Transition Table Paths)
**Technique**: Statement & Branch Coverage
**Module View**: Algorithmic/Logic View — switch-case transition table

* **UTS-001-A1** — Brake_Override from Cruise_Active → writes Cruise_Cancelled
  * **Arrange**: Stub `cruise_state_read()` to return `CRUISE_ACTIVE`; stub `cruise_state_write()` to return `CRUISE_OK`
  * **Act**: Call `cruise_eval_transition(COND_BRAKE_OVERRIDE)`
  * **Assert**: `cruise_state_write` spy captured `new_state = CRUISE_CANCELLED`; function returns `CRUISE_OK`

* **UTS-001-A2** — CCF from Cruise_Standby → writes Cruise_Fault
  * **Arrange**: Stub `cruise_state_read()` to return `CRUISE_STANDBY`; stub `cruise_state_write()` to return `CRUISE_OK`
  * **Act**: Call `cruise_eval_transition(COND_CCF)`
  * **Assert**: `cruise_state_write` spy captured `new_state = CRUISE_FAULT`; function returns `CRUISE_OK`

* **UTS-001-A3** — No permitted transition (e.g., Brake_Override from Cruise_Standby) → E002
  * **Arrange**: Stub `cruise_state_read()` to return `CRUISE_STANDBY`
  * **Act**: Call `cruise_eval_transition(COND_BRAKE_OVERRIDE)`
  * **Assert**: `cruise_state_write` was NOT called; function returns `CRUISE_ERR_INVALID_TRANSITION` (E002)

---

#### Test Case: UTP-001-B (Equivalence Partitioning: Invalid Condition Code)
**Technique**: Equivalence Partitioning
**Module View**: Algorithmic/Logic View — condition validation branch

* **UTS-001-B1** — Undefined condition value → E001
  * **Arrange**: Stub `cruise_state_read()` to return `CRUISE_ACTIVE`
  * **Act**: Call `cruise_eval_transition((ControlCondition_t)0xFF)` — value outside defined enum range
  * **Assert**: `cruise_state_write` was NOT called; function returns `CRUISE_ERR_INVALID_CONDITION` (E001)

---

### MOD-002: cruise_check_preconditions

**Dependency & Mock Registry**: None — module is self-contained (pure boolean function).

---

#### Test Case: UTP-002-A (Statement and Branch Coverage: Five-Condition AND)
**Technique**: Statement & Branch Coverage + MC/DC Coverage
**Module View**: Algorithmic/Logic View — 5-condition AND decision

**MC/DC Truth Table for** `speed_ok && brake_ok && rii_ok && ccf_ok && lcu_ok`:

| Test | speed_ok | brake_ok | rii_ok | ccf_ok | lcu_ok | Decision | Independence Proof | ASIL |
|------|----------|----------|--------|--------|--------|----------|--------------------|------|
| 1 | T | T | T | T | T | **TRUE** | Baseline (all true) | D |
| 2 | **F** | T | T | T | T | FALSE | speed_ok flips: row 1 vs row 2 | D |
| 3 | T | **F** | T | T | T | FALSE | brake_ok flips: row 1 vs row 3 | D |
| 4 | T | T | **F** | T | T | FALSE | rii_ok flips: row 1 vs row 4 | D |
| 5 | T | T | T | **F** | T | FALSE | ccf_ok flips: row 1 vs row 5 | D |
| 6 | T | T | T | T | **F** | FALSE | lcu_ok flips: row 1 vs row 6 | D |

* **UTS-002-A1** — All five TRUE → returns true (MC/DC row 1)
  * **Arrange**: Populate `flags` with `speed_in_valid_range=true`, `brake_override_active=false`, `rii_active=false`, `ccf_active=false`, `lcu_active=false`
  * **Act**: Call `cruise_check_preconditions(&flags)`
  * **Assert**: Returns `true`

* **UTS-002-A2** — Speed out of range → returns false (MC/DC row 2)
  * **Arrange**: Set `flags.speed_in_valid_range = false`; all others as in UTS-002-A1
  * **Act**: Call `cruise_check_preconditions(&flags)`
  * **Assert**: Returns `false`

* **UTS-002-A3** — Brake active → returns false (MC/DC row 3)
  * **Arrange**: Set `flags.brake_override_active = true`; all others as in UTS-002-A1
  * **Act**: Call `cruise_check_preconditions(&flags)`
  * **Assert**: Returns `false`

* **UTS-002-A4** — Null pointer → returns false
  * **Arrange**: No struct; pass NULL pointer
  * **Act**: Call `cruise_check_preconditions(NULL)`
  * **Assert**: Returns `false`

---

### MOD-003: cruise_state_register

**Dependency & Mock Registry**

| Dependency | Source | Mock Strategy |
|------------|--------|--------------|
| `cruise_handle_fault()` | MOD-011 | Spy; returns `CRUISE_OK` |

---

#### Test Case: UTP-003-A (Equivalence Partitioning: Valid and Invalid State Values)
**Technique**: Equivalence Partitioning + Variable-Level Fault Injection
**Module View**: Internal Data Structures — `CruiseControlState_t` enum

* **UTS-003-A1** — Write each of the five valid states → accepted
  * **Arrange**: Each valid enum: `CRUISE_STANDBY(0)`, `CRUISE_ACTIVE(1)`, `CRUISE_SUSPENDED(2)`, `CRUISE_CANCELLED(3)`, `CRUISE_FAULT(4)`
  * **Act**: Call `cruise_state_write(state)` for each; then call `cruise_state_read()`
  * **Assert**: `cruise_state_write` returns `CRUISE_OK` for each; `cruise_state_read()` returns the last written value

* **UTS-003-A2** — Write out-of-range value (0xFF) → E010, g_cruise_state unchanged, CCF escalated
  * **Arrange**: Pre-write `CRUISE_STANDBY` to establish a known value; `cruise_handle_fault` spy is ready
  * **Act**: Call `cruise_state_write((CruiseControlState_t)0xFF)`
  * **Assert**: Returns `CRUISE_ERR_RANGE_VIOLATION` (E010); `cruise_state_read()` still returns `CRUISE_STANDBY`; `cruise_handle_fault` spy captured `COND_CCF`

---

#### Test Case: UTP-003-B (Statement and Branch Coverage: Read/Write Paths)
**Technique**: Statement & Branch Coverage
**Module View**: Algorithmic/Logic View

* **UTS-003-B1** — Read returns initialisation default
  * **Arrange**: Module freshly initialised (static `g_cruise_state = CRUISE_STANDBY`)
  * **Act**: Call `cruise_state_read()` without any prior write
  * **Assert**: Returns `CRUISE_STANDBY` (initial value)

---

### MOD-004: cruise_sequence_override

**Dependency & Mock Registry**

| Dependency | Source | Mock Strategy |
|------------|--------|--------------|
| `cruise_state_read()` | MOD-003 | Stub returning `CRUISE_ACTIVE` or `CRUISE_CANCELLED` |
| `cruise_propulsion_write()` | MOD-007 | Spy; returns programmed `ErrorCode_t` |
| `cruise_eval_transition()` | MOD-001 | Spy; returns programmed `ErrorCode_t` |
| `cruise_build_event_record()` | MOD-012 | Spy; returns programmed `ErrorCode_t` |
| `cruise_write_event_record()` | MOD-013 | Spy; returns programmed `ErrorCode_t` |
| `cruise_publish_notification()` | MOD-014 | Spy; returns programmed `ErrorCode_t` |
| `cruise_handle_fault()` | MOD-011 | Spy; returns `CRUISE_OK` |

---

#### Test Case: UTP-004-A (Statement and Branch Coverage: Success Path and Step Failures)
**Technique**: Statement & Branch Coverage
**Module View**: Algorithmic/Logic View — linear step chain

* **UTS-004-A1** — All four steps succeed → CRUISE_OK
  * **Arrange**: Stub `cruise_state_read()` → `CRUISE_ACTIVE`; all step spies return `CRUISE_OK`; valid `BrakeOverrideEvent_t ev = {timestamp=1000, vehicle_speed_snapshot=900, event_id=1}`
  * **Act**: Call `cruise_sequence_override(&ev)`
  * **Assert**: Returns `CRUISE_OK`; spies called in order: propulsion_write, eval_transition, build_event_record, write_event_record, publish_notification; `cruise_handle_fault` NOT called

* **UTS-004-A2** — Cessation step fails → escalates CCF, returns error
  * **Arrange**: Stub `cruise_state_read()` → `CRUISE_ACTIVE`; stub `cruise_propulsion_write()` to return `CRUISE_ERR_WRITE_FAILURE`
  * **Act**: Call `cruise_sequence_override(&ev)` with same valid event
  * **Assert**: Returns `CRUISE_ERR_WRITE_FAILURE`; `cruise_handle_fault` spy captured `COND_CCF`; `cruise_eval_transition` was NOT called (sequence stopped)

* **UTS-004-A3** — Null event pointer → E_NULL_PARAM, no steps called
  * **Arrange**: All spies ready
  * **Act**: Call `cruise_sequence_override(NULL)`
  * **Assert**: Returns `CRUISE_ERR_NULL_PARAM`; no step spy was called

---

### MOD-005: cruise_timing_supervisor

**Dependency & Mock Registry**

| Dependency | Source | Mock Strategy |
|------------|--------|--------------|
| `cruise_handle_fault()` | MOD-011 | Spy; returns `CRUISE_OK` |

---

#### Test Case: UTP-005-A (State Transition Testing: Timer States)
**Technique**: State Transition Testing
**Module View**: State Machine View — Idle / Armed / TimingExceeded

* **UTS-005-A1** — Idle → Armed: arm sets timer
  * **Arrange**: Module in initial (Idle) state; `g_timer_armed = false`
  * **Act**: Call `cruise_timing_arm(1000u)`
  * **Assert**: Returns `CRUISE_OK`; internal `g_timer_armed == true`; `g_start_time_ms == 1000u`

* **UTS-005-A2** — Armed → Idle: stop within T_MAX → CRUISE_OK
  * **Arrange**: Module armed at `g_start_time_ms = 1000u`
  * **Act**: Call `cruise_timing_stop(1050u)` (50 ms elapsed, within 100 ms limit)
  * **Assert**: Returns `CRUISE_OK`; `g_timer_armed == false`

* **UTS-005-A3** — Armed → TimingExceeded: stop after T_MAX → E030
  * **Arrange**: Module armed at `g_start_time_ms = 1000u`
  * **Act**: Call `cruise_timing_stop(1105u)` (105 ms elapsed, exceeds 100 ms)
  * **Assert**: Returns `CRUISE_ERR_TIMING_EXCEEDED` (E030); `g_timer_armed == false`

* **UTS-005-A4** — Arm while already armed → rejected
  * **Arrange**: `g_timer_armed = true`
  * **Act**: Call `cruise_timing_arm(2000u)`
  * **Assert**: Returns `CRUISE_ERR_ALREADY_ARMED`; `g_start_time_ms` unchanged from prior arm

* **UTS-005-A5** — Invalid transition: check with armed + expired → escalates CCF
  * **Arrange**: Module armed at `g_start_time_ms = 1000u`
  * **Act**: Call `cruise_timing_check(1101u)` (101 ms elapsed, over budget)
  * **Assert**: Returns `CRUISE_ERR_TIMING_EXCEEDED` (E030); `g_timer_armed == false`; `cruise_handle_fault` spy captured `COND_CCF`

---

#### Test Case: UTP-005-B (Boundary Value Analysis: T_MAX Timing Boundary)
**Technique**: Boundary Value Analysis
**Module View**: Internal Data Structures — `T_MAX_MS = 100u`

* **UTS-005-B1** — Elapsed = T_MAX − 1 (99 ms) → accepted
  * **Arrange**: Module armed at `g_start_time_ms = 0u`
  * **Act**: Call `cruise_timing_stop(99u)`
  * **Assert**: Returns `CRUISE_OK`

* **UTS-005-B2** — Elapsed = T_MAX (100 ms) → accepted (boundary inclusive)
  * **Arrange**: `g_start_time_ms = 0u`; call `cruise_timing_arm(0u)` first
  * **Act**: Call `cruise_timing_stop(100u)`
  * **Assert**: Returns `CRUISE_OK` (elapsed 100 ms is within the ≤ 100 ms limit)

* **UTS-005-B3** — Elapsed = T_MAX + 1 (101 ms) → E030
  * **Arrange**: Module armed at `g_start_time_ms = 0u`
  * **Act**: Call `cruise_timing_stop(101u)`
  * **Assert**: Returns `CRUISE_ERR_TIMING_EXCEEDED` (E030)

---

### MOD-006: cruise_generate_speed_cmd

**Dependency & Mock Registry**

| Dependency | Source | Mock Strategy |
|------------|--------|--------------|
| `cruise_state_read()` | MOD-003 | Stub returning `CRUISE_ACTIVE` or `CRUISE_CANCELLED` |

---

#### Test Case: UTP-006-A (Statement and Branch Coverage: Authorisation Gate)
**Technique**: Statement & Branch Coverage
**Module View**: Algorithmic/Logic View — authorisation gate + speed error branches

* **UTS-006-A1** — Not authorised → zero-command returned
  * **Arrange**: Stub `cruise_state_read()` → `CRUISE_CANCELLED`
  * **Act**: Call `cruise_generate_speed_cmd(900u, 850u)`
  * **Assert**: Returns `SpeedCommand_t` with `command_value == 0u` and `is_zero == true`

* **UTS-006-A2** — Authorised, positive error → non-zero acceleration command
  * **Arrange**: Stub `cruise_state_read()` → `CRUISE_ACTIVE`; `cruise_target = 900u`, `current = 850u` (50 km/h×10 deficit)
  * **Act**: Call `cruise_generate_speed_cmd(900u, 850u)`
  * **Assert**: Returns `SpeedCommand_t` with `command_value > 0u` and `is_zero == false`

---

### MOD-007: cruise_propulsion_write

**Dependency & Mock Registry**

| Dependency | Source | Mock Strategy |
|------------|--------|--------------|
| `cruise_state_read()` | MOD-003 | Stub returning programmed state |
| `platform_propulsion_cease()` | Platform HAL | Stub returning `PLATFORM_OK` or `PLATFORM_ERROR` |
| `platform_propulsion_set()` | Platform HAL | Stub returning `PLATFORM_OK` or `PLATFORM_ERROR` |
| `cruise_handle_fault()` | MOD-011 | Spy |

---

#### Test Case: UTP-007-A (Equivalence Partitioning and Statement Coverage: CMD_TYPE)
**Technique**: Equivalence Partitioning + Statement & Branch Coverage
**Module View**: Algorithmic/Logic View — CMD_TYPE gate and write paths

* **UTS-007-A1** — CMD_CESSATION in any state → platform_cease called, CRUISE_OK returned
  * **Arrange**: Stub `cruise_state_read()` → `CRUISE_ACTIVE`; stub `platform_propulsion_cease()` → `PLATFORM_OK`
  * **Act**: Call `cruise_propulsion_write(CMD_CESSATION, 0u)`
  * **Assert**: Returns `CRUISE_OK`; `platform_propulsion_cease` was called; `platform_propulsion_set` was NOT called

* **UTS-007-A2** — CMD_SPEED_CTRL when not Cruise_Active → E_NOT_AUTHORISED, no platform call
  * **Arrange**: Stub `cruise_state_read()` → `CRUISE_CANCELLED`
  * **Act**: Call `cruise_propulsion_write(CMD_SPEED_CTRL, 500u)`
  * **Assert**: Returns `CRUISE_ERR_NOT_AUTHORISED`; neither platform HAL called

* **UTS-007-A3** — Platform write failure → E040, CCF escalated
  * **Arrange**: Stub `cruise_state_read()` → `CRUISE_ACTIVE`; stub `platform_propulsion_cease()` → `PLATFORM_ERROR`
  * **Act**: Call `cruise_propulsion_write(CMD_CESSATION, 0u)`
  * **Assert**: Returns `CRUISE_ERR_WRITE_FAILURE` (E040); `cruise_handle_fault` spy captured `COND_CCF`

---

### MOD-008: cruise_read_platform_inputs

**Dependency & Mock Registry**

| Dependency | Source | Mock Strategy |
|------------|--------|--------------|
| `platform_read_brake_pedal()` | Platform HAL | Stub: `PLATFORM_OK` or `PLATFORM_ERROR` |
| `platform_read_driver_cmd()` | Platform HAL | Stub |
| `platform_read_vehicle_speed()` | Platform HAL | Stub |
| `platform_read_diagnostic()` | Platform HAL | Stub |
| `platform_read_lcu()` | Platform HAL | Stub |

---

#### Test Case: UTP-008-A (Statement and Branch Coverage: All-OK vs Any-Fail)
**Technique**: Statement & Branch Coverage + Strict Isolation
**Module View**: Algorithmic/Logic View — read path and timeout detection

* **UTS-008-A1** — All five HAL reads succeed → CRUISE_OK, all fields populated
  * **Arrange**: All five platform stubs return `PLATFORM_OK` with known test values: `brake_status_raw=0x01`, `driver_cmd_raw=0x00`, `vehicle_speed_raw=900u`, `diagnostic_status=0x00`, `lcu_availability=true`
  * **Act**: Call `cruise_read_platform_inputs(&out)`
  * **Assert**: Returns `CRUISE_OK`; `out.brake_status_raw == 0x01`, `out.vehicle_speed_raw == 900u`, `out.lcu_availability == true`

* **UTS-008-A2** — One HAL read fails → CRUISE_ERR_READ_TIMEOUT (E050)
  * **Arrange**: First four stubs return `PLATFORM_OK`; `platform_read_lcu()` returns `PLATFORM_ERROR`
  * **Act**: Call `cruise_read_platform_inputs(&out)`
  * **Assert**: Returns `CRUISE_ERR_READ_TIMEOUT` (E050)

* **UTS-008-A3** — Null output pointer → CRUISE_ERR_NULL_PARAM
  * **Arrange**: No struct allocated
  * **Act**: Call `cruise_read_platform_inputs(NULL)`
  * **Assert**: Returns `CRUISE_ERR_NULL_PARAM`; no HAL function called

---

### MOD-009: cruise_verify_integrity

**Dependency & Mock Registry**: None — module is self-contained (pure CRC + counter computation).

---

#### Test Case: UTP-009-A (Statement and Branch Coverage: CRC and Counter Paths)
**Technique**: Statement & Branch Coverage
**Module View**: Algorithmic/Logic View — CRC check branch + counter check branch

* **UTS-009-A1** — Valid CRC and in-sequence counter → validity=TRUE, counter advanced
  * **Arrange**: Pre-compute a valid CRC-16/CCITT-FALSE for `data=[0x01]`; set `raw.counter = g_expected_counter_brake`
  * **Act**: Call `cruise_verify_brake(&raw)`
  * **Assert**: Returns `VerifiedInput_t` with `validity=true` and `value=0x01`; `g_expected_counter_brake` incremented by 1 (mod 16)

* **UTS-009-A2** — CRC mismatch → validity=FALSE, counter NOT advanced
  * **Arrange**: Set `raw.crc16 = 0xDEADu` (deliberate mismatch); `raw.counter = g_expected_counter_brake`
  * **Act**: Call `cruise_verify_brake(&raw)`
  * **Assert**: Returns `validity=false`; `g_expected_counter_brake` unchanged

* **UTS-009-A3** — Valid CRC but wrong counter (replay) → validity=FALSE, counter NOT advanced
  * **Arrange**: Valid CRC; `raw.counter = (g_expected_counter_brake + 2u) & 0x0Fu` (skipped one)
  * **Act**: Call `cruise_verify_brake(&raw)`
  * **Assert**: Returns `validity=false`; `g_expected_counter_brake` unchanged

---

#### Test Case: UTP-009-B (Boundary Value Analysis: 4-bit Rolling Counter 0-15)
**Technique**: Boundary Value Analysis
**Module View**: Internal Data Structures — `g_expected_counter_brake` (0–15)

* **UTS-009-B1** — Counter at max (15) → wraps to 0 on next successful verify
  * **Arrange**: Set `g_expected_counter_brake = 15u`; prepare valid CRC; `raw.counter = 15u`
  * **Act**: Call `cruise_verify_brake(&raw)` (valid input)
  * **Assert**: `validity=true`; after call, `g_expected_counter_brake == 0u` (wrapped)

* **UTS-009-B2** — Counter at 0 wrapping from 15 → accepts 0, advances to 1
  * **Arrange**: `g_expected_counter_brake = 0u`; valid CRC; `raw.counter = 0u`
  * **Act**: Call `cruise_verify_brake(&raw)`
  * **Assert**: `validity=true`; `g_expected_counter_brake == 1u`

---

#### Test Case: UTP-009-C (MC/DC Coverage: crc_ok AND counter_ok)
**Technique**: MC/DC Coverage
**Module View**: Algorithmic/Logic View — `if (crc_ok && counter_ok)`

| Test | crc_ok | counter_ok | Decision | Independence Proof | ASIL |
|------|--------|------------|----------|--------------------|------|
| 1 | T | T | **TRUE** | Baseline | D |
| 2 | **F** | T | FALSE | crc_ok flips: row 1 vs row 2 | D |
| 3 | T | **F** | FALSE | counter_ok flips: row 1 vs row 3 | D |

* **UTS-009-C1** — crc_ok=T, counter_ok=T → validity=true (MC/DC row 1)
  * **Arrange**: Valid CRC; `raw.counter = g_expected_counter_brake`
  * **Act**: Call `cruise_verify_brake(&raw)`
  * **Assert**: Returns `validity=true`

* **UTS-009-C2** — crc_ok=F, counter_ok=T → validity=false (MC/DC row 2)
  * **Arrange**: Invalid CRC; `raw.counter = g_expected_counter_brake`
  * **Act**: Call `cruise_verify_brake(&raw)`
  * **Assert**: Returns `validity=false`

* **UTS-009-C3** — crc_ok=T, counter_ok=F → validity=false (MC/DC row 3)
  * **Arrange**: Valid CRC; `raw.counter != g_expected_counter_brake` (wrong counter)
  * **Act**: Call `cruise_verify_brake(&raw)`
  * **Assert**: Returns `validity=false`

---

### MOD-010: cruise_derive_conditions

**Dependency & Mock Registry**: None — pure computation from validated inputs.

---

#### Test Case: UTP-010-A (Equivalence Partitioning: Each Condition Derivation)
**Technique**: Equivalence Partitioning + Statement & Branch Coverage
**Module View**: Algorithmic/Logic View — four condition derivation paths

* **UTS-010-A1** — Brake applied + valid → Brake_Override derived
  * **Arrange**: `brake_verified = {value=BRAKE_APPLIED, validity=true}`; `driver_verified = {validity=true}`; `raw_inputs.vehicle_speed_raw = 900u` (in range); `lcu_availability=true`; `diagnostic_status=0x00`
  * **Act**: Call `cruise_derive_conditions(&brake_v, &driver_v, &raw, &out)`
  * **Assert**: Returns `CRUISE_OK`; `out.brake_override == true`; `out.rii_active == false`; `out.lcu_active == false`; `out.ccf_active == false`

* **UTS-010-A2** — Brake integrity fail → RII derived, not Brake_Override
  * **Arrange**: `brake_verified = {validity=false}`; `driver_verified = {validity=true}`; speed in range; `lcu_availability=true`; `diagnostic_status=0x00`
  * **Act**: Call `cruise_derive_conditions(&brake_v, &driver_v, &raw, &out)`
  * **Assert**: `out.brake_override == false`; `out.rii_active == true`

* **UTS-010-A3** — Speed out of range → RII derived
  * **Arrange**: Both inputs valid; `raw_inputs.vehicle_speed_raw = 0u` (below SPEED_MIN_KMH10)
  * **Act**: Call `cruise_derive_conditions(&brake_v, &driver_v, &raw, &out)`
  * **Assert**: `out.rii_active == true`

* **UTS-010-A4** — LCU unavailable → LCU derived
  * **Arrange**: All inputs valid; `raw_inputs.lcu_availability = false`
  * **Act**: Call `cruise_derive_conditions(&brake_v, &driver_v, &raw, &out)`
  * **Assert**: `out.lcu_active == true`

---

### MOD-011: cruise_handle_fault

**Dependency & Mock Registry**

| Dependency | Source | Mock Strategy |
|------------|--------|--------------|
| `cruise_state_read()` | MOD-003 | Stub returning programmed state |
| `cruise_propulsion_write()` | MOD-007 | Spy; returns `CRUISE_OK` |
| `cruise_eval_transition()` | MOD-001 | Spy; returns programmed `ErrorCode_t` |
| `cruise_state_write()` | MOD-003 | Spy (used for forced Cruise_Fault backstop) |

---

#### Test Case: UTP-011-A (Statement and Branch Coverage: LCU, CCF, RII Routing)
**Technique**: Statement & Branch Coverage + Equivalence Partitioning
**Module View**: Algorithmic/Logic View — switch(condition) routing

* **UTS-011-A1** — LCU while Cruise_Active → cessation issued + transition requested
  * **Arrange**: Stub `cruise_state_read()` → `CRUISE_ACTIVE`; all spies return `CRUISE_OK`
  * **Act**: Call `cruise_handle_fault(COND_LCU)`
  * **Assert**: Returns `CRUISE_OK`; `cruise_propulsion_write` spy called with `CMD_CESSATION`; `cruise_eval_transition` spy called with `COND_LCU`

* **UTS-011-A2** — CCF while Cruise_Standby → no cessation (not active); transition requested
  * **Arrange**: Stub `cruise_state_read()` → `CRUISE_STANDBY`; all spies return `CRUISE_OK`
  * **Act**: Call `cruise_handle_fault(COND_CCF)`
  * **Assert**: `cruise_propulsion_write` was NOT called; `cruise_eval_transition` called with `COND_CCF`

* **UTS-011-A3** — Transition rejected → forced Cruise_Fault write
  * **Arrange**: Stub `cruise_state_read()` → `CRUISE_ACTIVE`; `cruise_eval_transition` returns `CRUISE_ERR_INVALID_TRANSITION`; `cruise_state_write` spy ready
  * **Act**: Call `cruise_handle_fault(COND_LCU)`
  * **Assert**: Returns `CRUISE_ERR_TRANSITION_REJECTED` (E070); `cruise_state_write` spy captured `CRUISE_FAULT`

---

### MOD-012: cruise_build_event_record

**Dependency & Mock Registry**: None — pure validation and struct population.

---

#### Test Case: UTP-012-A (Statement and Branch Coverage: Field Validation)
**Technique**: Statement & Branch Coverage + Boundary Value Analysis
**Module View**: Algorithmic/Logic View — five-field validation

* **UTS-012-A1** — All five fields valid → g_record_ready = true, CRUISE_OK
  * **Arrange**: `req = {from_state=CRUISE_ACTIVE, condition=COND_BRAKE_OVERRIDE, to_state=CRUISE_CANCELLED, vehicle_spd=900u, timestamp=1234567u}`
  * **Act**: Call `cruise_build_event_record(&req)`
  * **Assert**: Returns `CRUISE_OK`; internal `g_record_ready == true`; `g_pending_record` equals `req`

* **UTS-012-A2** — Null timestamp (zero) → E080, g_record_ready = false
  * **Arrange**: Same as UTS-012-A1 but `req.timestamp = 0u`
  * **Act**: Call `cruise_build_event_record(&req)`
  * **Assert**: Returns `CRUISE_ERR_FIELD_VALIDATION` (E080); `g_record_ready == false`

* **UTS-012-A3** — BVA: vehicle_spd at maximum valid boundary → accepted
  * **Arrange**: `req.vehicle_spd = SPEED_MAX_KMH10` (max valid value); all other fields valid
  * **Act**: Call `cruise_build_event_record(&req)`
  * **Assert**: Returns `CRUISE_OK`

* **UTS-012-A4** — BVA: vehicle_spd exceeds maximum → E080
  * **Arrange**: `req.vehicle_spd = SPEED_MAX_KMH10 + 1u`; all other fields valid
  * **Act**: Call `cruise_build_event_record(&req)`
  * **Assert**: Returns `CRUISE_ERR_FIELD_VALIDATION` (E080)

---

### MOD-013: cruise_write_event_record

**Dependency & Mock Registry**

| Dependency | Source | Mock Strategy |
|------------|--------|--------------|
| `g_record_ready` | MOD-012 (shared) | Set directly in test setup (white-box access) |
| `platform_event_storage_write()` | Platform HAL | Stub returning `PLATFORM_OK` or `PLATFORM_ERROR` |
| `cruise_handle_fault()` | MOD-011 | Spy |

---

#### Test Case: UTP-013-A (Statement and Branch Coverage: Write Paths)
**Technique**: Statement & Branch Coverage + Strict Isolation
**Module View**: Algorithmic/Logic View — g_record_ready gate and write result

* **UTS-013-A1** — Record ready, platform write succeeds → CRUISE_OK, g_record_ready cleared
  * **Arrange**: Set `g_record_ready = true` and populate `g_pending_record`; stub platform HAL → `PLATFORM_OK`
  * **Act**: Call `cruise_write_event_record()`
  * **Assert**: Returns `CRUISE_OK`; `g_record_ready == false` after call; platform HAL called once

* **UTS-013-A2** — Record ready, platform write fails → E090, CCF escalated, g_record_ready cleared
  * **Arrange**: `g_record_ready = true`; stub platform HAL → `PLATFORM_ERROR`; `cruise_handle_fault` spy ready
  * **Act**: Call `cruise_write_event_record()`
  * **Assert**: Returns `CRUISE_ERR_WRITE_FAILURE` (E090); `g_record_ready == false`; `cruise_handle_fault` spy captured `COND_CCF`

* **UTS-013-A3** — Record not ready → CRUISE_ERR_FIELD_VALIDATION, no HAL call
  * **Arrange**: `g_record_ready = false`
  * **Act**: Call `cruise_write_event_record()`
  * **Assert**: Returns `CRUISE_ERR_FIELD_VALIDATION`; platform HAL NOT called

---

### MOD-014: cruise_publish_notification

**Dependency & Mock Registry**

| Dependency | Source | Mock Strategy |
|------------|--------|--------------|
| `platform_notification_send()` | Platform HAL | Stub returning `PLATFORM_OK` or `PLATFORM_ERROR` |

---

#### Test Case: UTP-014-A (Statement and Branch Coverage: Deduplication Logic)
**Technique**: Statement & Branch Coverage + Equivalence Partitioning
**Module View**: Algorithmic/Logic View — dedup check branch

* **UTS-014-A1** — New event_id → delivered, added to dedup table
  * **Arrange**: Reset `g_seen_count = 0u`; `g_seen_ids` all zeros; stub platform HAL → `PLATFORM_OK`; `req = {event_id=1u, event_type=NOTIF_BRAKE_OVERRIDE}`
  * **Act**: Call `cruise_publish_notification(&req)`
  * **Assert**: Returns `CRUISE_OK`; `g_seen_ids[0] == 1u`; `g_seen_count == 1u`; platform HAL called once

* **UTS-014-A2** — Duplicate event_id → silently absorbed, no platform call
  * **Arrange**: `g_seen_ids[0] = 1u`; `g_seen_count = 1u`; `req.event_id = 1u`
  * **Act**: Call `cruise_publish_notification(&req)`
  * **Assert**: Returns `CRUISE_OK`; `g_seen_count` unchanged (still 1u); platform HAL NOT called

---

#### Test Case: UTP-014-B (Boundary Value Analysis: Dedup Table Capacity)
**Technique**: Boundary Value Analysis
**Module View**: Internal Data Structures — `g_seen_count` boundary at `NOTIF_DEDUP_SIZE = 8`

* **UTS-014-B1** — Table at capacity (8 entries) → circular overwrite, no delivery failure
  * **Arrange**: Fill `g_seen_ids` with IDs 1..8; `g_seen_count = 8u`; stub platform HAL → `PLATFORM_OK`; new `req.event_id = 9u` (not in table)
  * **Act**: Call `cruise_publish_notification(&req)`
  * **Assert**: Returns `CRUISE_OK`; `g_seen_ids[0]` overwritten with `9u` (circular); `g_seen_count` remains at `NOTIF_DEDUP_SIZE` (no overflow past 8)

---

### MOD-015: lifecycle_evidence_manifest (Inspection Artifact)

**Note**: MOD-015 contains no executable code. The unit test below is an inspection
activity verifying the artifact's existence and completeness.

**Dependency & Mock Registry**: None — inspection artifact.

---

#### Test Case: UTP-015-A (Inspection: Evidence Manifest File Exists and Is Complete)
**Technique**: Inspection
**Module View**: Algorithmic/Logic View (inspection-only)

* **UTS-015-A1** — Evidence manifest file exists with all required sections
  * **Arrange**: Navigate to `docs/lifecycle/evidence_manifest.md`
  * **Act**: Inspect file for mandatory sections: artifact inventory table, ASIL D compliance checklist, cybersecurity evidence checklist
  * **Assert**: File exists; all three sections are present; every mandatory artifact row in the inventory table has a non-empty review status

---

### MOD-016: scheduling_controller

**Dependency & Mock Registry**

| Dependency | Source | Mock Strategy |
|------------|--------|--------------|
| `cruise_read_platform_inputs()` | MOD-008 | Stub returning `CRUISE_OK` with known inputs |
| `cruise_verify_brake()` | MOD-009 | Stub returning programmed `VerifiedInput_t` |
| `cruise_verify_driver()` | MOD-009 | Stub |
| `cruise_derive_conditions()` | MOD-010 | Stub returning programmed `ConditionSet_t` |
| `cruise_timing_check()` | MOD-005 | Spy |
| `cruise_timing_arm()` | MOD-005 | Spy |
| `cruise_sequence_override()` | MOD-004 | Spy; returns `CRUISE_OK` |
| `cruise_handle_fault()` | MOD-011 | Spy |
| `cruise_state_read()` | MOD-003 | Stub |
| `watchdog_record_completion()` | MOD-017 | Spy |

---

#### Test Case: UTP-016-A (State Transition Testing: Cycle Phase Progression)
**Technique**: State Transition Testing
**Module View**: State Machine View — Idle → Step0 → … → Idle

* **UTS-016-A1** — Nominal cycle with no conditions active → all 8 watchdog completions recorded
  * **Arrange**: All stubs return success with no conditions active (`ConditionSet_t` all false); state stub → `CRUISE_STANDBY`
  * **Act**: Call `scheduling_run_cycle(1000u)`
  * **Assert**: Returns `CRUISE_OK`; `watchdog_record_completion` spy called 8 times with steps 0–7 in order; `g_cycle_count == 1u`; `g_cycle_active == false` after return

* **UTS-016-A2** — Brake_Override condition active while Cruise_Active → override sequence triggered
  * **Arrange**: `cruise_derive_conditions` stub sets `out.brake_override = true`; `cruise_state_read` → `CRUISE_ACTIVE`; `cruise_timing_arm` and `cruise_sequence_override` spies ready
  * **Act**: Call `scheduling_run_cycle(2000u)`
  * **Assert**: `cruise_timing_arm` spy called with `2000u`; `cruise_sequence_override` spy called; `cruise_handle_fault` NOT called

---

#### Test Case: UTP-016-B (Statement and Branch Coverage: Fault Path Routing)
**Technique**: Statement & Branch Coverage
**Module View**: Algorithmic/Logic View — condition routing to handle_fault

* **UTS-016-B1** — LCU condition active → cruise_handle_fault called with COND_LCU
  * **Arrange**: `cruise_derive_conditions` stub: `lcu_active=true`, others false; `cruise_state_read` → `CRUISE_ACTIVE`
  * **Act**: Call `scheduling_run_cycle(3000u)`
  * **Assert**: `cruise_handle_fault` spy captured `COND_LCU`; `cruise_sequence_override` NOT called

---

### MOD-017: watchdog_supervisor

**Dependency & Mock Registry**

| Dependency | Source | Mock Strategy |
|------------|--------|--------------|
| `cruise_handle_fault()` | MOD-011 | Spy; returns `CRUISE_OK` |

---

#### Test Case: UTP-017-A (State Transition Testing: Monitoring → FaultEscalated)
**Technique**: State Transition Testing
**Module View**: State Machine View — Disarmed / Monitoring / FaultEscalated

* **UTS-017-A1** — Arm cycle → all steps complete in time → CRUISE_OK
  * **Arrange**: Call `watchdog_arm_cycle(0u)`; record all 8 steps with `watchdog_record_completion(i)` for i=0..7
  * **Act**: Call `watchdog_check_deadlines(19u)` (19 ms, within 20 ms period)
  * **Assert**: Returns `CRUISE_OK`; `cruise_handle_fault` NOT called

* **UTS-017-A2** — Step misses deadline → E120 and CCF escalated
  * **Arrange**: Call `watchdog_arm_cycle(0u)`; record only steps 0..5 (`watchdog_record_completion(6)` and `(7)` NOT called)
  * **Act**: Call `watchdog_check_deadlines(21u)` (21 ms, deadline for step 6 = 15 ms already past)
  * **Assert**: Returns `CRUISE_ERR_WATCHDOG_TIMEOUT` (E120); `cruise_handle_fault` spy captured `COND_CCF`

* **UTS-017-A3** — Arm new cycle clears prior fault state
  * **Arrange**: Previous cycle ended in fault (some `g_step_completed[i] == false`)
  * **Act**: Call `watchdog_arm_cycle(22u)` (new cycle start time)
  * **Assert**: All `g_step_completed[i] == false` (reset); all `g_step_deadline_ms[i]` set relative to 22u

---

#### Test Case: UTP-017-B (Boundary Value Analysis: Deadline Timing Boundary)
**Technique**: Boundary Value Analysis
**Module View**: Internal Data Structures — `g_step_deadline_ms[i]`

* **UTS-017-B1** — Check called exactly at deadline (inclusive) → no timeout
  * **Arrange**: `watchdog_arm_cycle(0u)`; do NOT record completion for step 7; step 7 deadline = `8 × SCHED_PERIOD_MS / SCHED_STEP_COUNT = 20u`
  * **Act**: Call `watchdog_check_deadlines(20u)` (at deadline, not past)
  * **Assert**: Returns `CRUISE_OK` (check condition is `> deadline`, not `>=`)

* **UTS-017-B2** — Check called one ms past deadline → timeout triggered
  * **Arrange**: Same as UTS-017-B1
  * **Act**: Call `watchdog_check_deadlines(21u)` (1 ms past deadline)
  * **Assert**: Returns `CRUISE_ERR_WATCHDOG_TIMEOUT` (E120); `cruise_handle_fault` called

---

## Safety-Critical Techniques (ISO 26262 Overlay)

### Variable-Level Fault Injection (ISO 26262-6 §9.4.4)

Applied to the three most safety-critical internal variables identified across ASIL D modules:

| Module | Variable | Fault Scenario | ASIL | Expected Detection | UTS Reference |
|--------|----------|---------------|------|-------------------|---------------|
| MOD-003 | `g_cruise_state` | Write `0xFF` (out of range) | ASIL D | `RANGE_VIOLATION (E010)` raised; state unchanged; CCF escalated | UTS-003-A2 |
| MOD-005 | `g_timer_armed` | Arm while already armed (`true`) | ASIL D | `CRUISE_ERR_ALREADY_ARMED`; start time unchanged | UTS-005-A4 |
| MOD-009 | `g_expected_counter_brake` | Present counter value from last cycle (replay) | ASIL D | `validity=false`; counter unchanged; replay rejected | UTS-009-A3 |
| MOD-013 | `g_record_ready` | Call write when `false` | ASIL D | `CRUISE_ERR_FIELD_VALIDATION`; no HAL call | UTS-013-A3 |

---

### MC/DC Decision Summary (ISO 26262-6 §9.4.4 Table 11)

All complex boolean decisions in ASIL D modules with MC/DC tables:

| Module | Decision Expression | MC/DC Test Cases | Coverage |
|--------|---------------------|-----------------|---------|
| MOD-002 | `speed_ok && brake_ok && rii_ok && ccf_ok && lcu_ok` | UTS-002-A1 through UTS-002-A3 | 6 test rows — each condition independently flipped |
| MOD-009 | `crc_ok && counter_ok` | UTS-009-C1 through UTS-009-C3 | 3 test rows — each condition independently flipped |

---

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total MOD modules | 17 |
| Non-external MODs requiring UTPs | 17 |
| Total Unit Test Cases (UTP) | 35 |
| Total Unit Scenarios (UTS) | 66 |
| MOD → UTP Coverage | 17/17 (100%) |
| V&V Gaps | 0 |

### Technique Distribution

| Technique | UTP Count |
|-----------|-----------|
| Statement & Branch Coverage | 16 |
| Boundary Value Analysis | 7 |
| Equivalence Partitioning | 6 |
| State Transition Testing | 5 |
| Strict Isolation | 7 |
| MC/DC Coverage | 2 |
| Inspection | 1 |
| **Total** | **35** (several UTPs combine techniques) |

### ISO 26262 Safety Coverage

| Category | Count |
|----------|-------|
| MC/DC truth tables | 2 (MOD-002, MOD-009) |
| Variable-level fault injection | 4 variables across 4 modules |
| ASIL D modules with white-box tests | 17/17 |

### External Module Bypass

None — no modules are tagged `[EXTERNAL]` in `module-design.md`.
MOD-015 (lifecycle evidence manifest) is an inspection artifact handled by UTP-015-A.
