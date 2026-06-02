/* Implements: REQ-013, REQ-014, REQ-015, REQ-017, REQ-IF-004,
               ARCH-011, ARCH-012, SYS-006, MOD-012, MOD-013 */
#ifndef EVENT_LOGGER_H
#define EVENT_LOGGER_H

#include "state_machine.h"

typedef struct {
    CruiseControlState_t from_state;
    ControlCondition_t   condition;
    CruiseControlState_t to_state;
    uint16_t             vehicle_spd;
    uint32_t             timestamp;
} EventRecord_t;

/* Implements: REQ-013, REQ-014, ARCH-011, MOD-012 */
ErrorCode_t cruise_build_event_record(const EventRecord_t *req);
/* Implements: REQ-013, REQ-017, ARCH-012, MOD-013 */
ErrorCode_t cruise_write_event_record(void);

#endif /* EVENT_LOGGER_H */
