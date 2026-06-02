# Contract: event-storage-adapter

<!-- v-model:traces source: v-model/architecture-design.md Interface View; version: v0.7.0 -->

**Module**: ARCH-012 (Event Storage Adapter / MOD-013)
**Platform Interface**: platform local persistent event storage
**ASIL**: ASIL D — Confirmed
**Date**: 2026-06-02

## Function Signature

`cruise_write_event_record(void)`

## Return Value

ErrorCode_t — CRUISE_OK or CRUISE_ERR_WRITE_FAILURE (E090)

## Pre-conditions

MOD-012 must have built the record first (g_record_ready == true)

## Error Contracts

CRUISE_ERR_FIELD_VALIDATION if no record ready; CRUISE_ERR_WRITE_FAILURE escalates CCF per REQ-017

## Notes

Refer to `v-model/architecture-design.md` Interface View and `v-model/integration-test.md`
for the full interface contract test cases (ITP) that exercise this boundary.
