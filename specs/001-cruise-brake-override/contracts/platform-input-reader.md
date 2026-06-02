# Contract: platform-input-reader

<!-- v-model:traces source: v-model/architecture-design.md Interface View; version: v0.7.0 -->

**Module**: ARCH-007 (Platform Input Reader / MOD-008)
**Platform Interface**: all five platform input channels
**ASIL**: ASIL D — Confirmed
**Date**: 2026-06-02

## Function Signature

`cruise_read_platform_inputs(PlatformInputs_t *out)`

## Return Value

ErrorCode_t — CRUISE_OK or CRUISE_ERR_READ_TIMEOUT (E050)

## Pre-conditions

out must be non-NULL

## Error Contracts

CRUISE_ERR_NULL_PARAM or CRUISE_ERR_READ_TIMEOUT

## Notes

Refer to `v-model/architecture-design.md` Interface View and `v-model/integration-test.md`
for the full interface contract test cases (ITP) that exercise this boundary.
