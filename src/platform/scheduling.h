/* Implements: REQ-NF-001, REQ-CN-004, ARCH-015, ARCH-016, MOD-016, MOD-017 */
#ifndef SCHEDULING_H
#define SCHEDULING_H

#include "../cruise_control/state_machine.h"

#define SCHED_PERIOD_MS  (20u)   /* T_MAX / 5 — OQ-002 resolved */
#define SCHED_STEP_COUNT (8u)

typedef enum {
    SCHED_STEP_INPUT_READ = 0u,
    SCHED_STEP_INTEGRITY  = 1u,
    SCHED_STEP_CONDITIONS = 2u,
    SCHED_STEP_REACTIVE   = 3u,
    SCHED_STEP_SPEED_GEN  = 4u,
    SCHED_STEP_PROP_WRITE = 5u,
    SCHED_STEP_LOGGING    = 6u,
    SCHED_STEP_NOTIFY     = 7u
} SchedStep_t;

/* Implements: REQ-NF-001, ARCH-015, MOD-016 */
ErrorCode_t scheduling_run_cycle(uint32_t current_time_ms);

/* Implements: ARCH-016, MOD-017 */
void        watchdog_arm_cycle(uint32_t cycle_start_ms);
void        watchdog_record_completion(SchedStep_t step);
ErrorCode_t watchdog_check_deadlines(uint32_t current_time_ms);

#endif /* SCHEDULING_H */
