/* Implements: REQ-002, REQ-005, REQ-006, REQ-009, REQ-IF-001, REQ-CN-001,
               ARCH-005, ARCH-006, SYS-003, MOD-006, MOD-007,
               UTP-006-A, UTP-007-A, HAZ-007, HAZ-008 */
/* Speed Control Regulator and Propulsion Interface Adapter
 * ASIL D — Confirmed
 * Language: C + MISRA C:2012
 */
#include "speed_controller.h"
#include "fault_handler.h"

#define SPEED_CMD_MAX       (1000u)
#define SPEED_CMD_MIN       (0u)
#define SPEED_CMD_MAX_DELTA (500u)   /* 50 km/h error */
#define K_P_NUMERATOR       (2u)
#define K_P_DENOMINATOR     (10u)

/* Implements: REQ-002, ARCH-005, MOD-006, UTP-006-A */
SpeedCommand_t cruise_generate_speed_cmd(uint16_t cruise_target_kmh10,
                                         uint16_t current_speed_kmh10)
{
    SpeedCommand_t result;
    result.command_value = 0u;
    result.is_zero       = true;

    if (cruise_state_read() == CRUISE_ACTIVE) {
        int32_t error = (int32_t)cruise_target_kmh10 - (int32_t)current_speed_kmh10;
        uint16_t abs_err = (error >= 0) ? (uint16_t)error : (uint16_t)(-error);

        if (abs_err > SPEED_CMD_MAX_DELTA) {
            result.command_value = SPEED_CMD_MAX;
        } else {
            result.command_value = (uint16_t)((uint32_t)K_P_NUMERATOR
                                 * (uint32_t)abs_err / K_P_DENOMINATOR);
        }
        result.is_zero = (result.command_value == 0u);
    }
    return result;
}

/* Implements: REQ-002, REQ-IF-001, ARCH-006, MOD-007, UTP-007-A, HAZ-007 */
ErrorCode_t cruise_propulsion_write(CmdType_t cmd_type, uint16_t cmd_value)
{
    ErrorCode_t      result      = CRUISE_ERR_WRITE_FAILURE;
    PlatformStatus_t plat_status;

    if ((cmd_type == CMD_SPEED_CTRL) && (cruise_state_read() != CRUISE_ACTIVE)) {
        result = CRUISE_ERR_NOT_AUTHORISED;
    } else {
        if (cmd_type == CMD_CESSATION) {
            plat_status = platform_propulsion_cease();
        } else {
            plat_status = platform_propulsion_set(cmd_value);
        }

        if (plat_status == PLATFORM_OK) {
            result = CRUISE_OK;
        } else {
            result = CRUISE_ERR_WRITE_FAILURE;  /* E040 */
            (void)cruise_handle_fault(COND_CCF);
        }
    }
    return result;
}
