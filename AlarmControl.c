#include "qpc.h"
#include "incubator_hatcher.h"
#include "alarm_ctrl.h"
#include "priorities.h"
#include "bsp.h"

typedef struct {
    QActive super;
    QTimeEvt timeEvt;
} AlarmControl;

extern AlarmControl AlarmControl_inst;

static QState AlarmControl_initial(AlarmControl * const me, void const * const par);
static QState AlarmControl_idle(AlarmControl * const me, QEvt const * const e);
static QState AlarmControl_alarming(AlarmControl * const me, QEvt const * const e);

void AlarmControl_ctor(void) {
    AlarmControl *me = &AlarmControl_inst;
    QActive_ctor(&me->super, Q_STATE_CAST(&AlarmControl_initial));
    QTimeEvt_ctorX(&me->timeEvt, &me->super, 0U, 0U);
    
}

QActive * const AO_AlarmControl = &AlarmControl_inst.super;
static QState AlarmControl_initial(AlarmControl * const me, void const * const par) {
    (void)par;
    return Q_TRAN(&AlarmControl_idle);
}

static QState AlarmControl_idle(AlarmControl * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case TEMPERATURE_ALARM_SIG:
        case HUMIDITY_ALARM_SIG:
        case CO2_ALARM_SIG:
        case INSUFFICIENT_AIRFLOW_ALARM_SIG:
            status = Q_TRAN(&AlarmControl_alarming);
            break;
        default:
            status = Q_SUPER(&QHsm_top);
            break;
    }
    return status;
}

static QState AlarmControl_alarming(AlarmControl * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG:
          
            BSP_activate_buzzer();
            BSP_blink_lcd();
            status = Q_HANDLED();
            break;
        case ALARM_RESET_SIG:
            status = Q_TRAN(&AlarmControl_idle);
            break;
        case Q_EXIT_SIG:
            DisplayAlarmEvt *disalarmevt = Q_NEW(DisplayAlarmEvt, ALARM_Off_SIG);
            QActive_post(AO_LcdDisplay, &disalarmevt->super, me);
            BSP_deactivate_buzzer();
            BSP_stop_blink_lcd();
            status = Q_HANDLED();
            break;
        default:
            status = Q_SUPER(&QHsm_top);
            break;
    }
    return status;
}