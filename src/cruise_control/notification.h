/* Implements: REQ-012, REQ-IF-003, ARCH-013, SYS-007, MOD-014 */
#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include "state_machine.h"
#include "../platform_hal.h"

typedef struct {
    uint8_t          event_id;
    NotifEventType_t event_type;
} NotificationRequest_t;

/* Implements: REQ-012, ARCH-013, MOD-014 */
ErrorCode_t cruise_publish_notification(const NotificationRequest_t *req);

#endif /* NOTIFICATION_H */
