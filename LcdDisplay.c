#include "qpc.h"
#include "bsp.h"
#include "incubator_hatcher.h"
#include "lcdDisplay.h"
#include <stdio.h>


typedef struct {
    QActive super;
    QTimeEvt timeEvt;
    bool  alarm_blinking;
} LcdDisplay;

extern LcdDisplay LcdDisplay_inst;

static QState LcdDisplay_initial(LcdDisplay * const me, void const * const par);
static QState LcdDisplay_active(LcdDisplay * const me, QEvt const * const e);
static QState LcdDisplay_alarm(LcdDisplay * const me, QEvt const * const e);

void LcdDisplay_ctor(void) {
    LcdDisplay * const me = &LcdDisplay_inst;
    QActive_ctor(&me->super, Q_STATE_CAST(&LcdDisplay_initial));
    QTimeEvt_ctorX(&me->timeEvt, &me->super, TIMER_TICK_SIG, 0U);
    
}

QActive * const AO_LcdDisplay = &LcdDisplay_inst.super;

static QState LcdDisplay_initial(LcdDisplay * const me, void const * const par) {
    (void)par;
    me->alarm_blinking = false;
    QTimeEvt_armX(&me->timeEvt, 100, 100); // Tick every second
    // Initialize LCDs
    return Q_TRAN(&LcdDisplay_active);
}



static QState LcdDisplay_active(LcdDisplay * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case TEMPERATURE_TICK_SIG:{
            float TempUpd = (TempLevelUpdateEvt*)->Temp_level;
            BSP_updateLCD1(true, TempUpd,false, 0.0f);
            status = Q_HANDLED();
            break;
        }
        
        case HUMIDITY_TICK_SIG:{
            float HumUpd = (HumLevelUpdateEvt*)->Hum_level;
            BSP_updateLCD1(false,0.0f,true, HumUpd);
            status = Q_HANDLED();
            break;
        }    
        
       case CO2_TICK_SIG: {
            float Co2Upd= (CO2LevelUpdateEvt*)->co2_level;
            BSP_updateLCD2(true,Co2Upd,false,0.0f,false,NULL);
            status = Q_HANDLED();
            break;
       }
       
       case O2_TICK_SIG:   {
            float O2Upd= (O2LevelUpdateEvt*)->o2_level;
            BSP_updateLCD2(false,0.0f,true,O2Upd,false,NULL);
            status = Q_HANDLED();
            break
      }
       case TIMER_TICK_SIG: {
            const char *time_str = BSP_get_time_string();
            BSP_updateLCD2(false,0.0f,false,true,time_str);
            status = Q_HANDLED();
            break;
      }
            
      case ALARM_ON_SIG:{
            me->alarm_blinking = true;
            status = Q_TRAN(&LcdDisplay_alarm);
            break;
     }
     
    default: {
            status = Q_SUPER(&QHsm_top);
            break;
    }
 }
    return status;
}

static QState LcdDisplay_alarm(LcdDisplay * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG:
            QTimeEvt_armX(&me->timeEvt, 50, 50); // Blink every 500ms
            me->alarm_blinking = true;
            status = Q_HANDLED();
            break;
        case TIMER_TICK_SIG:
            // Toggle LCD backlight or text
            status = Q_HANDLED();
            break;
        case ALARM_OFF_SIG:
            me->alarm_blinking = false;
            status = Q_TRAN(&LCDDisplay_active);
            break;
        case Q_EXIT_SIG:
            QTimeEvt_armX(&me->timeEvt, 100, 100); // Restore normal tick
            me->alarm_blinking = false;            // Restore LCD state
            status = Q_HANDLED();
            break;
        default:
            status = Q_SUPER(&LCDDisplay_active);
            break;
    }
    return status;
}


