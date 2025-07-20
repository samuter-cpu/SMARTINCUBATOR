#include "qpc.h"
#include "incubator_hatcher.h"
#include "alarm_ctrl.h"
#include "priorities.h"
#include "bsp.h"
#include <math.h>
#include "controlUtils.h"


static PIController humidPI = {
    .kp = 1.5f,
    .ki = 0.2f,
    .target = 60.0f,
    .integral = 0.0f
};


typedef struct {
    QActive super;
    QTimeEvt timeEvt;
    float current_humidity;
    bool alarm_active;
} HumidityControl;


extern HumidityControl HumidityControl_inst;

static QState HumidityControl_initial(HumidityControl * const me, void const * const par);
static QState HumidityControl_active(HumidityControl * const me, QEvt const * const e);
static bool should_freeze_control(float deviation);

static bool should_freeze_control(float deviation) {
    return fabsf(deviation) >= HUMIDITY_ALARM_THRESHOLD;
}

void HumidityControl_ctor(void) {
    HumidityControl *me = &HumidityControl_inst;
    QActive_ctor(&me->super, Q_STATE_CAST(&HumidityControl_initial));
    QTimeEvt_ctorX(&me->timeEvt, &me->super, 0U, 0U);
    
}


QActive * const AO_HumidityControl = &HumidityControl_inst.super;

static QState HumidityControl_initial(HumidityControl * const me, void const * const par) {
    (void)par;
    me->alarm_active = false;
    return Q_TRAN(&HumidityControl_active);
}

static QState HumidityControl_active(HumidityControl * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG:
            QTimeEvt_armX(&me->timeEvt, BSP_TICKS_PER_SEC * 5, BSP_TICKS_PER_SEC * 5);
            status = Q_HANDLED();
            break;
        case Q_TIMEOUT_SIG: {
            float current_humidity = BSP_read_humidity_sensor();  // Replace with your sensor call
            float deviation = current_humidity - humidPI.target;
            float duty = apply_PI_control(&humidPI, current_humidity);

            if (!should_freeze_control(deviation)) {
                // Normal operation
                HumLevelUpdateEvt *humupd = Q_NEW(HumLevelUpdateEvt, HUMIDITY_TICK_SIG);
                humupd->Hum_level = me->current_humidity ;
                QActive_post(AO_LcdDisplay, &humupd->super, me);
                BSP_setHumidifierPower(duty);
                BSP_enableUltrasonic();
            } else {
                // Alarm condition or too much deviation
                BSP_setHumidifierPower(0.0f);
                BSP_disableUltrasonic();
            }

            // ?? Alarm logic
            if (should_freeze_control(deviation)) {
                 if (!me->alarm_active) {
                    me->alarm_active = true;
                    QTimeEvt_disarm(&me->timeEvt);
                    QTimeEvt_armX(&me->timeEvt, ALARM_TIMEOUT, 0U);
            }   else {
                    AlarmEvt *evt = Q_NEW(AlarmEvt, HUMIDITY_ALARM_SIG);
                    evt->value = current_humidity;
                    QActive_post(AO_AlarmControl, &evt->super, me);
                    DisplayAlarmEvt *disalarmevt = Q_NEW(DisplayAlarmEvt, ALARM_ON_SIG);
                    QActive_post(AO_LcdDisplay, &disalarmevt->super, me);
           }
         } else {
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


