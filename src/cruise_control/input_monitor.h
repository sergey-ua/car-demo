/* Implements: REQ-016, REQ-CN-007, REQ-IF-002, REQ-IF-005, REQ-IF-006,
               REQ-IF-007, REQ-CN-001, ARCH-007, ARCH-008, ARCH-009,
               SYS-004, MOD-008, MOD-009, MOD-010 */
#ifndef INPUT_MONITOR_H
#define INPUT_MONITOR_H

#include "state_machine.h"
#include "../platform_hal.h"

/* Implements: REQ-IF-002, ARCH-008, MOD-009 */
#define BRAKE_APPLIED    (0x01u)
#define DIAG_CCF_CODE    (0x01u)
#define SPEED_MIN_KMH10  (50u)   /* 5 km/h */
#define SPEED_MAX_KMH10  (3000u) /* 300 km/h */

typedef struct {
    uint8_t  data[8u];
    uint8_t  data_length;
    uint16_t crc16;
    uint8_t  counter;
} RawInput_t;

typedef struct {
    uint8_t value;
    bool    validity;
} VerifiedInput_t;

typedef struct {
    bool brake_override;
    bool rii_active;
    bool lcu_active;
    bool ccf_active;
} ConditionSet_t;

/* Implements: REQ-016, ARCH-007, MOD-008 */
ErrorCode_t cruise_read_platform_inputs(PlatformInputs_t *out);

/* Implements: REQ-CN-007, ARCH-008, MOD-009 */
VerifiedInput_t cruise_verify_brake(const RawInput_t *raw);
VerifiedInput_t cruise_verify_driver(const RawInput_t *raw);

/* Implements: REQ-IF-002, ARCH-009, MOD-010 */
ErrorCode_t cruise_derive_conditions(const VerifiedInput_t *brake_v,
                                     const VerifiedInput_t *driver_v,
                                     const PlatformInputs_t *raw,
                                     ConditionSet_t *out);

#endif /* INPUT_MONITOR_H */
