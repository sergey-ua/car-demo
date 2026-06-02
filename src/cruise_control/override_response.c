/* Implements: REQ-002, REQ-003, REQ-004, REQ-012, REQ-013, REQ-NF-001,
               ARCH-003, ARCH-004, SYS-002, MOD-004, MOD-005,
               UTP-004-A, UTP-005-A, UTP-005-B, HAZ-004, HAZ-005, HAZ-006 */
/* Override Response Sequencer + Timing Supervisor
 * ASIL B(D) each (decomposition of SYS-002, OQ-001 resolved 2026-06-02)
 * Language: C + MISRA C:2012
 */
#include "override_response.h"
#include "speed_controller.h"
#include "event_logger.h"
#include "notification.h"
#include "fault_handler.h"

/* ── MOD-005: Timing Supervisor ──────────────────────────────────────── */
/* Implements: REQ-NF-001, ARCH-004, MOD-005 */
static bool     g_timer_armed   = false;
static uint32_t g_start_time_ms = 0u;
#define T_MAX_MS (100u)  /* OQ-002 resolved 2026-06-02 */

ErrorCode_t cruise_timing_arm(uint32_t start_time_ms)
{
    ErrorCode_t result = CRUISE_ERR_ALREADY_ARMED;
    if (!g_timer_armed) {
        g_start_time_ms = start_time_ms;
        g_timer_armed   = true;
        result          = CRUISE_OK;
    }
    return result;
}

ErrorCode_t cruise_timing_stop(uint32_t stop_time_ms)
{
    ErrorCode_t result = CRUISE_ERR_NOT_ARMED;
    if (g_timer_armed) {
        uint32_t elapsed = stop_time_ms - g_start_time_ms;
        g_timer_armed = false;
        result = (elapsed <= T_MAX_MS) ? CRUISE_OK : CRUISE_ERR_TIMING_EXCEEDED;
    }
    return result;
}

ErrorCode_t cruise_timing_check(uint32_t current_time_ms)
{
    ErrorCode_t result = CRUISE_OK;
    if (g_timer_armed) {
        uint32_t elapsed = current_time_ms - g_start_time_ms;
        if (elapsed > T_MAX_MS) {
            g_timer_armed = false;
            result = CRUISE_ERR_TIMING_EXCEEDED;
            (void)cruise_handle_fault(COND_CCF);
        }
    }
    return result;
}

/* ── MOD-004: Override Response Sequencer ───────────────────────────── */
/* Implements: REQ-002, REQ-003, REQ-012, REQ-013, ARCH-003, MOD-004 */
ErrorCode_t cruise_sequence_override(const BrakeOverrideEvent_t *event)
{
    ErrorCode_t result = CRUISE_ERR_NULL_PARAM;

    if (event == NULL) {
        result = CRUISE_ERR_NULL_PARAM;
    } else if (cruise_state_read() != CRUISE_ACTIVE) {
        result = CRUISE_ERR_INVALID_TRANSITION;
    } else {
        /* Step 1: cessation */
        result = cruise_propulsion_write(CMD_CESSATION, 0u);

        /* Step 2: state transition */
        if (result == CRUISE_OK) {
            result = cruise_eval_transition(COND_BRAKE_OVERRIDE);
        }

        /* Step 3: persist event record */
        if (result == CRUISE_OK) {
            EventRecord_t rec;
            rec.from_state  = CRUISE_ACTIVE;
            rec.condition   = COND_BRAKE_OVERRIDE;
            rec.to_state    = cruise_state_read();
            rec.vehicle_spd = event->vehicle_speed_snapshot;
            rec.timestamp   = event->timestamp;
            result = cruise_build_event_record(&rec);
        }
        if (result == CRUISE_OK) {
            result = cruise_write_event_record();
        }

        /* Step 4: notify */
        if (result == CRUISE_OK) {
            NotificationRequest_t notif;
            notif.event_id   = event->event_id;
            notif.event_type = NOTIF_BRAKE_OVERRIDE;
            result = cruise_publish_notification(&notif);
        }

        if (result != CRUISE_OK) {
            (void)cruise_handle_fault(COND_CCF);
        }
    }
    return result;
}
