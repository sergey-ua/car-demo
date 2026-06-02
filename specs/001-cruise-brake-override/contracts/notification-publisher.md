# Contract: notification-publisher

<!-- v-model:traces source: v-model/architecture-design.md Interface View; version: v0.7.0 -->

**Module**: ARCH-013 (Notification Publisher / MOD-014)
**Platform Interface**: platform notification interface
**ASIL**: ASIL D — Confirmed
**Date**: 2026-06-02

## Function Signature

`cruise_publish_notification(const NotificationRequest_t *req)`

## Return Value

ErrorCode_t — CRUISE_OK or CRUISE_ERR_DELIVERY_FAILURE (E100)

## Pre-conditions

req must be non-NULL; deduplication by event_id (exactly-once)

## Error Contracts

CRUISE_ERR_NULL_PARAM or CRUISE_ERR_DELIVERY_FAILURE (diagnostic only)

## Notes

Refer to `v-model/architecture-design.md` Interface View and `v-model/integration-test.md`
for the full interface contract test cases (ITP) that exercise this boundary.
