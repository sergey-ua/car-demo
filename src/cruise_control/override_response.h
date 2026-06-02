/* Implements: REQ-002, REQ-003, REQ-012, REQ-013, REQ-NF-001,
               ARCH-003, ARCH-004, SYS-002, MOD-004, MOD-005 */
#ifndef OVERRIDE_RESPONSE_H
#define OVERRIDE_RESPONSE_H

#include "state_machine.h"

typedef struct {
    uint32_t timestamp;
    uint16_t vehicle_speed_snapshot;
    uint8_t  event_id;
} BrakeOverrideEvent_t;

/* Implements: REQ-002..REQ-013, ARCH-003, MOD-004 */
ErrorCode_t cruise_sequence_override(const BrakeOverrideEvent_t *event);

/* Implements: REQ-NF-001, ARCH-004, MOD-005 */
ErrorCode_t cruise_timing_arm(uint32_t start_time_ms);
ErrorCode_t cruise_timing_stop(uint32_t stop_time_ms);
ErrorCode_t cruise_timing_check(uint32_t current_time_ms);

#endif /* OVERRIDE_RESPONSE_H */
