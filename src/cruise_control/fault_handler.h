/* Implements: REQ-006, REQ-007, REQ-008, REQ-009, REQ-010, REQ-011,
               ARCH-010, SYS-005, MOD-011 */
#ifndef FAULT_HANDLER_H
#define FAULT_HANDLER_H

#include "state_machine.h"

/* Implements: REQ-006..REQ-011, ARCH-010, MOD-011, UTP-011-A */
ErrorCode_t cruise_handle_fault(ControlCondition_t condition);

#endif /* FAULT_HANDLER_H */
