/* Implements: REQ-016, REQ-CN-007, REQ-IF-002, REQ-IF-005, REQ-IF-006,
               REQ-IF-007, REQ-CN-001, REQ-CN-003, ARCH-007, ARCH-008,
               ARCH-009, SYS-004, MOD-008, MOD-009, MOD-010,
               UTP-008-A, UTP-009-A, UTP-009-B, UTP-009-C, UTP-010-A,
               HAZ-009, HAZ-010, HAZ-011, HAZ-012 */
/* Platform Input Monitor — read, integrity-verify, derive conditions
 * ASIL D — Confirmed
 * Language: C + MISRA C:2012; CRC-16/CCITT-FALSE (REQ-CN-007)
 */
#include "input_monitor.h"
#include "fault_handler.h"

#define COUNTER_MASK  (0x0Fu)
#define CRC16_POLY    (0x1021u)
#define CRC16_INIT    (0xFFFFu)
#define MAX_INPUT_SIZE (8u)

/* Rolling counter state per channel (MOD-009 static state) */
/* Implements: REQ-CN-007, MOD-009 */
static uint8_t g_expected_counter_brake  = 0u;
static uint8_t g_expected_counter_driver = 0u;

/* Private CRC-16/CCITT-FALSE — bounded loops, MISRA C:2012 compliant */
/* Implements: REQ-CN-007, MOD-009 */
static uint16_t cruise_compute_crc16(const uint8_t *data, uint8_t length)
{
    uint16_t crc   = CRC16_INIT;
    uint8_t  i     = 0u;
    uint8_t  j     = 0u;
    uint8_t  lim   = (length <= MAX_INPUT_SIZE) ? length : MAX_INPUT_SIZE;

    for (i = 0u; i < lim; i++) {
        crc ^= ((uint16_t)data[i] << 8u);
        for (j = 0u; j < 8u; j++) {
            if ((crc & 0x8000u) != 0u) {
                crc = (uint16_t)((crc << 1u) ^ CRC16_POLY);
            } else {
                crc = (uint16_t)(crc << 1u);
            }
        }
    }
    return crc;
}

/* Private integrity check for one channel */
static VerifiedInput_t cruise_verify_integrity(const RawInput_t *raw,
                                               uint8_t          *expected_counter)
{
    VerifiedInput_t result;
    result.value    = 0u;
    result.validity = false;

    if ((raw == NULL) || (expected_counter == NULL)) {
        result.validity = false;
    } else {
        uint16_t computed = cruise_compute_crc16(raw->data, raw->data_length);
        bool crc_ok     = (computed == raw->crc16);
        bool counter_ok = ((raw->counter & COUNTER_MASK)
                          == (*expected_counter & COUNTER_MASK));

        if (crc_ok && counter_ok) {
            result.value    = raw->data[0u];
            result.validity = true;
            *expected_counter = (uint8_t)((*expected_counter + 1u) & COUNTER_MASK);
        }
    }
    return result;
}

/* Implements: REQ-CN-007, ARCH-008, MOD-009 */
VerifiedInput_t cruise_verify_brake(const RawInput_t *raw)
{
    return cruise_verify_integrity(raw, &g_expected_counter_brake);
}

VerifiedInput_t cruise_verify_driver(const RawInput_t *raw)
{
    return cruise_verify_integrity(raw, &g_expected_counter_driver);
}

/* Implements: REQ-016, ARCH-007, MOD-008 */
ErrorCode_t cruise_read_platform_inputs(PlatformInputs_t *out)
{
    ErrorCode_t result = CRUISE_ERR_NULL_PARAM;

    if (out == NULL) {
        result = CRUISE_ERR_NULL_PARAM;
    } else {
        PlatformStatus_t s1 = platform_read_brake_pedal(&out->brake_status_raw);
        PlatformStatus_t s2 = platform_read_driver_cmd(&out->driver_cmd_raw);
        PlatformStatus_t s3 = platform_read_vehicle_speed(&out->vehicle_speed_raw);
        PlatformStatus_t s4 = platform_read_diagnostic(&out->diagnostic_status);
        PlatformStatus_t s5 = platform_read_lcu(&out->lcu_availability);

        if ((s1 == PLATFORM_OK) && (s2 == PLATFORM_OK) && (s3 == PLATFORM_OK)
         && (s4 == PLATFORM_OK) && (s5 == PLATFORM_OK)) {
            result = CRUISE_OK;
        } else {
            result = CRUISE_ERR_READ_TIMEOUT;  /* E050 */
        }
    }
    return result;
}

/* Implements: REQ-IF-002, ARCH-009, MOD-010 */
ErrorCode_t cruise_derive_conditions(const VerifiedInput_t *brake_v,
                                     const VerifiedInput_t *driver_v,
                                     const PlatformInputs_t *raw,
                                     ConditionSet_t *out)
{
    ErrorCode_t result = CRUISE_ERR_NULL_PARAM;

    if ((brake_v == NULL) || (driver_v == NULL) || (raw == NULL) || (out == NULL)) {
        result = CRUISE_ERR_NULL_PARAM;
    } else {
        bool speed_ok = (raw->vehicle_speed_raw >= SPEED_MIN_KMH10)
                     && (raw->vehicle_speed_raw <= SPEED_MAX_KMH10);

        out->brake_override = brake_v->validity
                           && (brake_v->value == BRAKE_APPLIED);
        out->rii_active     = !brake_v->validity || !driver_v->validity || !speed_ok;
        out->lcu_active     = !raw->lcu_availability;
        out->ccf_active     = (raw->diagnostic_status == DIAG_CCF_CODE);

        result = CRUISE_OK;
    }
    return result;
}
