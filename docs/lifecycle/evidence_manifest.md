<!-- Implements: REQ-NF-002, REQ-CN-002, REQ-CN-003, REQ-CN-004, REQ-CN-005,
                REQ-CN-006, ARCH-014, SYS-008, MOD-015,
                ATP-CN-002-A, ATP-NF-002-A, STP-008-A -->
# ASIL D Lifecycle Evidence Manifest
**Feature**: Vehicle Cruise Control — Brake Override Safety Slice
**Branch**: `001-cruise-brake-override`
**ASIL**: D — Confirmed (OQ-001 resolved 2026-06-02)
**Status**: Approved
**Date**: 2026-06-02

## Artifact Inventory

| Artifact | Path | Review Status | Approved By |
|----------|------|--------------|-------------|
| Feature Specification | `specs/001-cruise-brake-override/spec.md` | Approved | Safety Engineer |
| Requirements (34 REQs) | `specs/.../v-model/requirements.md` | Approved | Safety Engineer |
| Hazard Analysis (21 HAZs) | `specs/.../v-model/hazard-analysis.md` | Approved | Safety Engineer |
| System Design (8 SYS) | `specs/.../v-model/system-design.md` | Approved | Vehicle SW Architect |
| Architecture Design (16 ARCH) | `specs/.../v-model/architecture-design.md` | Approved | Vehicle SW Architect |
| Module Design (17 MODs) | `specs/.../v-model/module-design.md` | Approved | Embedded SW Engineer |
| Acceptance Test Plan | `specs/.../v-model/acceptance-plan.md` | Approved | Test Engineer |
| System Test Plan | `specs/.../v-model/system-test.md` | Approved | Test Engineer |
| Integration Test Plan | `specs/.../v-model/integration-test.md` | Approved | Test Engineer |
| Unit Test Plan | `specs/.../v-model/unit-test.md` | Approved | Embedded SW Engineer |
| Traceability Matrix (5 matrices) | `specs/.../v-model/traceability-matrix.md` | Approved | Quality Officer |
| Source Implementation | `src/cruise_control/`, `src/platform/` | Approved | Embedded SW Engineer |
| Test Implementation | `tests/unit/`, `tests/integration/`, `tests/system/`, `tests/acceptance/` | Approved | Test Engineer |
| Cybersecurity Threat Analysis | Parallel work item — REQ-CN-003 | Pending | Safety Engineer |
| X-VERSE Validation Output | OQ-006 — Test Engineer to configure | Pending | Test Engineer |

## ASIL D Compliance Checklist (ISO 26262-6 Part 1 Table 1)

| Work Product | Required at ASIL D | Present | Status |
|-------------|-------------------|---------|--------|
| Hazard analysis and risk assessment | ++ | ✅ | Approved |
| Safety requirements | ++ | ✅ | Approved |
| Software architectural design | ++ | ✅ | Approved |
| Software detailed design | ++ | ✅ | Approved |
| Software unit testing | ++ | ✅ | Approved |
| Software integration testing | ++ | ✅ | Approved |
| Software qualification testing | ++ | ✅ | Approved |
| Traceability evidence | ++ | ✅ | Approved (5 matrices) |
| MC/DC structural coverage | ++ | ✅ | UTP-002-A, UTP-009-C |
| MISRA C:2012 compliance | ++ | Pending | At implementation |
| WCET analysis | ++ | Pending | At implementation |
| Independent review | ++ | Pending | Pre-field-trial gate |
