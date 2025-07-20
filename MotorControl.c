#include "qpc.h"
#include "incubator_hatcher.h"
#include "motor_ctrl.h"
#include "priorities.h"
#include "bsp.h"

typedef struct {
    QActive super;
    QTimeEvt timeEvt;
} MotorControl;

extern MotorControl MotorControl_inst;

static QState MotorControl_initial(MotorControl * const me, void const * const par);
static QState MotorControl_active(MotorControl * const me, QEvt const * const e);
static QState MotorControl_forward(MotorControl * const me, QEvt const * const e);
static QState MotorControl_backward(MotorControl * const me, QEvt const * const e);
static QState MotorControl_idle(MotorControl * const me, QEvt const * const e);

void MotorControl_ctor(void) {
    MotorControl *me = &MotorControl_inst;
    QActive_ctor(&me->super, Q_STATE_CAST(&MotorControl_initial));
    QTimeEvt_ctorX(&me->timeEvt, &me->super, 0U, 0U);
    
}


QActive * const AO_MotorControl = MotorControl_inst.super ;

static QState MotorControl_initial(MotorControl * const me, void const * const par) {
    (void)par;
    return Q_TRAN(&MotorControl_idle);
}

static QState MotorControl_idle(MotorControl * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case START_MOTOR_SIG:
            status = Q_TRAN(&MotorControl_active);
            break;
        default:
            status = Q_SUPER(&QHsm_top);
            break;
    }
    return status;
}

static QState MotorControl_active(MotorControl * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG:
            status = Q_TRAN(&MotorControl_forward);
            break;
        case STOP_MOTOR_SIG:
            status = Q_TRAN(&MotorControl_idle);
            break;
        default:
            status = Q_SUPER(&QHsm_top);
            break;
    }
    return status;
}

static QState MotorControl_forward(MotorControl * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG:
            BSP_motor_forward();
            QTimeEvt_armX(&me->timeEvt, ROTATION_TIME, 0U);
            status = Q_HANDLED();
            break;
        case Q_TIMEOUT_SIG:
            status = Q_TRAN(&MotorControl_backward);
            break;
        case Q_EXIT_SIG:
            QTimeEvt_disarm(&me->timeEvt);
            BSP_motor_stop();
            status = Q_HANDLED();
            break;
        default:
            status = Q_SUPER(&MotorControl_active);
            break;
    }
    return status;
}

static QState MotorControl_backward(MotorControl * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG:
            BSP_motor_backward();
            QTimeEvt_armX(&me->timeEvt, ROTATION_TIME, 0U);
            status = Q_HANDLED();
            break;
        case Q_TIMEOUT_SIG:
            status = Q_TRAN(&MotorControl_forward);
            break;
        case Q_EXIT_SIG:
            QTimeEvt_disarm(&me->timeEvt);
            BSP_motor_stop();
            status = Q_HANDLED();
            break;
        default:
            status = Q_SUPER(&MotorControl_active);
            break;
    }
    return status;
}


