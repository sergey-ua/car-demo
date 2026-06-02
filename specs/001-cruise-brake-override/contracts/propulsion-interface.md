# Contract: propulsion-interface

<!-- v-model:traces source: v-model/architecture-design.md Interface View; version: v0.7.0 -->

**Module**: ARCH-006 (Propulsion Interface Adapter / MOD-007)
**Platform Interface**: platform propulsion command interface
**ASIL**: ASIL D — Confirmed
**Date**: 2026-06-02

## Function Signature

`cruise_propulsion_write(CmdType_t cmd_type, uint16_t cmd_value)`

## Return Value

ErrorCode_t — CRUISE_OK or CRUISE_ERR_WRITE_FAILURE (E040)

## Pre-conditions

CMD_SPEED_CTRL requires Cruise_Active state; CMD_CESSATION is unconditional

## Error Contracts

CRUISE_ERR_NOT_AUTHORISED if CMD_SPEED_CTRL outside Cruise_Active; CRUISE_ERR_WRITE_FAILURE escalates CCF

## Notes

Refer to `v-model/architecture-design.md` Interface View and `v-model/integration-test.md`
for the full interface contract test cases (ITP) that exercise this boundary.
