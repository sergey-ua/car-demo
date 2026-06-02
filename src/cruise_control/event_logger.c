/* Implements: REQ-013, REQ-014, REQ-015, REQ-017, REQ-IF-004,
               ARCH-011, ARCH-012, SYS-006, MOD-012, MOD-013,
               UTP-012-A, UTP-013-A, HAZ-016, HAZ-017 */
/* Event Record Builder + Storage Adapter
 * ASIL D — Confirmed
 * Language: C + MISRA C:2012
 */
#include "event_logger.h"
#include "fault_handler.h"
#include "../platform_hal.h"

static EventRecord_t g_pending_record;
static bool          g_record_ready = false;

/* Implements: REQ-013, REQ-014, ARCH-011, MOD-012 */
ErrorCode_t cruise_build_event_record(const EventRecord_t *req)
{
    ErrorCode_t result = CRUISE_ERR_FIELD_VALIDATION;

    if (req == NULL) {
        result = CRUISE_ERR_NULL_PARAM;
    } else {
        bool from_ok = ((req->from_state == CRUISE_STANDBY)
                     || (req->from_state == CRUISE_ACTIVE)
                     || (req->from_state == CRUISE_SUSPENDED)
                     || (req->from_state == CRUISE_CANCELLED)
                     || (req->from_state == CRUISE_FAULT));
        bool cond_ok = ((req->condition >= CONDITION_MIN)
                     && (req->condition <= CONDITION_MAX));
        bool to_ok   = ((req->to_state == CRUISE_STANDBY)
                     || (req->to_state == CRUISE_ACTIVE)
                     || (req->to_state == CRUISE_SUSPENDED)
                     || (req->to_state == CRUISE_CANCELLED)
                     || (req->to_state == CRUISE_FAULT));
        bool spd_ok  = (req->vehicle_spd <= 3000u);
        bool ts_ok   = (req->timestamp != 0u);

        if (from_ok && cond_ok && to_ok && spd_ok && ts_ok) {
            g_pending_record = *req;
            g_record_ready   = true;
            result           = CRUISE_OK;
        } else {
            g_record_ready = false;
            result         = CRUISE_ERR_FIELD_VALIDATION;
        }
    }
    return result;
}

/* Implements: REQ-013, REQ-017, ARCH-012, MOD-013 */
ErrorCode_t cruise_write_event_record(void)
{
    ErrorCode_t result = CRUISE_ERR_FIELD_VALIDATION;

    if (!g_record_ready) {
        result = CRUISE_ERR_FIELD_VALIDATION;
    } else {
        StorageRecord_t sr;
        sr.from_state  = (uint8_t)g_pending_record.from_state;
        sr.condition   = (uint8_t)g_pending_record.condition;
        sr.to_state    = (uint8_t)g_pending_record.to_state;
        sr.vehicle_spd = g_pending_record.vehicle_spd;
        sr.timestamp   = g_pending_record.timestamp;

        g_record_ready = false;

        if (platform_event_storage_write(&sr) == PLATFORM_OK) {
            result = CRUISE_OK;
        } else {
            result = CRUISE_ERR_WRITE_FAILURE;
            (void)cruise_handle_fault(COND_CCF);  /* REQ-017 */
        }
    }
    return result;
}
