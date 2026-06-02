/* Implements: REQ-CN-001, REQ-IF-001, REQ-IF-002, REQ-IF-003, REQ-IF-004,
               REQ-IF-005, REQ-IF-006, REQ-IF-007, ARCH-006, ARCH-007,
               ARCH-012, ARCH-013, MOD-007, MOD-008, MOD-013, MOD-014 */
/* Platform HAL Abstraction — EV Prototype Platform
 * ASIL D — Confirmed (OQ-001 resolved 2026-06-02)
 * Language: C + MISRA C:2012
 */
#ifndef PLATFORM_HAL_H
#define PLATFORM_HAL_H

#include <stdint.h>
#include <stdbool.h>

/* Implements: REQ-IF-001, ARCH-006 */
typedef enum {
    PLATFORM_OK    = 0u,
    PLATFORM_ERROR = 1u
} PlatformStatus_t;

/* Implements: REQ-IF-002, ARCH-007 — raw platform input bundle */
typedef struct {
    uint8_t  brake_status_raw;    /* raw brake pedal word (pre-CRC) */
    uint8_t  brake_crc;           /* CRC-16 high byte (simplified) */
    uint8_t  brake_counter;       /* 4-bit rolling counter */
    uint8_t  driver_cmd_raw;      /* raw driver command word */
    uint8_t  driver_crc;
    uint8_t  driver_counter;
    uint16_t vehicle_speed_raw;   /* km/h x10 */
    uint8_t  diagnostic_status;   /* fault code byte */
    bool     lcu_availability;    /* true = available */
} PlatformInputs_t;

/* Implements: REQ-IF-005, ARCH-007 */
PlatformStatus_t platform_read_brake_pedal(uint8_t *out);
/* Implements: REQ-IF-005, ARCH-007 */
PlatformStatus_t platform_read_driver_cmd(uint8_t *out);
/* Implements: REQ-IF-006, ARCH-007 */
PlatformStatus_t platform_read_vehicle_speed(uint16_t *out);
/* Implements: REQ-IF-007, ARCH-007 */
PlatformStatus_t platform_read_diagnostic(uint8_t *out);
/* Implements: REQ-CN-001, ARCH-007 */
PlatformStatus_t platform_read_lcu(bool *out);

/* Implements: REQ-IF-001, ARCH-006 */
PlatformStatus_t platform_propulsion_cease(void);
/* Implements: REQ-IF-001, ARCH-006 */
PlatformStatus_t platform_propulsion_set(uint16_t cmd_value);

/* Implements: REQ-IF-004, ARCH-012 */
typedef struct {
    uint8_t  from_state;
    uint8_t  condition;
    uint8_t  to_state;
    uint16_t vehicle_spd;
    uint32_t timestamp;
} StorageRecord_t;
PlatformStatus_t platform_event_storage_write(const StorageRecord_t *rec);

/* Implements: REQ-IF-003, ARCH-013 */
typedef enum { NOTIF_BRAKE_OVERRIDE = 0u } NotifEventType_t;
PlatformStatus_t platform_notification_send(NotifEventType_t event_type);

/* Implements: REQ-NF-001, ARCH-015 — platform clock in ms */
uint32_t platform_get_time_ms(void);

#endif /* PLATFORM_HAL_H */
