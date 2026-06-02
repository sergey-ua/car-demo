/* Implements: REQ-001, REQ-004, REQ-005, REQ-007, REQ-008, REQ-010, REQ-011,
               REQ-018, ARCH-001, ARCH-002, SYS-001,
               MOD-001, MOD-002, MOD-003,
               UTP-001-A, UTP-001-B, UTP-002-A, UTP-003-A, UTP-003-B,
               HAZ-001, HAZ-002, HAZ-003 */
/* State Machine — cruise_eval_transition / cruise_check_preconditions / cruise_state_register
 * ASIL D — Confirmed
 * Language: C + MISRA C:2012
 */
#include "state_machine.h"
#include "fault_handler.h"
#include <stdbool.h>

/* ── MOD-003: State Register ──────────────────────────────────────────── */
/* Implements: REQ-001, ARCH-002, MOD-003, UTP-003-A, UTP-003-B */
static CruiseControlState_t g_cruise_state = CRUISE_STANDBY;

/* Private helper — range check (single exit, MISRA C:2012 Rule 15.5) */
static bool cruise_is_valid_state(CruiseControlState_t s)
{
    return ((s == CRUISE_STANDBY)   ||
            (s == CRUISE_ACTIVE)    ||
            (s == CRUISE_SUSPENDED) ||
            (s == CRUISE_CANCELLED) ||
            (s == CRUISE_FAULT));
}

/* Implements: REQ-001, ARCH-002, MOD-003 */
ErrorCode_t cruise_state_write(CruiseControlState_t new_state)
{
    ErrorCode_t result = CRUISE_ERR_RANGE_VIOLATION;  /* E010 default */

    if (cruise_is_valid_state(new_state)) {
        g_cruise_state = new_state;
        result         = CRUISE_OK;
    } else {
        /* Defensive: escalate CCF per ARCH-002 FFI range-violation contract */
        (void)cruise_handle_fault(COND_CCF);
        result = CRUISE_ERR_RANGE_VIOLATION;
    }

    return result;
}

/* Implements: REQ-001, ARCH-002, MOD-003 */
CruiseControlState_t cruise_state_read(void)
{
    return g_cruise_state;
}

/* ── MOD-002: Activation Precondition Check ───────────────────────────── */
/* Implements: REQ-018, REQ-005, ARCH-001, MOD-002, UTP-002-A, HAZ-003 */
bool cruise_check_preconditions(const PreconditionFlags_t *flags)
{
    bool result = false;

    if (flags == NULL) {
        result = false;
    } else {
        bool speed_ok  = flags->speed_in_valid_range;
        bool brake_ok  = !flags->brake_override_active;
        bool rii_ok    = !flags->rii_active;
        bool ccf_ok    = !flags->ccf_active;
        bool lcu_ok    = !flags->lcu_active;
        result = speed_ok && brake_ok && rii_ok && ccf_ok && lcu_ok;
    }

    return result;
}

/* ── MOD-001: State Transition Evaluator ─────────────────────────────── */
/* Implements: REQ-001, REQ-004, REQ-007, REQ-008, REQ-010, REQ-011,
               ARCH-001, MOD-001, UTP-001-A, UTP-001-B, HAZ-001, HAZ-002 */
ErrorCode_t cruise_eval_transition(ControlCondition_t condition)
{
    ErrorCode_t          result    = CRUISE_ERR_INVALID_CONDITION;
    CruiseControlState_t current   = cruise_state_read();
    CruiseControlState_t next_st   = current;
    bool                 permitted = false;

    if ((condition < CONDITION_MIN) || (condition > CONDITION_MAX)) {
        result = CRUISE_ERR_INVALID_CONDITION;  /* E001 */
    } else {
        switch (current) {
            case CRUISE_ACTIVE:
                if      (condition == COND_BRAKE_OVERRIDE) { next_st = CRUISE_CANCELLED; permitted = true; }
                else if (condition == COND_LCU)            { next_st = CRUISE_CANCELLED; permitted = true; }
                else if (condition == COND_RII)            { next_st = CRUISE_CANCELLED; permitted = true; }
                else if (condition == COND_CCF)            { next_st = CRUISE_FAULT;     permitted = true; }
                else                                       { permitted = false; }
                break;
            case CRUISE_STANDBY:
                if (condition == COND_CCF) { next_st = CRUISE_FAULT; permitted = true; }
                break;
            case CRUISE_CANCELLED:
            case CRUISE_SUSPENDED:
                if (condition == COND_CCF) { next_st = CRUISE_FAULT; permitted = true; }
                break;
            case CRUISE_FAULT:
                permitted = false;
                break;
            default:
                result = CRUISE_ERR_INVALID_TRANSITION;
                break;
        }

        if (permitted) {
            result = cruise_state_write(next_st);
        } else if (result == CRUISE_ERR_INVALID_CONDITION) {
            /* result already set — do nothing */
        } else {
            result = CRUISE_ERR_INVALID_TRANSITION;  /* E002 */
        }
    }

    return result;
}
