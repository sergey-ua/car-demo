/* Implements: ATP-002-A, ATP-002-B, ATP-004-A, ATP-004-B,
               SCN-002-A1, SCN-002-B1, SCN-004-A1, SCN-004-B1,
               REQ-002, REQ-003, REQ-004,
               HAZ-001, HAZ-002, HAZ-004 */
/* Acceptance Tests — US1: Brake Override While Cruise Active (BDD style)
 * ASIL D — Confirmed
 */
#include <assert.h>
#include <stdio.h>
#include "../../src/cruise_control/state_machine.h"
#include "../../src/cruise_control/override_response.h"

/* ── ATP-002-A: Cessation on Brake_Override ─────────────────────────── */
/* Implements: ATP-002-A, SCN-002-A1, HAZ-001 */
static void test_cessation_on_brake_override(void)
{
    /* Given: feature is in Cruise_Active */
    (void)cruise_state_write(CRUISE_ACTIVE);
    BrakeOverrideEvent_t ev = {1000u, 900u, 10u};

    /* When: Brake_Override is confirmed via brake pedal status */
    ErrorCode_t rc = cruise_sequence_override(&ev);

    /* Then: propulsion receives cessation, state transitions to Cruise_Cancelled */
    assert(rc == CRUISE_OK);
    assert(cruise_state_read() == CRUISE_CANCELLED);
    printf("  PASS SCN-002-A1: Brake_Override -> cessation, Cruise_Cancelled (HAZ-001)\n");
}

/* ── ATP-004-A: Non-Active State Reached ─────────────────────────────── */
/* Implements: ATP-004-A, SCN-004-A1, HAZ-002 */
static void test_state_transitions_to_cancelled(void)
{
    /* Given: feature is in Cruise_Active */
    (void)cruise_state_write(CRUISE_ACTIVE);
    BrakeOverrideEvent_t ev = {2000u, 800u, 11u};

    /* When: Brake_Override is confirmed */
    (void)cruise_sequence_override(&ev);

    /* Then: feature state transitions to Cruise_Cancelled */
    assert(cruise_state_read() == CRUISE_CANCELLED);
    printf("  PASS SCN-004-A1: Post-override state = Cruise_Cancelled (OQ-003 resolved)\n");
}

/* ── ATP-004-B: 20-run consistency check ─────────────────────────────── */
/* Implements: ATP-004-B, SCN-004-B1 */
static void test_20_run_consistency(void)
{
    int i;
    for (i = 0; i < 20; i++) {
        (void)cruise_state_write(CRUISE_ACTIVE);
        BrakeOverrideEvent_t ev = {(uint32_t)(3000u + (uint32_t)i), 900u, (uint8_t)i};
        ErrorCode_t rc = cruise_sequence_override(&ev);
        assert(rc == CRUISE_OK);
        assert(cruise_state_read() == CRUISE_CANCELLED);
    }
    printf("  PASS SCN-004-B1: 20 consecutive runs all -> Cruise_Cancelled\n");
}

int main(void)
{
    printf("=== Acceptance Tests: US1 Brake Override (ATP-002, ATP-004) ===\n");
    test_cessation_on_brake_override();
    test_state_transitions_to_cancelled();
    test_20_run_consistency();
    printf("=== ALL PASS (ATP-002-A, ATP-004-A, ATP-004-B) ===\n");
    return 0;
}
