# Research: Vehicle Cruise Control — Brake Override Safety Slice

<!-- v-model:traces version: v0.7.0 -->

**Branch**: `001-cruise-brake-override` | **Phase 0 output** | **Date**: 2026-06-02

## Derived Requirements

Derived requirements and derived modules identified during V-Model decomposition.
All items were reviewed and resolved before design baseline.

### DR-001 — Activation Precondition Registry (RESOLVED)

**Source artifact**: `v-model/system-design.md` (SYS-001 Dependency View)
**Identified**: System design phase
**Resolution**: Resolved 2026-06-02 via **REQ-018** — five-condition safety minimum:
(1) vehicle speed in valid range, (2) Brake_Override not active, (3) RII not active,
(4) CCF not active, (5) LCU not active.
**Implementation**: MOD-002 (`cruise_check_preconditions`) + ARCH-001 (reactivation guard)

### DR-002 — Write-Failure Handling for Event Logger (RESOLVED)

**Source artifact**: `v-model/system-design.md` (SYS-006 Data Design View)
**Identified**: Architecture design phase
**Resolution**: Resolved 2026-06-02 via **REQ-017** — write failure raises
`Cruise_Control_Fault` → `Cruise_Fault`. No silent degradation.
**Implementation**: MOD-013 (`cruise_write_event_record`) + ARCH-012

## Open Questions Resolved

All 7 original open questions from `spec.md` are resolved:

| OQ | Decision | Date |
|----|----------|------|
| OQ-001 ASIL classification | ASIL D confirmed | 2026-06-02 |
| OQ-002 Timing budget | T_max = 100 ms | 2026-06-02 |
| OQ-003 Non-active target state | Cruise_Cancelled | 2026-06-02 |
| OQ-004 Component architecture | Defined in architecture-design.md (16 ARCH modules) | 2026-06-02 |
| OQ-005 Language and coding standard | C + MISRA C:2012 | 2026-06-02 |
| OQ-006 X-VERSE scenario config | Deferred to Test Engineer post-baseline | — |
| OQ-007 Event-log retention policy | Oldest-first circular overwrite | 2026-06-02 |

## Remaining Pre-Field-Testing Work Items

| Item | Owner | Blocker? |
|------|-------|----------|
| Cybersecurity threat analysis (REQ-CN-003) — confirm CRC-16+counter covers all attack classes | Safety Engineer / Cybersecurity | No (parallel work item) |
| X-VERSE scenario configuration (OQ-006) | Test Engineer | No (post-baseline) |
| Per-module WCET analysis | Embedded SW Engineer | No (implementation phase) |
