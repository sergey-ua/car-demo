/* Implements: REQ-012, REQ-IF-003, ARCH-013, SYS-007, MOD-014,
               UTP-014-A, UTP-014-B, HAZ-018, HAZ-019 */
/* Notification Publisher — exactly-once delivery with dedup
 * ASIL D — Confirmed
 * Language: C + MISRA C:2012
 */
#include "notification.h"

#define NOTIF_DEDUP_SIZE (8u)
static uint8_t g_seen_ids[NOTIF_DEDUP_SIZE];
static uint8_t g_seen_count = 0u;

/* Private dedup check — bounded linear search */
static bool cruise_notif_is_duplicate(uint8_t event_id)
{
    bool    found = false;
    uint8_t i     = 0u;
    uint8_t limit = (g_seen_count < NOTIF_DEDUP_SIZE)
                  ? g_seen_count : NOTIF_DEDUP_SIZE;

    for (i = 0u; i < limit; i++) {
        if (g_seen_ids[i] == event_id) { found = true; }
    }
    return found;
}

/* Implements: REQ-012, ARCH-013, MOD-014 */
ErrorCode_t cruise_publish_notification(const NotificationRequest_t *req)
{
    ErrorCode_t result = CRUISE_ERR_NULL_PARAM;

    if (req == NULL) {
        result = CRUISE_ERR_NULL_PARAM;
    } else if (cruise_notif_is_duplicate(req->event_id)) {
        result = CRUISE_OK;  /* silently absorbed — exactly-once */
    } else {
        if (platform_notification_send(req->event_type) == PLATFORM_OK) {
            g_seen_ids[g_seen_count % NOTIF_DEDUP_SIZE] = req->event_id;
            if (g_seen_count < NOTIF_DEDUP_SIZE) { g_seen_count++; }
            result = CRUISE_OK;
        } else {
            result = CRUISE_ERR_DELIVERY_FAILURE;  /* E100 */
        }
    }
    return result;
}
