#include "qpc.h"
#include "incubator_hatcher.h"
#include "alarm_ctrl.h"
#include "priorities.h"
#include "bsp.h"

typedef struct {
    QActive super;
    QTimeEvt timeEvt;
    float current_co2;
    bool alarm_active;
} CO2Control;

extern CO2Control CO2Control_inst;

static QState CO2Control_initial(CO2Control * const me, void const * const par);
static QState CO2Control_active(CO2Control * const me, QEvt const * const e);

void CO2Control_ctor(void) {
    CO2Control *me = &CO2Control_inst;
    QActive_ctor(&me->super, Q_STATE_CAST(&CO2Control_initial));
    QTimeEvt_ctorX(&me->timeEvt, &me->super, 0U, 0U);
    
}

QActive * const AO_CO2Control = &CO2Control_inst.super;

static QState CO2Control_initial(CO2Control * const me, void const * const par) {
    (void)par;
    me->alarm_active = false;
    return Q_TRAN(&CO2Control_active);
}

static QState CO2Control_active(CO2Control * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG:
            QTimeEvt_armX(&me->timeEvt, BSP_TICKS_PER_SEC * 5, BSP_TICKS_PER_SEC * 5);
            status = Q_HANDLED();
            break;
        case Q_TIMEOUT_SIG: {
               me->current_co2 = BSP_read_co2_sensor();
               CO2LevelUpdateEvt *co2upd = Q_NEW(CO2LevelUpdateEvt, CO2_TICK_SIG);
               co2upd->co2_level = me->current_co2;
               QActive_post(AO_Co2Control, &co2upd->super, me);
            // ??? Fan control based on CO2 level
            if (me->current_co2 > CO2_ALARM_THRESHOLD) {
                BSP_start_inlet_fan();     // Introduce fresh air
                BSP_start_outlet_fan();    // Expel indoor air

                if (!me->alarm_active) {
                    me->alarm_active = true;
                    QTimeEvt_disarm(&me->timeEvt);
                    QTimeEvt_armX(&me->timeEvt, ALARM_TIMEOUT, 0U);
              } else {
               AlarmEvt *evt = Q_NEW(AlarmEvt, CO2_ALARM_SIG);
               evt->value = me->current_co2;
               QActive_post(AO_AlarmControl, &evt->super, me);
               DisplayAlarmEvt *disalarmevt = Q_NEW(DisplayAlarmEvt, ALARM_ON_SIG);
                   QActive_post(AO_LcdDisplay, &disalarmevt->super, me);
          }
        } else {
          BSP_stop_inlet_fan();      // Stop fan action
          BSP_stop_outlet_fan();

          if (me->alarm_active) {
              me->alarm_active = false;
              QTimeEvt_disarm(&me->timeEvt);
              QTimeEvt_armX(&me->timeEvt, BSP_TICKS_PER_SEC * 5, BSP_TICKS_PER_SEC * 5);
        }
    }

    status = Q_HANDLED();
    break;
}

        case Q_EXIT_SIG:
            QTimeEvt_disarm(&me->timeEvt);
            status = Q_HANDLED();
            break;
        default:
            status = Q_SUPER(&QHsm_top);
            break;
    }
    return status;
}
