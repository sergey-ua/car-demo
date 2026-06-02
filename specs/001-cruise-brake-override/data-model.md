# Data Model: Vehicle Cruise Control — Brake Override Safety Slice

<!-- v-model:traces source: v-model/system-design.md Data Design View; version: v0.7.0 -->

**Branch**: `001-cruise-brake-override` | **Phase 1 output** | **Date**: 2026-06-02

## Core Entities

### CruiseControlState_t (MOD-003)

Represents the current operational mode of the cruise control feature.

```c
typedef enum {
    CRUISE_STANDBY   = 0u,  /* available, not controlling */
    CRUISE_ACTIVE    = 1u,  /* holds longitudinal authority */
    CRUISE_SUSPENDED = 2u,  /* temporarily suspended */
    CRUISE_CANCELLED = 3u,  /* session ended (post-override) */
    CRUISE_FAULT     = 4u   /* self-diagnosed fault; no commands permitted */
} CruiseControlState_t;
```

**Storage**: Static module variable `g_cruise_state` in `state_machine.c`
**Protection at rest**: Spatial isolation (dedicated data segment); range-checked on every write
**Retention**: Discarded at component reset; no persistence

---

### ControlCondition_t (MOD-010)

Named input condition derived from validated platform inputs.

```c
typedef enum {
    COND_BRAKE_OVERRIDE = 0u,  /* brake pedal applied, integrity OK */
    COND_RII            = 1u,  /* Required_Input_Invalid */
    COND_LCU            = 2u,  /* Longitudinal_Control_Unavailable */
    COND_CCF            = 3u   /* Cruise_Control_Fault */
} ControlCondition_t;
```

**Derived by**: ARCH-009 (Condition Deriver / MOD-010) from validated platform inputs
**Consumers**: ARCH-001 (state machine), ARCH-003 (override sequencer), ARCH-004 (timing), ARCH-010 (fault handler)

---

### StateTransitionEvent (MOD-012)

Persistent record of a safety-relevant state change (REQ-014).

```c
typedef struct {
    CruiseControlState_t from_state;  /* state before transition */
    ControlCondition_t   condition;   /* triggering condition */
    CruiseControlState_t to_state;    /* state after transition */
    uint16_t             vehicle_spd; /* km/h × 10 at time of event */
    uint32_t             timestamp;   /* platform clock ticks; non-zero */
} EventRecord_t;
```

**Storage**: Platform local persistent event storage (REQ-IF-004)
**Retention**: Oldest-first circular overwrite (OQ-007 resolved)
**Protection in transit**: Synchronous write confirmation before transition completes (REQ-013)
**Write failure**: Escalates to Cruise_Control_Fault → Cruise_Fault (REQ-017)

---

### PreconditionFlags_t (MOD-002)

The five activation preconditions evaluated before Cruise_Standby → Cruise_Active (REQ-018).

```c
typedef struct {
    bool speed_in_valid_range;   /* vehicle speed within activation range */
    bool brake_override_active;  /* brake pedal applied */
    bool rii_active;             /* Required_Input_Invalid */
    bool ccf_active;             /* Cruise_Control_Fault */
    bool lcu_active;             /* Longitudinal_Control_Unavailable */
} PreconditionFlags_t;
```

**Evaluation**: ARCH-001 / MOD-002 evaluates AND of all five on each activation attempt
**Storage**: In-memory, per-call; no persistence

---

### VerifiedInput_t (MOD-009)

Output of CRC-16 + 4-bit rolling counter integrity check (REQ-CN-007).

```c
typedef struct {
    uint8_t value;    /* validated data byte (valid only if validity == true) */
    bool    validity; /* true = passed CRC-16 and counter checks */
} VerifiedInput_t;
```

**Produced by**: ARCH-008 / MOD-009
**Consumed by**: ARCH-009 / MOD-010 for condition derivation
