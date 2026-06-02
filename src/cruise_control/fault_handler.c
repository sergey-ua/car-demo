/* Implements: REQ-006, REQ-007, REQ-008, REQ-009, REQ-010, REQ-011,
               ARCH-010, SYS-005, MOD-011,
               UTP-011-A, HAZ-013, HAZ-014, HAZ-015 */
/* Fault and Condition Handler
 * ASIL D — Confirmed
 * Language: C + MISRA C:2012
 */
#include "fault_handler.h"
#include "speed_controller.h"

/* Implements: REQ-006..REQ-011, ARCH-010, MOD-011 */
ErrorCode_t cruise_handle_fault(ControlCondition_t condition)
{
    ErrorCode_t result    = CRUISE_ERR_INVALID_CONDITION;
    bool        needs_cess = false;

    switch (condition) {
        case COND_LCU:
            needs_cess = (cruise_state_read() == CRUISE_ACTIVE);
            result     = CRUISE_OK;
            break;
        case COND_CCF:
            needs_cess = (cruise_state_read() == CRUISE_ACTIVE);
            result     = CRUISE_OK;
            break;
        case COND_RII:
            needs_cess = (cruise_state_read() == CRUISE_ACTIVE);
            result     = CRUISE_OK;
            break;
        default:
            result = CRUISE_ERR_INVALID_CONDITION;
            break;
    }

    if (result == CRUISE_OK) {
        if (needs_cess) {
            (void)cruise_propulsion_write(CMD_CESSATION, 0u);
        }
        ErrorCode_t trans = cruise_eval_transition(condition);
        if (trans != CRUISE_OK) {
            if (condition != COND_CCF) {
                result = CRUISE_ERR_TRANSITION_REJECTED;  /* E070 */
                (void)cruise_state_write(CRUISE_FAULT);   /* backstop */
            }
        }
    }

    return result;
}
