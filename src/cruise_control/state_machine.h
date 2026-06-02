/* Implements: REQ-001, REQ-004, REQ-005, REQ-007, REQ-008, REQ-010, REQ-011,
               REQ-018, ARCH-001, ARCH-002, SYS-001,
               MOD-001, MOD-002, MOD-003 */
/* State Machine — Cruise Control Brake Override Safety Slice
 * ASIL D — Confirmed (ASIL B(D)+B(D) decomposition for MOD-001+MOD-003)
 * Language: C + MISRA C:2012
 */
#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <stdint.h>
#include <stdbool.h>

/* Implements: REQ-001, ARCH-002, MOD-003 */
typedef enum {
    CRUISE_STANDBY   = 0u,
    CRUISE_ACTIVE    = 1u,
    CRUISE_SUSPENDED = 2u,
    CRUISE_CANCELLED = 3u,
    CRUISE_FAULT     = 4u
} CruiseControlState_t;

/* Implements: REQ-016, ARCH-009, MOD-010 */
typedef enum {
    COND_BRAKE_OVERRIDE = 0u,
    COND_RII            = 1u,
    COND_LCU            = 2u,
    COND_CCF            = 3u,
    CONDITION_MIN       = COND_BRAKE_OVERRIDE,
    CONDITION_MAX       = COND_CCF
} ControlCondition_t;

/* Implements: REQ-018, MOD-002 */
typedef struct {
    bool speed_in_valid_range;
    bool brake_override_active;
    bool rii_active;
    bool ccf_active;
    bool lcu_active;
} PreconditionFlags_t;

typedef enum {
    CRUISE_OK                        = 0,
    CRUISE_ERR_INVALID_CONDITION     = 1,  /* E001 */
    CRUISE_ERR_INVALID_TRANSITION    = 2,  /* E002 */
    CRUISE_ERR_RANGE_VIOLATION       = 10, /* E010 */
    CRUISE_ERR_ALREADY_ARMED         = 11,
    CRUISE_ERR_NOT_ARMED             = 12,
    CRUISE_ERR_TIMING_EXCEEDED       = 30, /* E030 */
    CRUISE_ERR_NOT_AUTHORISED        = 31,
    CRUISE_ERR_WRITE_FAILURE         = 40, /* E040 */
    CRUISE_ERR_INTEGRITY_FAULT       = 60, /* E060 */
    CRUISE_ERR_FIELD_VALIDATION      = 80, /* E080 */
    CRUISE_ERR_DELIVERY_FAILURE      = 100,/* E100 */
    CRUISE_ERR_WATCHDOG_TIMEOUT      = 120,/* E120 */
    CRUISE_ERR_NULL_PARAM            = 200,
    CRUISE_ERR_TRANSITION_REJECTED   = 70, /* E070 */
    CRUISE_ERR_READ_TIMEOUT          = 50  /* E050 */
} ErrorCode_t;

/* Implements: REQ-001, ARCH-001, MOD-001 */
ErrorCode_t cruise_eval_transition(ControlCondition_t condition);

/* Implements: REQ-018, REQ-005, ARCH-001, MOD-002 */
bool cruise_check_preconditions(const PreconditionFlags_t *flags);

/* Implements: REQ-001, ARCH-002, MOD-003 */
ErrorCode_t         cruise_state_write(CruiseControlState_t new_state);
CruiseControlState_t cruise_state_read(void);

#endif /* STATE_MACHINE_H */
