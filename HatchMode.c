#include "qpc.h"
#include "incubator_hatcher.h"
#include "priorities.h"
#include "bsp.h"

#define INCUBATION_TIME (18 * 24 * 60 * 60 * BSP_TICKS_PER_SEC) // 18 Days completed 
#define CANDLE_TIME (16 * 24 * 60 * 60 * BSP_TICKS_PER_SEC) // 16 Days completed 

typedef struct {
    QActive super;
    QTimeEvt HatchModeEvt;
    QTimeEvt CandleDaytimeEvt;
} HatchMode;

extern HatchMode HatchMode_inst;

static QState HatchMode_initial(HatchMode * const me, void const * const par);
static QState HatchMode_incubation(HatchMode * const me, QEvt const * const e);
static QState HatchMode_hatch(HatchMode * const me, QEvt const * const e);

void HatchMode_ctor(void) {
    HatchMode *me = &HatchMode_inst;
    QActive_ctor(&me->super, Q_STATE_CAST(&HatchMode_initial));
    QTimeEvt_ctorX(&me->HatchModeEvt, &me->super, HATCH_MODE_SIG, 0U);
    QTimeEvt_ctorX(&me->CandleDaytimeEvt, &me->super, CANDLE_DAY_SIG, 0U);
    
}

QActive * const AO_HatchMode = &HatchMode_inst.super;

static QState HatchMode_initial(HatchMode * const me, void const * const par) {
    (void)par;
    return Q_TRAN(&HatchMode_incubation);
}

static QState HatchMode_incubation(HatchMode * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG:
            QActive_post(AO_MotorControl, &Q_ROM_EVT(START_MOTOR_SIG), me);
            QTimeEvt_armX(&me->timeEvt, INCUBATION_TIME, 0U);
            status = Q_HANDLED();
            break;
        case CANDLE_DAY_SIG:
             //Buzz and Display to notify candling post to Display buzer 
            status = Q_TRAN(&HatchMode_Candle);// 
            break;
        case HATCH_MODE_SIG:
            status = Q_TRAN(&HatchMode_hatch);
            break;
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

static QState HatchMode_hatch(HatchMode * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG:
            QActive_post(AO_MotorControl, &Q_ROM_EVT(STOP_MOTOR_SIG), me);
            status = Q_HANDLED();
            break;
        default:
            status = Q_SUPER(&QHsm_top);
            break;
    }
    return status;
}
