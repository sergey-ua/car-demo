/* Implements: REQ-CN-001, ARCH-007, ARCH-006, ARCH-012, ARCH-013,
               MOD-008, MOD-007, MOD-013, MOD-014 */
/* Platform HAL Stub — configurable return codes for all test levels
 * ASIL D — Confirmed
 */
#include "../../src/platform_hal.h"
#include <string.h>

/* Configurable stub state */
static PlatformStatus_t stub_brake_status  = PLATFORM_OK;
static uint8_t          stub_brake_value   = 0x00u;
static uint8_t          stub_brake_crc     = 0u;
static uint8_t          stub_brake_counter = 0u;
static PlatformStatus_t stub_driver_status = PLATFORM_OK;
static uint8_t          stub_driver_value  = 0x00u;
static PlatformStatus_t stub_speed_status  = PLATFORM_OK;
static uint16_t         stub_speed_value   = 900u;  /* 90 km/h */
static PlatformStatus_t stub_diag_status   = PLATFORM_OK;
static uint8_t          stub_diag_value    = 0x00u;
static PlatformStatus_t stub_lcu_status    = PLATFORM_OK;
static bool             stub_lcu_value     = true;
static PlatformStatus_t stub_propulsion_status = PLATFORM_OK;
static PlatformStatus_t stub_storage_status    = PLATFORM_OK;
static PlatformStatus_t stub_notif_status      = PLATFORM_OK;
static uint32_t         stub_time_ms           = 0u;

/* Implements: REQ-IF-002, ARCH-007, MOD-008 */
PlatformStatus_t platform_read_brake_pedal(uint8_t *out)
{
    if (out != NULL) { *out = stub_brake_value; }
    return stub_brake_status;
}
PlatformStatus_t platform_read_driver_cmd(uint8_t *out)
{
    if (out != NULL) { *out = stub_driver_value; }
    return stub_driver_status;
}
PlatformStatus_t platform_read_vehicle_speed(uint16_t *out)
{
    if (out != NULL) { *out = stub_speed_value; }
    return stub_speed_status;
}
PlatformStatus_t platform_read_diagnostic(uint8_t *out)
{
    if (out != NULL) { *out = stub_diag_value; }
    return stub_diag_status;
}
PlatformStatus_t platform_read_lcu(bool *out)
{
    if (out != NULL) { *out = stub_lcu_value; }
    return stub_lcu_status;
}
PlatformStatus_t platform_propulsion_cease(void)   { return stub_propulsion_status; }
PlatformStatus_t platform_propulsion_set(uint16_t v) { (void)v; return stub_propulsion_status; }
PlatformStatus_t platform_event_storage_write(const StorageRecord_t *r) { (void)r; return stub_storage_status; }
PlatformStatus_t platform_notification_send(NotifEventType_t t) { (void)t; return stub_notif_status; }
uint32_t         platform_get_time_ms(void) { return stub_time_ms; }

/* Stub configuration API for tests */
void stub_set_brake(uint8_t val, PlatformStatus_t st) { stub_brake_value=val; stub_brake_status=st; }
void stub_set_speed(uint16_t val, PlatformStatus_t st) { stub_speed_value=val; stub_speed_status=st; }
void stub_set_propulsion_status(PlatformStatus_t st)   { stub_propulsion_status=st; }
void stub_set_storage_status(PlatformStatus_t st)      { stub_storage_status=st; }
void stub_set_notif_status(PlatformStatus_t st)        { stub_notif_status=st; }
void stub_set_time_ms(uint32_t t)                      { stub_time_ms=t; }
void stub_set_lcu(bool val)                            { stub_lcu_value=val; }
void stub_set_diag(uint8_t val)                        { stub_diag_value=val; }
