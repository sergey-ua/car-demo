/* Implements: REQ-002, REQ-006, REQ-009, REQ-IF-001, REQ-CN-001,
               ARCH-005, ARCH-006, SYS-003, MOD-006, MOD-007 */
#ifndef SPEED_CONTROLLER_H
#define SPEED_CONTROLLER_H

#include "state_machine.h"
#include "../platform_hal.h"

typedef enum { CMD_SPEED_CTRL = 0u, CMD_CESSATION = 1u } CmdType_t;

typedef struct {
    uint16_t command_value;
    bool     is_zero;
} SpeedCommand_t;

/* Implements: REQ-002, ARCH-005, MOD-006 */
SpeedCommand_t cruise_generate_speed_cmd(uint16_t cruise_target_kmh10,
                                         uint16_t current_speed_kmh10);

/* Implements: REQ-002, REQ-IF-001, ARCH-006, MOD-007 */
ErrorCode_t cruise_propulsion_write(CmdType_t cmd_type, uint16_t cmd_value);

#endif /* SPEED_CONTROLLER_H */
