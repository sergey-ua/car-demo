# Tasks: Vehicle Cruise Control — Brake Override Safety Slice

<!-- v-model:traces
  requirements: [REQ-001..REQ-018, REQ-NF-001, REQ-NF-002, REQ-IF-001..REQ-IF-007, REQ-CN-001..REQ-CN-007]
  system:       [SYS-001..SYS-008]
  architecture: [ARCH-001..ARCH-016]
  modules:      [MOD-001..MOD-017]
  hazards:      [HAZ-001..HAZ-021]
  version:      v0.7.0
-->

**Input**: Design documents from `specs/001-cruise-brake-override/`

**Prerequisites**: plan.md (required), spec.md (required for user stories), research.md, data-model.md, contracts/

**Tests**: Tests are required — this is an ASIL D safety-critical feature. TDD order is
mandatory per ISO 26262-6 §9. Unit tests → Implementation → Integration → System → Acceptance.

**Organization**: Tasks grouped by user story (US1–US4) enabling independent delivery of each safety slice.

**ASIL D note**: Tasks implementing ASIL D mitigations are prefixed `**[HAZARD-ELEVATED]**` and appear first within their story phase. Per-HAZ verification tasks appear in Phase N.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: User story (US1–US4)
- Include exact file paths in descriptions
- `<!-- traces-to: MOD → ARCH → SYS → REQ [, HAZ] -->` on every task

## Path Conventions

- **Single project**: `src/cruise_control/`, `src/platform/`, `tests/` at repository root
- All paths match `plan.md` source tree (C embedded, single-project structure)
- HAL stubs live in `tests/stubs/platform_hal_stub.c`

---

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Repository structure, build toolchain, and platform HAL abstraction.

- [ ] T001 Create directory tree: `src/cruise_control/`, `src/platform/`, `tests/unit/`, `tests/integration/`, `tests/system/`, `tests/acceptance/`, `tests/stubs/`, `docs/lifecycle/` <!-- traces-to: REQ-CN-004 → ARCH-001 → SYS-001 -->
- [ ] T002 Create `src/platform_hal.h` — abstract all 7 platform interfaces (propulsion, brake pedal, driver cmd, vehicle speed, diagnostic, notification, event storage) used by ARCH-006/007/012/013 <!-- traces-to: REQ-CN-001 → ARCH-006 → SYS-003, REQ-IF-001 -->
- [ ] T003 [P] Configure static analysis and MISRA C:2012 rule enforcement toolchain (REQ-CN-005) <!-- traces-to: REQ-CN-005 → ARCH-014 → SYS-008 -->
- [ ] T004 [P] Create `tests/stubs/platform_hal_stub.c` — stub implementation of `platform_hal.h` for unit and integration test isolation (configurable return codes) <!-- traces-to: REQ-CN-004 → ARCH-007 → SYS-004 -->

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Cross-cutting infrastructure — scheduling, watchdog, fault handler, and state register — that ALL user stories depend on. No story can begin until this phase is complete.

**⚠️ CRITICAL**: No user story work can begin until this phase is complete.

**Write unit tests** (RED) — write these tests first; they MUST fail before implementation

- [ ] T005 [P] Write unit test UTP-003-A/B: `cruise_state_write` valid states and out-of-range rejection (E010) in `tests/unit/test_state_machine.c` <!-- traces-to: MOD-003 → ARCH-002 → SYS-001 → REQ-001 -->
- [ ] T006 [P] Write unit test UTP-011-A: `cruise_handle_fault` routing for LCU, CCF, RII in `tests/unit/test_fault_handler.c` <!-- traces-to: MOD-011 → ARCH-010 → SYS-005 → REQ-008 -->
- [ ] T007 [P] Write unit tests UTP-016-A/B + UTP-017-A/B: scheduling cycle phase ordering and watchdog deadline detection in `tests/unit/test_scheduling.c` and `tests/unit/test_watchdog.c` <!-- traces-to: MOD-016 → ARCH-015 → SYS-001 [CROSS-CUTTING], MOD-017 → ARCH-016 → SYS-001 [CROSS-CUTTING] -->

**Implement modules** (GREEN) — implement AFTER tests fail

- [ ] T008 **[HAZARD-ELEVATED]** Implement `src/cruise_control/state_machine.h/.c` — MOD-003 `cruise_state_register` with range check and CCF escalation (ARCH-002 / REQ-001) <!-- traces-to: MOD-003 → ARCH-002 → SYS-001 → REQ-001, HAZ-001, HAZ-002 -->
- [ ] T009 Implement `src/cruise_control/fault_handler.h/.c` — MOD-011 `cruise_handle_fault` routing LCU/CCF/RII to cessation and transition (ARCH-010 / REQ-006..REQ-011) <!-- traces-to: MOD-011 → ARCH-010 → SYS-005 → REQ-008 -->
- [ ] T010 Implement `src/platform/scheduling.h/.c` — MOD-016 `scheduling_controller` 8-step cyclic executive at ≤ 20 ms (ARCH-015 [CROSS-CUTTING]) and MOD-017 `watchdog_supervisor` deadline monitoring (ARCH-016 [CROSS-CUTTING]) <!-- traces-to: MOD-016 → ARCH-015 → REQ-NF-001, MOD-017 → ARCH-016 → REQ-NF-001 -->

**Run unit tests** (gate — all foundational tests must pass)

- [ ] T011 Run foundational unit tests: `tests/unit/test_state_machine.c`, `test_fault_handler.c`, `test_scheduling.c`, `test_watchdog.c` — all must pass; 0 MISRA violations <!-- traces-to: UTP-003-A, UTP-003-B, UTP-011-A, UTP-016-A, UTP-017-A -->

**Checkpoint**: Foundation ready — user story implementation can now begin in parallel.

---

## Phase 3: User Story 1 - [Title] (Priority: P1) 🎯 MVP

**US1**: Brake Override While Cruise Active — the primary safety behaviour (REQ-002..REQ-013)

**Goal**: When `Cruise_Active` and `Brake_Override` confirmed: cease speed control, release authority, transition to `Cruise_Cancelled`, notify interface, persist event record — all within 100 ms.

**Independent Test**: `ATP-002-A`: place in `Cruise_Active`, trigger `Brake_Override`, verify cessation + `Cruise_Cancelled` + event record in storage.

**Write unit tests** (RED) — write and confirm failing BEFORE implementing

- [ ] T012 **[HAZARD-ELEVATED]** [P] [US1] Write unit tests UTP-001-A/B (eval_transition branch coverage + invalid condition E001) in `tests/unit/test_state_machine.c` <!-- traces-to: MOD-001 → ARCH-001 → SYS-001 → REQ-001, HAZ-001, HAZ-002 -->
- [ ] T013 [P] [US1] Write unit tests UTP-002-A (MC/DC 6-row truth table for 5-condition AND) in `tests/unit/test_state_machine.c` <!-- traces-to: MOD-002 → ARCH-001 → SYS-001 → REQ-018, HAZ-003 -->
- [ ] T014 **[HAZARD-ELEVATED]** [P] [US1] Write unit tests UTP-004-A (sequence step ordering + step failure) in `tests/unit/test_override_response.c` <!-- traces-to: MOD-004 → ARCH-003 → SYS-002 → REQ-002, HAZ-004 -->
- [ ] T015 **[HAZARD-ELEVATED]** [P] [US1] Write unit tests UTP-005-A/B (timing state transitions + T_MAX BVA: 99 ms, 100 ms, 101 ms) in `tests/unit/test_override_response.c` <!-- traces-to: MOD-005 → ARCH-004 → SYS-002 → REQ-NF-001, HAZ-006 -->
- [ ] T016 **[HAZARD-ELEVATED]** [P] [US1] Write unit tests UTP-006-A (authorisation gate) + UTP-007-A (CMD_TYPE EP + write failure E040) in `tests/unit/test_speed_controller.c` <!-- traces-to: MOD-006 → ARCH-005 → SYS-003 → REQ-002, MOD-007 → ARCH-006 → SYS-003 → REQ-002, HAZ-007 -->
- [ ] T017 **[HAZARD-ELEVATED]** [P] [US1] Write unit tests UTP-008-A (all-OK vs any-fail HAL) + UTP-009-A/B/C (CRC branch + counter BVA + MC/DC 3-row) in `tests/unit/test_input_monitor.c` <!-- traces-to: MOD-008 → ARCH-007 → SYS-004 → REQ-016, MOD-009 → ARCH-008 → SYS-004 → REQ-CN-007, HAZ-009, HAZ-010 -->
- [ ] T018 [P] [US1] Write unit test UTP-010-A (condition derivation EP for Brake_Override/RII/LCU/CCF paths) in `tests/unit/test_input_monitor.c` <!-- traces-to: MOD-010 → ARCH-009 → SYS-004 → REQ-IF-002 -->

**Implement modules** (GREEN) — in dependency order: state machine → input pipeline → override coordinator

- [ ] T019 **[HAZARD-ELEVATED]** [US1] Implement `state_machine.c` MOD-001 `cruise_eval_transition` — transition table (Cruise_Active/Standby/Fault states, all 4 conditions) + MOD-002 `cruise_check_preconditions` (5-condition AND) (ARCH-001 / REQ-001, REQ-018) <!-- traces-to: MOD-001 → ARCH-001 → SYS-001 → REQ-001, HAZ-001, HAZ-002 -->
- [ ] T020 **[HAZARD-ELEVATED]** [US1] Implement `input_monitor.h/.c` MOD-008 `cruise_read_platform_inputs` (5 HAL reads / ARCH-007) + MOD-009 `cruise_verify_integrity` (CRC-16/CCITT-FALSE + 4-bit rolling counter / ARCH-008 / REQ-CN-007) + MOD-010 `cruise_derive_conditions` (ARCH-009) <!-- traces-to: MOD-009 → ARCH-008 → SYS-004 → REQ-CN-007, HAZ-009, HAZ-010 -->
- [ ] T021 **[HAZARD-ELEVATED]** [US1] Implement `speed_controller.h/.c` MOD-006 `cruise_generate_speed_cmd` (auth gate / ARCH-005) + MOD-007 `cruise_propulsion_write` (CMD_CESSATION / CMD_SPEED_CTRL / exclusive interface / ARCH-006 / REQ-IF-001) <!-- traces-to: MOD-007 → ARCH-006 → SYS-003 → REQ-002, HAZ-007 -->
- [ ] T022 **[HAZARD-ELEVATED]** [US1] Implement `override_response.h/.c` MOD-004 `cruise_sequence_override` (4-step sequence: cessation→transition→log→notify / ARCH-003 / REQ-002..REQ-013) + MOD-005 `cruise_timing_supervisor` (T_MAX = 100 ms arm/stop/check / ARCH-004 / REQ-NF-001) <!-- traces-to: MOD-004 → ARCH-003 → SYS-002 → REQ-002, HAZ-004, HAZ-006 -->

**Run unit tests** (gate)

- [ ] T023 [US1] Run all US1 unit tests (UTP-001..UTP-010); fix all failures; MISRA C:2012 clean; MC/DC coverage confirmed for UTP-002-A and UTP-009-C <!-- traces-to: UTP-001-A, UTP-002-A, UTP-004-A, UTP-005-A, UTP-005-B, UTP-006-A, UTP-007-A, UTP-008-A, UTP-009-C -->

**Write integration tests** — test module-boundary contracts

- [ ] T024 **[HAZARD-ELEVATED]** [P] [US1] Write ITP-001-A/B (ARCH-009→ARCH-001 condition signal contract + fault injection: undefined code) and ITP-002-A/B (ARCH-001↔ARCH-002 write-read contract + range violation) in `tests/integration/test_state_machine_itp.c` <!-- traces-to: ITP-001-A → ARCH-001 → SYS-001 → REQ-001, HAZ-001 -->
- [ ] T025 **[HAZARD-ELEVATED]** [P] [US1] Write ITP-003-A/B (override response sequence + log data flow) and ITP-004-A/B (timing arm/stop contract + deadline E030) in `tests/integration/test_override_response_itp.c` <!-- traces-to: ITP-003-A → ARCH-003 → SYS-002 → REQ-002, HAZ-004, HAZ-006 -->
- [ ] T026 **[HAZARD-ELEVATED]** [P] [US1] Write ITP-006-A/B (propulsion cessation + concurrent cessation from ARCH-003 and ARCH-010) in `tests/integration/test_speed_controller_itp.c` <!-- traces-to: ITP-006-B → ARCH-006 → SYS-003 → REQ-002, HAZ-007 -->
- [ ] T027 **[HAZARD-ELEVATED]** [P] [US1] Write ITP-007-A/B (5 raw inputs + READ_TIMEOUT→RII) and ITP-008-A/B (valid→TRUE + CRC fail→FALSE data flow) and ITP-009-A/B (condition published to all consumers) in `tests/integration/test_input_monitor_itp.c` <!-- traces-to: ITP-008-B → ARCH-008 → SYS-004 → REQ-CN-007, HAZ-009, HAZ-010 -->

**Run integration tests** (gate — SIL mode with stubbed platform HAL)

- [ ] T028 [US1] Run US1 integration tests (ITP-001..ITP-009); all must pass in SIL mode (platform HAL stubbed) <!-- traces-to: ITP-001-A, ITP-003-A, ITP-006-A, ITP-008-B -->

**Write system tests** — full SYS-level verification

- [ ] T029 [P] [US1] Write STP-001-A/B/C (state machine completeness, boundary, fault injection) and STP-002-A/B/C (override response sequence, SYS-003 failure, SYS-006 failure) in `tests/system/test_state_machine_stp.c` and `test_override_response_stp.c` <!-- traces-to: STP-002-A → SYS-002 → REQ-002, STP-001-C → SYS-001 → REQ-001 -->
- [ ] T030 [P] [US1] Write STP-003-A/B/C/D (propulsion interface exclusivity, cessation from SYS-002, state gate BVA, concurrent cessation) and STP-004-A/B/C/D (brake pedal integrity, condition publication, EP, speed range BVA) in `tests/system/` <!-- traces-to: STP-003-A → SYS-003 → REQ-IF-001, STP-004-B → SYS-004 → REQ-CN-007 -->

**Run system tests** (gate — HIL required for STP-003-A/007-A/012-A/013-A; others SIL)

- [ ] T031 [US1] Run US1 system tests (STP-001..STP-004); SIL tests in CI, HIL tests on EV prototype bench <!-- traces-to: STP-001-A, STP-002-A, STP-003-A, STP-004-A -->

**Write acceptance tests** — top US1 BDD scenarios

- [ ] T032 [P] [US1] Write ATP-002-A/B/C (cessation on override, cessation during speed correction, fault injection), ATP-003-A/B (authority release, no re-assumption), ATP-004-A/B (Cruise_Cancelled target, 20-run consistency) in `tests/acceptance/test_us1_atp.c` <!-- traces-to: ATP-002-A → REQ-002, ATP-004-A → REQ-004 -->

**Checkpoint**: US1 fully verified — Brake_Override response confirmed end-to-end.

---

## Phase 4: User Story 2 - [Title] (Priority: P2)

**US2**: Audit and Event Reconstruction — every transition reproducible from event log (REQ-013..REQ-015)

**Goal**: Every safety-relevant state transition persists a 5-field record to platform storage before the transition completes; an auditor can reconstruct the full session from the log alone.

**Independent Test**: `ATP-015-A`: trigger multi-event session; Safety Engineer reconstructs sequence from log alone, matches ground-truth harness record.

**Write unit tests** (RED)

- [ ] T033 [P] [US2] Write unit tests UTP-012-A (5-field validation + BVA vehicle_spd boundary) and UTP-013-A (write paths: record ready/not-ready, write success/failure + REQ-017 CCF escalation) in `tests/unit/test_event_logger.c` <!-- traces-to: MOD-012 → ARCH-011 → SYS-006 → REQ-014, MOD-013 → ARCH-012 → SYS-006 → REQ-017, HAZ-016 -->

**Implement modules** (GREEN)

- [ ] T034 **[HAZARD-ELEVATED]** [US2] Implement `event_logger.h/.c` MOD-012 `cruise_build_event_record` (5-field validation, g_pending_record / ARCH-011 / REQ-013, REQ-014) + MOD-013 `cruise_write_event_record` (synchronous HAL write + write failure → CCF per REQ-017 / ARCH-012 / REQ-013, REQ-017) <!-- traces-to: MOD-013 → ARCH-012 → SYS-006 → REQ-017, HAZ-016, HAZ-017 -->

**Run unit tests** (gate)

- [ ] T035 [US2] Run US2 unit tests (UTP-012-A, UTP-013-A); write failure path must trigger CCF escalation (REQ-017 verified) <!-- traces-to: UTP-012-A, UTP-013-A -->

**Write integration tests**

- [ ] T036 [P] [US2] Write ITP-011-A/B (log event request → 5-field record contract + null field E080) and ITP-012-A/B (platform storage write + write failure → CCF) in `tests/integration/test_event_logger_itp.c` <!-- traces-to: ITP-012-B → ARCH-012 → SYS-006 → REQ-017, HAZ-016 -->

**Run integration tests** (gate — SIL with storage stub)

- [ ] T037 [US2] Run US2 integration tests (ITP-011, ITP-012); write failure must escalate CCF (ITP-012-B) <!-- traces-to: ITP-012-B → SYS-006 → REQ-017 -->

**Write system tests**

- [ ] T038 [P] [US2] Write STP-006-A/B/C (record written to platform storage, 5-field completeness, write failure → CCF fault injection) in `tests/system/test_event_logger_stp.c` <!-- traces-to: STP-006-C → SYS-006 → REQ-017, HAZ-016 -->

**Run system tests** (gate — STP-006-A/B SIL; STP-006-C requires storage fault injection harness)

- [ ] T039 [US2] Run US2 system tests (STP-006-A/B/C) <!-- traces-to: STP-006-A, STP-006-C -->

**Write acceptance tests**

- [ ] T040 [P] [US2] Write ATP-013-A/B (record written for each transition, record written before transition completes), ATP-014-A/B (5 fields present, field values accurate), ATP-015-A (full reconstruction demonstration) in `tests/acceptance/test_us2_atp.c` <!-- traces-to: ATP-015-A → REQ-015, ATP-013-B → REQ-013 -->

**Checkpoint**: US2 complete — event log supports full post-incident reconstruction.

---

## Phase 5: User Story 3 - [Title] (Priority: P3)

**US3**: Reactivation Guard After Brake Override — cruise cannot re-engage until all 5 preconditions valid (REQ-005, REQ-018)

**Goal**: After any `Brake_Override` override, cruise reactivation is refused until all five activation preconditions are simultaneously satisfied.

**Independent Test**: `ATP-005-A/B/C`: trigger override, verify activation refused before preconditions valid, verify activation permitted after all five satisfied.

**Write unit tests** (RED)

- [ ] T041 [P] [US3] Write unit tests UTP-002-A (MC/DC 6-row truth table: all 5 conditions can independently block activation) in `tests/unit/test_state_machine.c` (if not already written in T013) <!-- traces-to: MOD-002 → ARCH-001 → SYS-001 → REQ-018, HAZ-003 -->

**Implement modules** (GREEN — MOD-002 already implemented in T019 if US1 done first)

- [ ] T042 [US3] Verify `cruise_check_preconditions` (MOD-002) correctly gates all 5 preconditions; confirm ARCH-001 reactivation guard wiring in `state_machine.c` (verify `cruise_check_preconditions` called on every Standby→Active attempt) <!-- traces-to: MOD-002 → ARCH-001 → SYS-001 → REQ-005, REQ-018, HAZ-003 -->

**Run unit tests** (gate)

- [ ] T043 [US3] Run US3 unit tests (UTP-002-A MC/DC 6 rows all pass); confirm each precondition independently returns false <!-- traces-to: UTP-002-A -->

**Write integration tests**

- [ ] T044 [P] [US3] Write ITP-005-A (authorisation gate: ARCH-002→ARCH-005 command blocked outside Cruise_Active) in `tests/integration/test_speed_controller_itp.c`; confirm reactivation guard prevents speed control on premature activation <!-- traces-to: ITP-005-A → ARCH-005 → SYS-003 → REQ-005 -->

**Run integration tests** (gate)

- [ ] T045 [US3] Run US3 integration tests (ITP-005-A) <!-- traces-to: ITP-005-A -->

**Write system tests**

- [ ] T046 [P] [US3] Write STP-005-A/B/C (reactivation refused before preconditions, permitted after, partial preconditions insufficient) in `tests/system/test_fault_handler_stp.c` <!-- traces-to: STP-005-A → SYS-005 → REQ-005 -->

**Run system tests** (gate)

- [ ] T047 [US3] Run US3 system tests (STP-005-A/B/C) <!-- traces-to: STP-005-A, STP-005-B, STP-005-C -->

**Write acceptance tests**

- [ ] T048 [P] [US3] Write ATP-005-A/B/C (activation refused pre-preconditions, permitted post-preconditions, partial failure) and ATP-018-A/B (5-condition coverage) in `tests/acceptance/test_us3_atp.c` <!-- traces-to: ATP-005-A → REQ-005, ATP-018-B → REQ-018 -->

**Checkpoint**: US3 complete — reactivation guard verified against all 5-condition combinations.

---

## Phase N: Polish & Cross-Cutting Concerns

**Purpose**: US4 (cybersecurity), notification publisher, per-HAZ verification, X-VERSE preparation, and evidence package sign-off.

### US4: Cybersecurity — Invalid/Tampered Input (P4)

**Write unit tests** (RED)

- [ ] T049 **[HAZARD-ELEVATED]** [P] [US4] Write unit tests UTP-009-C (MC/DC 3-row: crc_ok && counter_ok) in `tests/unit/test_input_monitor.c` (if not already in T017) <!-- traces-to: MOD-009 → ARCH-008 → SYS-004 → REQ-CN-007, HAZ-010, HAZ-021 -->

**Implement modules** (GREEN)

- [ ] T050 **[HAZARD-ELEVATED]** [US4] Verify MOD-009 `cruise_verify_integrity` CRC-16/CCITT-FALSE computation and counter wrap 0→15→0 (ARCH-008 / REQ-CN-007 / HAZ-010); confirm integrity failure produces RII not Brake_Override <!-- traces-to: MOD-009 → ARCH-008 → SYS-004 → REQ-CN-007, HAZ-010 -->

**Run unit tests** (gate)

- [ ] T051 [US4] Run US4 unit tests (UTP-009-A/B/C all pass) <!-- traces-to: UTP-009-C -->

**Write integration tests**

- [ ] T052 **[HAZARD-ELEVATED]** [P] [US4] Write ITP-008-B (integrity-failed input → validity=FALSE → RII at ARCH-009; not Brake_Override) in `tests/integration/test_input_monitor_itp.c` <!-- traces-to: ITP-008-B → ARCH-008 → SYS-004 → REQ-CN-007, HAZ-010 -->

**Run integration tests** (gate)

- [ ] T053 [US4] Run US4 integration tests (ITP-008-B) <!-- traces-to: ITP-008-B -->

**Write system tests**

- [ ] T054 [P] [US4] Write STP-004-A/B (brake pedal integrity validation + EP valid/invalid/out-of-range) and STP-008-B (cybersecurity threat analysis completeness inspection) in `tests/system/test_input_monitor_stp.c` <!-- traces-to: STP-004-A → SYS-004 → REQ-CN-007, HAZ-010 -->

**Run system tests** (gate)

- [ ] T055 [US4] Run US4 system tests (STP-004-A/B, STP-008-B) <!-- traces-to: STP-004-A, STP-008-B -->

**Write acceptance tests**

- [ ] T056 [P] [US4] Write ATP-CN-007-A (CRC failure → RII, not Brake_Override; counter replay → RII), ATP-CN-003-A (threat analysis documentation inspection) in `tests/acceptance/test_us4_atp.c` <!-- traces-to: ATP-CN-007-A → REQ-CN-007, HAZ-010 -->

### Notification Publisher (cross-cutting, all stories)

**Write unit tests** (RED)

- [ ] T057 [P] Write unit tests UTP-014-A/B (dedup logic: new vs duplicate event_id; BVA at capacity 8) in `tests/unit/test_notification.c` <!-- traces-to: MOD-014 → ARCH-013 → SYS-007 → REQ-012 -->

**Implement modules** (GREEN)

- [ ] T058 [P] Implement `notification.h/.c` MOD-014 `cruise_publish_notification` (CRC-16 dedup by event_id, circular buffer 8 entries / ARCH-013 / REQ-012, REQ-IF-003) <!-- traces-to: MOD-014 → ARCH-013 → SYS-007 → REQ-012, HAZ-018, HAZ-019 -->

**Run unit tests** (gate)

- [ ] T059 Run notification unit tests (UTP-014-A/B) <!-- traces-to: UTP-014-A, UTP-014-B -->

**Write integration and acceptance tests**

- [ ] T060 [P] Write ITP-013-A/B (notification delivered to platform, duplicate absorbed) and ATP-012-A/B (notification issued, not duplicated) in `tests/integration/` and `tests/acceptance/` <!-- traces-to: ITP-013-A → ARCH-013 → SYS-007 → REQ-012, ATP-012-A → REQ-012 -->

### Per-HAZ Verification Tasks (ASIL D mandatory — one per hazard)

- [ ] T061 **[HAZARD-ELEVATED]** Verify mitigation for HAZ-001: `cruise_eval_transition` never stays in Cruise_Active on Brake_Override — run UTP-001-A, ITP-001-A, STP-002-A, ATP-002-A; all pass <!-- traces-to: HAZ-001 → REQ-001, REQ-002 → SYS-001, SYS-002 → ARCH-001, ARCH-003, UTP-001-A, ATP-002-A -->
- [ ] T062 **[HAZARD-ELEVATED]** Verify mitigation for HAZ-002: state variable never holds undefined value — run UTP-001-B (EP undefined condition), UTP-003-A2 (range violation E010), ITP-002-B; all pass <!-- traces-to: HAZ-002 → REQ-001, REQ-004 → SYS-001 → ARCH-002, UTP-003-A2 -->
- [ ] T063 **[HAZARD-ELEVATED]** Verify mitigation for HAZ-003: reactivation guard blocks premature activation — run UTP-002-A (all 6 MC/DC rows), STP-005-A; pass <!-- traces-to: HAZ-003 → REQ-005, REQ-018 → SYS-001 → ARCH-001, UTP-002-A, STP-005-A -->
- [ ] T064 **[HAZARD-ELEVATED]** Verify mitigation for HAZ-004: override coordinator never omits step — run UTP-004-A2 (step failure path), ITP-003-A (sequence order); pass <!-- traces-to: HAZ-004 → REQ-002, REQ-003 → SYS-002 → ARCH-003, UTP-004-A, ITP-003-A -->
- [ ] T065 Verify mitigation for HAZ-005: partial sequence completion does not lose authority — run UTP-004-A2, ATP-003-B (authority not re-assumed); pass <!-- traces-to: HAZ-005 → REQ-003, REQ-013 → SYS-002 → ARCH-003, ATP-003-B -->
- [ ] T066 **[HAZARD-ELEVATED]** Verify mitigation for HAZ-006: T_MAX = 100 ms enforced by ARCH-004 — run UTP-005-B (BVA: 99/100/101 ms), ATP-NF-001-A (100-run latency test); all latencies ≤ 100 ms <!-- traces-to: HAZ-006 → REQ-NF-001 → SYS-002 → ARCH-004, UTP-005-B, ATP-NF-001-A -->
- [ ] T067 **[HAZARD-ELEVATED]** Verify mitigation for HAZ-007: cessation command terminates any in-progress speed command — run UTP-007-A1 (cessation path), ITP-006-A (cessation + ack), STP-003-B (cessation within one cycle); pass <!-- traces-to: HAZ-007 → REQ-002 → SYS-003 → ARCH-006, UTP-007-A1, STP-003-B -->
- [ ] T068 Verify mitigation for HAZ-008: no speed commands outside Cruise_Active — run UTP-006-A1 (not-authorised → zero-command), STP-003-C (BVA all 4 non-active states); pass <!-- traces-to: HAZ-008 → REQ-001 → SYS-003 → ARCH-006, UTP-006-A1, STP-003-C -->
- [ ] T069 **[HAZARD-ELEVATED]** Verify mitigation for HAZ-009: Brake_Override missed detection — run UTP-008-A2 (HAL timeout → RII), ITP-007-A (5-input contract), STP-004-A (brake-to-condition path); pass <!-- traces-to: HAZ-009 → REQ-016, REQ-IF-002 → SYS-004 → ARCH-007, UTP-008-A, ITP-007-A -->
- [ ] T070 **[HAZARD-ELEVATED]** Verify mitigation for HAZ-010: tampered brake signal produces RII not Brake_Override — run UTP-009-C (MC/DC rows 2+3), ITP-008-B (integrity fail → RII), ATP-CN-007-A; pass <!-- traces-to: HAZ-010 → REQ-CN-007, REQ-IF-002 → SYS-004 → ARCH-008, UTP-009-C, ATP-CN-007-A -->
- [ ] T071 Verify mitigation for HAZ-011: spurious Brake_Override (false positive) — run UTP-009-A3 (valid CRC but wrong counter → false), STP-004-C (EP invalid class); pass <!-- traces-to: HAZ-011 → REQ-016 → SYS-004 → ARCH-008, UTP-009-A3 -->
- [ ] T072 Verify mitigation for HAZ-012: RII not detected (integrity passed but value wrong) — run UTP-010-A3 (speed out of range → RII), STP-004-D (speed BVA max+1); pass <!-- traces-to: HAZ-012 → REQ-010, REQ-011 → SYS-004 → ARCH-009, UTP-010-A3 -->
- [ ] T073 **[HAZARD-ELEVATED]** Verify mitigation for HAZ-013: LCU not processed → speed control continues — run UTP-011-A1 (LCU routing: cessation + transition), ITP-010-A (SYS-005 contract), STP-005-A (LCU triggers cessation and transition); pass <!-- traces-to: HAZ-013 → REQ-006, REQ-007 → SYS-005 → ARCH-010, UTP-011-A1, STP-005-A -->
- [ ] T074 Verify mitigation for HAZ-014: CCF not processed → remains in faulty state — run UTP-011-A2 (CCF no-cessation-in-Standby), STP-005-B (CCF from all 5 states → Cruise_Fault); pass <!-- traces-to: HAZ-014 → REQ-008, REQ-009 → SYS-005 → ARCH-010, STP-005-B -->
- [ ] T075 Verify mitigation for HAZ-015: RII while active not responded to — run UTP-011-A1 (RII branch), STP-005-A (LCU/RII path in fault handler); pass <!-- traces-to: HAZ-015 → REQ-010, REQ-011 → SYS-005 → ARCH-009, ARCH-010 -->
- [ ] T076 **[HAZARD-ELEVATED]** Verify mitigation for HAZ-016: event record not written before transition completes — run UTP-013-A2 (write failure → CCF), ITP-012-B (write failure path), STP-006-C (fault injection: write failure); pass <!-- traces-to: HAZ-016 → REQ-013, REQ-017 → SYS-006 → ARCH-012, UTP-013-A2, STP-006-C -->
- [ ] T077 Verify mitigation for HAZ-017: incomplete event record — run UTP-012-A2 (null timestamp → E080), ATP-014-A (5 fields present); pass <!-- traces-to: HAZ-017 → REQ-014 → SYS-006 → ARCH-011, UTP-012-A2, ATP-014-A -->
- [ ] T078 Verify mitigation for HAZ-018: notification not delivered — run UTP-014-A1 (new event_id delivered), ITP-013-A (platform notification interface), ATP-012-A; pass <!-- traces-to: HAZ-018 → REQ-012 → SYS-007 → ARCH-013, UTP-014-A1, ATP-012-A -->
- [ ] T079 Verify mitigation for HAZ-019: duplicate notifications — run UTP-014-A2 (duplicate absorbed), ITP-013-B (exactly-once), ATP-012-B; pass <!-- traces-to: HAZ-019 → REQ-012 → SYS-007 → ARCH-013, UTP-014-A2, ITP-013-B -->
- [ ] T080 **[HAZARD-ELEVATED]** Verify mitigation for HAZ-020: lifecycle artifacts incomplete before field testing — run STP-008-A (lifecycle audit inspection), ATP-CN-002-A, ATP-NF-002-A; all mandatory artifacts present and approved <!-- traces-to: HAZ-020 → REQ-CN-002, REQ-NF-002 → SYS-008 → ARCH-014, STP-008-A, ATP-NF-002-A -->
- [ ] T081 **[HAZARD-ELEVATED]** Verify mitigation for HAZ-021: threat analysis absent before field testing — run STP-008-B (cybersecurity inspection), ATP-CN-003-A; threat analysis present with all treatments documented <!-- traces-to: HAZ-021 → REQ-CN-003 → SYS-004, SYS-008 → ARCH-008, ARCH-014, STP-008-B, ATP-CN-003-A -->

### Final Pre-Field-Testing Gates

- [ ] T082 [P] Create `docs/lifecycle/evidence_manifest.md` (MOD-015 / ARCH-014 / SYS-008) — complete ASIL D artifact inventory with review status; supports STP-008-A inspection <!-- traces-to: MOD-015 → ARCH-014 → SYS-008 → REQ-NF-002, HAZ-020 -->
- [ ] T083 [P] Run full traceability matrix rebuild (`/speckit-v-model-trace`) — verify all 5 matrices remain `has_gaps: false` after all test implementations are in place <!-- traces-to: REQ-CN-004 → ARCH-014 → SYS-008 -->
- [ ] T084 [P] Run quickstart.md validation — execute the 6 top acceptance scenarios (SCN-002-A1, SCN-004-A1, SCN-013-B1, SCN-015-A1, SCN-005-A1, SCN-018-B1) and confirm all pass <!-- traces-to: ATP-002-A, ATP-004-A, ATP-015-A, ATP-005-A, ATP-018-B -->
- [ ] T085 [P] X-VERSE virtual validation activity — Test Engineer configures and executes X-VERSE scenarios per OQ-006 (deferred to Test Engineer post-baseline) <!-- traces-to: REQ-CN-006 → ARCH-014 → SYS-008 → REQ-CN-006, ATP-CN-006-A -->
- [ ] T086 Final pre-field-testing compliance review — Safety Engineer, Test Engineer, Compliance Officer sign off on evidence package per STP-008-A, ATP-NF-002-A <!-- traces-to: HAZ-020 → REQ-NF-002, REQ-CN-002 → SYS-008 -->

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies — start immediately
- **Foundational (Phase 2)**: Depends on Phase 1 — BLOCKS all user stories
- **US1 (Phase 3)**: Depends on Phase 2 — ASIL D highest priority, start first
- **US2 (Phase 4)**: Depends on Phase 2; can run in parallel with US1 after T011
- **US3 (Phase 5)**: Depends on Phase 2; MOD-002 implemented in Phase 3 T019
- **Phase N (Polish)**: US4 depends on Phase 2; HAZ verification depends on respective US phases

### User Story Dependencies

- **US1 (P1)**: After Foundational — no dependency on US2/US3
- **US2 (P2)**: After Foundational — MOD-012/013 independent of US1 MODs
- **US3 (P3)**: After US1 (MOD-002 implemented in T019); MOD-002 verify step T042
- **US4 (P4)**: After Foundational — MOD-009 implemented in T020 (parallel with US1)

### ASIL D Elevation Order (within each phase)

Tasks prefixed `**[HAZARD-ELEVATED]**` must be implemented before non-elevated peers within the same phase. HAZ-001/002/004/006/007/009 (primary Brake_Override chain) are the highest priority.

### Within Each User Story (TDD Order — non-negotiable per REQ-011)

1. Write unit tests → verify they FAIL
2. Implement modules (dependency order within story)
3. Run unit tests (gate — must pass)
4. Write integration tests
5. Run integration tests (gate)
6. Write system tests
7. Run system tests (gate)
8. Write acceptance tests

---

## Parallel Example: User Story 1

```bash
# All unit test write tasks can run in parallel (different files):
T012: tests/unit/test_state_machine.c (UTP-001-A/B)
T013: tests/unit/test_state_machine.c (UTP-002-A MC/DC)   # same file — serialise with T012
T014: tests/unit/test_override_response.c (UTP-004-A)
T015: tests/unit/test_override_response.c (UTP-005-A/B)   # same file — serialise with T014
T016: tests/unit/test_speed_controller.c (UTP-006-A, UTP-007-A)
T017: tests/unit/test_input_monitor.c (UTP-008-A, UTP-009-A/B/C)
T018: tests/unit/test_input_monitor.c (UTP-010-A)          # same file — serialise with T017

# Implementation tasks ordered by dependency (state register first):
T019: state_machine.c — MOD-001 + MOD-002 (depends on T008 foundation)
T020: input_monitor.c — MOD-008 + MOD-009 + MOD-010 (independent of T019)
T021: speed_controller.c — MOD-006 + MOD-007 (independent of T019, T020)
T022: override_response.c — MOD-004 + MOD-005 (depends on T019, T020, T021)
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Phase 1: Setup (T001–T004)
2. Phase 2: Foundational (T005–T011) — state register + fault handler + scheduling
3. Phase 3: US1 TDD cycle (T012–T032) — full Brake_Override safety path
4. **STOP and VALIDATE**: all US1 acceptance scenarios pass (ATP-002-A, ATP-004-A)
5. HAZ verification for HAZ-001/002/004/006/007/009 (T061, T062, T064, T066, T067, T069)

### Incremental Delivery

1. Foundation + US1 → Brake_Override safety path verified → pre-field-testing candidate
2. US2 → Event log reconstruction verified → safety evidence complete
3. US3 → Reactivation guard verified → full cruise session lifecycle covered
4. US4 + Phase N → Cybersecurity + full HAZ evidence package → field trial authorised

### ASIL D Discipline

- All `**[HAZARD-ELEVATED]**` tasks must complete and pass before moving to non-elevated peers
- MC/DC coverage must be measured and reported (UTP-002-A, UTP-009-C)
- HIL system tests (STP-003-A, STP-007-A, STP-012-A, STP-013-A) require EV prototype bench — schedule with hardware availability

---

## Notes

- [P] tasks = different files, no dependencies — safe to run concurrently
- [USN] label maps task to user story for sprint tracking
- `<!-- traces-to: -->` comments link each task to V-Model IDs for audit
- `**[HAZARD-ELEVATED]**` tasks must complete first within their phase (ISO 26262 risk priority)
- HAZ verification tasks (T061–T081) are a mandatory ASIL D gate — all 21 must pass before field trials
- TDD order per REQ-011 is non-negotiable — write tests, confirm they FAIL, then implement
- Stop at each phase checkpoint to validate story independently before proceeding
- Commit after each task or logical group

## V-Model Trace Summary

| Metric | Value |
|--------|-------|
| Total tasks emitted | 86 |
| HAZARD-ELEVATED tasks | 26 |
| Per-HAZ verification tasks | 21 (T061–T081, one per HAZ-001..HAZ-021) |
| Matrices all 100% | A, B, C, D, H — `has_gaps: false` confirmed |
