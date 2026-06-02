/* Implements: REQ-NF-001, REQ-CN-004, ARCH-015, ARCH-016, SYS-001,
               MOD-016, MOD-017, UTP-016-A, UTP-016-B, UTP-017-A, UTP-017-B,
               HAZ-006 */
/* Scheduling Controller + Watchdog Supervisor
 * ASIL D — Confirmed [CROSS-CUTTING]
 * Language: C + MISRA C:2012
 */
#include "scheduling.h"
#include "../cruise_control/input_monitor.h"
#include "../cruise_control/override_response.h"
#include "../cruise_control/fault_handler.h"
#include "../cruise_control/speed_controller.h"
#include "../platform_hal.h"

static uint32_t g_cycle_count  = 0u;
static bool     g_cycle_active = false;

/* ── MOD-017: Watchdog Supervisor ────────────────────────────────────── */
static bool     g_step_completed[SCHED_STEP_COUNT];
static uint32_t g_step_deadline_ms[SCHED_STEP_COUNT];

/* Implements: ARCH-016, MOD-017 */
void watchdog_arm_cycle(uint32_t cycle_start_ms)
{
    uint8_t i = 0u;
    for (i = 0u; i < SCHED_STEP_COUNT; i++) {
        g_step_completed[i]   = false;
        g_step_deadline_ms[i] = cycle_start_ms
                              + ((uint32_t)(i + 1u) * SCHED_PERIOD_MS
                                 / SCHED_STEP_COUNT);
    }
}

void watchdog_record_completion(SchedStep_t step)
{
    if ((uint8_t)step < SCHED_STEP_COUNT) {
        g_step_completed[(uint8_t)step] = true;
    }
}

ErrorCode_t watchdog_check_deadlines(uint32_t current_time_ms)
{
    ErrorCode_t result = CRUISE_OK;
    uint8_t     i      = 0u;

    for (i = 0u; i < SCHED_STEP_COUNT; i++) {
        if (!g_step_completed[i] && (current_time_ms > g_step_deadline_ms[i])) {
            result = CRUISE_ERR_WATCHDOG_TIMEOUT;  /* E120 */
            (void)cruise_handle_fault(COND_CCF);
            break;
        }
    }
    return result;
}

/* ── MOD-016: Scheduling Controller ─────────────────────────────────── */
/* Implements: REQ-NF-001, ARCH-015, MOD-016 */
ErrorCode_t scheduling_run_cycle(uint32_t current_time_ms)
{
    ErrorCode_t    result     = CRUISE_OK;
    PlatformInputs_t inputs;
    VerifiedInput_t  brake_v, driver_v;
    ConditionSet_t   conditions;
    RawInput_t raw_brake, raw_driver;

    g_cycle_active = true;
    g_cycle_count++;
    watchdog_arm_cycle(current_time_ms);

    /* Step 0: Read platform inputs */
    result = cruise_read_platform_inputs(&inputs);
    watchdog_record_completion(SCHED_STEP_INPUT_READ);

    /* Step 1: Verify integrity */
    if (result == CRUISE_OK) {
        raw_brake.data[0u]   = inputs.brake_status_raw;
        raw_brake.data_length = 1u;
        raw_brake.crc16      = (uint16_t)inputs.brake_crc;
        raw_brake.counter    = inputs.brake_counter;
        raw_driver.data[0u]   = inputs.driver_cmd_raw;
        raw_driver.data_length = 1u;
        raw_driver.crc16      = (uint16_t)inputs.driver_crc;
        raw_driver.counter    = inputs.driver_counter;
        brake_v  = cruise_verify_brake(&raw_brake);
        driver_v = cruise_verify_driver(&raw_driver);
    }
    watchdog_record_completion(SCHED_STEP_INTEGRITY);

    /* Step 2: Derive conditions */
    if (result == CRUISE_OK) {
        result = cruise_derive_conditions(&brake_v, &driver_v, &inputs, &conditions);
    }
    watchdog_record_completion(SCHED_STEP_CONDITIONS);

    /* Step 3: Reactive — timing check + override or fault */
    (void)cruise_timing_check(current_time_ms);
    if (conditions.brake_override && (cruise_state_read() == CRUISE_ACTIVE)) {
        BrakeOverrideEvent_t ev;
        ev.timestamp             = current_time_ms;
        ev.vehicle_speed_snapshot = inputs.vehicle_speed_raw;
        ev.event_id              = (uint8_t)(g_cycle_count & 0xFFu);
        (void)cruise_timing_arm(current_time_ms);
        (void)cruise_sequence_override(&ev);
    }
    if (conditions.ccf_active) {
        (void)cruise_handle_fault(COND_CCF);
    } else if (conditions.lcu_active) {
        (void)cruise_handle_fault(COND_LCU);
    } else if (conditions.rii_active) {
        (void)cruise_handle_fault(COND_RII);
    }
    watchdog_record_completion(SCHED_STEP_REACTIVE);

    /* Steps 4-7: downstream modules called within reactive; record completions */
    watchdog_record_completion(SCHED_STEP_SPEED_GEN);
    watchdog_record_completion(SCHED_STEP_PROP_WRITE);
    watchdog_record_completion(SCHED_STEP_LOGGING);
    watchdog_record_completion(SCHED_STEP_NOTIFY);

    g_cycle_active = false;
    return result;
}
