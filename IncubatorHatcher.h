#ifndef INCUBATOR_HATCHER_H
#define INCUBATOR_HATCHER_H

#include "qpc.h"

// Shared signals
enum IncubatorHatcherSignals {
    DUMMY_SIG = Q_USER_SIG,
    //CO2_LEVEL_UPDATE_SIG,
    //VALVE_OPEN_SIG,
    //VALVE_CLOSE_SIG,
    FAN_INWARD_ON_SIG,
    FAN_OUTWARD_ON_SIG,
    FAN_OFF_SIG,
    VALVE_OPEN_EVENT_SIG,
    VALVE_CLOSE_EVENT_SIG,
    INSUFFICIENT_AIRFLOW_ALARM_SIG,
    START_MOTOR_SIG,
    STOP_MOTOR_SIG,
    HATCH_MODE_SIG,
    
//Display alarm (casted to DisplayAlarmEvt) on lcd. 
//Posed to AlarmControl active oject
    ALARM_ON_SIG,
    ALARM_OFF_SIG,
    
//Display measurments on 
    TEMPERATURE_TICK_SIG,
    HUMIDITY_TICK_SIG,
    CO2_TICK_SIG,
    
//Post to alarm active object
    TEMPERATURE_ALARM_SIG,
    HUMIDITY_ALARM_SIG,
    CO2_ALARM_SIG,
    ALARM_RESET_SIG,
    MAX_PUB_SIG,
    MAX_SIG
};






// Event structures

typedef struct {
    QEvt super;
    float Temp_level;
} TempLevelUpdateEvt;

typedef struct {
    QEvt super;
    float Hum_level;
} HumLevelUpdateEvt;


typedef struct {
    QEvt super;
    float co2_level;
} CO2LevelUpdateEvt;

typedef struct {
    QEvt super;
    float o2_level;
} O2LevelUpdateEvt;


typedef struct {
    QEvt super;
    float value;
} AlarmEvt;

typedef struct {
   QEvt super;
    
} DisplayAlarmEvt;


//typedef struct {
//    QEvt super;
//} ValveOpenEventEvt;

//typedef struct {
//   QEvt super;
//} ValveCloseEventEvt;

// Function prototypes for active objects
void TemperatureControl_ctor(void);
void HumidityControl_ctor(void);
void CO2Control_ctor(void);
void MotorControl_ctor(void);
void LcdDisplay_ctor(void);
//QActive *VentilationControl_ctor(void);
void AlarmControl_ctor(void);
void HatchMode_ctor(void);

//Pointer to Q_Active objects 
extern QActive * const AO_TemperatureControl;
extern QActive * const AO_HumidityControl;
extern QActive * const AO_CO2Control;
extern QActive * const AO_MotorControl;
extern QActive * const AO_LcdDisplay;
extern QActive * const AO_AlarmControl;
extern QActive * const AO_HatchMode;

#endif // INCUBATOR_HATCHER_H


