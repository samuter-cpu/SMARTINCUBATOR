#include "qpc.h"
#include "incubator_hatcher.h"
#include "alarm_ctrl.h"
#include "priorities.h"
#include "bsp.h"
#include <math.h>
#include "controlUtils.h"


#define FULL_POWER_THRESHOLD 2.0f
#define RAW_OUTPUT_MIN       -50.0f   // Minimum PI output range
#define RAW_OUTPUT_MAX       50.0f    // Maximum PI output range
#define PWM_MIN_DUTY         0.0f
#define PWM_MAX_DUTY         100.0f

static PIController tempPI = {
    .kp = 2.0f,
    .ki = 0.4f,
    .target = 37.5f,
    .integral = 0.0f
};

typedef struct {
    QActive super;
    QTimeEvt timeEvt;
    float current_temp;
    bool alarm_active;
} TemperatureControl;

extern TemperatureControl TemperatureControl_inst;


static QState TemperatureControl_initial(TemperatureControl * const me, void const * const par);
static QState TemperatureControl_active(TemperatureControl * const me, QEvt const * const e);
static bool should_freeze_control(float deviation);


static bool should_freeze_control(float deviation) {
    return fabsf(deviation) >= TEMP_ALARM_THRESHOLD;
}


void TemperatureControl_ctor(void) {
    TemperatureControl *me = &TemperatureControl_inst;
    QActive_ctor(&me->super, Q_STATE_CAST(&TemperatureControl_initial));
    QTimeEvt_ctorX(&me->timeEvt, &me->super, 0U, 0U);
    
}

QActive * const AO_TemperatureControl = &TemperatureControl_inst.super;

static QState TemperatureControl_initial(TemperatureControl * const me, void const * const par) {
    (void)par;
    me->alarm_active = false;
    return Q_TRAN(&TemperatureControl_active);
}

static QState TemperatureControl_active(TemperatureControl * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG:
            QTimeEvt_armX(&me->timeEvt, BSP_TICKS_PER_SEC * 5, BSP_TICKS_PER_SEC * 5);
            status = Q_HANDLED();
            break;
        case Q_TIMEOUT_SIG: {
            me->current_temp = BSP_read_temperature_sensor();
            float deviation = me->current_temp - tempPI.target;
            float duty = apply_PI_control(&tempPI, me->current_temp);

            // ?? Guarded actuator control
            if (!should_freeze_control(deviation)) {
              TempLevelUpdateEvt *temupd = Q_NEW(TempLevelUpdateEvt, TEMPERATURE_TICK_SIG);
              temupd->Temp_level = me->current_temp ;
              QActive_post(AO_LcdDisplay, &temupd->super, me);
                if (deviation < 0.0f) {
                    BSP_setHeaterPower(duty);
                    BSP_setCoolerPower(0.0f);
                } else {
                   BSP_setCoolerPower(duty);
                   BSP_setHeaterPower(0.0f);
               }
           } else {
               BSP_setHeaterPower(0.0f);
               BSP_setCoolerPower(0.0f);
           }

           // ?? Alarm logic reused same deviation
           if (should_freeze_control(deviation)) {
               if (!me->alarm_active) {
                   me->alarm_active = true;
                   QTimeEvt_disarm(&me->timeEvt);
                   QTimeEvt_armX(&me->timeEvt, ALARM_TIMEOUT, 0U);
               } else {
                   AlarmEvt *evt = Q_NEW(AlarmEvt, TEMPERATURE_ALARM_SIG);
                   evt->value = me->current_temp;
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




