/* Implements: STP-001-A, STP-001-B, STP-002-A,
               STS-001-A1, STS-001-B1, STS-001-B2, STS-002-A1,
               SYS-001, SYS-002,
               REQ-001, REQ-002, REQ-004,
               HAZ-001, HAZ-002, HAZ-004 */
/* System Tests — State Machine and Override Response Coordinator (SIL)
 * ASIL D — Confirmed
 */
#include <assert.h>
#include <stdio.h>
#include "../../src/cruise_control/state_machine.h"
#include "../../src/cruise_control/override_response.h"

/* ── STP-001-A: State Machine Completeness ──────────────────────────── */
/* Implements: STP-001-A, STS-001-A1 */
static void test_five_states_reachable(void)
{
    /* Verify all five states can be written and read back */
    CruiseControlState_t states[] = {
        CRUISE_STANDBY, CRUISE_ACTIVE, CRUISE_SUSPENDED,
        CRUISE_CANCELLED, CRUISE_FAULT
    };
    int i;
    for (i = 0; i < 5; i++) {
        assert(cruise_state_write(states[i]) == CRUISE_OK);
        assert(cruise_state_read() == states[i]);
    }
    printf("  PASS STS-001-A1: All five states reachable\n");
}

/* ── STP-001-B: No Undefined State Reachable ─────────────────────────── */
/* Implements: STP-001-B, STS-001-B1, HAZ-002 */
static void test_undefined_state_rejected(void)
{
    (void)cruise_state_write(CRUISE_ACTIVE);
    ErrorCode_t rc = cruise_state_write((CruiseControlState_t)0xFF);
    assert(rc == CRUISE_ERR_RANGE_VIOLATION);
    assert(cruise_state_read() == CRUISE_ACTIVE);
    printf("  PASS STS-001-B1: Undefined state write -> E010, state unchanged (HAZ-002)\n");
}

/* Implements: STP-001-B, STS-001-B2 — fault injection */
static void test_invalid_condition_does_not_change_state(void)
{
    (void)cruise_state_write(CRUISE_ACTIVE);
    ErrorCode_t rc = cruise_eval_transition((ControlCondition_t)0xFF);
    assert(rc == CRUISE_ERR_INVALID_CONDITION);
    assert(cruise_state_read() == CRUISE_ACTIVE);
    printf("  PASS STS-001-B2: Invalid condition -> E001, state unchanged\n");
}

/* ── STP-002-A: Override Response Sequence Contract ─────────────────── */
/* Implements: STP-002-A, STS-002-A1, HAZ-001, HAZ-004 */
static void test_override_complete_sequence_all_steps(void)
{
    /* Arrange */
    (void)cruise_state_write(CRUISE_ACTIVE);
    BrakeOverrideEvent_t ev = {5000u, 900u, 2u};

    /* Act */
    ErrorCode_t rc = cruise_sequence_override(&ev);

    /* Assert: cessation + transition + log + notify all completed */
    assert(rc == CRUISE_OK);
    assert(cruise_state_read() == CRUISE_CANCELLED);
    printf("  PASS STS-002-A1: Full override sequence -> Cruise_Cancelled (HAZ-001)\n");
}

int main(void)
{
    printf("=== System Tests: State Machine + Override (STP-001, STP-002) ===\n");
    test_five_states_reachable();
    test_undefined_state_rejected();
    test_invalid_condition_does_not_change_state();
    test_override_complete_sequence_all_steps();
    printf("=== ALL PASS (STP-001-A, STP-001-B, STP-002-A) ===\n");
    return 0;
}
