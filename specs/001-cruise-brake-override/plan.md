# Implementation Plan: Vehicle Cruise Control — Brake Override Safety Slice

<!-- v-model:traces
  requirements: [REQ-001, REQ-002, REQ-003, REQ-004, REQ-005, REQ-006, REQ-007, REQ-008, REQ-009, REQ-010, REQ-011, REQ-012, REQ-013, REQ-014, REQ-015, REQ-016, REQ-017, REQ-018, REQ-NF-001, REQ-NF-002, REQ-IF-001, REQ-IF-002, REQ-IF-003, REQ-IF-004, REQ-IF-005, REQ-IF-006, REQ-IF-007, REQ-CN-001, REQ-CN-002, REQ-CN-003, REQ-CN-004, REQ-CN-005, REQ-CN-006, REQ-CN-007]
  system:       [SYS-001, SYS-002, SYS-003, SYS-004, SYS-005, SYS-006, SYS-007, SYS-008]
  architecture: [ARCH-001, ARCH-002, ARCH-003, ARCH-004, ARCH-005, ARCH-006, ARCH-007, ARCH-008, ARCH-009, ARCH-010, ARCH-011, ARCH-012, ARCH-013, ARCH-014, ARCH-015, ARCH-016]
  modules:      [MOD-001, MOD-002, MOD-003, MOD-004, MOD-005, MOD-006, MOD-007, MOD-008, MOD-009, MOD-010, MOD-011, MOD-012, MOD-013, MOD-014, MOD-015, MOD-016, MOD-017]
  hazards:      [HAZ-001, HAZ-002, HAZ-003, HAZ-004, HAZ-005, HAZ-006, HAZ-007, HAZ-008, HAZ-009, HAZ-010, HAZ-011, HAZ-012, HAZ-013, HAZ-014, HAZ-015, HAZ-016, HAZ-017, HAZ-018, HAZ-019, HAZ-020, HAZ-021]
  version:      v0.7.0
-->

**Branch**: `001-cruise-brake-override` | **Date**: 2026-06-02 | **Spec**: [spec.md](spec.md)

**Input**: Feature specification from `specs/001-cruise-brake-override/spec.md`

## Summary

The Brake Override Safety Slice is a safety-critical embedded software feature for Asteria
Mobility Labs' EV prototype. It implements a deterministic five-state cruise control machine
that, when the driver applies the brake while cruise is active, immediately ceases longitudinal
speed control, releases authority to the driver, transitions to `Cruise_Cancelled`, persists a
five-field audit record, and dispatches a notification — all within 100 ms. The feature also
handles three secondary fault conditions (`Longitudinal_Control_Unavailable`,
`Cruise_Control_Fault`, `Required_Input_Invalid`) and enforces a five-condition reactivation
guard before cruise may be re-engaged. The implementation is 17 C modules, ASIL D–confirmed,
MISRA C:2012-compliant, with no dynamic memory allocation and a ≤ 20 ms cyclic execution period.

## Technical Context

**Language/Version**: C (C99 conformant subset per MISRA C:2012) — OQ-005 resolved 2026-06-02

**Primary Dependencies**: EV prototype platform HAL (7 interfaces: vehicle speed, brake pedal
status, driver command, propulsion command, diagnostic status, notification, local event storage);
no third-party middleware or RTOS selected (REQ-CN-005 — toolchain must support static analysis
and MISRA rule enforcement)

**Storage**: Local persistent event storage provided by platform HAL (ARCH-012 / REQ-IF-004);
oldest-first circular overwrite policy (OQ-007 resolved); no heap allocation (ISO 26262-6
§8.4.5 Table 13)

**Testing**: Four-tier test plan: acceptance (58 ATPs / 59 SCNs), system (24 STPs / 30 STSs),
integration (30 ITPs / 32 ITSs), unit (25 UTPs / 64 UTSs); X-VERSE downstream virtual
validation (REQ-CN-006 / OQ-006 — Test Engineer to configure)

**Target Platform**: Automotive embedded ECU on the existing EV prototype (no new hardware
interfaces — REQ-CN-001); cyclic executive with ≤ 20 ms scheduling period (ARCH-015)

**Project Type**: embedded-safety-critical-library

**Performance Goals**: T_max ≤ 100 ms from Brake_Override detection to confirmed cessation
(REQ-NF-001; OQ-002 resolved 2026-06-02); scheduling period ≤ 20 ms = T_max ÷ 5

**Constraints**:
- No dynamic memory allocation after initialisation
- MISRA C:2012 mandatory ruleset; cyclomatic complexity ≤ 10 per function (ASIL D)
- ASIL D confirmed (OQ-001 resolved 2026-06-02); ASIL B(D)+B(D) decomposition for
  state machine (ARCH-001/002) and override coordinator (ARCH-003/004)
- All loops must have provable worst-case iteration count
- Single entry/exit per function (MISRA C:2012 Rule 15.5)
- Platform HAL abstraction mandatory for all hardware reads/writes

**Scale/Scope**: 17 C source modules across 9 translation units; 34 active requirements
(all open questions resolved); 21 hazards (6 ASIL D in primary safety chain)

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

> **Pre-design gate**: The project constitution file (`.specify/memory/constitution.md`) is
> a blank template — no project-specific principles have been ratified. The following ISO
> 26262 / embedded safety principles from the V-Model artifacts serve as the functional
> equivalent for this feature:

| Principle | Source | Status |
|-----------|--------|--------|
| Safety-first decomposition: state machine, override responder, speed controller separated | REQ-CN-004 / ARCH design | ✅ Applied |
| No dynamic allocation | ISO 26262-6 §8.4.5 / REQ-NF (implicit) | ✅ Applied |
| MISRA C:2012 — single entry/exit, bounded loops | REQ-CN-005 | ✅ Applied |
| Test-first (TDD order enforced by `/speckit.v-model.tasks`) | Downstream task bridge | ✅ Deferred to tasks |
| No new hardware interfaces beyond existing platform | REQ-CN-001 | ✅ Applied |
| All open questions resolved before design baseline | OQ-001 through OQ-007 | ✅ All resolved |
| Evidence package complete before field testing | REQ-CN-002, HAZ-020 | ✅ All artifacts present |

*Post-design gate (re-check after Phase 1):* All five sibling artifacts produced; schema
validation passed; no derived requirements flagged as unresolved; all ARCH modules have
interface contracts. ✅ PASS.

## Project Structure

### Documentation (this feature)

```text
specs/001-cruise-brake-override/
├── plan.md              # This file (v-model bridge output)
├── research.md          # Phase 0 — derived requirements and open items
├── data-model.md        # Phase 1 — data entities from system design
├── quickstart.md        # Phase 1 — top BDD acceptance scenarios
├── contracts/           # Phase 1 — external interface contracts per ARCH module
│   ├── propulsion-interface.md    # ARCH-006
│   ├── platform-input-reader.md   # ARCH-007
│   ├── event-storage-adapter.md   # ARCH-012
│   └── notification-publisher.md  # ARCH-013
├── spec.md
├── checklists/
│   └── requirements.md
├── v-model/
│   ├── requirements.md
│   ├── hazard-analysis.md
│   ├── system-design.md
│   ├── system-test.md
│   ├── architecture-design.md
│   ├── integration-test.md
│   ├── module-design.md
│   ├── unit-test.md
│   ├── acceptance-plan.md
│   └── traceability-matrix.md
└── tasks.md             # Phase 2 output (/speckit-v-model-tasks command)
```

### Source Code (repository root)

```text
src/
├── cruise_control/
│   ├── state_machine.h          # MOD-001 (cruise_eval_transition)
│   ├── state_machine.c          # MOD-001, MOD-002 (preconditions), MOD-003 (state register)
│   ├── override_response.h      # MOD-004 (override sequencer)
│   ├── override_response.c      # MOD-004, MOD-005 (timing supervisor)
│   ├── speed_controller.h       # MOD-006 (speed regulator)
│   ├── speed_controller.c       # MOD-006, MOD-007 (propulsion adapter)
│   ├── input_monitor.h          # MOD-008 (input reader)
│   ├── input_monitor.c          # MOD-008, MOD-009 (integrity verifier), MOD-010 (condition deriver)
│   ├── fault_handler.h          # MOD-011 (fault condition handler)
│   ├── fault_handler.c          # MOD-011
│   ├── event_logger.h           # MOD-012 (event record builder)
│   └── event_logger.c           # MOD-012, MOD-013 (storage adapter), MOD-014 (notification publisher)
├── platform/
│   ├── scheduling.h             # MOD-016 (scheduling controller)
│   ├── scheduling.c             # MOD-016, MOD-017 (watchdog supervisor)
│   ├── watchdog.h
│   └── watchdog.c
└── platform_hal.h               # Platform HAL abstraction layer (stubs provided by platform)

tests/
├── acceptance/                  # ATP-NNN-X / SCN-NNN-X# — acceptance test runners
├── system/                      # STP-NNN-X / STS-NNN-X# — system test runners
├── integration/                 # ITP-NNN-X / ITS-NNN-X# — integration test runners (mock HAL)
└── unit/                        # UTP-NNN-X / UTS-NNN-X# — unit test runners (stub dependencies)

docs/
└── lifecycle/
    └── evidence_manifest.md     # MOD-015 — ASIL D lifecycle artifact inventory
```

**Structure Decision**: Single-project embedded C layout. No frontend, no backend split.
`src/cruise_control/` contains the seven safety-critical translation units; `src/platform/`
contains the two cross-cutting scheduling and watchdog modules. `tests/` mirrors the
four-tier V-Model test plan hierarchy.

## Complexity Tracking

> **Fill ONLY if Constitution Check has violations that must be justified**

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| ASIL B(D)+B(D) decomposition of SYS-001 into ARCH-001+002 | Required for ISO 26262-9 §5 ASIL D evidence; spatial independence argument reduces per-module rigor | Monolithic ASIL D implementation is acceptable but doubles the verification cost for the state machine — decomposition is the documented simplification for this level |
| ASIL B(D)+B(D) decomposition of SYS-002 into ARCH-003+004 | Functional independence: timing supervisor (ARCH-004) independently monitors the 100 ms budget without participating in the override sequence | Without ARCH-004, the timing constraint (REQ-NF-001 / HAZ-006) would require ARCH-003 to self-monitor — increasing its cyclomatic complexity above the ≤ 10 limit |
| Two cross-cutting modules (ARCH-015 scheduling, ARCH-016 watchdog) not traceable to a single SYS | Required by ASIL D temporal isolation (ISO 26262-6 §7.4.4); scheduling period ≤ 20 ms must be enforced by dedicated infrastructure | Without these modules, temporal isolation cannot be independently verified or monitored — the ASIL D freedom-from-interference argument would fail |

## V-Model Trace Summary

| Matrix | Chain | Coverage |
|--------|-------|---------|
| A — Validation | REQ → ATP → SCN | 34/34 REQs · 58 ATPs · 59 SCNs (100%) |
| B — Verification | REQ → SYS → STP → STS | 8/8 SYS · 24 STPs · 30 STSs (100%) |
| C — Integration | SYS → ARCH → ITP → ITS | 16/16 ARCH · 30 ITPs · 32 ITSs (100%) |
| D — Implementation | ARCH → MOD → UTP → UTS | 17/17 MODs · 25 UTPs · 64 UTSs (100%) |
| H — Hazard | HAZ → REQ/SYS → ATP/STP | 21/21 HAZs (100%) |

**ASIL D safety chain** (HAZ-001/002/004/006/007/009): REQ-002 → SYS-002/003 → ARCH-003/006 → MOD-004/007 → ITP-003-A/006-A → UTP-004-A/007-A.
All open questions resolved; evidence package complete for pre-field-testing review.
