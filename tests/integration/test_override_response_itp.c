/* Implements: ITP-003-A, ITP-003-B, ITP-004-A, ITP-004-B,
               ITS-003-A1, ITS-003-B1, ITS-004-A1, ITS-004-B1,
               ARCH-003, ARCH-004, SYS-002,
               REQ-002, REQ-003, REQ-012, REQ-013, REQ-NF-001,
               HAZ-004, HAZ-005, HAZ-006 */
/* Integration Tests — Override Response Sequencer + Timing Supervisor
 * ASIL D — Confirmed; SIL mode (platform HAL stubbed)
 */
#include <assert.h>
#include <stdio.h>
#include "../../src/cruise_control/state_machine.h"
#include "../../src/cruise_control/override_response.h"

/* ── ITP-003-A: Interface Contract — Brake_Override Response Sequence ── */
/* Implements: ITP-003-A, ITS-003-A1, HAZ-004 */
static void test_override_sequence_full_success(void)
{
    /* Arrange: state = CRUISE_ACTIVE, all stubs return OK */
    (void)cruise_state_write(CRUISE_ACTIVE);
    BrakeOverrideEvent_t ev = {1000u, 900u, 1u};

    /* Act */
    ErrorCode_t rc = cruise_sequence_override(&ev);

    /* Assert: returns OK; state is now CRUISE_CANCELLED */
    assert(rc == CRUISE_OK);
    assert(cruise_state_read() == CRUISE_CANCELLED);
    printf("  PASS ITS-003-A1: Full override sequence -> CRUISE_OK, Cancelled\n");
}

/* Implements: ITP-003-B, ITS-003-B1 */
static void test_override_null_event_returns_error(void)
{
    (void)cruise_state_write(CRUISE_ACTIVE);
    ErrorCode_t rc = cruise_sequence_override(NULL);
    assert(rc == CRUISE_ERR_NULL_PARAM);
    printf("  PASS ITS-003-B1: NULL event -> CRUISE_ERR_NULL_PARAM\n");
}

/* ── ITP-004-A: Interface Contract — Timer Arm/Stop Within T_MAX ──────── */
/* Implements: ITP-004-A, ITS-004-A1 */
static void test_timing_within_100ms_passes(void)
{
    /* Arrange */
    ErrorCode_t arm_rc = cruise_timing_arm(1000u);
    assert(arm_rc == CRUISE_OK);

    /* Act: stop at 1050ms (50ms elapsed, within 100ms) */
    ErrorCode_t stop_rc = cruise_timing_stop(1050u);

    /* Assert */
    assert(stop_rc == CRUISE_OK);
    printf("  PASS ITS-004-A1: 50ms elapsed -> CRUISE_OK\n");
}

/* Implements: ITP-004-B, ITS-004-B1, HAZ-006 */
static void test_timing_exceeds_100ms_returns_e030(void)
{
    /* Arrange */
    (void)cruise_timing_arm(2000u);

    /* Act: stop at 2105ms (105ms elapsed, over 100ms limit) */
    ErrorCode_t stop_rc = cruise_timing_stop(2105u);

    /* Assert */
    assert(stop_rc == CRUISE_ERR_TIMING_EXCEEDED);
    printf("  PASS ITS-004-B1: 105ms elapsed -> CRUISE_ERR_TIMING_EXCEEDED (HAZ-006)\n");
}

/* BVA boundary: exactly 100ms -> accepted */
static void test_timing_exactly_100ms_passes(void)
{
    (void)cruise_timing_arm(0u);
    ErrorCode_t rc = cruise_timing_stop(100u);
    assert(rc == CRUISE_OK);
    printf("  PASS ITS-004-BVA: Exactly 100ms -> CRUISE_OK\n");
}

/* BVA boundary: 101ms -> rejected */
static void test_timing_101ms_rejected(void)
{
    (void)cruise_timing_arm(0u);
    ErrorCode_t rc = cruise_timing_stop(101u);
    assert(rc == CRUISE_ERR_TIMING_EXCEEDED);
    printf("  PASS ITS-004-BVA+1: 101ms -> CRUISE_ERR_TIMING_EXCEEDED\n");
}

int main(void)
{
    printf("=== Integration Tests: Override Response (ITP-003, ITP-004) ===\n");
    test_override_sequence_full_success();
    test_override_null_event_returns_error();
    test_timing_within_100ms_passes();
    test_timing_exceeds_100ms_returns_e030();
    test_timing_exactly_100ms_passes();
    test_timing_101ms_rejected();
    printf("=== ALL PASS (ITP-003-A, ITP-003-B, ITP-004-A, ITP-004-B) ===\n");
    return 0;
}
